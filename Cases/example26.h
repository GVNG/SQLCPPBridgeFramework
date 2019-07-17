//
//  example26.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 16/07/2019.
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
#ifndef example26_h
#define example26_h

#include "sqlcppbridge.h"
#include <cmath>

class Case26;
class Case26Ext;
typedef std::vector<Case26Ext> _TCase26ExtContainer;
typedef std::vector<Case26> Case26Container;

class Case26Ext
{
    DECLARE_SQL_ACCESS(Case26Ext);
public:
    Case26Ext()
        : first_(0)
        , second_(0)
        {};
    Case26Ext(int k)
        : first_(std::sin(M_PI+k))
        , second_(std::cos(M_E*k))
        {};
    inline bool operator == (Case26Ext const& rv) const {return first_==rv.first_ && second_==rv.second_;}
private:
    double first_;
    double second_;
};

class Case26
    : private _TCase26ExtContainer
{
    DECLARE_SQL_ACCESS(Case26);
    DECLARE_SQL_INHERITANCE_ACCESS(Case26,_TCase26ExtContainer);
public:
    Case26()
        : key_(0)
        {}
    Case26(int k)
        : key_(k)
    {
        for(int i=0; i<10; ++i)
            _TCase26ExtContainer::push_back(Case26Ext(i));
    }
    inline bool operator == (Case26 const& rv) const
    {
        return  key_==rv.key_ &&
                static_cast<_TCase26ExtContainer const&>(*this)==rv;
    }
private:
    int key_;
};

#endif /* example26_h */
