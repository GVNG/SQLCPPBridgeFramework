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
    std::string src_str("aaa"),src_str2("bbb"),dst_str,dst_str2;
    int src_int(100500),dst_int;
    
    storage().save("test", src_rl);
    storage().save("test", src_str);
    storage().save("test", src_int);
    storage().save("to_remove",src_str2);
    
    storage().remove<std::string>("to_remove");
    
    dst_str2 = storage().load("to_remove", std::string("def"));

    dst_rl = storage().load("test",0.0);
    dst_str = storage().load("test",std::string());
    dst_int = storage().load("test",0);

    ASSERT_EQ(src_rl, dst_rl);
    ASSERT_EQ(src_str, dst_str);
    ASSERT_EQ(src_int, dst_int);
    ASSERT_NE(src_str2, dst_str2);
    ASSERT_EQ(dst_str2, "def");
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
    storage().remove< std::map<int,std::string> >(200,"IntStringMap");
    mapstr1_src.erase(200);
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

TEST_F(DBFixture, Blob)
{
    sql_bridge::bytes_block src(100),dst,dst2;
    std::memset(src.data(), 1, src.size());
    storage().save("Content1",src);
    storage().save("Content2",src);
    dst = storage().load("Content1",sql_bridge::bytes_block());
    ASSERT_EQ(src,dst);
    storage().remove<sql_bridge::bytes_block>("Content2");
    dst2 = storage().load("Content2",src);
    ASSERT_EQ(dst2,src);
    
    using block_vector = std::vector<sql_bridge::bytes_block>;
    using block_map = std::map<std::string,sql_bridge::bytes_block>;
    block_vector src3(50,sql_bridge::bytes_block(50)),dst3;
    for(auto& s : src3)
        std::memset(s.data(), 'a', s.size());
    storage().save("blob_vector", src3);
    dst3 = storage().load("blob_vector",block_vector());
    ASSERT_EQ(dst3,src3);

    block_map src4,dst4;
    src4 = {{"val50",sql_bridge::bytes_block(50)},
            {"val100",sql_bridge::bytes_block(100)}};
    std::memset(src4["val50"].data(), 'f', src4["val50"].size());
    std::memset(src4["val100"].data(), 's', src4["val100"].size());
    storage().save("blob_map", src4);
    storage().remove<block_map>("val50","blob_map");
    dst4 = storage().load("blob_map", block_map());
    src4.erase("val50");
    ASSERT_EQ(dst4,src4);

}
