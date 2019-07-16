//
//  example25.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 14/07/2019.
//  Copyright © 2019 DataArt.
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
#ifndef example25_h
#define example25_h

#include "sqlcppbridge.h"

class Case25;
typedef std::set<std::string> _TCase25Strings;
typedef std::map<int,int> _TCase25Map;
typedef std::vector<Case25> Case25Container;

class Case25
    : protected _TCase25Strings
    , private _TCase25Map
{
    DECLARE_SQL_ACCESS(Case25);
    DECLARE_SQL_INHERITANCE_ACCESS(Case25,_TCase25Strings);
    DECLARE_SQL_INHERITANCE_ACCESS(Case25,_TCase25Map);
public:
    Case25()
        : key_(0)
        {}
    Case25(int k)
        : key_(k)
    {
        for(int i=0; i<10; ++i)
            _TCase25Strings::insert(sql_bridge::to_string() << "test_" << i << "_" << k);
        for(int i=0; i<100; ++i)
            _TCase25Map::insert({i,k*i});
    }
    inline bool operator == (Case25 const& rv) const
    {
        return  key_==rv.key_ &&
                static_cast<_TCase25Strings const&>(*this)==rv &&
                static_cast<_TCase25Map const&>(*this)==rv;
    }
private:
    int key_;
};


#endif /* example25_h */
