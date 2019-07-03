//
//  example1.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 12/03/16.
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
#ifndef example1_hpp
#define example1_hpp

#include "sqlcppbridge.h"
#include <cmath>
#include <list>


class Case1Attach
{
    DECLARE_SQL_ACCESS(Case1Attach);
public:
    Case1Attach(){};
    Case1Attach(double av)
        : attached_value_(av)
        {}
    inline bool operator == (Case1Attach const& rv) const
    {
        return attached_value_==rv.attached_value_;
    }
private:
    double attached_value_;
};

class Case1Related
{
    DECLARE_SQL_ACCESS(Case1Related);
public:
    Case1Related(){};
    Case1Related(long long t)
        : test_(t*10)
        , attaches_(10,Case1Attach(t/M_PI))
        {}
    inline bool operator == (Case1Related const& rv) const
    {
        return  test_==rv.test_ &&
                attaches_==rv.attaches_;
    }
private:
    long long test_;
    std::vector<Case1Attach> attaches_;
};

class Case1
{
    DECLARE_SQL_ACCESS(Case1);
public:
    Case1(){};
    Case1(long long i)
        : id_(i)
        , value_(M_PI)
        , name_("Test")
        , rel_info_(2,Case1Related(i))
        {};
    inline bool operator < (Case1 const& rv) const {return id_<rv.id_;}
    inline bool operator == (Case1 const& rv) const
    {
        return  id_==rv.id_ &&
                name_==rv.name_ &&
                value_==rv.value_ &&
                rel_info_==rv.rel_info_;
    }
protected:
private:
    long long id_;
    std::string name_;
    double value_;
    std::vector<Case1Related> rel_info_;
};

#endif /* example1_hpp */
