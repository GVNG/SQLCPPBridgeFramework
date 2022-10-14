//
//  sb_context_engine.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 09/05/2020.
//  Copyright © 2020, DataArt.
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
#ifndef sb_context_engine_h
#define sb_context_engine_h

#include "sb_tasks_queue.h"
#include "sb_data_section.h"
#include "sb_stm_builder.h"


namespace sql_bridge
{
    class context_engine
    {
    public:
#pragma mark - public methods
        inline void limit(size_t count, size_t offset = 0) {_limit(range(offset,count));}
        inline void sql_or() {_sql_or();}
        inline void sql_and() {_sql_and();}

        template<typename T, typename TFn> inline void order(TFn const T::*mem_ptr) {_order<T>(mem_ptr);}
        template<typename T, typename TFn> inline void order_desc(TFn const T::*mem_ptr) {_order_desc<T>(mem_ptr);}

        template<typename T, typename TFn> inline void where(TFn const T::*mem_ptr, std::string const& op, TFn const& val) {_where<T>(mem_ptr,op,val);}
        template<typename T, typename TFn> inline void where_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {_where_between<T>(mem_ptr,from,to);}
        template<typename T, typename TFn> inline void where_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {_where_between<T>(val,mem_ptr_from,mem_ptr_to);}
        template<typename T, typename TFn> inline void where_not_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {_where_not_between<T>(mem_ptr,from,to);}
        template<typename T, typename TFn> inline void where_not_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {_where_not_between<T>(val,mem_ptr_from,mem_ptr_to);}
        template<typename T, typename TFn, typename TCont> inline void where_in(TFn const T::*mem_ptr, TCont const& cnt) {_where_in<T,TCont,TFn>(mem_ptr,cnt);}
        template<typename T, typename TFn, typename TCont> inline void where_not_in(TFn const T::*mem_ptr, TCont const& cnt) {_where_not_in<T,TCont,TFn>(mem_ptr,cnt);}
        template<typename T> inline void where_like(std::string const T::*mem_ptr, std::string const& val) {_where_like<T>(mem_ptr,val);}
        template<typename T> inline void where_not_like(std::string const T::*mem_ptr, std::string const& val) {_where_not_like<T>(mem_ptr,val);}

    private:
#pragma mark - order
        template<typename T,typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _order(TFn const T::*mem_ptr)
        {
            std::string field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_order>(field,false));
        }

        template<typename T,typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _order_desc(TFn const T::*mem_ptr)
        {
            std::string field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_order>(field,true));
        }

#pragma mark - limit

        void _limit(range rng)
        {
            suffixes_.erase(std::remove_if(suffixes_.begin(),
                                           suffixes_.end(),
                                           [](suffix_bare_ptr sfx){return sfx->weight()==e_weight::LIMIT;}),suffixes_.end());
            suffixes_.push_back(std::make_shared<suffix_limit>(rng));
        }

