//
//  sb_data_section_descriptor.h
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
#ifndef sb_data_section_descriptor_h
#define sb_data_section_descriptor_h

#include "sb_core.h"
#include "sb_foundation.h"
#include "sb_class_descriptor.h"
#include "sb_links.h"

namespace sql_bridge
{
    
    class data_section_descriptors_bare;
    using data_section_descriptors_map = std::unordered_map<std::string,data_section_descriptors_ptr>;
    using protected_data_section_descriptors_map = protected_section<data_section_descriptors_map>;
    using init_routine = std::function<void()>;
    using init_queue = std::queue<init_routine>;
    
    class data_section_descriptors
    {
    public:
        data_section_descriptors_ptr operator [](std::string const& name);
        void add_data_section(std::string const& name, data_section_descriptors_ptr sect);
        void add_init(init_routine fn) {init_queue_.push(std::move(fn));}
        static data_section_descriptors& instance();
    private:
        data_section_descriptors() {};
        void proc_init_queue();
        // data
        protected_data_section_descriptors_map index_;
        init_queue init_queue_;
    };

    template<typename TStrategy, typename TNamesSelector, typename... T> class _t_data_section_descriptor
        : public data_section_descriptor
    {
    public:
        
        static data_section_descriptors_ptr create(std::string const& name, size_t ver)
        {
            struct make_shared_enabler : public _t_data_section_descriptor {make_shared_enabler(std::string const& name, size_t ver) : _t_data_section_descriptor(name,ver){}};
            std::shared_ptr<_t_data_section_descriptor> ret(std::make_shared<make_shared_enabler>(name,ver));
            _init<T...>(*ret);
            _check_inheritance<T...>(*ret);
            class_links_container rel;
            using long_sql = typename TStrategy::template sql_types<long long>;
            using string_sql = typename TStrategy::template sql_types<std::string>;
            ret->prepare_relations(rel,
                                   long_sql::type_name(),
                                   string_sql::type_name());
            TStrategy::create_statements(rel);
            ret->dump_statements(rel);
            ret->apply_links(rel);
            return ret;
        }

    private:

#pragma mark - check inheritance
        
        template<typename... Ts> inline static void _check_inheritance(_t_data_section_descriptor& inst) {inst._check_inheritance<Ts...>(std::integral_constant<bool, sizeof...(Ts)<=2>());}
        template<typename TFs, typename TSc, typename... Ts> inline void _check_inheritance(std::false_type)
        {
            _check_inheritance<TFs,TSc>(std::true_type());
            _check_inheritance<TFs,Ts...>(std::integral_constant<bool, sizeof...(Ts)==1>());
            _check_inheritance<TSc,Ts...>(std::integral_constant<bool, sizeof...(Ts)==1>());
        }
        template<typename TFs, typename TSc> inline void _check_inheritance(std::true_type) {_proc_inheritance<TFs,TSc>(std::integral_constant<bool, std::is_base_of<TSc, TFs>::value && !std::is_same<TFs, TSc>::value>());}
        template<typename TFs> inline void _check_inheritance(std::true_type) {};
        template<typename TFs, typename TSc> inline void _proc_inheritance(std::false_type) {};
        template<typename TFs, typename TSc> inline void _proc_inheritance(std::true_type)
        {
            class_descriptors_map::iterator fspos = classes_map_.find(typeid(TFs).hash_code());
            class_descriptors_map::iterator scpos = classes_map_.find(typeid(TSc).hash_code());
            if (fspos==classes_map_.end() || scpos==classes_map_.end())
                throw sql_bridge_error(g_internal_error_text,g_architecture_error_text);
            class_descriptors_ptr desc = _t_class_descriptor<TStrategy,TFs>::template create_inheritance<TSc>(scpos->second);
            fspos->second->update_for_inheritances(desc);
        };

#pragma mark - init
        
        template<typename... Ts> inline static void _init(_t_data_section_descriptor& inst) {inst._init<Ts...>(std::integral_constant<bool, sizeof...(Ts)==1>());}
        template<typename TPar> inline void _init(std::true_type) {_register_class<TPar>();}
        template<typename TPar, typename... Ts> inline void _init(std::false_type)
        {
            _init<TPar>(std::true_type());
            _init<Ts...>(std::integral_constant<bool,sizeof...(Ts)==1>());
        }

#pragma mark - register classes
        
        template<typename TFn> inline typename std::enable_if<is_sql_acceptable<TFn>::value>::type _register_class() {};
        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value || is_map<TFn>::value>::type _register_class() {_register_container<TFn>();}
        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value && !is_map<TFn>::value && !is_sql_acceptable<TFn>::value>::type _register_class()
        {
            class_descriptors_ptr desc = _t_class_descriptor<TStrategy,TFn>::template create_description<TFn>();
            if (!desc)
                throw sql_bridge_error(g_internal_error_text,g_architecture_error_text);
            size_t code = typeid(TFn).hash_code();
            classes_map_.insert({code,desc});
            for(auto const& m : desc->members())
            {
                class_descriptors_pair pr(m->reference_description());
                if (pr.second!=nullptr)
                    classes_map_.insert(pr);
            }
        }
        
        template<typename TFn> inline typename std::enable_if<is_trivial_container<TFn>::value || is_trivial_map<TFn>::value>::type _register_container()
        {
            using type = _t_container_descriptor<TStrategy,TFn>;
            size_t code = typeid(TFn).hash_code();
            classes_map_.insert({code,std::make_shared<type>()});
        }
        
        template<typename TFn> inline typename std::enable_if<!is_trivial_container<TFn>::value && !is_trivial_map<TFn>::value>::type _register_container()
        {
            class_descriptors_ptr desc = _t_class_descriptor<TStrategy,TFn>::template create_description<TFn>();
            if (!desc)
                throw sql_bridge_error(g_internal_error_text,g_architecture_error_text);
            size_t code = typeid(TFn).hash_code();
            classes_map_.insert({code,desc});
        }
        
#pragma mark - misc

        _t_data_section_descriptor(std::string const& name, size_t ver)
            : data_section_descriptor(name,ver)
            {};
        
        struct updater
            : TStrategy::sql_file
            , TStrategy::sql_file::transactions_lock
        {
            updater(std::string const& fname,string_container const& drps)
                : TStrategy::sql_file(fname)
                , TStrategy::sql_file::transactions_lock((typename TStrategy::sql_file const&)*this)
                , drop_statements_(drps)
                {};
            void upgrade_structure(size_t from, size_t to);
            inline void drop_all()
            {
                for(auto const& cmd : drop_statements_)
                    TStrategy::sql_file::execute(cmd);
            }
            string_container const& drop_statements_;
        };

        void update(std::string const& fname, size_t from, size_t to) const
        {
            string_container rmsttm;
            get_drop_all_statements(rmsttm);
            updater fl(fname,rmsttm);
            fl.upgrade_structure(from, to);
        }
        
        struct sql_proc
            : TStrategy::sql_file
            , TStrategy::sql_file::transactions_lock
        {
            sql_proc(std::string const& fname)
                : TStrategy::sql_file(fname)
                , TStrategy::sql_file::transactions_lock((typename TStrategy::sql_file const&)*this)
                {};
        };
        
        void create_tables(std::string const& fname) const
        {
            sql_proc fl(fname);
            string_container crstm;
            get_create_statements(crstm);
            for(auto const& st : crstm)
                if (!st.empty())
                    fl.execute(st);
        }
    };
    
    template<typename TStrategy, typename TNamesSelector, typename ...T> struct _t_data_section_descriptors_creator
    {
        using type = _t_data_section_descriptor<TStrategy,TNamesSelector,T...>;
        _t_data_section_descriptors_creator(std::string const& name, size_t ver)
        {
            std::string locname(name);
            data_section_descriptors::instance().add_init([locname,ver]()
            {
                try
                {
                    data_section_descriptors_ptr desc = type::create(locname, ver);
                    data_section_descriptors::instance().add_data_section(locname, desc);
                }
                catch(std::exception& ex)
                {
                    std::cerr << "*** Exception: " << ex.what() << std::endl;
                    throw;
                }
            });
        }
    };
    
};


#endif /* sb_data_section_descriptor_h */
