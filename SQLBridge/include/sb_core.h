//
//  sb_core.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 01/08/16.
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
#ifndef sb_core_h
#define sb_core_h

#include "sb_core_misc.h"
#include "sb_traits.h"

namespace sql_bridge
{
    
    template<typename T> class _t_to_string
    {
    public:
        using type = T;
        using type_elem = typename T::value_type;
        using type_stream = std::basic_ostringstream<type_elem>;
        
        template<typename TFn> inline _t_to_string& operator << (TFn const& src) {_write(src);return *this;}
        inline operator type() const {return str();}
        inline type str() const {return buf_.str().substr(0,(size_t)buf_.tellp());}
        inline void remove_from_tail(long offs) {buf_.seekp(-offs, std::ios_base::cur);buf_.put(0);buf_.seekp(-1, std::ios_base::cur);}
        inline operator type_stream&() {return buf_;}
    private:
        mutable type_stream buf_;
        template<typename TFn> inline typename std::enable_if<std::is_enum<TFn>::value>::type _write(TFn const& src) {buf_ << static_cast<typename std::underlying_type<TFn>::type>(src);}
        template<typename TFn> inline typename std::enable_if<is_chrono<TFn>::value>::type _write(TFn const& src) {double pr = static_cast<double>(src.time_since_epoch().count()) / TFn::period::den * TFn::period::num; buf_ << std::setprecision(15) << pr;}
        template<typename TFn> inline typename std::enable_if<!std::is_enum<TFn>::value && !is_chrono<TFn>::value>::type _write(TFn const& src) {buf_ << src;}
    };
    
    using to_string = _t_to_string<std::string>;
    using to_wstring = _t_to_string<std::wstring>;

    template<typename T> class protected_section
    {
        using type = typename std::decay<typename std::remove_pointer<T>::type>::type;
        template<bool,typename TInt> struct check_for_container {typedef TInt value_type;};
        template<typename TInt> struct check_for_container<true,TInt> {typedef typename TInt::value_type value_type;};
        using init_type = typename check_for_container<is_any_container<type>::value,type>::value_type;
        using pointer_type = std::unique_ptr<type>;
        using lock_guard = std::lock_guard<std::mutex>;
    public:
        class access
        {
            friend protected_section;
        public:
            access(protected_section const& src)
                : guard_(src.access_)
                , value_(src.pointer_.get())
                , mt_value_(nullptr)
                {}
            access(protected_section& src)
                : guard_(src.access_)
                , value_(src.pointer_.get())
                , mt_value_(src.pointer_.get())
                {}
            inline bool empty() const {return !value_;}
            inline type const& data() const {return *value_;}
            inline type& mutable_data() {assert(mt_value_);return *mt_value_;}
        private:
            lock_guard guard_;
            type const* value_;
            type* mt_value_;
        };
        inline protected_section()
            : pointer_(std::make_unique<type>())
            {};
        protected_section(protected_section const&) = delete;
        protected_section(protected_section&&) = delete;
        
        explicit inline protected_section(type&& src)
            : pointer_(std::make_unique<type>(std::move(src)))
            {}
        explicit inline protected_section(type const& src)
            : pointer_(std::make_unique<type>(src))
            {}
        inline protected_section(std::initializer_list<init_type> src)
            : pointer_(std::make_unique<type>(src))
            {}
        ~protected_section() {lock_guard lk(access_);pointer_.reset();}
    private:
        mutable std::mutex access_;
        pointer_type pointer_;
    };

};

#endif /* sb_core_h */
