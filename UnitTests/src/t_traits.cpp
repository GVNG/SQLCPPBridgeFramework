//
//  t_traits.cpp
//  Tests
//
//  Created by Roman Makhnenko on 09/01/2019.
//  Copyright © 2019 DataArt. All rights reserved.
//

#include "t_db_fixture.h"

TEST(Core, Traits)
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
    
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_vector>::value, true);
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_map>::value, false);
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_set>::value, false);
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_array>::value, false);
}

TEST(Core,Traits2)
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

TEST(Core,TraitsOptional)
{
    typedef std::pair<bool,bool> _t_pair;
    typedef std::vector<float> _t_vector;
    typedef sql_bridge::optional_value<int> _t_optional_int;
    typedef sql_bridge::optional_value<_t_vector> _t_optional_vector;
    
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_optional_int>::value, true);
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_vector>::value, false);
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_pair>::value, false);
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_optional_vector>::value, false);
    ASSERT_EQ(sql_bridge::is_kind_of_optional<_t_optional_vector>::value, true);
}

TEST(Core,TraitsPointers)
{
    typedef int* _t_simple_int_pointer;
    typedef int const* _t_simple_int_const_pointer;
    typedef std::shared_ptr<int> _t_shared_int_pointer;
    typedef std::unique_ptr<int> _t_unique_int_pointer;
    typedef std::weak_ptr<int> _t_weak_int_pointer;
    typedef std::vector<int> _t_vector_int;

    ASSERT_EQ(sql_bridge::is_pointer<_t_simple_int_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<_t_simple_int_const_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<int>::value, false);
    ASSERT_EQ(sql_bridge::is_pointer<_t_shared_int_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<_t_unique_int_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<_t_weak_int_pointer>::value, false); // doesn't support the operator *
    ASSERT_EQ(sql_bridge::is_pointer<_t_vector_int>::value, false);
}
