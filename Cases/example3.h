//
//  example3.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 31/03/16.
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
#ifndef example3_h
#define example3_h

#include "sqlcppbridge.h"
#include <cmath>

typedef std::vector<std::string> _TCase3Strings;

class Case3
{
    DECLARE_SQL_ACCESS(Case3);
public:
    Case3(){};
    Case3(long i)
        : id_(i)
        , field1_((float)(i/M_2_PI))
        , field2_(i*M_PI)
        , field3_(sql_bridge::to_string() << "blah-blah:" << i)
        , field4_(3,sql_bridge::to_string() << "T:" << i)
        {}
    inline bool operator == (Case3 const& rv) const
    {
        return  id_==rv.id_ &&
                field1_==rv.field1_ &&
                field2_==rv.field2_ &&
                field3_==rv.field3_ &&
                field4_==rv.field4_;
    }
private:
    long id_;
    float field1_;
    double field2_;
    std::string field3_;
    _TCase3Strings field4_;
};

#endif /* example3_h */
