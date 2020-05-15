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
        void bind_at(void const*,void const*,data_update_context&,sql_value const&) override {};
        void bind_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        sql_value expand(void const* src) override {return _expand<TMb>(*static_cast<T const*>(src));}
        sql_value try_cast() const override {return sql_value(TMb());}
        void read(void* dst, data_update_context& cnt) override {_read<TMb>(*static_cast<T*>(dst),cnt);}
        void read_comp(void* dst, data_update_context& cnt, sql_value const& extkey) override {_read_comp<TMb>(*static_cast<T*>(dst),cnt,extkey);}
        void read_at(void*,void* root,data_update_context& cont,sql_value const& extkey, std::string const& flt) override {_read_at<TMb>(*static_cast<T*>(root),cont,extkey,flt);}
        void read_inheritance(size_t tid,void* root,data_update_context& cont,sql_value const& extkey) override {};
        void remove_at(void const* memb,void const* src,data_update_context& cont,sql_value const& extkey) override {_remove_at<TMb>(*static_cast<T const*>(src),memb,cont,extkey);};
        void remove_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        bool is_this_mem_ptr(void const* base, void const* memptr) const override {T const* chk = static_cast<T const*>(base);return &(chk->*member_)==memptr;}
        bool is_target_map() const override {return is_map<TMb>::value;}
        bool is_not_empty_container(void const* src) const override {return _is_not_empty_container<TMb>(*static_cast<T const*>(src));}

    protected:
        
#pragma mark - constructor
        
        _t_member_descriptor(std::string const& fn, TMb T::* m, e_db_index_type it, class_descriptors_ptr desc)
            : class_descriptor(typeid(typename types_selector<TMb>::type).hash_code(),is_pointer<TMb>::value,fn,it)
            , member_(m)
            , description_(desc)
            {}
        
    private:
        
#pragma mark - check for empty containers
        
        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value ||
                                                              is_map<TFn>::value,bool>::type _is_not_empty_container(T const& el) const
        {
            return !(el.*member_).empty();
        }

        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value &&
                                                              !is_map<TFn>::value,bool>::type _is_not_empty_container(T const& el) const
        {
            return false;
        }

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

        template<typename TFn> inline typename std::enable_if<is_optional_or_trivial<TFn>::value ||
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
        template<typename TFn> inline typename std::enable_if<!is_optional_or_trivial<TFn>::value &&
                                                              !is_container<TFn>::value &&
                                                              !is_map<TFn>::value, class_descriptors_container const&>::type _members() const
        {
            return description_->members();
        }


#pragma mark - sql types
        
        template<typename TFn> inline typename std::enable_if<is_optional_or_trivial<TFn>::value,std::string const&>::type _sql_type() const
        {
            typedef typename TStrategy::template sql_types<TFn> type;
            return type::template type_name<TFn>();
        };
        template<typename TFn> inline typename std::enable_if<!is_optional_or_trivial<TFn>::value,std::string const&>::type _sql_type() const
        {
            static std::string const def;
            return def;
        };
        
#pragma mark - remove at

        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _remove_at(T const&, void const*, data_update_context&,sql_value const&) {};
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _remove_at(T const& el, void const* memb, data_update_context& dst,sql_value const& extkey)
        {
            size_t elemt = types_selector<TFn>::destination_id();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->remove_rel_by_key(extkey);
        }
        
