//
//  example11.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 18/07/16.
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

#ifndef example11_h
#define example11_h

#include "sqlcppbridge.h"

class Case11Elem
{
    DECLARE_SQL_ACCESS(Case11Elem);
public:
    Case11Elem()
        : val1_(0)
        , val2_(0)
        {};
    Case11Elem(int i)
        : val1_(i*10)
        , val2_(i+10)
        {}
    inline bool operator == (Case11Elem const& rv) const {return val1_==rv.val1_ && val2_==rv.val2_;}
private:
    int val1_;
    int val2_;
};

typedef std::vector<Case11Elem> Case11Arr;
typedef std::map<std::string,Case11Arr> Case11Member;

class Case11
{
    DECLARE_SQL_ACCESS(Case11);
public:
    Case11() : id_(0) {};
    Case11(int num)
        : id_(num)
    {
        for(int i=0;i<num;++i)
            member_.insert(Case11Member::value_type(sql_bridge::to_string()<<"key:"<<i+1,Case11Arr(10,i)));
    }
    inline bool operator == (Case11 const& rv) const {return id_==rv.id_ && member_==rv.member_;}
private:
    int id_;
    Case11Member member_;
};

#endif /* example11_hpp */
