//
//  example17.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 26/01/2017.
//  Copyright © 2017, DataArt.
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
#ifndef example17_h
#define example17_h

#include "sqlcppbridge.h"

class Case17;
class Case17Extra;

using _TStringToLongMap = std::map<std::string,long long>;
using _TCase17ExtraMap = std::map<long,Case17Extra>;
using _TLongToCase17Map = std::map<long,Case17>;

class Case17Extra
{
    DECLARE_SQL_ACCESS(Case17Extra);
public:
    Case17Extra(long k=0,std::string const& v="")
        : key_(k)
        , value_(v)
        {}
    inline bool operator == (Case17Extra const& rv) const {return key_==rv.key_ && value_==rv.value_;}
private:
    long key_;
    std::string value_;
};

class Case17
{
    DECLARE_SQL_ACCESS(Case17);
public:
    Case17(long v = 0, bool init = false)
        : key_(v*5)
    {
        if (!init) return;
        for(long i=0; i<10; ++i)
            values1_.insert({sql_bridge::to_string() << "v1_" << i,i*23});
        for(long i=0; i<40; ++i)
            values2_.insert({sql_bridge::to_string() << "v2_" << i,i*17});
        for(long i=v*3; i<v*3+50; ++i)
            values3_.insert({i,Case17Extra(i*7,sql_bridge::to_string() << "v3_" << i)});
    }
    inline bool operator == (Case17 const& rv) const
    {
        return  key_==rv.key_ &&
                values1_==rv.values1_ &&
                values2_==rv.values2_ &&
                values3_==rv.values3_;
    }
private:
    long key_;
    _TStringToLongMap values1_;
    _TStringToLongMap values2_;
    _TCase17ExtraMap values3_;
};

#endif /* example17_h */
