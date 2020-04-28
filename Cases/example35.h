//
//  example35.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 28/04/2020.
//  Copyright © 2020 DataArt.
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
#ifndef example35_h
#define example35_h

#include "sqlcppbridge.h"

class Case35;
class Case35Log;
typedef std::vector<Case35Log> Case35LogContainer;

class Case35Log
{
    DECLARE_SQL_ACCESS(Case35Log);
public:
    Case35Log()
        : db_id_(0)
        {};
    Case35Log(long t)
        : value_(sql_bridge::to_string() << "log_str_" << t)
        , db_id_(0)
        {}
    inline bool operator < (Case35Log const& rv) const {return db_id_<rv.db_id_;}
    inline bool operator == (Case35Log const& rv) const {return value_==rv.value_;}
private:
    std::string value_;
    long long db_id_;
};

class Case35
{
    DECLARE_SQL_ACCESS(Case35);
public:
    Case35()
        : key_(0)
        {}
    Case35(long k, long cnt)
        : key_(k)
    {
        for(long i=0; i<cnt; ++i)
            log_container_.push_back(i);
    };
    inline bool operator == (Case35 const& rv) const {return key_==rv.key_ && log_container_==rv.log_container_;}
private:
    long key_;
    Case35LogContainer log_container_;
};

#endif /* example35_h */
