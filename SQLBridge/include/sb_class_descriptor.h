//
//  sb_class_descriptor.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 14/03/2016.
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
#ifndef sb_class_descriptor_h
#define sb_class_descriptor_h

#include "sb_traits.h"
#include "sb_foundation.h"
#include "sb_members_descriptor.h"
#include "sb_inheritance_descriptor.h"
#include "sb_trivial_members_descriptor.h"
#include "sb_container_descriptor.h"

namespace sql_bridge
{
    template<typename TStrategy, typename T> class _t_class_descriptor
        : public class_descriptor
    {
    private:
        static std::string const table_name_;
        static class_descriptors_container const members_;
        
    public:
        _t_class_descriptor(bool used_pointers = false)
            : class_descriptor(typeid(T).hash_code(),used_pointers)
            {}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-var-template"
        virtual class_descriptors_container const& members() const override {return members_;}
        std::string const& table_name() const override {return table_name_;}
#pragma clang diagnostic pop // And unhide them again!
        class_descriptors_pair reference_description() const override {return empty_descriptors_pair;}
        class_descriptors_pair prefix_description() const override {return empty_descriptors_pair;}
        std::string const& sql_type() const override {static std::string const def; return def;}
        void bind(void const* val, data_update_context& cont) override {}
        void bind_comp(void const* val, data_update_context& cont, sql_value const& extkey) override {_bind_comp<T>(*static_cast<T const*>(val),cont,extkey);}
        void bind_at(void const* memb,void const* root,data_update_context& cont,sql_value const& extkey) override {_bind_at<T>(*static_cast<T const*>(root),memb,cont,extkey);}
        void bind_inheritance(size_t tid,void const* root,data_update_context& cont,sql_value const& extkey) override {_bind_inheritance<T>(*static_cast<T const*>(root),tid,cont,extkey);}
        sql_value expand(void const*) override {return sql_value();}
        sql_value try_cast() const override {return _try_cast<T>();}
        void read(void* dst,data_update_context& cont) override {_read<T>(*static_cast<T*>(dst),cont);}
        void read_comp(void* dst,data_update_context& cont,sql_value const& extkey) override {_read_comp<T>(*static_cast<T*>(dst),cont,extkey);}
        void read_at(void* dst,void* root,data_update_context& cont,sql_value const& extkey) override {_read_at<T>(*static_cast<T*>(root),dst,cont,extkey);};
        void read_inheritance(size_t tid,void* root,data_update_context& cont,sql_value const& extkey) override {_read_inheritance<T>(*static_cast<T*>(root),tid,cont,extkey);};
        void remove_at(void const* dst,void const* root,data_update_context& cont,sql_value const& extkey) override {_remove_at<T>(*static_cast<T const*>(root),dst,cont,extkey);};
        void remove_inheritance(size_t tid,void const* root,data_update_context& cont,sql_value const& extkey) override {_remove_inheritance<T>(*static_cast<T const*>(root),tid,cont,extkey);};
        bool is_this_mem_ptr(void const* base, void const* memptr) const override {return false;}
        bool is_target_map() const override {return is_map<T>::value;}
        bool is_not_empty_container(void const* src) const override {return _is_not_empty_container<T>(*static_cast<T const*>(src));}

        template<typename TFn> static class_descriptors_ptr create_description() {return _create_description_pub<TFn>();}
        template<typename TFn> static class_descriptors_ptr create_inheritance(class_descriptors_ptr desc)
        {
            typedef _t_inheritance_descriptor<TStrategy, T, TFn> type;
            struct make_shared_enabler : public type {make_shared_enabler(class_descriptors_ptr desc):type(desc){}};
            std::shared_ptr<type> ret(std::make_shared<make_shared_enabler>(desc));
            return ret;
        }
        
    private:
        template<typename TMb, typename TCl> inline static typename std::enable_if<std::is_base_of<TCl, T>::value,class_descriptors_ptr>::type bind(std::string const& fn, TMb TCl::* m, e_db_index_type it = e_db_index_type::None)
        {
            typedef _t_member_descriptor<TStrategy,TCl,TMb> type;
            struct make_shared_enabler : public type {make_shared_enabler(std::string const& fn, TMb TCl::* m, e_db_index_type it, class_descriptors_ptr desc) : type(fn,m,it,desc){}};
            std::shared_ptr<type> ret(std::make_shared<make_shared_enabler>(fn,m,it,_create_description<TMb>()));
            return ret;
        }
        
#pragma mark - check for empty containers
        
        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value ||
                                                              is_map<TFn>::value,bool>::type _is_not_empty_container(T const& el) const
        {
            return !el.empty();
        }

        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value &&
                                                              !is_map<TFn>::value,bool>::type _is_not_empty_container(T const& el) const
        {
            return false;
        }

#pragma mark - try cast
        
        template<typename TFn> inline static typename std::enable_if<is_sql_acceptable<TFn>::value,sql_value>::type _try_cast() {return sql_value(TFn());}
        template<typename TFn> inline static typename std::enable_if<!is_sql_acceptable<TFn>::value,sql_value>::type _try_cast() {return sql_value();}
        
#pragma mark - create description
        
        template<typename TFn> inline static typename std::enable_if<is_optional_or_trivial<TFn>::value,class_descriptors_ptr>::type _create_description() {return class_descriptors_ptr();}
        template<typename TFn> inline static typename std::enable_if<is_optional_or_trivial<TFn>::value,class_descriptors_ptr>::type _create_description_pub() {return class_descriptors_ptr();}
        template<typename TFn> inline static typename std::enable_if<is_container<TFn>::value || is_map<TFn>::value,class_descriptors_ptr>::type _create_description() {return _create_containers_description<TFn>();}
        template<typename TFn> inline static typename std::enable_if<!is_trivial_container<TFn>::value && !is_trivial_map<TFn>::value && !is_container_of_containers<TFn>::value,class_descriptors_ptr>::type _create_containers_description() {return _create_nt_containers_description<TFn>();}
        template<typename TFn> inline static typename std::enable_if<is_container<TFn>::value || is_any_map<TFn>::value,class_descriptors_ptr>::type _create_description_pub()
        {
            typedef _t_container_descriptor<TStrategy,TFn> type;
            return std::make_shared<type>();
        }
        
        template<typename TFn> inline static typename std::enable_if<!is_optional_or_trivial<TFn>::value && !is_container<TFn>::value && !is_map<TFn>::value,class_descriptors_ptr>::type _create_description_pub()
        {
            typedef _t_class_descriptor<TStrategy,TFn> type;
            return std::make_shared<type>();
        }
        
        template<typename TFn> inline static typename std::enable_if<!is_optional_or_trivial<TFn>::value && !is_container<TFn>::value && !is_map<TFn>::value,class_descriptors_ptr>::type _create_description()
        {
            typedef _t_class_descriptor<TStrategy,TFn> type;
            return std::make_shared<type>();
        }
        
        template<typename TFn> inline static typename std::enable_if<is_trivial_container<TFn>::value ||
                                                                     is_trivial_map<TFn>::value ||
                                                                     is_container_of_containers<TFn>::value,class_descriptors_ptr>::type _create_containers_description()
        {
            typedef _t_container_descriptor<TStrategy,TFn> type;
            return std::make_shared<type>();
        }
        
        template<typename TFn> inline static typename std::enable_if<is_container<TFn>::value &&
                                                                     is_pointer<typename TFn::value_type>::value &&
                                                                     !is_container_of_containers<TFn>::value,class_descriptors_ptr>::type _create_nt_containers_description()
        {
            typedef _t_class_descriptor<TStrategy, typename is_pointer<typename TFn::value_type>::type> type;
            return std::make_shared<type>(true);
        }
        
        template<typename TFn> inline static typename std::enable_if<is_container<TFn>::value &&
                                                                     !is_pointer<typename TFn::value_type>::value &&
                                                                     !is_container_of_containers<TFn>::value,class_descriptors_ptr>::type _create_nt_containers_description()
        {
            typedef _t_class_descriptor<TStrategy, typename TFn::value_type> type;
            return std::make_shared<type>();
        }

        template<typename TFn> inline static typename std::enable_if<is_any_map<TFn>::value &&
                                                                     is_pointer<typename TFn::mapped_type>::value &&
                                                                     !is_container_of_containers<TFn>::value, class_descriptors_ptr>::type _create_nt_containers_description()
        {
            typedef typename TFn::key_type key;
            typedef typename is_pointer<typename TFn::mapped_type>::type mapped;
            static_assert(is_sql_acceptable<key>::value, "The key of the maps-like containers must be trivial");
            typedef _t_class_descriptor<TStrategy,mapped> type;
            return std::make_shared<type>(true);
        }

        template<typename TFn> inline static typename std::enable_if<is_any_map<TFn>::value &&
                                                                     !is_pointer<typename TFn::mapped_type>::value &&
                                                                     !is_container_of_containers<TFn>::value, class_descriptors_ptr>::type _create_nt_containers_description()
        {
            typedef typename TFn::key_type key;
            typedef typename TFn::mapped_type mapped;
            static_assert(is_sql_acceptable<key>::value, "The key of the maps-like containers must be trivial");
            typedef _t_class_descriptor<TStrategy,mapped> type;
            return std::make_shared<type>();
        }

#pragma mark - bind inheritance

        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _bind_inheritance(TFn const&,size_t,data_update_context&,sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _bind_inheritance(TFn const& el,size_t tid,data_update_context& cont,sql_value const& extkey)
        {
            for(auto const& inh : cont.inheritances())
                if (inh->type_id()==tid)
                    inh->bind_comp(&el, cont, extkey);
        }

#pragma mark - bind at
        
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _bind_at(TFn const&,void const*,data_update_context&,sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _bind_at(TFn const& el,void const* memb,data_update_context& cont,sql_value const& extkey)
        {
            for(auto const& md : cont.members())
                if (md->is_this_mem_ptr(&el, memb))
                    md->bind_comp(&el, cont, extkey);
        }

#pragma mark - bind
        
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _bind_comp(TFn const&,data_update_context&,sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _bind_comp(TFn const& el,data_update_context& cont,sql_value const& extkey)
        {
            if (cont.use_pages())
            {
                sql_value uid = cont.id_for_members(&el);
                if (uid.empty()) return;
                for(auto const& md : cont.members())
                    if (md->index_type()!=e_db_index_type::PrimaryKey)
                        md->bind_comp(&el, cont, uid);
                cont.page().enable();
                return;
            }
            cont.check_for_update_ability(&el);
            cont.remove_if_possible(&el);
            for(auto const& md : cont.members())
                if (md->index_type()!=e_db_index_type::PrimaryKey)
                    md->bind(&el, cont);
            if (!extkey.empty())
                cont.add(extkey);
            cont.next(&el);
            sql_value uid = cont.id_for_members(&el);
            if (uid.empty()) return;
            for(auto const& inh : cont.inheritances())
                inh->bind_comp(&el, cont, uid);
            for(auto const& md : cont.members())
                if (md->index_type()!=e_db_index_type::PrimaryKey)
                    md->bind_comp(&el, cont, uid);
        }

#pragma mark - read inheritance

        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _read_inheritance(T& dst, size_t tid, data_update_context& cont, sql_value const& extkey) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _read_inheritance(T& dst, size_t tid, data_update_context& cont, sql_value const& extkey)
        {
            for(auto const& inh : cont.inheritances())
                if (inh->type_id()==tid)
                    inh->read_comp(&dst, cont, extkey);
        }

#pragma mark - read at

        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _read_at(T& dst, void* memb, data_update_context& cont, sql_value const& extkey) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _read_at(T& dst, void* memb, data_update_context& cont, sql_value const& extkey)
        {
            for(auto const& md : cont.members())
                if (md->is_this_mem_ptr(&dst, memb))
                    md->read_comp(&dst, cont, extkey);
        }
        
#pragma mark - read
        
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _read(T&, data_update_context&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _read(T& dst, data_update_context& cont) {_read_comp<TFn>(dst,cont,sql_value());}
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _read_comp(T&, data_update_context&, sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _read_comp(T& dst, data_update_context& cont, sql_value const& extkey)
        {
            cont.read_counter_inc();
            cont.page().enable();
            if (!cont.use_pages() || !has_not_empty_members(&dst))
            {
                for(auto const& md : cont.members())
                    md->read(&dst, cont);
            }
            cont.next(nullptr);
            sql_value uid = cont.id_for_members(&dst);
            if (!uid.empty())
            {
                for(auto const& inh : cont.inheritances())
                    inh->read_comp(&dst, cont, uid);
                for(auto const& md : cont.members())
                    md->read_comp(&dst, cont, uid);
            }
        }

#pragma mark - remove at
        
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _remove_at(TFn const&,void const*,data_update_context&,sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _remove_at(TFn const& el,void const* memb,data_update_context& cont,sql_value const& extkey)
        {
            for(auto const& md : cont.members())
                if (md->is_this_mem_ptr(&el, memb))
                    md->remove_at(memb, &el, cont, extkey);
        }

#pragma mark - remove inheritance
        
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _remove_inheritance(TFn const&,void const*,data_update_context&,sql_value const&) {}
        template<typename TFn> inline typename std::enable_if<!is_sql_acceptable<TFn>::value>::type _remove_inheritance(TFn const& el,size_t tid,data_update_context& cont,sql_value const& extkey)
        {
            for(auto const& inh : cont.inheritances())
                if (inh->type_id()==tid)
                    inh->remove_inheritance(tid, &el, cont, extkey);
        }

    };
        
};

#endif /* sb_class_descriptor_h */
