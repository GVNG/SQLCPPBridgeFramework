//
//  sb_kv_storage.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 11/03/2016.
//  Copyright © 2016, DataArt.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//      * Neither the name of the DataArt nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL DataArt BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once
#ifndef sb_kv_storage_h
#define sb_kv_storage_h

#include "sb_core.h"
#include "sb_data_section.h"

namespace sql_bridge
{
    template<typename TStrategy> class key_value_storage
        : protected TStrategy::sql_file
        , protected TStrategy
    {
        
    public:
        key_value_storage(std::string const& sqlname)
            : TStrategy::sql_file(sqlname)
            {}
        
        template<typename T> inline void save(std::string const& key, T const& val) {_save(key,val);}
        template<typename T, typename TKey> inline void remove(TKey const& key, std::string const& tab) {_remove<T>(key,tab);}
        template<typename T> inline void load(std::string const& key, T& trg) const {_load(key,trg);}
        inline void load(std::string const& key, std::string& trg) const {_load(key, trg);}
        
        inline data_sections_ptr create_section(std::string const& name,std::string const& path, fn_change_file_name fnch)
        {
            std::string fname(TStrategy::create_section(*this,name,path,fnch));
            return std::make_shared< _t_data_section<TStrategy> >(fname,name);
        }
    private:

#pragma mark - load
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _load(std::string const& key, T& trg) const {typename TStrategy::sql_reader_kv rd(*this,to_string() << TStrategy::template sql_types<T>::type_name() << "_VAL", key); if (rd.is_valid()) rd.read_value(trg, 0);}
        template<typename T> inline typename std::enable_if<is_trivial_container<T>::value || is_trivial_map<T>::value>::type _load(std::string const& key, T& trg) const {_load_container(key,trg);}
        template<typename T> inline typename std::enable_if<is_back_pushable_container<T>::value>::type _load_container(std::string const& key, T& trg) const {typedef typename T::value_type TV;TV val;typename TStrategy::sql_reader_kv rd(*this, to_string() << key << TStrategy::template sql_types<T>::table_name(),true);if (!rd.is_valid()) return; trg.clear(); do {rd.read_value(val, 0);trg.push_back(val);} while(rd.next());}
        template<typename T> inline typename std::enable_if<is_set<T>::value>::type _load_container(std::string const& key, T& trg) const {typedef typename T::value_type TV;TV val;typename TStrategy::sql_reader_kv rd(*this,to_string() << key << TStrategy::template sql_types<T>::table_name(),true);if (!rd.is_valid()) return; trg.clear(); do{rd.read_value(val, 0);trg.insert(val);}while(rd.next());}
        template<typename T> inline typename std::enable_if<is_trivial_map<T>::value>::type _load_container(std::string const& key, T& trg) const {typedef typename T::key_type _TKey;typedef typename T::mapped_type _TValue;_TKey kv;_TValue val;typename TStrategy::sql_reader_kv rd(*this,to_string() << key << TStrategy::template sql_types<T>::table_name(), false);if (!rd.is_valid()) return;trg.clear();do {rd.read_value(kv,0);rd.read_value(val,1);trg.insert({kv,val});}while(rd.next());}
        template<typename T> inline typename std::enable_if<is_kind_of_array<T>::value>::type _load_container(std::string const& key, T& trg) const {typedef typename T::value_type TV;typedef typename T::iterator TPos;TV val;typename TStrategy::sql_reader_kv rd(*this, to_string() << key << TStrategy::template sql_types<T>::table_name(),true); if (!rd.is_valid()) return; for(TPos pos = trg.begin(); pos!=trg.end(); ++pos) {rd.read_value(val, 0);*pos = val;if (!rd.next()) break;}}

#pragma mark - save
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save(std::string const& key, T const& val) {TStrategy::create_table(*this,TStrategy::template sql_types<T>::type_name()).bind_key(key).bind_value(val);}
        template<typename T> inline typename std::enable_if<is_trivial_container<T>::value>::type _save(std::string const& key, T const& val) {typename TStrategy::sql_file::transactions_lock trk(*this);try{typename TStrategy::sql_inserter_kv dst(TStrategy::create_table_for_array(*this,to_string() << key << TStrategy::template sql_types<T>::table_name(), TStrategy::template sql_types<typename T::value_type>::type_name()));for(auto const& vt : val) {dst.bind_value(vt); dst.next();}} catch(std::exception& ex){trk.rollback();throw;}}
        template<typename T> inline typename std::enable_if<is_trivial_map<T>::value>::type _save(std::string const& key, T const& val) {typedef typename T::key_type _TKey;typedef typename T::mapped_type _TValue;typename TStrategy::sql_file::transactions_lock trk(*this);try{typename TStrategy::sql_inserter_kv dst(TStrategy::create_table_for_map(*this, to_string() << key << TStrategy::template sql_types<T>::table_name(), TStrategy::template sql_types<_TKey>::type_name(),TStrategy::template sql_types<_TValue>::type_name())); for(auto const& nv : val) {dst.bind_value(nv.first,1);dst.bind_value(nv.second,2);dst.next();}} catch (std::exception&) {trk.rollback();throw;}}
        
#pragma mark - remove

        template<typename T, typename TKey> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _remove(TKey const& key, std::string const&) {TStrategy::create_statement_for_remove_value(*this,TStrategy::template sql_types<T>::type_name()).bind_key(key);}
        template<typename T, typename TKey> inline typename std::enable_if<is_trivial_map<T>::value>::type _remove(TKey const& key, std::string const& tab) {typename TStrategy::sql_file::transactions_lock trk(*this);try{TStrategy::create_statement_for_remove_in_map(*this,tab,TStrategy::template sql_types<TKey>::type_name(),TStrategy::template sql_types<typename T::mapped_type>::type_name()).bind_key(key);}catch (std::exception&){trk.rollback();throw;}}

    };
};

#endif /* sb_kv_storage_h */
