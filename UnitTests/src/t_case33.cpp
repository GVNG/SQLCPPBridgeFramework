//
//  t_case33.cpp
//  Tests
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

#include "t_db_fixture.h"
#include "example33.h"

TEST_F(DBFixture, Case33)
{
    sql_bridge::context cont(storage()["case33"]);
    
    Case33Container dst,src
    {
        new Case33(1),
        new Case33(2),
        new Case33(3),
        new Case33(4),
    };
    Case33Map chk;
    cont.save(src);
    cont.order(&Case33::key_).load(dst);
    cont.load(chk);

    ASSERT_EQ(src.size(),dst.size());
    for(Case33Container::size_type i = 0; i!=src.size(); ++i)
    {
        ASSERT_EQ(*src[i],*dst[i]);
        ASSERT_EQ(*src[i],*chk[src[i]->key_]);
    }

    for(auto v : src) delete v;
    for(auto v : dst) delete v;
    for(auto v : chk) delete v.second;
}
