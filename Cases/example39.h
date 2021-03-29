//
//  example39.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 15/05/2020.
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
#ifndef example39_h
#define example39_h

#include "sqlcppbridge.h"

class Case39Member;
typedef std::vector<Case39Member> Case39MembersContainer;

class Case39Member
{
    DECLARE_SQL_ACCESS(Case39Member);
public:
    Case39Member()
        : ival_(0)
        {};
    Case39Member(int iv, long k)
        : ival_(iv)
        , value_(sql_bridge::to_string() << "a" << iv+1 << "-" << k)
        {};
    inline bool operator == (Case39Member const& rv) const {return ival_==rv.ival_ && value_==rv.value_;}
    inline bool operator == (std::string const& tx) const {return value_.find(tx)==std::string::npos;}

    std::string value_;
private:
    int ival_;
};

class Case39
{
    DECLARE_SQL_ACCESS(Case39);
public:
    Case39()
        : key_(0)
        {};
    Case39(long k)
        : key_(k)
    {
        for(int i=0; i!=100; ++i)
            members_.push_back(Case39Member(i,k));
    };
    inline bool operator == (Case39 const& rv) const {return key_==rv.key_ && members_==rv.members_;}
    void search(std::string const& tx,sql_bridge::context&);
    void deflate(std::string const& tx);
private:
    long key_;
    Case39MembersContainer members_;
};


#endif /* example39_h */
