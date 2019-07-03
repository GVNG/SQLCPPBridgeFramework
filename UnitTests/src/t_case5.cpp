//
//  t_case5.cpp
//  UnitTests
//
//  Created by Roman Makhnenko on 31/12/2018.
//  Copyright © 2018, DataArt.
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
#include "example5.h"

TEST_F(DBFixture, Case5)
{
    sql_bridge::context cont(storage()["case5"]);
    Case5a src5a, dst5a;
    Case5b src5b, dst5b;
    for(long long i=0; i<100; ++i)
        src5a.push_back(sql_bridge::to_string() << "var: " << i+1);
    for(long long i=0; i<300; ++i)
        src5b[sql_bridge::to_string() << "key: " << i+1] = (long)i;
    
    cont.replace(src5a);
    cont.replace(src5b);
    
    cont.load(dst5a);
    cont.load(dst5b);
    ASSERT_EQ(src5a,dst5a);
    ASSERT_EQ(src5b,dst5b);
}
