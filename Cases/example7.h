//
//  example7.hpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 06/07/16.
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
#ifndef example7_h
#define example7_h

#include "sqlcppbridge.h"

using Case7Member = std::map<std::string,float>;
using Case7Alt = std::set<float>;

class Case7
{
    DECLARE_SQL_ACCESS(Case7);
public:
    Case7(){};
    Case7(long i)
        : id_(i)
        , alt_({i+0.1f,i+1.0f,i+2.0f})
    {
        for(int k=0; k<10; k++)
        {
            std::ostringstream os;
            os << "val_" << k+1;
            value_.insert({os.str(),i*100.0f});
        }
    }
    inline bool operator < (Case7 const& rv) const {return id_<rv.id_;}
    inline bool operator == (Case7 const& rv) const
    {
        return  id_==rv.id_ &&
                value_==rv.value_ &&
                alt_==rv.alt_;
    }
private:
    long id_;
    Case7Member value_;
    Case7Alt alt_;
};

#endif
