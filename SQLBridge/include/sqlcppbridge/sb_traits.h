//
//  sb_traits.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 01/08/16.
//  Copyright © 2016, DataArt.
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

#pragma once
#ifndef sb_traits_h
#define sb_traits_h

#include <chrono>
#include <set>
#include <deque>
#include <vector>
#include <array>
#include <list>
#include <queue>
#include <tuple>
#include <typeinfo>
#include <memory>
#include <string>

#include "sb_bytes_block.h"
#include "sb_optional.h"

namespace sql_bridge
{
    template<typename T, typename = void> struct is_key_mapped : std::false_type {};
    template<typename T> struct is_key_mapped<T, std::void_t<typename T::key_type,typename T::mapped_type> > : std::true_type
    {
        using type = T;
    };
    
    template<typename T, typename = void> struct has_const_iterator : std::false_type {};
    template<typename T> struct has_const_iterator<T, std::void_t<typename T::const_iterator*> > : std::true_type
    {
        using type = T;
    };

    template<typename T, typename = void> struct has_element_type : std::false_type {};
    template<typename T> struct has_element_type<T, std::void_t<typename T::element_type*> > : std::true_type
    {
        using type = T;
    };

    template<typename T, typename = void> struct has_begin_end : std::false_type {};
    template<typename T> struct has_begin_end<T,std::void_t<decltype(std::declval<const T&>().begin()),
                                                            decltype(std::declval<const T&>().end())> > : std::true_type
    {
        using type = T;
    };
    
    template<typename T, typename = void> struct has_push_back : std::false_type{};
    template<typename T> struct has_push_back<T,std::void_t<typename T::const_reference,
                                                            decltype(std::declval<T&>().push_back(std::declval<typename T::const_reference>()))> > : std::true_type
    {
        using type = T;
    };
    
    template<typename T, typename = void> struct has_at : std::false_type{};
    template<typename T> struct has_at<T,std::void_t<typename T::key_type,
                                                     decltype(std::declval<T&>().at(std::declval<const typename T::key_type&>()))> > : std::true_type
    {
        using type = T;
    };

    template<typename T> struct is_pair : std::false_type {};
    template<typename T,typename T2> struct is_pair< std::pair<T, T2> > : std::true_type {};
    template<typename T> struct is_kind_of_time_point : std::false_type {};
    template<typename T> struct is_kind_of_time_point< std::chrono::time_point<T> > : std::true_type {};
    template<typename T> struct is_kind_of_duration : std::false_type {};
    template<typename T,typename T2> struct is_kind_of_duration< std::chrono::duration<T, T2> > : std::true_type {};
    template<typename T> struct is_optional_bare : std::false_type {};
    template<typename T> struct is_optional_bare< optional_value<T> > : std::true_type {};
    template<typename T> struct is_optional_bare< std::optional<T> > : std::true_type {};
    template<typename T> struct is_vector_bool : std::false_type {};
    template<> struct is_vector_bool< std::vector<bool> > : std::true_type {};

    template<typename T> struct is_ordered_set
        : std::integral_constant<bool,  std::is_base_of<std::set<typename T::value_type>, T>::value ||
                                        std::is_base_of<std::multiset<typename T::value_type>, T>::value> {};

    template<typename T> struct is_any_container : std::integral_constant<bool, has_begin_end<T>::value && has_const_iterator<T>::value> {};
    
    template<bool,typename T> struct check_for_multimap : std::integral_constant<bool, false>{};
    template<typename T> struct check_for_multimap<true,T> : std::integral_constant<bool, !has_at<T>::value>{};
    template<typename T> struct is_map : std::integral_constant<bool,  is_any_container<T>::value && is_key_mapped<T>::value && !check_for_multimap<is_key_mapped<T>::value,T>::value> {};
    template<typename T> struct is_multimap : std::integral_constant<bool,  is_any_container<T>::value && is_key_mapped<T>::value && check_for_multimap<is_key_mapped<T>::value,T>::value>{};
    template<typename T> struct is_any_map : std::integral_constant<bool, is_map<T>::value || is_multimap<T>::value> {};

    template<bool,typename T> struct check_for_set : std::integral_constant<bool, false>{};
    template<typename T> struct check_for_set<true,T> : std::integral_constant<bool, is_ordered_set<T>::value>{};
    template<typename T> struct is_set : std::integral_constant<bool,  check_for_set<is_any_container<T>::value,T>::value> {};
    
    template<typename T> struct is_container
        : std::integral_constant<bool,  is_any_container<T>::value &&
                                        !std::is_base_of<std::string,T>::value &&
                                        !is_any_map<T>::value> {};
    
    template<typename T> struct is_convertible_to_text : std::integral_constant<bool, std::is_base_of<std::string,T>::value> {};
    template<typename T> struct is_convertible_to_int : std::integral_constant<bool,  std::is_integral<T>::value || std::is_enum<T>::value> {};
    template<typename T> struct is_convertible_to_float : std::integral_constant<bool,  std::is_floating_point<T>::value> {};
    template<typename T> struct is_convertible_to_bool : std::integral_constant<bool,  std::is_same<bool, T>::value> {};
    template<typename T> struct is_duration : std::integral_constant<bool, is_kind_of_duration<T>::value && !is_kind_of_time_point<T>::value> {};
    template<typename T> struct is_chrono : std::integral_constant<bool, is_kind_of_time_point<T>::value> {};
    
    template<bool,typename T> struct is_smart_pointer : std::integral_constant<bool, false> {using type = T;};
    template<typename T> struct is_smart_pointer<true,T>
        : std::integral_constant<bool,  std::is_same<T,std::shared_ptr<typename T::element_type> >::value ||
                                        std::is_same<T,std::unique_ptr<typename T::element_type> >::value>
    {
        using type = typename T::element_type;
    };
    
