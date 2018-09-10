//
//  example21.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 12/07/2018.
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
#ifndef example21_h
#define example21_h

#include "sqlcppbridge.h"

class Case21;
class Case21Ext;
typedef std::vector<Case21Ext> Case21Container;

class Case21Ext
{
    DECLARE_SQL_ACCESS(Case21Ext);
public:
    Case21Ext(int init = 0)
        : var1_(init*M_PI)
        , var2_(init)
        , var3_(sql_bridge::to_string() << "Case21-" << init)
        {}
    inline bool operator == (Case21Ext const& rv) const {return var1_==rv.var1_ && var2_==rv.var2_ && var3_==rv.var3_;}
private:
    float var1_;
    int var2_;
    std::string var3_;
};

class Case21 : public Case21Ext
{
    DECLARE_SQL_ACCESS(Case21);
public:
    Case21(int init = 0)
        : Case21Ext(init)
        , val_(init)
    {
        if (!init) return;
        for(int i=init; i!=init+100; ++i)
            member_.push_back(Case21Ext(i));
    }
    inline bool operator == (Case21 const& rv) const {return static_cast<Case21Ext const&>(rv)==*this && member_==rv.member_;}
private:
    Case21Container member_;
    int val_;
};

#endif /* example21_h */
