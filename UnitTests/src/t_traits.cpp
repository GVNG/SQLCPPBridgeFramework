//
//  t_traits.cpp
//  Tests
//
//  Created by Roman Makhnenko on 09/01/2019.
//  Copyright © 2019 DataArt. All rights reserved.
//

#include "t_db_fixture.h"

TEST_F(DBFixture, Traits)
{
    typedef std::map<long,long> _t_map;
    typedef std::pair<bool,bool> _t_pair;
    typedef std::set<int> _t_set;
    typedef std::vector<float> _t_vector;
    typedef std::array<char, 20> _t_array;
    struct _t_set_inheritance : _t_set {};
    struct _t_set_priv_inheritance : private _t_set {};
    struct _t_vector_inheritance : _t_vector {};
    struct _t_vector_priv_inheritance : private _t_vector {};

    ASSERT_EQ(sql_bridge::is_map<_t_map>::value, true);
    ASSERT_EQ(sql_bridge::is_map<_t_pair>::value, false);
    ASSERT_EQ(sql_bridge::is_pair<_t_map>::value, false);
    ASSERT_EQ(sql_bridge::is_pair<_t_pair>::value, true);
    ASSERT_EQ(sql_bridge::is_set<_t_map>::value, false);
    ASSERT_EQ(sql_bridge::is_set<_t_set>::value, true);
    ASSERT_EQ(sql_bridge::is_set<_t_set_inheritance>::value, true);
    ASSERT_EQ(sql_bridge::is_set<_t_set_priv_inheritance>::value, false);
    ASSERT_EQ(sql_bridge::is_container<_t_vector>::value, true);
    ASSERT_EQ(sql_bridge::is_container<_t_vector_inheritance>::value, true);
    ASSERT_EQ(sql_bridge::is_container<_t_vector_priv_inheritance>::value, false);
    ASSERT_EQ(sql_bridge::is_kind_of_array<_t_array>::value, true);
    ASSERT_EQ(sql_bridge::is_kind_of_array<_t_vector>::value, false);
    ASSERT_EQ(sql_bridge::is_kind_of_array<std::string>::value, false);
}
