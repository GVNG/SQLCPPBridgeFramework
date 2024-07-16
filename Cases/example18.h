//
//  example18.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 23/06/2018.
//  Copyright © 2018, DataArt.
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
#ifndef example18_h
#define example18_h

#include "sqlcppbridge.h"

class Case18;
class Case18Member;
class Case18Par1;
class Case18Par2;
using _TCase18Members = std::vector<Case18Member>;

class Case18Par1
{
    DECLARE_SQL_ACCESS(Case18Par1);
public:
    Case18Par1(long i)
        : val_(sql_bridge::to_string() << "TT "<< i)
        {}
    inline bool operator == (Case18Par1 const& rv) const {return val_==rv.val_;}
private:
    std::string val_;
};

class Case18Par2
{
    DECLARE_SQL_ACCESS(Case18Par2);
public:
    Case18Par2(long i)
        : val_(i)
        {}
    inline bool operator == (Case18Par2 const& rv) const {return val_==rv.val_;}
private:
    double val_;
};

class Case18Member
    : public Case18Par1
    , public Case18Par2
{
    DECLARE_SQL_ACCESS(Case18Member);
public:
    Case18Member(long i = 0)
        : Case18Par1(i)
        , Case18Par2(i)
        , mem_id_(i)
        {}
    inline bool operator == (Case18Member const& rv) const
    {
        return  static_cast<Case18Par1 const&>(*this)==static_cast<Case18Par1 const&>(rv) &&
                static_cast<Case18Par2 const&>(*this)==static_cast<Case18Par2 const&>(rv) &&
                mem_id_==rv.mem_id_;
    }

private:
    long mem_id_;
};

class Case18
{
    DECLARE_SQL_ACCESS(Case18);
public:
    Case18(long d = 0)
        : db_id_(d)
    {
        if (d)
            for(long i=d+10;i<d+20;++i)
                members_.push_back(Case18Member(i));
    }
    inline bool operator == (Case18 const& rv) const
    {
        return  db_id_==rv.db_id_ &&
        members_==rv.members_;
    }
private:
    long db_id_;
    _TCase18Members members_;
};

#endif /* example18_h */
