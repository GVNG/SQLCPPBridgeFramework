//
//  example36.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 30/04/2020.
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
#ifndef example36_h
#define example36_h

#include "sqlcppbridge.h"

class Case36;
class Case36Item;
typedef std::deque<Case36Item> Case36ItemsContainer;
typedef std::vector<Case36Item> Case36ItemsAltContainer;

class Case36Item
{
    DECLARE_SQL_ACCESS(Case36Item);
public:
    Case36Item()
        : db_id_(0)
        {};
    Case36Item(long t)
        : value_(sql_bridge::to_string() << "message_" << t)
        , db_id_(t)
        {}
    inline bool operator < (Case36Item const& rv) const {return db_id_<rv.db_id_;}
    inline bool operator == (Case36Item const& rv) const {return value_==rv.value_ && db_id_==rv.db_id_;}
    inline long db_id() const {return db_id_;}
private:
    std::string value_;
    long db_id_;
};

class Case36
{
    DECLARE_SQL_ACCESS(Case36);
public:
    Case36()
        : key_(0)
        {}
    Case36(long k, long cnt)
        : key_(k)
    {
        for(long i=0; i<cnt; ++i)
            items_.push_front(i);
        for(long i=0; i<cnt*10; ++i)
            unpaged_.insert(unpaged_.begin(),i);
    };
    inline bool operator == (Case36 const& rv) const {return key_==rv.key_ && items_==rv.items_ && unpaged_==rv.unpaged_;}
    inline bool operator != (Case36 const& rv) const {return key_!=rv.key_ || items_!=rv.items_ || unpaged_!=rv.unpaged_;}

    inline void insert(long cnt)
    {
        long offs(items_.empty()?0:items_.front().db_id()+1);
        for(long i=0; i<cnt; ++i)
            items_.push_front(offs+i);
    }
    
    size_t load(size_t,sql_bridge::context&);
    void save(size_t,sql_bridge::context&);
private:
    long key_;
    Case36ItemsContainer items_;
    Case36ItemsAltContainer unpaged_;
};


#endif /* example36_h */
