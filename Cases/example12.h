//
//  example12.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 25/07/16.
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
#ifndef example12_h
#define example12_h

#include "sqlcppbridge.h"
#include <cmath>

class Case12Attach
{
    DECLARE_SQL_ACCESS(Case12Attach);
public:
    Case12Attach(long i = 0)
        : val_(i/M_PI)
        {}
    inline bool operator == (Case12Attach const& rv) const {return val_==rv.val_;}
private:
    float val_;
};

class Case12Member : public Case12Attach
{
    DECLARE_SQL_ACCESS(Case12Member);
public:
    Case12Member() : val_(0) {}
    Case12Member(std::string const& nm, long v)
        : Case12Attach(v)
        , name_(nm)
        , val_(v)
        {}
    inline bool operator == (Case12Member const& rv) const {return name_==rv.name_ && val_==rv.val_ && static_cast<Case12Attach const&>(*this)==rv;}

private:
    std::string name_;
    long val_;
};

typedef std::map<std::string, Case12Member> Case12Map;

class Case12
{
    DECLARE_SQL_ACCESS(Case12);
public:
    Case12(int cnt=0)
        : id_(cnt)
    {
        for(int i=0;i<cnt;++i)
        {
            std::string key(sql_bridge::to_string()<<"name_"<<i);
            members_.insert({key,Case12Member(key,i+1)});
        }
    };
    inline bool operator == (Case12 const& rv) const {return id_==rv.id_ && members_==rv.members_;}
private:
    int id_;
    Case12Map members_;
};

#endif /* example12_h */
