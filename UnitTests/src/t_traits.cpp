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
    using _t_map = std::map<long,long>;
    using _t_pair = std::pair<bool,bool>;
    using _t_set = std::set<int>;
    using _t_vector = std::vector<float>;
    using _t_array = std::array<char, 20>;
    using _tunorderedmap = std::unordered_map<int,int>;
    using _t_bool_vector = std::vector<bool>;
    struct _t_set_inheritance : _t_set {};
    struct _t_set_priv_inheritance : private _t_set {};
    struct _t_vector_inheritance : _t_vector {};
    struct _t_vector_priv_inheritance : private _t_vector {};

    ASSERT_EQ(sql_bridge::is_map<_t_map>::value, true);
    ASSERT_EQ(sql_bridge::is_map<_t_pair>::value, false);
    ASSERT_EQ(sql_bridge::is_pair<_t_map>::value, false);
    ASSERT_EQ(sql_bridge::is_pair<_t_pair>::value, true);
    ASSERT_EQ(sql_bridge::is_pair<_tunorderedmap>::value, false);
    ASSERT_EQ(sql_bridge::is_pair<_t_bool_vector>::value, false);
    ASSERT_EQ(sql_bridge::is_set<_t_map>::value, false);
    ASSERT_EQ(sql_bridge::is_set<_tunorderedmap>::value, false);
    ASSERT_EQ(sql_bridge::is_set<_t_set>::value, true);
    ASSERT_EQ(sql_bridge::is_set<_t_set_inheritance>::value, true);
    ASSERT_EQ(sql_bridge::is_set<_t_set_priv_inheritance>::value, false);
    ASSERT_EQ(sql_bridge::is_container<_t_vector>::value, true);
    ASSERT_EQ(sql_bridge::is_container<_t_vector_inheritance>::value, true);
    ASSERT_EQ(sql_bridge::is_container<_t_vector_priv_inheritance>::value, false);
    ASSERT_EQ(sql_bridge::is_container<_t_bool_vector>::value, true);
    ASSERT_EQ(sql_bridge::is_kind_of_array<_t_array>::value, true);
    ASSERT_EQ(sql_bridge::is_kind_of_array<_t_vector>::value, false);
    ASSERT_EQ(sql_bridge::is_kind_of_array<std::string>::value, false);
    ASSERT_EQ(sql_bridge::is_kind_of_array<_tunorderedmap>::value, false);
    ASSERT_EQ(sql_bridge::is_kind_of_array<_t_bool_vector>::value, false);

    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_vector>::value, true);
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_map>::value, false);
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_set>::value, false);
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_array>::value, false);
    ASSERT_EQ(sql_bridge::is_back_pushable_container<_t_bool_vector>::value, true);
}

