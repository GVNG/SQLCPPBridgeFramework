//
//  example31.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 02/04/2020.
//  Copyright © 2020 DataArt.
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
#ifndef example31_h
#define example31_h

#include "sqlcppbridge.h"

class Case31;
using Case31Ptr = std::shared_ptr<Case31>;
using Case31Container = std::vector<Case31Ptr>;

class Case31
{
    DECLARE_SQL_ACCESS(Case31);
public:
    Case31()
        : val2_(0)
        {};
    Case31(std::string const& k, std::string const& v1, int v2)
        : key_(k)
        , val1_(v1)
        , val2_(v2)
        {};
    inline bool operator == (Case31 const& rv) const {return key_==rv.key_ && val1_==rv.val1_ && val2_==rv.val2_;}

    std::string key_;
private:
    std::string val1_;
    int val2_;
};

#endif /* example31_h */
