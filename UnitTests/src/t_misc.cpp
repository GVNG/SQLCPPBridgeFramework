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

TEST(Core,Traits)
{
    typedef std::map<int,int> _tmap;
    typedef std::multimap<int, int> _tmultimap;
    typedef std::vector<int> _tvector;
    typedef std::set<int> _tset;
    
    ASSERT_EQ(sql_bridge::is_map<_tmap>::value,true);
    ASSERT_EQ(sql_bridge::is_map<_tmultimap>::value,false);
    ASSERT_EQ(sql_bridge::is_multimap<_tmap>::value,false);
    ASSERT_EQ(sql_bridge::is_multimap<_tmultimap>::value,true);
    ASSERT_EQ(sql_bridge::is_any_map<_tmap>::value,true);
    ASSERT_EQ(sql_bridge::is_any_map<_tmultimap>::value,true);
    ASSERT_EQ(sql_bridge::is_container<_tmap>::value,false);
    ASSERT_EQ(sql_bridge::is_container<_tmultimap>::value,false);
    ASSERT_EQ(sql_bridge::is_container<_tvector>::value,true);
    ASSERT_EQ(sql_bridge::is_container<std::string>::value,false);
    ASSERT_EQ(sql_bridge::is_set<_tvector>::value,false);
    ASSERT_EQ(sql_bridge::is_set<_tset>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_map<_tmap>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_map<_tmultimap>::value,true);
}
