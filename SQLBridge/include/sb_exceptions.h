//
//  sb_exceptions.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 03/04/16.
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
#ifndef sb_exceptions_h
#define sb_exceptions_h

#include <exception>
#include <string>
#include "sb_core.h"

namespace sql_bridge
{
    class base_sql_error : public std::exception
    {
    public:
        base_sql_error()
            : ret_code_(0)
            , empty_(true)
            {}
        const char* what() const throw() {return what_.c_str();}
        inline std::string const& error() const {return error_;}
        inline std::string const& reason() const {return reason_;}
        inline int sql_retcode() const {return ret_code_;}
        inline bool empty() const {return empty_;}
    protected:
        base_sql_error(std::string const& err, std::string const& rs, int rcd)
            : error_(err)
            , reason_(rs)
            , ret_code_(rcd)
            , empty_(false)
            {};
        std::string what_;
    private:
        std::string error_;
        std::string reason_;
        int ret_code_;
        bool empty_;
    };
    
    class file_sql_error : public base_sql_error
    {
    public:
        file_sql_error(std::string const& err, std::string const& rs)
            : base_sql_error(err,rs,0)
        {
            what_ = to_string() << "[FILE]: " << err << " \"" << rs << "\"";
        };
    };
    
    class sql_error : public base_sql_error
    {
    public:
        sql_error(std::string const& err, std::string const& rs, int rcd)
            : base_sql_error(err,rs,rcd)
        {
            what_ = to_string() << "[SQL]: " << err << " (errcode:" << rcd << ")"
                                << " with the query: \"" << rs << "\"";
        }
    };
    
    class sql_bridge_error : public base_sql_error
    {
    public:
        sql_bridge_error(std::string const& err, std::string const& rs)
            : base_sql_error(err,rs,0)
        {
            what_ = to_string() << "[BRIDGE]: " << err << " (" << rs << ").";
        }
    };
    
};

#endif /* sb_exceptions_h */