TEST(Core,Traits2)
{
    using _tmap = std::map<int,int>;
    using _tmultimap = std::multimap<int, int>;
    using _tvector = std::vector<int>;
    using _tset = std::set<int>;
    using _tunorderedmap = std::unordered_map<int,int>;
    using _t_bool_vector = std::vector<bool>;

    ASSERT_EQ(sql_bridge::is_map<_tmap>::value,true);
    ASSERT_EQ(sql_bridge::is_map<_tunorderedmap>::value,true);
    ASSERT_EQ(sql_bridge::is_map<_tmultimap>::value,false);
    ASSERT_EQ(sql_bridge::is_map<_t_bool_vector>::value,false);
    ASSERT_EQ(sql_bridge::is_multimap<_tmap>::value,false);
    ASSERT_EQ(sql_bridge::is_multimap<_tmultimap>::value,true);
    ASSERT_EQ(sql_bridge::is_multimap<_tunorderedmap>::value,false);
    ASSERT_EQ(sql_bridge::is_multimap<_t_bool_vector>::value,false);
    ASSERT_EQ(sql_bridge::is_any_map<_tmap>::value,true);
    ASSERT_EQ(sql_bridge::is_any_map<_tmultimap>::value,true);
    ASSERT_EQ(sql_bridge::is_any_map<_tunorderedmap>::value,true);
    ASSERT_EQ(sql_bridge::is_any_map<_t_bool_vector>::value,false);
    ASSERT_EQ(sql_bridge::is_container<_tmap>::value,false);
    ASSERT_EQ(sql_bridge::is_container<_tmultimap>::value,false);
    ASSERT_EQ(sql_bridge::is_container<_tunorderedmap>::value,false);
    ASSERT_EQ(sql_bridge::is_container<_tvector>::value,true);
    ASSERT_EQ(sql_bridge::is_container<std::string>::value,false);
    ASSERT_EQ(sql_bridge::is_container<_t_bool_vector>::value,true);
    ASSERT_EQ(sql_bridge::is_container<_tset>::value,true);
    ASSERT_EQ(sql_bridge::is_set<_tvector>::value,false);
    ASSERT_EQ(sql_bridge::is_set<_t_bool_vector>::value,false);
    ASSERT_EQ(sql_bridge::is_set<_tset>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_map<_tmap>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_map<_tmultimap>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_map<_tunorderedmap>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_container<_tvector>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_container<_t_bool_vector>::value,true);
    ASSERT_EQ(sql_bridge::is_trivial_container<_tmap>::value,false);
    ASSERT_EQ(sql_bridge::is_vector_bool<_t_bool_vector>::value,true);
    ASSERT_EQ(sql_bridge::is_vector_bool<_tvector>::value,false);
    ASSERT_EQ(sql_bridge::is_vector_bool<_tset>::value,false);
}

TEST(Core,TraitsOptional)
{
    using _t_pair = std::pair<bool,bool>;
    using _t_vector = std::vector<float>;
    using _t_optional_int = sql_bridge::optional_value<int>;
    using _t_optional_vector = sql_bridge::optional_value<_t_vector>;
    
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_optional_int>::value, true);
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_vector>::value, false);
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_pair>::value, false);
    ASSERT_EQ(sql_bridge::is_optional_or_trivial<_t_optional_vector>::value, false);
    ASSERT_EQ(sql_bridge::is_kind_of_optional<_t_optional_vector>::value, true);
}

TEST(Core,TraitsPointers)
{
    using _t_simple_int_pointer = int*;
    using _t_simple_int_const_pointer = int const*;
    using _t_shared_int_pointer = std::shared_ptr<int>;
    using _t_unique_int_pointer = std::unique_ptr<int>;
    using _t_weak_int_pointer = std::weak_ptr<int>;
    using _t_vector_int = std::vector<int> ;

    ASSERT_EQ(sql_bridge::is_pointer<_t_simple_int_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<_t_simple_int_const_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<int>::value, false);
    ASSERT_EQ(sql_bridge::is_pointer<_t_shared_int_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<_t_unique_int_pointer>::value, true);
    ASSERT_EQ(sql_bridge::is_pointer<_t_weak_int_pointer>::value, false); // doesn't support the operator *
    ASSERT_EQ(sql_bridge::is_pointer<_t_vector_int>::value, false);
}

TEST(Core,Chrono)
{
    ASSERT_EQ(sql_bridge::is_chrono<float>::value, false);
    ASSERT_EQ(sql_bridge::is_chrono<long>::value, false);
    ASSERT_EQ(sql_bridge::is_chrono<std::string>::value, false);
    ASSERT_EQ(sql_bridge::is_chrono<std::chrono::system_clock::time_point>::value, true);
    ASSERT_EQ(sql_bridge::is_chrono<std::chrono::steady_clock::time_point>::value, true);
    ASSERT_EQ(sql_bridge::is_chrono<std::chrono::hours>::value, false);
    ASSERT_EQ(sql_bridge::is_duration<float>::value, false);
    ASSERT_EQ(sql_bridge::is_duration<long>::value, false);
    ASSERT_EQ(sql_bridge::is_duration<std::string>::value, false);
    ASSERT_EQ(sql_bridge::is_duration<std::chrono::seconds>::value, true);
    ASSERT_EQ(sql_bridge::is_duration<std::chrono::system_clock::time_point>::value, false);
}
