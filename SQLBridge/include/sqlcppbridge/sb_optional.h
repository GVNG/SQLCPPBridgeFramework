//
//  sb_optional.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 16/01/2026.
//  Copyright © 2026 DataArt. All rights reserved.
//

#pragma once
#ifndef sb_optional_h
#define sb_optional_h

namespace sql_bridge
{
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

#endif /* sb_optional_h */
