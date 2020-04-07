//
//  example33.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 07/04/2020.
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
#ifndef example33_h
#define example33_h

#include "sqlcppbridge.h"

class Case33;
typedef Case33* Case33Ptr;
typedef std::vector<Case33Ptr> Case33Container; // bad style!!! Don't repeat it!
typedef std::map<long,Case33Ptr> Case33Map; // bad style!!! Don't repeat it!

class Case33
{
    DECLARE_SQL_ACCESS(Case33);
public:
    Case33()
        : key_(0)
        {};
    Case33(long k)
        : key_(k)
        , val_(sql_bridge::to_string() << "value: " << k*10)
        {};
    inline bool operator == (Case33 const& rv) const {return key_==rv.key_ && val_==rv.val_;}

    long key_;
private:
    std::string val_;
};


#endif /* example33_h */
