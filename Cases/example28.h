//
//  example28.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 12/09/2019.
//  Copyright © 2019 DataArt.
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

#ifndef example28_h
#define example28_h

#include "sqlcppbridge.h"

class Case28Extra
{
    DECLARE_SQL_ACCESS(Case28Extra);
public:
    Case28Extra(long v = 0)
        : val_(v)
        {};
    inline bool operator == (Case28Extra const& rv) const {return val_==rv.val_;}
protected:
    long val_;
};

class Case28
{
    DECLARE_SQL_ACCESS(Case28);
public:
    Case28()
        : db_id_(0)
        {}
    void fill(size_t num)
    {
        members_.clear();
        members_.reserve(num);
        for(size_t i=0; i!=num; ++i)
            members_.push_back(Case28Extra((long)i+1));
    }
    inline bool operator == (Case28 const& rv) const {return members_==rv.members_;}
    int64_t db_id_;
private:
    std::vector<Case28Extra> members_;
};

#endif /* example28_h */