#pragma mark - where
                
        template<typename T, typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _where(TFn const T::*mem_ptr, std::string const& op, TFn const& val) {_where_def<T,TFn>(mem_ptr,op,val);}
        template<typename T, typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _where_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {_where_between_def<T,TFn>(mem_ptr,from,to);}
        template<typename T, typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _where_not_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {_where_not_between_def<T,TFn>(mem_ptr,from,to);}
        template<typename T, typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _where_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {_where_between_def<T,TFn>(val,mem_ptr_from,mem_ptr_to);}
        template<typename T, typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _where_not_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {_where_not_between_def<T,TFn>(val,mem_ptr_from,mem_ptr_to);}
        template<typename T, typename TCont, typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value && is_container<TCont>::value>::type _where_in(TFn const T::*mem_ptr, TCont const& inval) {_where_in_def<T,TCont,TFn>(mem_ptr,inval);}
        template<typename T, typename TCont, typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value && is_container<TCont>::value && std::is_same<typename types_selector<TCont>::type, TFn>::value>::type _where_not_in(TFn const T::*mem_ptr, TCont const& inval) {_where_not_in_def<T,TCont,TFn>(mem_ptr,inval);}
        template<typename T, typename TFn> inline typename std::enable_if<is_convertible_to_text<TFn>::value>::type _where_def(TFn const T::*mem_ptr, std::string const& op, TFn const& val)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_where>(field,to_string() << op << make_text_quotation(val)));
        }
        template<typename T, typename TFn> inline typename std::enable_if<!is_convertible_to_text<TFn>::value>::type _where_def(TFn const T::*mem_ptr, std::string const& op, TFn const& val)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_where>(field,to_string() << op << val));
        }
        
        template<typename T, typename TFn> inline typename std::enable_if<is_convertible_to_text<TFn>::value>::type _where_between_def(TFn const T::*mem_ptr, TFn const& from, TFn const& to)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_between>(field, make_text_quotation(from), make_text_quotation(to), false));
        }
        template<typename T, typename TFn> inline typename std::enable_if<!is_convertible_to_text<TFn>::value>::type _where_between_def(TFn const T::*mem_ptr, TFn const& from, TFn const& to)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_between>(field,to_string() << from, to_string() << to, false));
        }
        template<typename T, typename TFn> inline typename std::enable_if<is_convertible_to_text<TFn>::value>::type _where_not_between_def(TFn const T::*mem_ptr, TFn const& from, TFn const& to)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_between>(field,make_text_quotation(from), make_text_quotation(to), true));
        }
        template<typename T, typename TFn> inline typename std::enable_if<!is_convertible_to_text<TFn>::value>::type _where_not_between_def(TFn const T::*mem_ptr, TFn const& from, TFn const& to)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_between>(field,to_string() << from, to_string() << to, true));
        }

        template<typename T, typename TFn> inline typename std::enable_if<is_convertible_to_text<TFn>::value>::type _where_between_def(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to)
        {
            static std::string const field = data_->field_name(mem_ptr_from);
            static std::string const field2 = data_->field_name(mem_ptr_to);
            suffixes_.push_back(std::make_shared<suffix_between_rev>(field,field2,make_text_quotation(val), false));
        }
        template<typename T, typename TFn> inline typename std::enable_if<!is_convertible_to_text<TFn>::value>::type _where_between_def(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to)
        {
            static std::string const field = data_->field_name(mem_ptr_from);
            static std::string const field2 = data_->field_name(mem_ptr_to);
            suffixes_.push_back(std::make_shared<suffix_between_rev>(field,field2,to_string() << val, false));
        }
        template<typename T, typename TFn> inline typename std::enable_if<is_convertible_to_text<TFn>::value>::type _where_not_between_def(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to)
        {
            static std::string const field = data_->field_name(mem_ptr_from);
            static std::string const field2 = data_->field_name(mem_ptr_to);
            suffixes_.push_back(std::make_shared<suffix_between_rev>(field,field2,make_text_quotation(val), true));
        }
        template<typename T, typename TFn> inline typename std::enable_if<!is_convertible_to_text<TFn>::value>::type _where_not_between_def(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to)
        {
            static std::string const field = data_->field_name(mem_ptr_from);
            static std::string const field2 = data_->field_name(mem_ptr_to);
            suffixes_.push_back(std::make_shared<suffix_between_rev>(field,field2,to_string() << val, true));
        }
        template<typename T, typename TCont, typename TFn> inline typename std::enable_if<is_convertible_to_text<typename types_selector<TCont>::type>::value>::type _where_in_def(TFn const T::*mem_ptr, TCont const& from)
        {
            to_string ts;
            static std::string const field = data_->field_name(mem_ptr);
            for(auto const& v : from)
                ts << make_text_quotation(v) << ",";
            ts.remove_from_tail(1);
            ts << " ";
            suffixes_.push_back(std::make_shared<suffix_where_in>(field,ts,false));
        }
        template<typename T, typename TCont, typename TFn> inline typename std::enable_if<!is_convertible_to_text<typename types_selector<TCont>::type>::value>::type _where_in_def(TFn const T::*mem_ptr, TCont const& from)
        {
            to_string ts;
            static std::string const field = data_->field_name(mem_ptr);
            for(auto const& v : from)
                ts << v << ",";
            ts.remove_from_tail(1);
            ts << " ";
            suffixes_.push_back(std::make_shared<suffix_where_in>(field,ts,false));
        }
        template<typename T, typename TCont, typename TFn> inline typename std::enable_if<is_convertible_to_text<typename types_selector<TCont>::type>::value>::type _where_not_in_def(TFn const T::*mem_ptr, TCont const& from)
        {
            to_string ts;
            static std::string const field = data_->field_name(mem_ptr);
            for(auto const& v : from)
                ts << make_text_quotation(v) << ",";
            ts.remove_from_tail(1);
            ts << " ";
            suffixes_.push_back(std::make_shared<suffix_where_in>(field,ts,true));
        }
        template<typename T, typename TCont, typename TFn> inline typename std::enable_if<!is_convertible_to_text<typename types_selector<TCont>::type>::value>::type _where_not_in_def(TFn const T::*mem_ptr, TCont const& from)
        {
            to_string ts;
            static std::string const field = data_->field_name(mem_ptr);
            for(auto const& v : from)
                ts << v << ",";
            ts.remove_from_tail(1);
            ts << " ";
            suffixes_.push_back(std::make_shared<suffix_where_in>(field,ts,false));
        }
        template<typename T> inline void _where_like(std::string const T::*mem_ptr, std::string const& substr)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_where_like>(field,substr,false));
        }
        template<typename T> inline void _where_not_like(std::string const T::*mem_ptr, std::string const& substr)
        {
            static std::string const field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_where_like>(field,substr,true));
        }

