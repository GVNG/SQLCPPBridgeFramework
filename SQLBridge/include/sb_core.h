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
    
    class to_string
    {
    public:
        template<typename T> inline to_string& operator << (T const& src) {_write(src);return *this;}
        inline operator std::string() const {return str();}
        inline std::string str() const {return buf_.str().substr(0,(size_t)buf_.tellp());}
        inline void remove_from_tail(long offs) {buf_.seekp(-offs, std::ios_base::cur);buf_.put(0);buf_.seekp(-1, std::ios_base::cur);}
        inline operator std::ostringstream&() {return buf_;}
    private:
        mutable std::ostringstream buf_;
        template<typename T> inline typename std::enable_if<std::is_enum<T>::value>::type _write(T const& src) {buf_ << static_cast<typename std::underlying_type<T>::type>(src);}
        template<typename T> inline typename std::enable_if<is_chrono<T>::value>::type _write(T const& src) {double pr = static_cast<double>(src.time_since_epoch().count()) / T::period::den * T::period::num; buf_ << pr;}
        template<typename T> inline typename std::enable_if<!std::is_enum<T>::value && !is_chrono<T>::value>::type _write(T const& src) {buf_ << src;}
    };
    
#pragma mark - class time_tracker -
    
    class time_tracker
    {
    public:
        time_tracker()
            : stamp_(std::chrono::system_clock::now())
            {}
        ~time_tracker()
        {
            std::chrono::duration<float> dif(std::chrono::system_clock::now() - stamp_);
            std::cout << "The delay is " << dif.count() << " sec." << std::endl;
        };
    private:
        std::chrono::system_clock::time_point stamp_;
    };
};

#endif /* sb_core_h */
