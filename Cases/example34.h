//
//  example34.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 27/04/2020.
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
#ifndef example34_h
#define example34_h

#include "sqlcppbridge.h"

class Case34Extra;
class Case34;
using Case34ExtraPtr = std::shared_ptr<Case34Extra>;
using Case34ExtraContainer = std::map<std::string,Case34ExtraPtr>;
using Case34Container = std::vector<Case34>;

class Case34Extra
{
    DECLARE_SQL_ACCESS(Case34Extra);
public:
    Case34Extra() {}
    Case34Extra(int i)
        : info_(sql_bridge::to_string() << "info_" << i)
        , data_(i)
        {}
    inline std::string const& info() const {return info_;}
    inline bool operator != (Case34Extra const& rv) const {return info_!=rv.info_ || data_!=rv.data_;}
    inline bool operator < (Case34Extra const& rv) const {return info_<rv.info_;}
private:
    std::string info_;
    int data_;
};

class Case34
{
    DECLARE_SQL_ACCESS(Case34);
public:
    Case34() {};
    Case34(int num)
    {
        for(int i=0; i!=100; ++i)
            extra_.insert({sql_bridge::to_string() << "k" << i,std::make_shared<Case34Extra>(i+num*100)});
    }
    inline bool operator == (Case34 const& rv) const
    {
        if (extra_.size()!=rv.extra_.size()) return false;
        for(auto const& kv : extra_)
            if (*kv.second!=*rv.extra_.find(kv.first)->second) return false;
        return true;
    }
    
private:
    Case34ExtraContainer extra_;
};


#endif /* example34_h */
