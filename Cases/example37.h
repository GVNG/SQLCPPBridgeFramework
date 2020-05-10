//
//  example37.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 09/05/2020.
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

#ifndef example37_h
#define example37_h

#include "sqlcppbridge.h"
#include <cmath>

class Parent1Case37
{
    DECLARE_SQL_ACCESS(Parent1Case37);
public:
    Parent1Case37(){};
    Parent1Case37(long long v)
        : val_(v)
        {};
    inline bool operator == (Parent1Case37 const& rv) const {return val_==rv.val_;}
protected:
private:
    long long val_;
};

class MemberCase37 : public Parent1Case37
{
    DECLARE_SQL_ACCESS(MemberCase37);
public:
    MemberCase37() {};
    MemberCase37(long long i)
        : Parent1Case37(i*7)
        , val_(i*M_LOG2E)
        {};
    inline bool operator == (MemberCase37 const& rv) const
    {
        return val_==rv.val_ && static_cast<Parent1Case37 const&>(*this) == rv;
    }
private:
    double val_;
};

class Case37
    : public Parent1Case37
{
    DECLARE_SQL_ACCESS(Case37);
public:
    Case37(){};
    Case37(long long i)
        : Parent1Case37(i*10)
        , id_(i)
        , value_(M_E*i)
        , name_("blah-blah")
        , members_(3,MemberCase37(i))
        {};
    inline bool operator == (Case37 const& rv) const
    {
        return  id_==rv.id_ &&
                name_==rv.name_ &&
                value_==rv.value_ &&
                members_==rv.members_ &&
                static_cast<Parent1Case37 const&>(*this) == rv;
    }
    
    void save(size_t,sql_bridge::context&);
private:
    long long id_;
    std::string name_;
    double value_;
    std::vector<MemberCase37> members_;
};


#endif /* example37_h */