#pragma mark - and/or
                
        void _sql_or() {suffixes_.push_back(std::make_shared<suffix_simple_operator>(e_simple_operator::OR));}
        void _sql_and() {suffixes_.push_back(std::make_shared<suffix_simple_operator>(e_simple_operator::AND));}

    protected:
        
#pragma mark - constructors
        
        context_engine(db_tasks_queue_interface_ptr q, data_sections_ptr ds)
            : queue_(q)
            , data_(ds)
            {}
        context_engine(context_engine&& src)
        {
            std::swap(queue_, src.queue_);
            std::swap(data_, src.data_);
            std::swap(suffixes_, src.suffixes_);
        }
        
    protected:
#pragma mark - members
        // members
        db_tasks_queue_interface_weak_ptr queue_;
        data_sections_ptr data_;
        suffixes_container suffixes_;
        
        std::string build_suffix(std::string const& usr, bool blockwhere = false)
        {
            if (usr.empty())
            {
                to_string ret;
                std::stable_sort(suffixes_.begin(),
                                 suffixes_.end(),
                                 [](suffix_bare_ptr lv,suffix_bare_ptr rv){return *lv < *rv;});
                e_weight cw = e_weight::BASE;
                bool repeat = false;
                for(auto const& sfx : suffixes_)
                {
                    if (sfx->weight()!=cw)
                    {
                        if (sfx->weight()==e_weight::WHERE && blockwhere)
                        {
                            suffix_simple_operator andop(e_simple_operator::AND);
                            ret << " " << andop.build(data_) << " " << sfx->build(data_);
                        }
                        else
                            ret << " " << sfx->general(data_) << " " << sfx->build(data_);
                        repeat = false;
                        cw = sfx->weight();
                    }
                    else
                    {
                        if (repeat) ret << sfx->repeat_delimiter();
                        repeat = true;
                        ret << " " << sfx->build(data_);
                    }
                }
                suffixes_.clear();
                return ret;
            }
            else
            {
                suffixes_.clear();
                return usr;
            }
        }

        static std::string make_text_quotation(std::string const& src)
        {
            if (src.find('\'')==std::string::npos)
                return to_string() << "'" << src << "'";
            else
            {
                to_string ret;
                ret << "'";
                for(auto const& c : src)
                {
                    if (c=='\'') ret << c;
                    ret << c;
                }
                ret << "'";
                return ret;
            }
        }
    };
};
#endif /* sb_context_engine_h */
