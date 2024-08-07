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
            : class_descriptor(0,false,"",e_db_index_type::None)
            {}

        class_descriptors_container const& members() const override {static const class_descriptors_container def;return def;}
        class_descriptors_pair reference_description() const override {return class_descriptors_pair(typeid(T).hash_code(),class_descriptors_ptr());}
        class_descriptors_pair prefix_description() const override {return empty_descriptors_pair;}
        std::string const& table_name() const override {static std::string const def;return def;}
        std::string const& sql_type() const override {static std::string const def;return def;}
        void bind(void const*,data_update_context&) override {};
        void bind_comp(void const*,data_update_context&,sql_value const&) override {};
        void bind_at(void const*,void const*,data_update_context&,sql_value const&) override {};
        void bind_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        sql_value expand(void const*) override {return sql_value();}
        sql_value try_cast() const override {return sql_value();}
        void read(void*,data_update_context&) override {}
        void read_comp(void* dst,data_update_context& cont,sql_value const& extkey) override {};
        void read_at(void* dst,void* root,data_update_context& cont,sql_value const& extkey, std::string const&) override {};
        void read_inheritance(size_t tid,void* root,data_update_context& cont,sql_value const& extkey) override {};
        void remove_at(void const*,void const*,data_update_context&,sql_value const&) override {};
        void remove_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        bool is_this_mem_ptr(void const* base, void const* memptr) const override {return false;}
        bool is_target_map() const override {return is_map<T>::value;}
        bool is_not_empty_container(void const*) const override {return false;}
    private:
    };
    
    template<typename TStrategy, typename T> class _t_trivial_member_descriptor
        : public class_descriptor
    {
    public:
        _t_trivial_member_descriptor(std::string const& fn, e_db_index_type idt = e_db_index_type::None)
            : class_descriptor(0,false,fn,idt)
            {}
        
        class_descriptors_container const& members() const override {static const class_descriptors_container def;return def;}
        class_descriptors_pair reference_description() const override {return empty_descriptors_pair;}
        class_descriptors_pair prefix_description() const override {return empty_descriptors_pair;}
        std::string const& table_name() const override {static std::string const def;return def;}
        std::string const& sql_type() const override {return _sql_type<T>();}
        void bind(void const*,data_update_context&) override {};
        void bind_comp(void const*,data_update_context&,sql_value const&) override {};
        void bind_at(void const*,void const*,data_update_context&,sql_value const&) override {};
        void bind_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        sql_value expand(void const* dat) override {return sql_value(*static_cast<T const*>(dat));}
        sql_value try_cast() const override {return sql_value();}
        void read(void*,data_update_context&) override {}
        void read_comp(void* dst,data_update_context& cont,sql_value const& extkey) override {};
        void read_at(void* dst,void* root,data_update_context& cont,sql_value const& extkey, std::string const&) override {};
        void read_inheritance(size_t tid,void* root,data_update_context& cont,sql_value const& extkey) override {};
        void remove_at(void const*,void const*,data_update_context&,sql_value const&) override {};
        void remove_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        bool is_this_mem_ptr(void const* base, void const* memptr) const override {return false;}
        bool is_target_map() const override {return is_map<T>::value;}
        bool is_not_empty_container(void const*) const override {return false;}

    private:
#pragma mark - sql types
        template<typename TFn> inline std::enable_if_t<is_optional_or_trivial<TFn>::value,std::string const&> _sql_type() const
        {
            using type = typename TStrategy::template sql_types<TFn>;
            return type::template type_name<TFn>();
        };
        template<typename TFn> inline std::enable_if_t<!is_optional_or_trivial<TFn>::value,std::string const&> _sql_type() const
        {
            static std::string const def;
            return def;
        };
    };
}

#endif /* sb_trivial_members_descriptor_h */
