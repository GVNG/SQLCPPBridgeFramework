//
//  example30.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 31/03/2020.
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
#ifndef example30_h
#define example30_h

#include "sqlcppbridge.h"

class Case30Extra;
class Case30;
using Case30ExtraPtr = std::shared_ptr<Case30Extra>;
using Case30Ptr = std::shared_ptr<Case30>;
using Case30ExtraContainer = std::vector<Case30ExtraPtr>;
using Case30Map = std::unordered_map<std::string,Case30Ptr>;

class Case30Extra
{
    DECLARE_SQL_ACCESS(Case30Extra);
public:
    Case30Extra() {}
    Case30Extra(int i)
        : info_(sql_bridge::to_string() << "info_" << i)
        , data_(i)
        {}
    inline std::string const& info() const {return info_;}
    inline bool operator != (Case30Extra const& rv) const {return info_!=rv.info_ || data_!=rv.data_;}
    inline bool operator < (Case30Extra const& rv) const {return info_<rv.info_;}
private:
    std::string info_;
    int data_;
};

class Case30
{
    DECLARE_SQL_ACCESS(Case30);
public:
    Case30() {};
    Case30(std::string const& k,int num)
        : key_(k)
    {
        for(int i=0; i!=100; ++i)
            extra_.push_back(std::make_shared<Case30Extra>(i+num*100));
    }
    inline bool operator == (Case30 const& rv) const
    {
        if (key_!=rv.key_) return false;
        if (extra_.size()!=rv.extra_.size()) return false;
        for(Case30ExtraContainer::size_type i=0; i!=extra_.size(); ++i)
            if (*extra_[i]!=*rv.extra_[i]) return false;
        return true;
    }
    inline std::string const& key() const {return key_;}
    
private:
    std::string key_;
    Case30ExtraContainer extra_;
};

#endif /* example30_h */
