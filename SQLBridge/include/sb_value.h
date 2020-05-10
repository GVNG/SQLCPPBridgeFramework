//
//  sb_value.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 25/04/2016.
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

#ifndef sb_value_h
#define sb_value_h

#include "sb_exceptions.h"

namespace sql_bridge
{
    class sql_value;
    typedef std::vector<sql_value> sql_values_container;
    typedef std::map<sql_value,sql_value> sql_values_map;
    typedef std::pair<size_t,sql_value> sql_context_reference;
    typedef std::deque<sql_context_reference> sql_context_references_container;
    
    class sql_value
    {
        template<bool> struct _t_optional_adapter{};
        template<bool> struct _t_real_adapter {};
        template<bool> struct _t_integral_adapter {};
        template<bool> struct _t_chrono_adapter {};
    public:
        enum class e_key_type {Empty,Integer,Real,String};
        
        inline bool empty() const {return type_==e_key_type::Empty;}
        inline e_key_type type() const {return type_;}
        
        inline bool operator < (sql_value const& rv) const
        {
            if (type_!=rv.type_) return type_<rv.type_;
            switch(type_)
            {
                case e_key_type::Integer: return iValue_<rv.iValue_;
                case e_key_type::Real: return rValue_<rv.rValue_;
                case e_key_type::String: return tValue_<rv.tValue_;
                default: break;
            }
            return false;
        }
        
        inline sql_value()
            : type_(e_key_type::Empty)
            , iValue_(0)
            , rValue_(0)
            {}
        inline sql_value(std::string const& tx)
            : type_(e_key_type::String)
            , tValue_(tx)
            , iValue_(0)
            , rValue_(0)
            {};
        inline sql_value(std::string const& tx, _t_real_adapter<false>)
            : type_(e_key_type::String)
            , tValue_(tx)
            , iValue_(0)
            , rValue_(0)
            {};
        template<typename T> inline sql_value(T const& v) : sql_value(v,_t_optional_adapter<is_kind_of_optional<T>::value>()) {}
        template<typename T> inline sql_value(T const& v, _t_optional_adapter<true>) : sql_value(v.value(),_t_real_adapter<std::is_floating_point<typename T::value_type>::value>()) {if (v.empty()) type_=e_key_type::Empty;};
        template<typename T> inline sql_value(T const& v, _t_optional_adapter<false>) : sql_value(v,_t_real_adapter<std::is_floating_point<T>::value>()) {};
        template<typename T> inline sql_value(T const& v, _t_real_adapter<false>) : sql_value(v,_t_integral_adapter<is_convertible_to_int<T>::value>()) {}
        template<typename T> inline sql_value(T const& v, _t_integral_adapter<false>) : sql_value(v,_t_chrono_adapter<is_chrono<T>::value>()) {}
        template<typename T> inline sql_value(T const& v, _t_real_adapter<true>)
            : type_(e_key_type::Real)
            , rValue_(static_cast<double>(v))
            , iValue_(0)
            {};
        template<typename T> inline sql_value(T const& iv, _t_integral_adapter<true>)
            : type_(e_key_type::Integer)
            , iValue_(static_cast<int64_t>(iv))
            , rValue_(0)
            {};
        template<typename T> inline sql_value(T const& rv, _t_chrono_adapter<true>)
            : type_(e_key_type::Real)
            , rValue_(static_cast<double>(rv.time_since_epoch().count()) / T::clock::period::den * T::clock::period::num)
            , iValue_(0)
            {};
        template<typename T> inline sql_value(T const& rv, _t_chrono_adapter<false>)
            : type_(e_key_type::Empty)
            , rValue_(0)
            , iValue_(0)
            {};
        template<typename T> inline typename std::enable_if<is_convertible_to_float<T>::value,T>::type value() const
        {
            if (type_!=e_key_type::Real)
                throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
            return static_cast<T>(rValue_);
        }
        template<typename T> inline typename std::enable_if<is_convertible_to_int<T>::value,T>::type value() const
        {
            if (type_!=e_key_type::Integer)
                throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
            return static_cast<T>(iValue_);
        }
        template<typename T> inline typename std::enable_if<is_convertible_to_text<T>::value,T const&>::type value() const
        {
            if (type_!=e_key_type::String)
                throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
            return static_cast<T const&>(tValue_);
        }
        template<typename T> inline typename std::enable_if<is_chrono<T>::value,T>::type value() const
        {
            if (type_!=e_key_type::Real)
                throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
            typename T::clock::duration ret(static_cast<typename T::clock::rep>(rValue_ / T::clock::period::num * T::clock::period::den));
            return T(ret);
        }
    private:
        // data
        e_key_type type_;
        int64_t iValue_;
        double rValue_;
        std::string tValue_;
    };

};

#endif /* sb_value_h */
