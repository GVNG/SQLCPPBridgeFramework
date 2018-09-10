//
//  example6.hpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 22/06/16.
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
#ifndef example6_hpp
#define example6_hpp

#include "sqlcppbridge.h"

class Case6Attach
{
    DECLARE_SQL_ACCESS(Case6Attach);
public:
    Case6Attach(){};
    Case6Attach(long v)
        : attached_value_(v)
        {};
    inline bool operator == (Case6Attach const& rv) const
    {
        return  attached_value_==rv.attached_value_;
    }
private:
    long attached_value_;
};

class Case6 : public Case6Attach
{
    DECLARE_SQL_ACCESS(Case6);
public:
    Case6(){};
    Case6(long v)
        : Case6Attach(v)
        , value_(v)
        {}
    inline bool operator == (Case6 const& rv) const
    {
        return  value_==rv.value_ &&
                static_cast<Case6Attach const&>(*this)==rv;
    }
private:
    long value_;
};

#endif /* example6_hpp */
