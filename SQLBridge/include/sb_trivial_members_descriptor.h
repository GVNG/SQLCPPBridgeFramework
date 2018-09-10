//
//  sb_trivial_members_descriptor.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 09/05/2016.
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
#ifndef sb_trivial_members_descriptor_h
#define sb_trivial_members_descriptor_h

#include "sb_foundation.h"

namespace sql_bridge
{
    template<typename TStrategy, typename T> class _t_link_member_descriptor
        : public class_descriptor
    {
    public:
        _t_link_member_descriptor()
            : class_descriptor(0,"",e_db_index_type::None)
            {}

        class_descriptors_container const& members() const {static const class_descriptors_container def;return def;}
        class_descriptors_pair reference_description() const {return class_descriptors_pair(typeid(T).hash_code(),class_descriptors_ptr());}
        class_descriptors_pair prefix_description() const {return empty_descriptors_pair;}
        std::string const& table_name() const {static std::string const def;return def;}
        std::string const& sql_type() const {static std::string const def;return def;}
        void bind(void const*,data_update_context&) {};
        void bind_comp(void const*,data_update_context&,sql_value const&) {};
        sql_value expand(void const*) {return sql_value();}
        void read(void*,data_update_context&) {}
        void read_comp(void* dst,data_update_context& cont,sql_value const& extkey) {};
    private:
    };
    
    template<typename TStrategy, typename T> class _t_trivial_member_descriptor
        : public class_descriptor
    {
    public:
        _t_trivial_member_descriptor(std::string const& fn, e_db_index_type idt = e_db_index_type::None)
            : class_descriptor(0,fn,idt)
            {}
        
        class_descriptors_container const& members() const {static const class_descriptors_container def;return def;}
        class_descriptors_pair reference_description() const {return empty_descriptors_pair;}
        class_descriptors_pair prefix_description() const {return empty_descriptors_pair;}
        std::string const& table_name() const {static std::string const def;return def;}
        std::string const& sql_type() const {return _sql_type<T>();}
        void bind(void const*,data_update_context&) {};
        void bind_comp(void const*,data_update_context&,sql_value const&) {};
        sql_value expand(void const*) {return sql_value();}
        void read(void*,data_update_context&) {}
        void read_comp(void* dst,data_update_context& cont,sql_value const& extkey) {};

    private:
#pragma mark - sql types
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value,std::string const&>::type _sql_type() const
        {
            typedef typename TStrategy::template sql_types<TFn> type;
            return type::template type_name<TFn>();
        };
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value,std::string const&>::type _sql_type() const
        {
            static std::string const def;
            return def;
        };
    };
}

#endif /* sb_trivial_members_descriptor_h */
