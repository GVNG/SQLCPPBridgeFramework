//
//  sb_members_descriptor.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 26/03/2016.
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
#ifndef sb_members_descriptor_h
#define sb_members_descriptor_h

#include "sb_core_misc.h"
#include "sb_foundation.h"
#include "sb_trivial_members_descriptor.h"

namespace sql_bridge
{

    template<typename TStrategy, typename T, typename TMb> class _t_member_descriptor
        : public class_descriptor
    {
    public:
#pragma mark - public methods
        inline class_descriptors_container const& members() const override {return _members<TMb>();}
        class_descriptors_pair reference_description() const override {return class_descriptors_pair(type_id(),description_);}
        inline class_descriptors_pair prefix_description() const override {return _prefix<TMb>();}
        std::string const& table_name() const override {static std::string const def;return def;}
        std::string const& sql_type() const override {return _sql_type<TMb>();}
        void bind(void const* src, data_update_context& cnt) override {_bind_elem<TMb>(*static_cast<T const*>(src),cnt);};
        void bind_comp(void const* src, data_update_context& cnt, sql_value const& extkey) override {_bind_comp_elem<TMb>(*static_cast<T const*>(src),cnt,extkey);};
        sql_value expand(void const* src) override {return _expand<TMb>(*static_cast<T const*>(src));}
        void read(void* dst, data_update_context& cnt) override {_read<TMb>(*static_cast<T*>(dst),cnt);}
        void read_comp(void* dst, data_update_context& cnt, sql_value const& extkey) override {_read_comp<TMb>(*static_cast<T*>(dst),cnt,extkey);}
        bool is_this_mem_ptr(void const* base, void const* memptr) const override
        {
            T const* chk = static_cast<T const*>(base);
            return &(chk->*member_)==memptr;
        }

    protected:
#pragma mark - constructor
        _t_member_descriptor(std::string const& fn, TMb T::* m, e_db_index_type it, class_descriptors_ptr desc)
            : class_descriptor(typeid(typename types_selector<TMb>::type).hash_code(),fn,it)
            , member_(m)
            , description_(desc)
            {}
    private:
        
#pragma mark - prefix for member
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value ||
                                                              is_container_of_containers<TFn>::value ||
                                                              is_trivial_container<TFn>::value ||
                                                              is_trivial_map<TFn>::value,class_descriptors_pair>::type _prefix() const
        {
            return empty_descriptors_pair;
        }
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value &&
                                                              !is_container<TFn>::value &&
                                                              !is_map<TFn>::value,class_descriptors_pair>::type _prefix() const
        {
            return empty_descriptors_pair;
        }
        template<typename TFn> inline typename std::enable_if<!is_container_of_containers<TFn>::value &&
                                                              is_container<TFn>::value &&
                                                              !is_trivial_container<TFn>::value,class_descriptors_pair>::type _prefix() const
        {
            return empty_descriptors_pair;
        }
        template<typename TFn> inline typename std::enable_if<!is_container_of_containers<TFn>::value &&
                                                              is_map<TFn>::value &&
                                                              !is_trivial_map<TFn>::value,class_descriptors_pair>::type _prefix() const
        {
            typedef _t_trivial_member_descriptor<TStrategy, typename TMb::key_type> k_type;
            return class_descriptors_pair(typeid(typename TMb::key_type).hash_code(),
                                          std::make_shared<k_type>(g_key_field_name,e_db_index_type::Basic));
        }

#pragma mark - members by reference
        inline class_descriptors_container _join_desc() const
        {
            class_descriptors_container ret(description_->members());
            ret.insert(ret.begin(), _prefix<TMb>().second);
            return ret;
        }

        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value ||
                                                              is_container_of_containers<TFn>::value ||
                                                              is_trivial_map<TFn>::value ||
                                                              is_trivial_container<TFn>::value, class_descriptors_container const&>::type _members() const
        {
            static const class_descriptors_container ret;
            return ret;
        }
        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value &&
                                                              !is_trivial_container<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value,class_descriptors_container const&>::type _members() const
        {
            return description_->members();
        }
        template<typename TFn> inline typename std::enable_if<is_map<TFn>::value &&
                                                              !is_trivial_map<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value,class_descriptors_container const&>::type _members() const
        {
            static const class_descriptors_container ret(_join_desc());
            return ret;
        }
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value &&
                                                              !is_container<TFn>::value &&
                                                              !is_map<TFn>::value, class_descriptors_container const&>::type _members() const
        {
            return description_->members();
        }


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
#pragma mark - bind
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _bind_elem(T const& el, data_update_context& dst) {dst.add(sql_value(el.*member_));}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _bind_elem(T const& el, data_update_context& dst) {}
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _bind_comp_elem(T const& el, data_update_context& dst, sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _bind_comp_elem(T const& el, data_update_context& dst, sql_value const& extkey) {_bind_comp<TMb>(el.*member_,dst,extkey);}

        template<typename TFn> inline typename std::enable_if<is_trivial_container<TFn>::value || is_trivial_map<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name()));
            ncnt->bind_comp(&el, extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value &&
                                                              !is_trivial_container<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            typedef typename TFn::value_type type;
            size_t elemt = typeid(type).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name()));
            for(auto const& ve : el)
                ncnt->bind_comp(&ve, extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_container_of_containers<TFn>::value &&
                                                              !is_trivial_container<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name()));
            ncnt->bind_comp(&el, extkey);
        }
        
        template<typename TFn> inline typename std::enable_if<is_map<TFn>::value &&
                                                              !is_trivial_map<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            typedef typename TFn::key_type k_type;
            typedef typename TFn::mapped_type m_type;
            size_t elemt = typeid(m_type).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name()));
            for(auto const& ve : el)
            {
                ncnt->add(ve.first);
                ncnt->bind_comp(&ve.second, extkey);
            }
        }

        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value && !is_map<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name()));
            ncnt->bind_comp(&el, extkey);
        }

