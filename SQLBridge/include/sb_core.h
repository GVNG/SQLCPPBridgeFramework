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
        typedef T type;
        typedef typename T::value_type type_elem;
        typedef std::basic_ostringstream<type_elem> type_stream;
        
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

};

#endif /* sb_core_h */