#pragma mark - bind
        
        template<typename TFn> inline typename std::enable_if<is_optional_or_trivial<TFn>::value>::type _bind_elem(T const& el, data_update_context& dst) {dst.add(sql_value(el.*member_));}
        template<typename TFn> inline typename std::enable_if<!is_optional_or_trivial<TFn>::value>::type _bind_elem(T const& el, data_update_context& dst) {}
        template<typename TFn> inline typename std::enable_if<is_optional_or_trivial<TFn>::value>::type _bind_comp_elem(T const& el, data_update_context& dst, sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_optional_or_trivial<TFn>::value>::type _bind_comp_elem(T const& el, data_update_context& dst, sql_value const& extkey) {_bind_comp<TMb>(el.*member_,dst,extkey);}

        template<typename TFn> inline typename std::enable_if<is_trivial_container<TFn>::value || is_trivial_map<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->bind_comp(&el, extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value && !is_trivial_container<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey) {_bind_comp_cont<TFn>(el,dst,extkey);}

        template<typename TFn> inline typename std::enable_if<!is_container_of_containers<TFn>::value && is_pointer<typename TFn::value_type>::value>::type _bind_comp_cont(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = types_selector<TFn>::destination_id();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));

            if (description_->used_pointers() && description_->has_unique_key())
            {
                for(auto const& ve : el)
                {
                    sql_value key = dst.id_for_members(&(*ve));
                    ncnt->add(key);
                    ncnt->add(extkey);
                    ncnt->next(&key);
                }
                data_update_context_ptr excnt(dst.context_from_root(elemt,"",range()));
                for(auto const& ve : el)
                    excnt->bind_comp(&(*ve), sql_value());
            }
            else
            {
                if (dst.use_pages())
                {
                    typedef typename TFn::const_iterator iterator;
                    iterator ve = el.begin();
                    for(size_t i=0; i!=dst.page().length() && ve!=el.end(); ++i,ve++)
                        ncnt->bind_comp(&(*(*ve)), extkey);
                    dst.page().disable();
                }
                else
                {
                    for(auto const& ve : el)
                        ncnt->bind_comp(&(*ve), extkey);
                }
            }
        }
        
        template<typename TFn> inline typename std::enable_if<!is_container_of_containers<TFn>::value && !is_pointer<typename TFn::value_type>::value>::type _bind_comp_cont(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            typedef typename TFn::value_type type;
            typedef typename TFn::const_iterator iterator;
            size_t elemt = typeid(type).hash_code();
            if (dst.use_pages())
            {
                if (dst.page().is_active())
                {
                    data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
                    iterator ve = el.begin();
                    for(size_t i=0; i!=dst.page().length() && ve!=el.end(); ++i,ve++)
                        ncnt->bind_comp(&(*ve), extkey);
                    dst.page().disable();
                }
            }
            else
            {
                data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
                for(auto const& ve : el)
                    ncnt->bind_comp(&ve, extkey);
            }
        }

        template<typename TFn> inline typename std::enable_if<is_container_of_containers<TFn>::value>::type _bind_comp_cont(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->bind_comp(&el, extkey);
        }
        
        template<typename TFn> inline typename std::enable_if<is_map<TFn>::value && !is_trivial_map<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey) {_bind_comp_map<TFn>(el,dst,extkey);}

        template<typename TFn> inline typename std::enable_if<is_container_of_containers<TFn>::value>::type _bind_comp_map(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->bind_comp(&el, extkey);
        }

        template<typename TFn> inline typename std::enable_if<!is_container_of_containers<TFn>::value &&
                                                              !is_pointer<typename TFn::mapped_type>::value>::type _bind_comp_map(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            typedef typename TFn::key_type k_type;
            typedef typename is_pointer<typename TFn::mapped_type>::type m_type;
            size_t elemt = typeid(m_type).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
            for(auto const& ve : el)
            {
                ncnt->add(ve.first);
                ncnt->bind_comp(&ve.second, extkey);
            }
        }

        template<typename TFn> inline typename std::enable_if<!is_container_of_containers<TFn>::value &&
                                                              is_pointer<typename TFn::mapped_type>::value>::type _bind_comp_map(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            typedef typename TFn::key_type k_type;
            typedef typename is_pointer<typename TFn::mapped_type>::type m_type;
            size_t elemt = typeid(m_type).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
            if (description_->used_pointers() && description_->has_unique_key())
            {
                data_update_context_ptr excnt(dst.context_from_root(elemt,"",range()));
                for(auto const& ve : el)
                {
                    sql_value key = excnt->id_for_members(&(*ve.second));
                    ncnt->add(ve.first);
                    ncnt->add(key);
                    ncnt->add(extkey);
                    ncnt->next(nullptr);
                }
                for(auto const& ve : el)
                    excnt->bind_comp(&(*ve.second), sql_value());
            }
            else
            {
                for(auto const& ve : el)
                {
                    ncnt->add(ve.first);
                    ncnt->bind_comp(&(*ve.second), extkey);
                }
            }
        }

        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value &&
                                                              !is_map<TFn>::value>::type _bind_comp(TFn const& el, data_update_context& dst, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(dst.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->bind_comp(&el, extkey);
        }

#pragma mark - expand
        
        template<typename TFn> inline typename std::enable_if<is_optional_or_trivial<TFn>::value,sql_value>::type _expand(T const& el) {return sql_value(el.*member_);}
        template<typename TFn> inline typename std::enable_if<!is_optional_or_trivial<TFn>::value,sql_value>::type _expand(T const& el) {return sql_value();}

#pragma mark - read at

        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value &&
                                                              !is_any_map<TFn>::value>::type _read_at(T& dst, data_update_context& cont, sql_value const& extkey, std::string const&) {_read_comp<TFn>(dst,cont,extkey);}
        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value>::type _read_at(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            if (flt.empty())
                _read_comp<TFn>(dst,cont,extkey);
            else
                _read_at_cont<TFn>(dst,cont,extkey,flt);
        }
        template<typename TFn> inline typename std::enable_if<is_any_map<TFn>::value>::type _read_at(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            if (flt.empty())
                _read_comp<TFn>(dst,cont,extkey);
            else
                _read_at_map<TFn>(dst,cont,extkey,flt);
        }
        template<typename TFn> inline typename std::enable_if<is_trivial_container<TFn>::value>::type _read_at_cont(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            size_t elemt = typeid(TFn).hash_code();
            _clear(dst.*member_);
            data_update_context_ptr ncnt(cont.context_for_filtered_member(elemt,extkey,field_name(),flt));
            ncnt->read_comp(&(dst.*member_), extkey);
        }
        template<typename TFn> inline typename std::enable_if<!is_trivial_container<TFn>::value &&
                                                              !is_pointer<typename TFn::value_type>::value>::type _read_at_cont(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            typedef typename TFn::value_type type;
            size_t elemt = types_selector<TFn>::destination_id();
            _clear(dst.*member_);
            data_update_context_ptr ncnt(cont.context_for_filtered_member(elemt,extkey,field_name(),flt));
            while(ncnt->is_ok())
            {
                type var;
                ncnt->read_comp(&var, extkey);
                add_to_container(dst.*member_, std::move(var));
                cont.read_counter_inc();
            }
        }
        template<typename TFn> inline typename std::enable_if<!is_trivial_container<TFn>::value &&
                                                              is_pointer<typename TFn::value_type>::value>::type _read_at_cont(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            typedef typename TFn::value_type type;
            typedef std::conditional_t<std::is_pointer<type>::value, std::unique_ptr<typename is_pointer<type>::type>, type> obj_type;
            size_t elemt = types_selector<TFn>::destination_id();
            _clear(dst.*member_);
            data_update_context_ptr ncnt(cont.context_for_filtered_member(elemt,extkey,field_name(),flt));
            if (description_->used_pointers() && description_->has_unique_key())
            {
                sql_values_container ids_container;
                sql_value key = description_->sql_value_for_unique_key();
                to_string fld;
                while(ncnt->is_ok())
                {
                    ncnt->read(key);
                    ncnt->next(nullptr);
                    ids_container.push_back(key);
                    fld << "?,";
                }
                if (ids_container.empty()) return;
                fld.remove_from_tail(1);
                std::string filter = to_string()
                    << TStrategy::sql_where()
                    << TStrategy::sql_where_in(description_->field_name_for_unique_key(),fld);
                data_update_context_ptr elmcnt(cont.context_from_root(elemt,filter,range()));
                for(auto const& kv : ids_container)
                    elmcnt->add(kv);
                while(elmcnt->is_ok())
                {
                    obj_type var(allocate_object<type>());
                    elmcnt->read_comp(&(*var), sql_value());
                    add_to_container(dst.*member_, std::move(var));
                    cont.read_counter_inc();
                }
            }
            else
            {
                while(ncnt->is_ok())
                {
                    obj_type var(allocate_object<type>());
                    ncnt->read_comp(&(*var), extkey);
                    add_to_container(dst.*member_, std::move(var));
                }
            }
        }
        template<typename TFn> inline typename std::enable_if<is_trivial_map<TFn>::value>::type _read_at_map(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            size_t elemt = typeid(TFn).hash_code();
            _clear(dst.*member_);
            data_update_context_ptr ncnt(cont.context_for_filtered_member(elemt,extkey,field_name(),flt));
            ncnt->read_comp(&(dst.*member_), extkey);
        }
        template<typename TFn> inline typename std::enable_if<!is_trivial_map<TFn>::value &&
                                                              !is_pointer<typename TFn::mapped_type>::value>::type _read_at_map(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            typedef typename TFn::key_type k_type;
            typedef typename TFn::mapped_type m_type;
            size_t elemt = types_selector<TFn>::destination_id();
            if (cont.use_pages() && !(dst.*member_).empty()) return;
            (dst.*member_).clear();
            data_update_context_ptr ncnt(cont.context_for_filtered_member(elemt,extkey,field_name(),flt));
            sql_value key((k_type()));
            while(ncnt->is_ok())
            {
                m_type var;
                ncnt->read(key);
                ncnt->read_comp(&var, extkey);
                add_to_map(dst.*member_, key.value<k_type>(), std::move(var));
                cont.read_counter_inc();
            }
        }
        template<typename TFn> inline typename std::enable_if<!is_trivial_map<TFn>::value &&
                                                              is_pointer<typename TFn::mapped_type>::value>::type _read_at_map(T& dst, data_update_context& cont, sql_value const& extkey, std::string const& flt)
        {
            typedef typename TFn::key_type k_type;
            typedef typename TFn::mapped_type m_type;
            typedef std::conditional_t<std::is_pointer<m_type>::value, std::unique_ptr<typename is_pointer<m_type>::type>, m_type> obj_type;

            size_t elemt = types_selector<TFn>::destination_id();
            (dst.*member_).clear();
            data_update_context_ptr ncnt(cont.context_for_filtered_member(elemt,extkey,field_name(),flt));
            sql_value key((k_type()));
            if (description_->used_pointers() && description_->has_unique_key())
            {
                sql_values_map ids_container;
                sql_value sbkey = description_->sql_value_for_unique_key();
                to_string fld;
                while(ncnt->is_ok())
                {
                    ncnt->read(key);
                    ncnt->read(sbkey);
                    ncnt->next(nullptr);
                    ids_container.insert({sbkey,key});
                    fld << "?,";
                }
                if (ids_container.empty()) return;
                fld.remove_from_tail(1);
                std::string filter = to_string()
                    << TStrategy::sql_where()
                    << TStrategy::sql_where_in(description_->field_name_for_unique_key(),fld);
                data_update_context_ptr elmcnt(cont.context_from_root(elemt,filter,range()));
                for(auto const& kv : ids_container)
                    elmcnt->add(kv.first);
                while(elmcnt->is_ok())
                {
                    obj_type var(allocate_object<m_type>());
                    elmcnt->read_comp(&(*var), sql_value());
                    sql_value kv = elmcnt->id_for_members(&(*var));
                    add_to_map(dst.*member_, ids_container.find(kv)->second.value<k_type>(), std::move(var));
                    cont.read_counter_inc();
                }
            }
            else
            {
                while(ncnt->is_ok())
                {
                    obj_type var(allocate_object<m_type>());
                    ncnt->read(key);
                    ncnt->read_comp(&(*var), extkey);
                    add_to_map(dst.*member_, key.value<k_type>(), std::move(var));
                    cont.read_counter_inc();
                }
            }
        }


#pragma mark - read
        
        template<typename TFn> inline typename std::enable_if<!is_optional_or_trivial<TFn>::value>::type _read(T& dst, data_update_context& cont) {}
        template<typename TFn> inline typename std::enable_if<is_optional_or_trivial<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<is_optional_or_trivial<TFn>::value>::type _read(T& dst, data_update_context& cont) {_read_trivial<TFn>(dst,cont);}
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _read_trivial(T& dst, data_update_context& cont)
        {
            sql_value vr(dst.*member_);
            cont.read(vr);
            dst.*member_ = vr.value<TMb>();
        }
        template<typename TFn> inline typename std::enable_if<is_kind_of_optional<TFn>::value>::type _read_trivial(T& dst, data_update_context& cont)
        {
            sql_value vr((dst.*member_).value());
            cont.read(vr);
            if (vr.empty())
                dst.*member_ = TMb();
            else
                dst.*member_ = vr.value<typename TMb::value_type>();
        }

        template<typename TFn> inline typename std::enable_if<!is_optional_or_trivial<TFn>::value &&
                                                              !is_container<TFn>::value && !is_map<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->read_comp(&(dst.*member_), extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_container_of_containers<TFn>::value &&
                                                              !is_trivial_container<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->read_comp(&(dst.*member_), extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_trivial_container<TFn>::value || is_trivial_map<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            _clear(dst.*member_);
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),range()));
            ncnt->read_comp(&(dst.*member_), extkey);
        }

        template<typename TFn> inline typename std::enable_if<is_map<TFn>::value &&
                                                              !is_trivial_map<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey) {_read_comp_map<TFn>(dst,cont,extkey);}

        template<typename TFn> inline typename std::enable_if<is_pointer<typename TFn::mapped_type>::value>::type _read_comp_map(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            typedef typename TFn::key_type k_type;
            typedef typename TFn::mapped_type m_type;
            typedef std::conditional_t<std::is_pointer<m_type>::value, std::unique_ptr<typename is_pointer<m_type>::type>, m_type> obj_type;

            size_t elemt = types_selector<TFn>::destination_id();
            (dst.*member_).clear();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),range()));
            sql_value key((k_type()));
            if (description_->used_pointers() && description_->has_unique_key())
            {
                sql_values_map ids_container;
                sql_value sbkey = description_->sql_value_for_unique_key();
                to_string fld;
                while(ncnt->is_ok())
                {
                    ncnt->read(key);
                    ncnt->read(sbkey);
                    ncnt->next(nullptr);
                    ids_container.insert({sbkey,key});
                    fld << "?,";
                }
                if (ids_container.empty()) return;
                fld.remove_from_tail(1);
                std::string filter = to_string()
                    << TStrategy::sql_where()
                    << TStrategy::sql_where_in(description_->field_name_for_unique_key(),fld);
                data_update_context_ptr elmcnt(cont.context_from_root(elemt,filter,range()));
                for(auto const& kv : ids_container)
                    elmcnt->add(kv.first);
                while(elmcnt->is_ok())
                {
                    obj_type var(allocate_object<m_type>());
                    elmcnt->read_comp(&(*var), sql_value());
                    sql_value kv = elmcnt->id_for_members(&(*var));
                    add_to_map(dst.*member_, ids_container.find(kv)->second.value<k_type>(), std::move(var));
                    cont.read_counter_inc();
                }
            }
            else
            {
                while(ncnt->is_ok())
                {
                    obj_type var(allocate_object<m_type>());
                    ncnt->read(key);
                    ncnt->read_comp(&(*var), extkey);
                    add_to_map(dst.*member_, key.value<k_type>(), std::move(var));
                    cont.read_counter_inc();
                }
            }
        }

        template<typename TFn> inline typename std::enable_if<!is_pointer<typename TFn::mapped_type>::value>::type _read_comp_map(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            typedef typename TFn::key_type k_type;
            typedef typename TFn::mapped_type m_type;
            size_t elemt = types_selector<TFn>::destination_id();
            if (cont.use_pages() && !(dst.*member_).empty()) return;
            (dst.*member_).clear();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),range()));
            sql_value key((k_type()));
            while(ncnt->is_ok())
            {
                m_type var;
                ncnt->read(key);
                ncnt->read_comp(&var, extkey);
                add_to_map(dst.*member_, key.value<k_type>(), std::move(var));
                cont.read_counter_inc();
            }
        }

        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value &&
                                                              !is_trivial_container<TFn>::value &&
                                                              !is_container_of_containers<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey) {_read_comp_cont<TFn>(dst,cont,extkey);}

        template<typename TFn> inline typename std::enable_if<is_kind_of_array<TFn>::value &&
                                                              !is_pointer<typename TFn::value_type>::value>::type _read_comp_cont(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            typedef typename TFn::value_type type;
            typedef typename TFn::iterator iterator;
            size_t elemt = types_selector<TFn>::destination_id();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),range()));
            iterator pos = (dst.*member_).begin();
            while(ncnt->is_ok())
            {
                if (pos==(dst.*member_).end())
                    throw sql_bridge_error(to_string() << "The table: \"" << ncnt->table_name() << "\" contains more elements than provided container",
                                           g_expand_static_recommendation);
                ncnt->read_comp(*pos, extkey);
                pos++;
            }
            if (pos!=(dst.*member_).end())
                throw sql_bridge_error(to_string() << "The table: \"" << ncnt->table_name() << "\" contains less elements than provided static container.",
                                       g_replace_static_recommendation);
        }

        template<typename TFn> inline typename std::enable_if<!is_kind_of_array<TFn>::value &&
                                                              is_pointer<typename TFn::value_type>::value>::type _read_comp_cont(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            typedef typename TFn::value_type type;
            typedef std::conditional_t<std::is_pointer<type>::value, std::unique_ptr<typename is_pointer<type>::type>, type> obj_type;
            size_t elemt = types_selector<TFn>::destination_id();
            _clear(dst.*member_);
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),range()));
            if (description_->used_pointers() && description_->has_unique_key())
            {
                sql_values_container ids_container;
                sql_value key = description_->sql_value_for_unique_key();
                to_string fld;
                while(ncnt->is_ok())
                {
                    ncnt->read(key);
                    ncnt->next(nullptr);
                    ids_container.push_back(key);
                    fld << "?,";
                }
                if (ids_container.empty()) return;
                fld.remove_from_tail(1);
                std::string filter = to_string()
                    << TStrategy::sql_where()
                    << TStrategy::sql_where_in(description_->field_name_for_unique_key(),fld);
                data_update_context_ptr elmcnt(cont.context_from_root(elemt,filter,range()));
                for(auto const& kv : ids_container)
                    elmcnt->add(kv);
                while(elmcnt->is_ok())
                {
                    obj_type var(allocate_object<type>());
                    elmcnt->read_comp(&(*var), sql_value());
                    add_to_container(dst.*member_, std::move(var));
                    cont.read_counter_inc();
                }
            }
            else
            {
                while(ncnt->is_ok())
                {
                    obj_type var(allocate_object<type>());
                    ncnt->read_comp(&(*var), extkey);
                    add_to_container(dst.*member_, std::move(var));
                }
            }
        }

        template<typename TFn> inline typename std::enable_if<!is_kind_of_array<TFn>::value &&
                                                              !is_pointer<typename TFn::value_type>::value>::type _read_comp_cont(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            typedef typename TFn::value_type type;
            size_t elemt = types_selector<TFn>::destination_id();
            range pg;
            if (cont.use_pages() && cont.page().is_active())
            {
                pg = range((dst.*member_).size(),cont.page().length());
                cont.page().disable();
            }
            else
            {
                if (cont.use_pages() && !(dst.*member_).empty()) return;
                _clear(dst.*member_);
            }
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,field_name(),pg));
            while(ncnt->is_ok())
            {
                type var;
                ncnt->read_comp(&var, extkey);
                add_to_container(dst.*member_, std::move(var));
                cont.read_counter_inc();
            }
        }

