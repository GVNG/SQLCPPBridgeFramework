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
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <set>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <iomanip>
#include <cstring>

namespace sql_bridge
{
    static std::string const g_key_field_name("sb_key");
    static std::string const g_value_field_name("sb_val");
    static std::string const g_internal_error_text("Internal error");
    static std::string const g_future_release_err_text("Not implemented yet");
    static std::string const g_incorrect_operation_err_text("This operation is inapplicable.");
    static std::string const g_without_reference_err_text("You can't use 'at' routine for this object.");
    static std::string const g_architecture_error_text("It's probably an error of architecture. Ask the author to resolve this issue.");
    static std::string const g_expand_static_recommendation("You should expand used static container or change it to dynamic.");
    static std::string const g_replace_static_recommendation("Some part of elements haven't been initialized. Use dynamic container instead.");

    enum class e_db_index_type {None, OrderAsc, OrderDesc, Basic, Unique, PrimaryKey};
    enum class e_db_key_mode {None, Unique, PrimaryKey, ExternalPrimaryKey};

    using size_t_set = std::set<size_t>;
    using string_set = std::set<std::string>;
    using string_container = std::vector<std::string>;
    using fn_change_file_name = std::function<std::string(std::string const&)>;
    using strings_pair = std::pair<std::string,std::string>;
    using index_to_field_pair = std::pair<e_db_index_type,std::string>;
    using index_to_field_pairs_container = std::vector<index_to_field_pair>;

    template<typename T> struct recursion_ref : T {};

    class range
    {
    public:
        range()
            : position_(0)
            , length_(0)
            , empty_(true)
            , active_(false)
            {};
        range(size_t p, size_t ln)
            : position_(p)
            , length_(ln)
            , empty_(false)
            , active_(true)
            {};
        inline range next() const {return range(position_+length_,length_);}
        inline bool empty() const {return empty_;}
        inline bool is_active() const {return active_;}
        inline void enable() const {active_=true;}
        inline void disable() const {active_=false;}
        inline size_t position() const {return position_;}
        inline size_t length() const {return length_;}
    private:
        size_t position_;
        size_t length_;
        bool empty_;
        mutable bool active_;
    };
    
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
    
    // sqlbridge uses this implementation instead of standard
    // the reasons are:
    //      - 'in place' allocation for data
    //      - 'extra compare rules' for the mixed set of 'optional' and presented data
    // please use std::optional (C++14) implementation outside the library
    template<typename T> class optional_value
    {
    public:
        using value_type = std::decay_t<T>;
        using optional_flag_type = bool;
        
        optional_value()
            : value_(default_init<value_type>())
            , optional_(true)
            {};
        explicit optional_value(value_type const& src)
            : value_(src)
            , optional_(false)
            {};
        optional_value(optional_value const& src)
            : value_(src.value_)
            , optional_(src.optional_)
            {}
        optional_value(optional_value&& src)
            : value_(std::move(src.value_))
            , optional_(src.optional_)
            {}
        inline bool operator == (optional_value const& rv) const
        {
            if (optional_ && rv.optional_) return true;
            if (!optional_ && !rv.optional_ && value_==rv.value_) return true;
            return false;
        }
        inline bool operator < (optional_value const& rv) const
        {
            if (optional_ && rv.optional_) return false;
            if (!optional_ && !rv.optional_) return value_<rv.value_;
            return optional_;
        }
        inline operator value_type const& () const {return value_;}
        inline void const* values_ptr() const {return &value_;}
        inline value_type const& value() const {return value_;}
        inline void operator = (optional_value const& src) {value_ = src.value_; optional_ = src.optional_;}
        inline void operator = (value_type const& src) {value_ = src; optional_ = false;}
        inline void operator = (optional_value&& src) {optional_ = src.optional_; value_ = std::move(src.value_);}
        inline void operator = (value_type&& src) {optional_ = false; value_ = std::move(src);}
        inline bool empty() const {return optional_;}
    private:
        value_type value_;
        optional_flag_type optional_;
        
        template<typename _tf> static std::enable_if_t<!std::is_pod<_tf>::value && std::is_default_constructible<_tf>::value,_tf> default_init() {return _tf();}
        template<typename _tf> static std::enable_if_t<std::is_pod<_tf>::value,_tf> default_init()
        {
            _tf ret;
            std::memset(&ret, 0, sizeof(ret));
            return ret;
        }
    };
};

#endif /* sb_core_misc_h */
