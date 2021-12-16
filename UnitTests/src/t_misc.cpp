//
//  t_misc.cpp
//  Tests
//
//  Created by Roman Makhnenko on 02/07/2019.
//  Copyright © 2019 DataArt. All rights reserved.
//

#include "t_db_fixture.h"

TEST(Core,Misc)
{
    std::chrono::system_clock::time_point ts = std::chrono::system_clock::from_time_t(1554188000);
    std::string tt = sql_bridge::to_string() << ts;
    ASSERT_EQ(tt,"1554188000");
}

TEST(Core,Optional)
{
    typedef sql_bridge::optional_value<int> _t_opt_int;

    _t_opt_int v1,v2,v3,v4;
    v3 = 10;
    v4 = 20;
    
    ASSERT_EQ(v1<v2,false);
    ASSERT_EQ(v2<v1,false);
    
    ASSERT_EQ(v3<v4,true);
    ASSERT_EQ(v4<v3,false);
    ASSERT_EQ(v3,10);
    ASSERT_NE(v1,20);

    ASSERT_EQ(v1<v3,true);
    ASSERT_EQ(v4<v2,false);

    ASSERT_EQ(v1.empty(),true);
    ASSERT_EQ(v4.empty(),false);
}

TEST(Core,TextQuotation)
{
    struct t_test : sql_bridge::context_engine
    {
        static std::string quotation(std::string const& src) {return sql_bridge::context_engine::make_text_quotation(src);}
    };
    
    ASSERT_EQ(t_test::quotation("test's"),"'test''s'");
    ASSERT_NE(t_test::quotation("test's"),"test's");
    ASSERT_EQ(t_test::quotation("aaa"),"'aaa'");
}