    template<typename T> struct is_pointer
        : std::integral_constant<bool,  std::is_pointer<T>::value ||
                                        is_smart_pointer<has_element_type<T>::value,T>::value>
    {
        using T1 = typename std::remove_pointer<T>::type;
        using T2 = typename std::conditional<is_smart_pointer<has_element_type<T>::value,T>::value, typename is_smart_pointer<has_element_type<T>::value,T>::type, T1>::type;
        using type = T2;
    };
    
    template<typename T> struct is_convertible_to_blob : std::integral_constant<bool,  std::is_same<T,bytes_block>::value> {};
    
    template<typename T> struct is_sql_acceptable
        : std::integral_constant<bool,  is_convertible_to_int<T>::value ||
                                        is_convertible_to_float<T>::value ||
                                        is_convertible_to_text<T>::value ||
                                        is_chrono<T>::value ||
                                        is_duration<T>::value ||
                                        is_convertible_to_blob<T>::value> {};

    template<bool,typename T> struct is_sql_optional : std::integral_constant<bool, is_sql_acceptable<T>::value>{};
    template<typename T> struct is_sql_optional<true,T> : std::integral_constant<bool,  is_sql_acceptable<typename T::value_type>::value> {};
    template<typename T> struct is_optional_or_trivial : is_sql_optional<is_optional_bare<T>::value,T> {};
    template<typename T> struct is_kind_of_optional : std::integral_constant<bool,  is_optional_bare<T>::value> {};

    template<typename T> struct is_back_pushable_container
        : std::integral_constant<bool,  has_push_back<T>::value ||
                                        std::is_same<T, std::vector<bool> >::value > {};

    template<typename T> struct sql_array_size : std::extent<T> {};
    template<typename T, size_t N> struct sql_array_size<std::array<T,N> > : std::tuple_size<std::array<T,N> > {};
    
    template<typename T> struct is_kind_of_array
        : std::integral_constant<bool,  is_any_container<T>::value &&
                                        sql_array_size<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::value> {};

    template<bool,typename T> struct check_container_of_containers : std::integral_constant<bool, false>{};
    template<typename T> struct check_container_of_containers<true,T> : std::integral_constant<bool, is_any_container<typename T::value_type>::value>{};
    template<bool,typename T> struct check_map_of_containers : std::integral_constant<bool, false>{};
    template<typename T> struct check_map_of_containers<true,T> : std::integral_constant<bool, is_any_container<typename T::mapped_type>::value>{};

    template<typename T> struct is_container_of_containers
        : std::integral_constant<bool,  check_container_of_containers<is_container<T>::value,T>::value ||
                                        check_map_of_containers<is_map<T>::value,T>::value> {};
    
    template<typename T> struct is_raw_init_allow
        : std::integral_constant<bool,  !std::is_default_constructible<T>::value &&
                                        is_pod_like<T>::value> {};
    
    template<typename T> struct can_create_by_default
        : std::integral_constant<bool,  std::is_default_constructible<T>::value ||
                                        is_pod_like<T>::value> {};
    
    template<bool,typename T> struct map_type_check : std::integral_constant<bool, false>{using type = T;};
    template<typename T> struct map_type_check<true,T> : std::integral_constant<bool, !is_sql_acceptable<typename T::mapped_type>::value && !is_container<typename T::mapped_type>::value> {using type = typename T::mapped_type;};
    template<bool,typename T> struct container_type_check : std::integral_constant<bool, false>{using type = T;};
    template<typename T> struct container_type_check<true,T> : std::integral_constant<bool, !is_sql_acceptable<typename T::value_type>::value && !is_container<typename T::value_type>::value> {using type = typename T::value_type;};
    template<bool,typename T> struct optional_type_check : std::integral_constant<bool, false>{using type = T;};
    template<typename T> struct optional_type_check<true,T> : std::integral_constant<bool, true> {using type = typename T::value_type;};

    template<typename T> struct types_selector
    {
        using T0 = typename is_pointer<T>::type;
        using T1 = typename optional_type_check<is_kind_of_optional<T0>::value,T0>::type;
        using T2 = typename std::conditional<map_type_check<is_any_map<T1>::value,T1>::value, typename map_type_check<is_any_map<T1>::value,T1>::type, T1>::type;
        using T3 = typename std::conditional<container_type_check<is_container<T2>::value,T2>::value, typename container_type_check<is_container<T2>::value, T2>::type, T2>::type;
        static_assert(!std::is_same<T3, void>::value,"No acceptable type selected");
        using type = typename is_pointer<T3>::type;
        static constexpr size_t destination_id() {return typeid(type).hash_code();}
    };
    
    template<bool,typename T> struct map_type_check_for_trivial : std::integral_constant<bool, false>{};
    template<typename T> struct map_type_check_for_trivial<true,T> : std::integral_constant<bool, is_sql_acceptable<typename T::mapped_type>::value && is_sql_acceptable<typename T::key_type>::value> {};
    template<bool,typename T> struct containers_type_check_for_trivial : std::integral_constant<bool, false>{};
    template<typename T> struct containers_type_check_for_trivial<true,T> : std::integral_constant<bool, is_sql_acceptable<typename T::value_type>::value> {};
    template<typename T> struct is_trivial_map : std::integral_constant<bool,  map_type_check_for_trivial<is_any_map<T>::value,T>::value> {};
    template<typename T> struct is_trivial_container : std::integral_constant<bool, containers_type_check_for_trivial<is_container<T>::value,T>::value> {};

};

#endif /* sb_traits_h */
