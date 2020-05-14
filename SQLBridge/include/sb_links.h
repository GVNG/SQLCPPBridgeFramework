//
//  sb_links.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 06/04/2016.
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
#ifndef sb_links_h
#define sb_links_h

#include "sb_core_misc.h"

namespace sql_bridge
{
    class class_link;
    struct fields_definition;
    typedef std::vector<class_link> class_links_container;
    typedef std::vector<fields_definition> fields_definitions_container;

    struct fields_definition
    {
        std::string name_;
        std::string type_;
        bool autoincremented_;
        bool simple_indexed_;
    };
    
    struct statements_set
    {
        std::string create_;
        string_container indexes_;
        string_container trigger_;
        std::string insert_;
        std::string update_;
        std::string remove_;
        std::string remove_all_;
        std::string remove_rel_;
        std::string select_;
        std::string select_app_;
    };
    
    class class_link
    {
    public:
        class_link(size_t src, std::string const& tabname, std::string const& ref_fld_name)
            : source_id_(src)
            , table_name_(tabname)
            , ref_field_name_(ref_fld_name)
            , use_trivial_key_for_remove_(false)
            {};
        
        inline bool empty() const {return source_id_==0;}
        inline bool is_trivial_key() const {return use_trivial_key_for_remove_;}
        inline size_t source_id() const {return source_id_;}
        inline std::string const& ref_field_name() const {return ref_field_name_;}
        inline member_for_index_ref const& index_ref() const {return index_ref_;}
        inline std::string const& table_name() const {return table_name_;}
        inline class_links_container const& target() const {return target_;}
        inline class_links_container& target() {return target_;}
        inline statements_set const& statements() const {return statements_;}
        inline fields_definitions_container const& fields() const {return fields_;}
        inline fields_definitions_container const& prefix_fields() const {return prefix_fields_;}
        inline index_to_field_pairs_container const& should_create_indexes() const {return should_create_indexes_;}

        inline void update_for_key_mode(member_for_index_ref const& r) {index_ref_ = r;}
        inline void update_for_create_statement(std::string const& stm) {statements_.create_ = stm;}
        inline void update_for_index_statement(std::string const& stm) {statements_.indexes_.push_back(stm);}
        inline void update_for_trigger_statement(std::string const& stm) {statements_.trigger_.push_back(stm);}
        inline void update_for_insert_statement(std::string const& stm) {statements_.insert_ = stm;}
        inline void update_for_remove_statement(std::string const& stm) {statements_.remove_ = stm;}
        inline void update_for_remove_rel_statement(std::string const& stm) {statements_.remove_rel_ = stm;}
        inline void update_for_remove_all_statement(std::string const& stm) {statements_.remove_all_ = stm;}
        inline void update_for_select_statement(std::string const& stm, std::string const& app) {statements_.select_ = stm; statements_.select_app_=app;}
        inline void update_for_update_statement(std::string const& stm) {statements_.update_ = stm;}
        inline void add_target(class_link const& cl) {target_.push_back(cl);}
        inline void add_index_to_create(index_to_field_pair const& ip) {should_create_indexes_.push_back(ip);}
        inline void add_field(fields_definition const& fl) {fields_.push_back(fl);}
        inline void add_prefix_field(fields_definition const& fl) {prefix_fields_.push_back(fl);}
        inline void switch_to_trivial_key() {use_trivial_key_for_remove_=true;}
    protected:
    private:
        size_t source_id_;
        member_for_index_ref index_ref_;
        std::string table_name_;
        std::string ref_field_name_;
        class_links_container target_;
        index_to_field_pairs_container should_create_indexes_;
        fields_definitions_container prefix_fields_,fields_;
        bool use_trivial_key_for_remove_;
        // statements
        statements_set statements_;
    };
};

#endif /* sb_links_h */
