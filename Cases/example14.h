//
//  example14.hpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 04/08/16.
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
#ifndef example14_hpp
#define example14_hpp

#include "sqlcppbridge.h"

class Case14Second
{
    DECLARE_SQL_ACCESS(Case14Second);
public:
    Case14Second(long k=0)
        : value_(k*3)
        {};
    inline bool operator == (Case14Second const& rv) const {return value_==rv.value_;}
private:
    long value_;
};

class Case14First
{
    DECLARE_SQL_ACCESS(Case14First);
public:
    Case14First(long k=0)
        : members_(3,Case14Second(k))
        {};
    inline bool operator == (Case14First const& rv) const {return members_==rv.members_;}
private:
    std::vector<Case14Second> members_;
};

class Case14
{
    DECLARE_SQL_ACCESS(Case14);
public:
    Case14(long k=0)
        : id_(k)
    {
        members_.reserve((k+1)*10);
        for(long i=0; i<(k+1)*10; ++i)
            members_.push_back(Case14First(i));
    };
    inline bool operator == (Case14 const& rv) const {return id_==rv.id_ && members_==rv.members_;}

    long id_;
    std::vector<Case14First> members_;
};

#endif /* example14_hpp */
