//
//  sb_core_misc.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 27/03/2016.
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
#ifndef sb_core_misc_h
#define sb_core_misc_h

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <set>
#include <iostream>
#include <chrono>

namespace sql_bridge
{
    static std::string const g_key_field_name("sb_key");
    static std::string const g_value_field_name("sb_val");
    static std::string const g_internal_error_text("Internal error");
    static std::string const g_architecture_error_text("It's probably an error of architecture. Ask the author to resolve this issue.");

    enum class e_db_index_type {None, Basic, Unique, PrimaryKey};
    enum class e_db_key_mode {None, Unique, PrimaryKey, ExternalPrimaryKey};

    typedef std::set<size_t> size_t_set;
    typedef std::set<std::string> string_set;
    typedef std::vector<std::string> string_container;
    typedef std::function<std::string(std::string const&)> fn_change_file_name;
    typedef std::pair<std::string,std::string> strings_pair;
    
    typedef std::pair<e_db_index_type,std::string> index_to_field_pair;
    typedef std::vector<index_to_field_pair> index_to_field_pairs_container;

    class member_for_index_ref
    {
    public:
        member_for_index_ref()
            : type_(e_db_key_mode::None)
            {}
        member_for_index_ref(std::string const& name)
            : type_(e_db_key_mode::None)
            , alt_name_(name)
            {}
        member_for_index_ref(e_db_key_mode km, std::string const& name)
            : type_(km)
            , name_(name)
            {}
        inline bool empty() const {return type_==e_db_key_mode::None;}
        inline e_db_key_mode type() const {return type_;}
        inline std::string const& name() const {return name_.empty()?alt_name_:name_;}
    private:
        e_db_key_mode type_;
        std::string name_;
        std::string alt_name_;
    };
    
    class mt_event
    {
    public:
        mt_event()
            : locker_(mtx_)
            {};
        inline void fire() {var_.notify_one();}
        template<typename T> inline bool wait_for(T const& dl) {return var_.wait_for(locker_, dl)!=std::cv_status::timeout;}
        inline void wait() {var_.wait(locker_);}
    private:
        std::mutex mtx_;
        std::condition_variable var_;
        std::unique_lock<std::mutex> locker_;
    };
};

#endif /* sb_core_misc_h */