#pragma mark - clear container
        
        template<typename TFn> inline typename std::enable_if<is_kind_of_array<TFn>::value>::type _clear(TFn& el) {}
        template<typename TFn> inline typename std::enable_if<!is_kind_of_array<TFn>::value>::type _clear(TFn& el) {el.clear();}

#pragma mark - containers inserter
        
        template<typename TFn, typename TKey, typename TArg> inline typename std::enable_if<std::is_pointer<typename TFn::mapped_type>::value>::type add_to_map(TFn& ct, TKey const& k, TArg&& ar) {ct.insert(typename TFn::value_type(k,ar.release()));}
        template<typename TFn, typename TKey, typename TArg> inline typename std::enable_if<!std::is_pointer<typename TFn::mapped_type>::value>::type add_to_map(TFn& ct, TKey const& k, TArg&& ar) {ct.insert(typename TFn::value_type(k,std::move(ar)));}
        template<typename TFn, typename TArg> inline typename std::enable_if<!std::is_pointer<typename TFn::value_type>::value>::type add_to_container(TFn& ct, TArg&& ar) {_add_to_container(ct,std::move(ar));}
        template<typename TFn, typename TArg> inline typename std::enable_if<std::is_pointer<typename TFn::value_type>::value>::type add_to_container(TFn& ct, TArg&& ar) {_add_to_container_ptr(ct,std::move(ar));}
        template<typename TFn, typename TArg> inline typename std::enable_if<is_back_pushable_container<TFn>::value>::type _add_to_container(TFn& ct, TArg&& ar) {ct.push_back(std::move(ar));}
        template<typename TFn, typename TArg> inline typename std::enable_if<!is_back_pushable_container<TFn>::value>::type _add_to_container(TFn& ct, TArg&& ar) {ct.insert(ct.end(),std::move(ar));}
        template<typename TFn, typename TArg> inline typename std::enable_if<is_back_pushable_container<TFn>::value>::type _add_to_container_ptr(TFn& ct, TArg&& ar) {ct.push_back(ar.release());}
        template<typename TFn, typename TArg> inline typename std::enable_if<!is_back_pushable_container<TFn>::value>::type _add_to_container_ptr(TFn& ct, TArg&& ar) {ct.insert(ct.end(),ar.release());}

        TMb T::*member_;
        class_descriptors_ptr description_;
    };
    
};

#endif /* sb_members_descriptor_h */
