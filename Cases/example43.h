//
//  example43.h
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

#ifndef example43_hpp
#define example43_hpp

#include "sqlcppbridge.h"

class Case43;

using Case43Container = std::vector<Case43>;

class Case43
{
    DECLARE_SQL_ACCESS(Case43);
public:
    Case43(std::size_t i = 0)
        : ts_(std::chrono::system_clock::now())
        , delta_(i)
        , st_ts_(std::chrono::steady_clock::now()+delta_)
        {};
    inline bool operator == (Case43 const& rv) const 
    {
        auto dist = std::chrono::duration_cast<std::chrono::microseconds>(st_ts_-rv.st_ts_);
        return  ts_==rv.ts_ &&
                delta_==rv.delta_ &&
                !dist.count();
    }
    inline bool operator < (Case43 const& rv) const
    {
        return delta_<rv.delta_;
    }
private:
    std::size_t db_id_;
    std::chrono::system_clock::time_point ts_;
    std::chrono::seconds delta_;
    std::chrono::steady_clock::time_point st_ts_;
};


#endif /* example43_hpp */