#pragma mark - expand
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value,sql_value>::type _expand(T const& el) {return sql_value(el.*member_);}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value,sql_value>::type _expand(T const& el) {return sql_value();}
        
#pragma mark - read
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _read(T& dst, data_update_context& cont) {}
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _read(T& dst, data_update_context& cont)
        {
            sql_value vr(dst.*member_);
            cont.read(vr);
            dst.*member_ = vr.value<TMb>();
        }

        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value &&
                                                              !is_container<TFn>::value && !is_map<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name()));
            ncnt->read_comp(&(dst.*member_), extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_container_of_containers<TFn>::value &&
                                                              !is_trivial_container<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name()));
            ncnt->read_comp(&(dst.*member_), extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_trivial_container<TFn>::value || is_trivial_map<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            _clear(dst.*member_);
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name()));
            ncnt->read_comp(&(dst.*member_), extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_map<TFn>::value &&
                                                              !is_trivial_map<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            typedef typename TFn::key_type k_type;
            typedef typename TFn::mapped_type m_type;
            size_t elemt = typeid(m_type).hash_code();
            (dst.*member_).clear();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name()));
            sql_value key((k_type()));
            while(ncnt->is_ok())
            {
                m_type var;
                ncnt->read(key);
                ncnt->read_comp(&var, extkey);
                (dst.*member_).insert({key.value<k_type>(),var});
            }
        }

        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value &&
                                                              !is_trivial_container<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            typedef typename TFn::value_type type;
            size_t elemt = typeid(type).hash_code();
            (dst.*member_).clear();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name()));
            while(ncnt->is_ok())
            {
                type var;
                ncnt->read_comp(&var, extkey);
                (dst.*member_).push_back(var);
            }
        }

#pragma mark - clear container
        
        template<typename TFn> inline typename std::enable_if<is_kind_of_array<TFn>::value>::type _clear(TFn& el) {}
        template<typename TFn> inline typename std::enable_if<!is_kind_of_array<TFn>::value>::type _clear(TFn& el) {el.clear();}

        TMb T::*member_;
        class_descriptors_ptr description_;
    };
    
};

#endif /* sb_members_descriptor_h */
