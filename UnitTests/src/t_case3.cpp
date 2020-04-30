//
//  t_case3.cpp
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
#include "example3.h"

TEST_F(DBFixture, Case3)
{
    sql_bridge::context cont(storage()["case3"]);
    
    std::deque<Case3> src3,dst3;
    std::map<long,Case3> dstmap;
    for(int i=0; i<100; ++i)
        src3.push_back(Case3(i+1));
    
    cont.save(src3);
    cont.remove(src3[50]);
    src3.erase(src3.begin()+50);
    cont.load(dst3);
    cont.load(dstmap);
    ASSERT_EQ(src3,dst3);
    cont.where(&Case3::id_, ">", 30l)
        .load(std::vector<Case3>(),"", [](std::vector<Case3>&& rv,size_t num)
    {
        ASSERT_EQ(rv.size(),69);
        ASSERT_EQ(num,69);
    });
}
