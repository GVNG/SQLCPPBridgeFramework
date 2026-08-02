//
//  sb_container_descriptor.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 22/05/2016.
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
#ifndef sb_container_descriptor_h
#define sb_container_descriptor_h

#include "sb_core.h"
#include "sb_foundation.h"
#include "sb_members_descriptor.h"
#include "sb_inheritance_descriptor.h"
#include "sb_trivial_members_descriptor.h"

namespace sql_bridge
{

    template<typename TStrategy, typename T> class _t_container_descriptor
        : public class_descriptor
    {
    private:
        static std::string const table_name_;
        static class_descriptors_container const members_;
        
    public:
        _t_container_descriptor()
            : class_descriptor(typeid(T).hash_code(),false)
            {}
        
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-var-template"
        class_descriptors_container const& members() const override {return members_;}
        std::string const& table_name() const override {return table_name_;}
#pragma clang diagnostic pop // And unhide them again!

        class_descriptors_pair reference_description() const override {return empty_descriptors_pair;}
        class_descriptors_pair prefix_description() const override {return empty_descriptors_pair;}
        std::string const& sql_type() const override {static std::string const def; return def;}
        void bind(void const*,data_update_context&) override {};
        void bind_comp(void const* src,data_update_context& cont,sql_value const& extkey) override {_bind_comp<T>(*static_cast<T const*>(src),cont,extkey);};
        void bind_at(void const*,void const*,data_update_context&,sql_value const&) override {};
        void bind_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        sql_value expand(void const*) override {return sql_value();}
        sql_value try_cast() const override {return sql_value();}
        void read(void* dst,data_update_context& cont) override {read_comp(dst,cont,sql_value());}
        void read_comp(void* dst,data_update_context& cont,sql_value const& extkey) override {_read_comp<T>(*static_cast<T*>(dst),cont,extkey);};
        void read_at(void* dst,void* root,data_update_context& cont,sql_value const& extkey, std::string const&) override {};
        void read_inheritance(size_t tid,void* root,data_update_context& cont,sql_value const& extkey) override {};
        void remove_at(void const*,void const*,data_update_context&,sql_value const&) override {};
        void remove_inheritance(size_t,void const*,data_update_context&,sql_value const&) override {};
        bool is_this_mem_ptr(void const* base, void const* memptr) const override {return false;}
        bool is_target_map() const override {return is_map<T>::value;}
        bool is_not_empty_container(void const* src) const override {return !static_cast<T const*>(src)->empty();}

        inline static class_descriptors_container create_members() {return _create_members<T>();}
    private:
        // methods
        
#pragma mark - create members

        template<typename TFn> inline static class_descriptors_container _create_members()
        {
            if constexpr (is_container<TFn>::value)
            {
                if constexpr (is_trivial_container<TFn>::value)
                {
                    using type = _t_trivial_member_descriptor<TStrategy, typename TFn::value_type>;
                    class_descriptors_container ret = {std::make_shared<type>(g_value_field_name),};
                    return ret;
                }
                else
                if constexpr (is_pointer<typename TFn::value_type>::value)
                {
                    using type = _t_link_member_descriptor<TStrategy, typename is_pointer<typename TFn::value_type>::type>;
                    class_descriptors_container ret = {std::make_shared<type>(),};
                    return ret;
                }
                else
                {
                    using type = _t_link_member_descriptor<TStrategy, typename TFn::value_type>;
                    class_descriptors_container ret = {std::make_shared<type>(),};
                    return ret;
                }
            }
            else
            if constexpr (is_any_map<TFn>::value)
            {
                using k_src_type = typename TFn::key_type;
                using vk_type = _t_trivial_member_descriptor<TStrategy, int64_t>;
                if constexpr (!is_sql_acceptable<k_src_type>::value)
                {
                    using k_type = _t_link_member_descriptor<TStrategy, k_src_type>;
                    using m_src_type = typename TFn::mapped_type;
                    if constexpr (is_sql_acceptable<m_src_type>::value)
                    {
                        using m_type = _t_trivial_member_descriptor<TStrategy, m_src_type>;
                        class_descriptors_container ret =
                        {
                            std::make_shared<vk_type>(g_key_field_name,_index_type_for_map<TFn>()),
                            std::make_shared<k_type>(),
                            std::make_shared<m_type>(g_value_field_name),
                        };
                        return ret;
                    }
                    else
                    {
                        using m_type = _t_link_member_descriptor<TStrategy, m_src_type>;
                        class_descriptors_container ret =
                        {
                            std::make_shared<vk_type>(g_key_field_name,_index_type_for_map<TFn>()),
                            std::make_shared<k_type>(),
                            std::make_shared<m_type>(),
                        };
                        return ret;
                    }
                }
                else
                if constexpr (is_trivial_map<TFn>::value)
                {
                    using k_type = _t_trivial_member_descriptor<TStrategy, k_src_type>;
                    using m_type = _t_trivial_member_descriptor<TStrategy, typename TFn::mapped_type>;
                    class_descriptors_container ret =
                    {
                        std::make_shared<k_type>(g_key_field_name,_index_type_for_map<TFn>()),
                        std::make_shared<m_type>(g_value_field_name),
                    };
                    return ret;
                }
                else
                if constexpr (is_pointer<typename TFn::mapped_type>::value)
                {
                    using k_type = _t_trivial_member_descriptor<TStrategy, k_src_type>;
                    using m_type = _t_link_member_descriptor<TStrategy, typename is_pointer<typename TFn::mapped_type>::type>;
                    class_descriptors_container ret =
                    {
                        std::make_shared<k_type>(g_key_field_name,_index_type_for_map<TFn>()),
                        std::make_shared<m_type>(),
                    };
                    return ret;
                }
                else
                {
                    using k_type = _t_trivial_member_descriptor<TStrategy, k_src_type>;
                    using m_type = _t_link_member_descriptor<TStrategy, typename TFn::mapped_type>;
                    class_descriptors_container ret =
                    {
                        std::make_shared<k_type>(g_key_field_name,_index_type_for_map<TFn>()),
                        std::make_shared<m_type>(),
                    };
                    return ret;
                }
            }
            else
                static_assert(false, "There is an unspecified routine for this container");
        }

        template<typename TFn> inline static e_db_index_type _index_type_for_map()
        {
            using k_type = typename TFn::key_type;
            if constexpr (is_multimap<TFn>::value)
                return e_db_index_type::Basic;
            else
            if constexpr (is_sql_acceptable<k_type>::value)
                return e_db_index_type::Unique;
            else
                return e_db_index_type::PrimaryKey;
        }

#pragma mark - bind

        template<typename TFn> inline void _bind_comp(TFn const& src,
                                                      data_update_context& cont,
                                                      sql_value const& extkey)
        {
            if constexpr (is_container<TFn>::value)
            {
                if constexpr (is_trivial_container<TFn>::value)
                {
                    if constexpr (is_vector_bool<TFn>::value)
                    {
                        for(auto v : src)
                        {
                            bool t = v;
                            cont.add(sql_value(t));
                            if (!extkey.empty())
                                cont.add(extkey);
                            cont.next(&t);
                        }
                    }
                    else
                    {
                        for(auto const& v : src)
                        {
                            cont.add(sql_value(v));
                            if (!extkey.empty())
                                cont.add(extkey);
                            cont.next(&v);
                        }
                    }
                }
                else
                if constexpr (is_pointer<typename TFn::value_type>::value)
                {
                    size_t tid = typeid(typename is_pointer<typename TFn::value_type>::type).hash_code();
                    std::string const& refname(cont.forward_ref());
                    for(auto const& v : src)
                    {
                        if (!extkey.empty())
                            cont.add(extkey);
                        cont.next(&v);
                        sql_value extid = cont.id_for_members(&v);
                        if (extid.empty())
                            throw sql_bridge_error(to_string() << "Table: " << table_name() << ". The undefined field for the forward link", "You should configure any type of index at least at one field in the definition of table");
                        data_update_context_ptr ncnt(cont.context_for_member(tid, extid, refname, range()));
                        ncnt->bind_comp(&(*v), extid);
                    }
                }
                else
                {
                    size_t tid = typeid(typename TFn::value_type).hash_code();
                    std::string const& refname(cont.forward_ref());
                    for(auto const& v : src)
                    {
                        if (!extkey.empty())
                            cont.add(extkey);
                        cont.next(&v);
                        sql_value extid = cont.id_for_members(&v);
                        if (extid.empty())
                            throw sql_bridge_error(to_string() << "Table: " << table_name() << ". The undefined field for the forward link", "You should configure any type of index at least at one field in the definition of table");
                        data_update_context_ptr ncnt(cont.context_for_member(tid, extid, refname, range()));
                        ncnt->bind_comp(&v, extid);
                    }
                }
            }
            else
            if constexpr (is_any_map<TFn>::value)
            {
                if constexpr (is_trivial_map<TFn>::value)
                {
                    for(auto const& v : src)
                    {
                        cont.add(sql_value(v.first));
                        cont.add(sql_value(v.second));
                        if (!extkey.empty())
                            cont.add(extkey);
                        cont.next(&v);
                    }
                }
                else
                if constexpr (is_sql_acceptable<typename TFn::key_type>::value)
                {
                    if constexpr (is_pointer<typename TFn::mapped_type>::value)
                    {
                        size_t tid = typeid(typename is_pointer<typename TFn::mapped_type>::type).hash_code();
                        std::string const& refname(cont.forward_ref());
                        for(auto const& v : src)
                        {
                            cont.add(sql_value(v.first));
                            if (!extkey.empty())
                                cont.add(extkey);
                            cont.next(&v);
                            sql_value extid = cont.id_for_members(&v);
                            if (extid.empty())
                                throw sql_bridge_error(to_string() << "Table: " << table_name() << ". The undefined field for the key", "You should configure any type of index at least at one field in the definition of table");
                            data_update_context_ptr ncnt(cont.context_for_member(tid, extid, refname, range()));
                            ncnt->bind_comp(&(*v.second), extid);
                        }
                    }
                    else
                    {
                        size_t tid = typeid(typename TFn::mapped_type).hash_code();
                        std::string const& refname(cont.forward_ref());
                        for(auto const& v : src)
                        {
                            cont.add(sql_value(v.first));
                            if (!extkey.empty())
                                cont.add(extkey);
                            cont.next(&v);
                            sql_value extid = cont.id_for_members(&v);
                            if (extid.empty())
                                throw sql_bridge_error(to_string() << "Table: " << table_name() << ". The undefined field for the key", "You should configure any type of index at least at one field in the definition of table");
                            data_update_context_ptr ncnt(cont.context_for_member(tid, extid, refname, range()));
                            ncnt->bind_comp(&v.second, extid);
                        }
                    }
                }
                else
                {
                    size_t fsid = typeid(typename TFn::key_type).hash_code();
                    if constexpr (is_sql_acceptable<typename TFn::mapped_type>::value)
                    {
                        for(auto const& v : src)
                        {
                            cont.add(sql_value(v.second));
                            if (!extkey.empty())
                                cont.add(extkey);
                            cont.next(&v);
                            sql_value extid = cont.id_for_members(&v);
                            if (extid.empty())
                                throw sql_bridge_error(to_string() << "Table: " << table_name() << ". The undefined field for the forward link", "You should configure any type of index at least at one field in the definition of table");
                            data_update_context_ptr fccnt(cont.context_for_member(fsid, extid, cont.forward_ref(), range()));
                            fccnt->bind_comp(&v.first, extid);
                        }
                    }
                    else
                    {
                        size_t scid = typeid(typename TFn::mapped_type).hash_code();
                        for(auto const& v : src)
                        {
                            if (!extkey.empty())
                                cont.add(extkey);
                            cont.next(&v);
                            sql_value extid = cont.id_for_members(&v);
                            if (extid.empty())
                                throw sql_bridge_error(to_string() << "Table: " << table_name() << ". The undefined field for the forward link", "You should configure any type of index at least at one field in the definition of table");
                            data_update_context_ptr fccnt(cont.context_for_member(fsid, extid, cont.forward_ref(), range()));
                            data_update_context_ptr sccnt(cont.context_for_member(scid, extid, cont.forward_ref_alt(), range()));
                            fccnt->bind_comp(&v.first, extid);
                            sccnt->bind_comp(&v.second, extid);
                        }
                    }
                }
            }
        }
        
#pragma mark - read

        template<typename TFn> inline void _read_comp(TFn& dst,
                                                      data_update_context& cont,
                                                      sql_value const& extkey)
        {
            if constexpr (is_kind_of_array<TFn>::value)
            {
                using type = typename TFn::value_type;
                using iterator = typename TFn::iterator;
                sql_value val((type()));
                iterator pos = dst.begin();
                while(cont.is_ok())
                {
                    if (pos==dst.end())
                        throw sql_bridge_error(to_string() << "The table: \"" << cont.table_name() << "\" contains more elements than provided container",
                                               g_expand_static_recommendation);
                    cont.read(val);
                    cont.next(nullptr);
                    *pos = val.value<type>();
                    pos++;
                }
                if (pos!=dst.end())
                    throw sql_bridge_error(to_string() << "The table: \"" << cont.table_name() << "\" contains less elements than provided static container.",
                                           g_replace_static_recommendation);
            }
            else
            if constexpr (is_trivial_container<TFn>::value)
            {
                using type = typename TFn::value_type;
                sql_value val((type()));
                while(cont.is_ok())
                {
                    cont.read(val);
                    cont.next(nullptr);
                    add_to_container(dst,val.value<type>());
                }
            }
            else
            if constexpr (is_trivial_map<TFn>::value)
            {
                using k_type = typename TFn::key_type;
                using m_type = typename TFn::mapped_type;
                sql_value key((k_type())),val((m_type()));
                while(cont.is_ok())
                {
                    cont.read(key);
                    cont.read(val);
                    cont.next(nullptr);
                    dst.insert(typename TFn::value_type(key.value<k_type>(),val.value<m_type>()));
                }
            }
            else
            if constexpr (is_container<TFn>::value)
            {
                if constexpr (is_pointer<typename TFn::value_type>::value)
                {
                    using type = typename is_pointer<typename TFn::value_type>::type;
                    using obj_type = std::conditional_t<std::is_pointer<typename TFn::value_type>::value, std::unique_ptr<type>, typename TFn::value_type>;
                    size_t tid = typeid(type).hash_code();
                    std::string const& refname(cont.forward_ref());
                    while(cont.is_ok())
                    {
                        cont.next(nullptr);
                        sql_value extid = cont.id_for_members(&dst);
                        data_update_context_ptr ncnt(cont.context_for_member(tid, extid, refname, range()));
                        obj_type v(allocate_object<typename TFn::value_type>());
                        ncnt->read_comp(&(*v), extid);
                        add_to_container(dst, std::move(v));
                    }
                }
                else
                {
                    using type = typename TFn::value_type;
                    size_t tid = typeid(type).hash_code();
                    std::string const& refname(cont.forward_ref());
                    while(cont.is_ok())
                    {
                        cont.next(nullptr);
                        sql_value extid = cont.id_for_members(&dst);
                        data_update_context_ptr ncnt(cont.context_for_member(tid, extid, refname, range()));
                        type v;
                        ncnt->read_comp(&v, extid);
                        add_to_container(dst, std::move(v));
                    }
                }
            }
            else
            if constexpr (is_map<TFn>::value)
            {
                using k_type = typename TFn::key_type;
                if constexpr (!is_sql_acceptable<k_type>::value)
                {
                    using m_type = typename TFn::mapped_type;
                    size_t ktid = typeid(k_type).hash_code();
                    if constexpr (is_sql_acceptable<m_type>::value)
                    {
                        std::string const& refname(cont.forward_ref());
                        sql_value key((int64_t(0))),dat((m_type()));
                        while(cont.is_ok())
                        {
                            cont.read(key);
                            cont.read(dat);
                            cont.next(nullptr);
                            data_update_context_ptr ncnt(cont.context_for_member(ktid, key, refname, range()));
                            k_type mapkey;
                            ncnt->read_comp(&mapkey, key);
                            dst.insert({std::move(mapkey),dat.value<m_type>()});
                        }
                    }
                    else
                    {
                        size_t vltid = typeid(m_type).hash_code();
                        sql_value key((int64_t(0)));
                        while(cont.is_ok())
                        {
                            cont.read(key);
                            cont.next(nullptr);
                            data_update_context_ptr fdcnt(cont.context_for_member(ktid, key, cont.forward_ref(), range()));
                            data_update_context_ptr sdcnt(cont.context_for_member(vltid, key, cont.forward_ref_alt(), range()));
                            k_type mapkey;
                            m_type mapdat;
                            fdcnt->read_comp(&mapkey, key);
                            sdcnt->read_comp(&mapdat, key);
                            dst.insert({std::move(mapkey),std::move(mapdat)});
                        }
                    }
                }
                else
                if constexpr (is_pointer<typename TFn::mapped_type>::value)
                {
                    using m_type = typename is_pointer<typename TFn::mapped_type>::type;
                    using obj_type = std::conditional_t<std::is_pointer<typename TFn::mapped_type>::value, std::unique_ptr<m_type>, typename TFn::mapped_type>;
                    sql_value key((k_type()));
                    size_t tid = typeid(m_type).hash_code();
                    std::string const& refname(cont.forward_ref());
                    while(cont.is_ok())
                    {
                        cont.read(key);
                        cont.next(nullptr);
                        sql_value extid = cont.id_for_members(&dst);
                        if (extid.empty())
                            throw sql_bridge_error(to_string() << "Table: " << table_name() << ". The undefined field for the key", "You should configure any type of index at least at one field in the definition of table");
                        obj_type v(allocate_object<typename TFn::mapped_type>());
                        data_update_context_ptr ncnt(cont.context_for_member(tid, extid, refname, range()));
                        ncnt->read_comp(&(*v), extid);
                        dst.insert(typename TFn::value_type(key.value<k_type>(),std::move(v)));
                    }
                }
                else
                {
                    using m_type = typename TFn::mapped_type;
                    sql_value key((k_type()));
                    size_t tid = typeid(m_type).hash_code();
                    std::string const& refname(cont.forward_ref());
                    while(cont.is_ok())
                    {
                        cont.read(key);
                        cont.next(nullptr);
                        sql_value extid = cont.id_for_read_members(&dst);
                        m_type v;
                        data_update_context_ptr ncnt(cont.context_for_member(tid, extid.empty()?key:extid, refname, range()));
                        ncnt->read_comp(&v, extid.empty()?key:extid);
                        dst.insert(typename TFn::value_type(key.value<k_type>(),std::move(v)));
                    }
                }
            }
        }

#pragma mark - add to container
        
        template<typename TFn, typename TVal> inline std::enable_if_t<is_back_pushable_container<TFn>::value> add_to_container(TFn& dst, TVal&& v) const {dst.push_back(std::move(v));}
        template<typename TFn, typename TVal> inline std::enable_if_t<!is_back_pushable_container<TFn>::value> add_to_container(TFn& dst, TVal&& v) const {dst.insert(dst.end(),std::move(v));}
    };
};

#endif /* sb_container_descriptor_h */
