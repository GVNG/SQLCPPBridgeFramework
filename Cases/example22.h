//
//  example22.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 09/09/2018.
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
#ifndef example22_h
#define example22_h

#include "sqlcppbridge.h"

class Case22;
class Case22Ext;
typedef std::map<std::string,Case22> Case22Container;

class Case22Ext
{
    DECLARE_SQL_ACCESS(Case22Ext);
public:
    Case22Ext(long v = 0)
        : key_(sql_bridge::to_string() << "key_" << v)
        , text_(sql_bridge::to_string() << "tx_" << v)
        , value_(v)
        {}
    inline bool operator == (Case22Ext const& rv) const {return text_==rv.text_ && value_==rv.value_ && key_==rv.key_;}
public:
    std::string key_;
private:
    std::string text_;
    long value_;
};

class Case22 : public Case22Ext
{
    DECLARE_SQL_ACCESS(Case22);
public:
    Case22(long i = 0)
        : Case22Ext(i+10)
        , data_(i)
        {}
    inline bool operator == (Case22 const& rv) const {return static_cast<Case22Ext const&>(rv)==*this && data_==rv.data_;}
    long data_;
};


#endif /* example22_h */
