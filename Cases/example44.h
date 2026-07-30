//
//  example44.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 12/03/16.
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
#ifndef example44_hpp
#define example44_hpp

#include "sqlcppbridge.h"

class Case44;
using Case44Container = std::vector<Case44>;
using Case44OptionalInternal = sql_bridge::optional_value< std::chrono::time_point<std::chrono::system_clock> >;
using Case44OptionalStd = std::optional<std::size_t>;

class Case44
{
    DECLARE_SQL_ACCESS(Case44);
public:
    Case44(std::size_t i = 0)
        : eid_(i)
    {
        if (i%2)
            optional_ts_ = std::chrono::system_clock::now();
        else
            value_ = i;
    }
    inline bool operator == (Case44 const& rv) const
    {
        return  eid_==rv.eid_ &&
                optional_ts_==rv.optional_ts_ &&
                value_==rv.value_;
    }
private:
    std::size_t eid_;
    Case44OptionalInternal optional_ts_;
    Case44OptionalStd value_;
};

#endif /* example44_hpp */
