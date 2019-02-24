//
//  t_kvdb.cpp
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

TEST_F(DBFixture, SimpleValues)
{
    double src_rl(M_PI),dst_rl;
    std::string src_str("aaa"),dst_str;
    int src_int(100500),dst_int;
    
    storage().save("test", src_rl);
    storage().save("test", src_str);
    storage().save("test", src_int);
    
    dst_rl = storage().load("test",0.0);
    dst_str = storage().load("test",std::string());
    dst_int = storage().load("test",0);

    ASSERT_EQ(src_rl, dst_rl);
    ASSERT_EQ(src_str, dst_str);
    ASSERT_EQ(src_int, dst_int);
}

TEST_F(DBFixture, StringVector)
{
    std::vector<std::string> arrstr1_src,arrstr1_dst;
    for(int i=0; i<1000; i++)
        arrstr1_src.push_back(sql_bridge::to_string() << "FF" << i);
    storage().save("FFStrings", arrstr1_src);
    arrstr1_dst = storage().load("FFStrings", std::vector<std::string>());
    ASSERT_EQ(arrstr1_src,arrstr1_dst);
}

TEST_F(DBFixture, StringSet)
{
    std::set<std::string> setstr1_src,setstr1_dst;
    for(int i=0; i<10000; i++)
        setstr1_src.insert(sql_bridge::to_string() << "FF" << i);
    storage().save("FFSet", setstr1_src);
    setstr1_dst = storage().load("FFSet", std::set<std::string>());
    ASSERT_EQ(setstr1_src,setstr1_dst);
}

TEST_F(DBFixture, CharArray)
{
    std::array<char,20> stat_src,stat_dst;
    std::fill(stat_src.begin(),stat_src.end(),10);
    storage().save("STAT", stat_src);
    stat_dst = storage().load("STAT", std::array<char,20>());
    ASSERT_EQ(stat_src,stat_dst);
}

TEST_F(DBFixture, IntStringMap)
{
    std::map<int,std::string> mapstr1_src,mapstr1_dst;
    for(int i=0; i<1000; i++)
        mapstr1_src[i] = sql_bridge::to_string() << "Map: " << i+1;
    storage().save("IntStringMap",mapstr1_src);
    mapstr1_dst = storage().load("IntStringMap",std::map<int,std::string>());
    ASSERT_EQ(mapstr1_src,mapstr1_dst);
}

TEST_F(DBFixture, Chrono)
{
    std::chrono::system_clock::time_point srcnow(std::chrono::system_clock::now()),dstnow;
    storage().save("NOW", srcnow);
    dstnow = storage().load("NOW", std::chrono::system_clock::time_point());
	auto dur = srcnow<dstnow?(dstnow-srcnow):(srcnow-dstnow);
	ASSERT_LT(dur,std::chrono::microseconds(1));
}
