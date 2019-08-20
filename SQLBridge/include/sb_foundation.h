//
//  sb_foundation.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 28/03/2016.
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
#ifndef sb_foundation_h
#define sb_foundation_h

#include "sb_core.h"
#include "sb_exceptions.h"
#include "sb_links.h"
#include "sb_value.h"

namespace sql_bridge
{
    class data_section_descriptor;
    class class_descriptor;
    class member_descriptor;
    class data_update_context;
    typedef std::shared_ptr<data_section_descriptor> data_section_descriptors_ptr;
    typedef std::shared_ptr<class_descriptor> class_descriptors_ptr;
    typedef std::map<size_t,class_descriptors_ptr> class_descriptors_map;
    typedef class_descriptors_map::value_type class_descriptors_pair;
    typedef std::vector<class_descriptors_ptr> class_descriptors_container;
    typedef std::unique_ptr<data_update_context> data_update_context_ptr;

    static class_descriptors_pair const empty_descriptors_pair = class_descriptors_pair(0,class_descriptors_ptr());

    class data_section_descriptor
    {
        friend class class_registrator;
    public:
        data_section_descriptor(std::string const& name, size_t ver)
            : version_(ver)
            , section_name_(name)
            {}
        virtual ~data_section_descriptor() {};
        virtual void update(std::string const& fname, size_t from, size_t to) const = 0;
        virtual void create_tables(std::string const& fname) const = 0;
        void prepare_relations(class_links_container&,std::string const&,std::string const&) const;
        void dump_statements(class_links_container const&) const;
        void apply_links(class_links_container const&);

        void get_create_statements(string_container&) const;
        
        template<typename TFn> bool has_description() const
        {
            static bool const ret = classes_map_.find(typeid(TFn).hash_code())!=classes_map_.end();
            return ret;
        }
        inline size_t version() const {return version_;}
        inline std::string const& section_name() const {return section_name_;}
        inline class_descriptors_ptr operator [](size_t tid) const
        {
            class_descriptors_map::const_iterator pos = classes_map_.find(tid);
            if (pos==classes_map_.end())
                throw sql_bridge_error(to_string() << "Section: " << section_name() << ". There're incomplete definition of tables","You should check the set of yours DEFINE_SQL_TABLE macroses");
            return pos->second;
        }
    protected:
        // methods
        member_for_index_ref lookup_for_key_mode(class_link&,std::string const&,std::string const&,size_t_set) const;
        void dump_statements(class_link const&,std::string const&) const;
        void get_create_statements(class_link const&,string_container&) const;
        // members
        size_t version_;
        std::string section_name_;
        class_descriptors_map classes_map_;
    };

    class class_descriptor
    {
    public:
        virtual ~class_descriptor() {};
        
        virtual class_descriptors_container const& members() const  = 0;
        virtual class_descriptors_pair reference_description() const = 0;
        virtual class_descriptors_pair prefix_description() const = 0;
        virtual std::string const& table_name() const = 0;
        virtual std::string const& sql_type() const = 0;
        virtual void bind(void const*,data_update_context&) = 0;
        virtual void bind_comp(void const*,data_update_context&,sql_value const&) = 0;
        virtual sql_value expand(void const*) = 0;
        virtual void read(void*,data_update_context&) = 0;
        virtual void read_comp(void*,data_update_context&,sql_value const&) = 0;
        virtual bool is_this_mem_ptr(void const*,void const*) const = 0;
        virtual bool is_target_map() const = 0;

        inline size_t type_id() const {return type_id_;}
        inline e_db_index_type index_type() const {return index_type_;}
        inline std::string const& field_name() const {return field_name_;}
        inline class_link const& depends() const {return depends_;}
        inline class_descriptors_container const& inheritances() const {return inheritances_;}
        bool has_child(size_t ch) const;

        inline void update_for_depends(class_link const& cl)
        {
            if (!depends_.empty())
                throw sql_bridge_error(g_internal_error_text,g_architecture_error_text);
            depends_ = cl;
        }
        inline void update_for_inheritances(class_descriptors_ptr cl) {inheritances_.push_back(cl);}
        
    protected:
        class_descriptor(size_t tp)
            : type_id_(tp)
            , index_type_(e_db_index_type::None)
            , depends_(0,"","")
            {};
        class_descriptor(size_t tp, std::string const& fn, e_db_index_type it)
            : type_id_(tp)
            , field_name_(fn)
            , index_type_(it)
            , depends_(0,"","")
            {};
    private:
        size_t type_id_;
        std::string field_name_;
        e_db_index_type index_type_;
        class_link depends_;
        class_descriptors_container inheritances_;
    };
    
    class data_update_context
    {
    public:
        virtual ~data_update_context() {};
        class_descriptors_container const& members() const {return descriptor_->members();}
        class_descriptors_container const& inheritances() const {return descriptor_->inheritances();}
        inline void bind(void const* dat) {descriptor_->bind(dat, *this);}
        inline void bind_comp(void const* dat,sql_value const& extid) {descriptor_->bind_comp(dat,*this,extid);}
        inline void read(void* dat) {return descriptor_->read(dat,*this);}
        inline void read_comp(void* dat,sql_value const& extid) {return descriptor_->read_comp(dat,*this,extid);}
        inline std::string const& forward_ref() const {return link_.target().front().ref_field_name();}
        inline std::string const& table_name() const {return link_.table_name();}

        virtual bool is_ok() = 0;
        virtual void add(sql_value const&) = 0;
        virtual bool next() = 0;
        virtual void read(sql_value&) = 0;
        virtual sql_value id_for_members(void const*) const = 0;
        virtual data_update_context_ptr context_for_member(size_t,sql_value const&, std::string const&) = 0;
        virtual void remove_if_possible(void const*) = 0;
        virtual void remove_by_key(sql_value const&) = 0;
        virtual void remove_all() = 0;
        
    protected:
        data_update_context(class_descriptors_ptr desc, class_link const& lnk)
            : descriptor_(desc)
            , link_(lnk)
            , remove_all_used_(false)
            {}
        class_descriptors_ptr descriptor_;
        class_link const& link_;
        bool remove_all_used_;
    };

};

#endif /* sb_foundation_h */
