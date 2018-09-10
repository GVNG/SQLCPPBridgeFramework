//
//  example4.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 08/04/16.
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
#ifndef example4_h
#define example4_h

#include "sqlcppbridge.h"
#include <cmath>

class Case4Member
{
    DECLARE_SQL_ACCESS(Case4Member);
public:
    Case4Member(){};
    Case4Member(float v)
        : val_(v)
        {}
    inline bool operator == (Case4Member const& rv) const {return val_==rv.val_;}
private:
    float val_;
};

class Case4
{
    DECLARE_SQL_ACCESS(Case4);
public:
    Case4(){};
    Case4(long long i)
        : member1_(i*M_PI)
        , member2_(i/M_E)
        {}
    inline bool operator == (Case4 const& rv) const
    {
        return  member1_==rv.member1_ &&
                member2_==rv.member2_;
    }
private:
    Case4Member member1_;
    Case4Member member2_;
};

#endif /* example4_h */
