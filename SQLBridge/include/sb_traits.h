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
#include <unordered_set>
#include <deque>
#include <vector>
#include <array>
#include <list>
#include <queue>
#include <tuple>
#include <typeinfo>

namespace sql_bridge
{
    template<typename T> struct is_key_mapped
    {
    private:
        typedef char                      yes;
        typedef struct { char array[2]; } no;
        
        template<typename C> static yes ft(typename C::key_type*);
        template<typename C> static no  ft(...);
        template<typename C> static yes st(typename C::mapped_type*);
        template<typename C> static no  st(...);
    public:
        static constexpr const bool value = sizeof(ft<T>(0)) == sizeof(yes) &&
                                            sizeof(st<T>(0)) == sizeof(yes);
    };
    
    template<typename T> struct is_kind_of_pair
    {
    private:
        typedef char                      yes;
        typedef struct { char array[2]; } no;
        
        template<typename C> static yes ft(typename C::first_type*);
        template<typename C> static yes st(typename C::second_type*);
        template<typename C> static no  ft(...);
        template<typename C> static no  st(...);
    public:
        static constexpr const bool value = sizeof(ft<T>(0)) == sizeof(yes) &&
                                            sizeof(st<T>(0)) == sizeof(yes);
    };

    template<typename T> struct is_kind_of_set
        : std::integral_constant<bool,  std::is_base_of<std::set<typename T::value_type>, T>::value ||
                                        std::is_base_of<std::multiset<typename T::value_type>, T>::value ||
                                        std::is_base_of<std::unordered_set<typename T::value_type>, T>::value>
    {
    };

    template<typename T> struct has_const_iterator
    {
    private:
        typedef char                      yes;
        typedef struct { char array[2]; } no;
        
        template<typename C> static yes test(typename C::const_iterator*);
        template<typename C> static no  test(...);
    public:
        static constexpr const bool value = sizeof(test<T>(0)) == sizeof(yes);
        typedef T type;
    };
    
    template <typename T> struct has_begin_end
    {
    private:
        typedef char                      yes;
        typedef struct { char array[2]; } no;
        
        template<typename C> static yes test_b(typename std::enable_if<
                                               std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
                                               typename C::const_iterator(C::*)() const>::value, void>::type*);
        template<typename C> static yes test_e(typename std::enable_if<
                                               std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
                                               typename C::const_iterator(C::*)() const>::value, void>::type*);
        template<typename C> static no test_b(...);
        template<typename C> static no test_e(...);
        
    public:
        static constexpr bool const value = sizeof(test_b<T>(0)) == sizeof(yes) &&
                                            sizeof(test_e<T>(0)) == sizeof(yes);
        typedef T type;
    };
    
    template<typename T> struct is_any_container
        : std::integral_constant<bool,  has_const_iterator<T>::value &&
                                        has_begin_end<T>::value>
    {
    };
    
    template<typename T> struct is_pair
        : std::integral_constant<bool, is_kind_of_pair<T>::value>
    {
    };
    
    template<typename T> struct is_map
        : std::integral_constant<bool,  is_any_container<T>::value &&
                                        is_key_mapped<T>::value>
    {
    };
    
    template<bool,typename T> struct check_for_set : std::integral_constant<bool, false>{};
    template<typename T> struct check_for_set<true,T> : std::integral_constant<bool, is_kind_of_set<T>::value>{};

    template<typename T> struct is_set
        : std::integral_constant<bool,  check_for_set<is_any_container<T>::value,T>::value>
    {
    };
    
    template<typename T> struct is_container
        : std::integral_constant<bool,  is_any_container<T>::value &&
                                        !std::is_base_of<std::string,T>::value &&
                                        !is_map<T>::value>
    {
    };
    
    template<typename T> struct is_convertible_to_text
        : std::integral_constant<bool, std::is_base_of<std::string,T>::value>
    {
    };
    
    template<typename T> struct is_convertible_to_int
        : std::integral_constant<bool,  std::is_integral<T>::value ||
                                        std::is_enum<T>::value>
    {
    };
    
    template<typename T> struct is_convertible_to_float
        : std::integral_constant<bool,  std::is_floating_point<T>::value>
    {
    };

    template<typename T> struct is_convertible_to_bool
        : std::integral_constant<bool,  std::is_same<bool, T>::value>
    {
    };
    
    template<typename T> struct is_chrono
        : std::integral_constant<bool,  std::is_same<T, std::chrono::system_clock::time_point>::value ||
                                        std::is_same<T, std::chrono::high_resolution_clock::time_point>::value ||
                                        std::is_same<T, std::chrono::steady_clock::time_point>::value>
    {
    };
    
    template<typename T> struct is_sql_acceptable
        : std::integral_constant<bool,  is_convertible_to_int<T>::value ||
                                        is_convertible_to_float<T>::value ||
                                        is_convertible_to_text<T>::value ||
                                        is_chrono<T>::value>
    {
    };
    
    template<typename T> struct is_back_pushable_container
        : std::integral_constant<bool,  std::is_base_of<std::vector<typename T::value_type>, T>::value ||
                                        std::is_base_of<std::deque<typename T::value_type>, T>::value ||
                                        std::is_base_of<std::list<typename T::value_type>, T>::value>
    {
    };
    
    template<typename T> struct sql_array_size : std::extent<T>
    {
    };
    
    template<typename T, size_t N> struct sql_array_size<std::array<T,N> > : std::tuple_size<std::array<T,N> >
    {
    };
    
    template<typename T> struct is_kind_of_array
        : std::integral_constant<bool,  is_any_container<T>::value &&
                                        sql_array_size<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::value>
    {
    };

    template<bool,typename T> struct check_container_of_containers : std::integral_constant<bool, false>{};
    template<typename T> struct check_container_of_containers<true,T> : std::integral_constant<bool, is_any_container<typename T::value_type>::value>{};
    template<bool,typename T> struct check_map_of_containers : std::integral_constant<bool, false>{};
    template<typename T> struct check_map_of_containers<true,T> : std::integral_constant<bool, is_any_container<typename T::mapped_type>::value>{};

    template<typename T> struct is_container_of_containers
        : std::integral_constant<bool,  check_container_of_containers<is_container<T>::value,T>::value ||
                                        check_map_of_containers<is_map<T>::value,T>::value>
    {
    };
    
    template<typename T> struct is_raw_init_allow
        : std::integral_constant<bool,  !std::is_default_constructible<T>::value &&
                                        std::is_pod<T>::value>
    {
    };
    
    template<typename T> struct can_create_by_default
        : std::integral_constant<bool,  std::is_default_constructible<T>::value ||
                                        std::is_pod<T>::value>
    {
    };
    
    template<bool,typename T> struct map_type_check : std::integral_constant<bool, false>{typedef T type;};
    template<typename T> struct map_type_check<true,T> : std::integral_constant<bool, !is_sql_acceptable<typename T::mapped_type>::value && !is_container<typename T::mapped_type>::value> {typedef typename T::mapped_type type;};
    template<bool,typename T> struct container_type_check : std::integral_constant<bool, false>{typedef T type;};
    template<typename T> struct container_type_check<true,T> : std::integral_constant<bool, !is_sql_acceptable<typename T::value_type>::value && !is_container<typename T::value_type>::value> {typedef typename T::value_type type;};
    
    template<typename T> struct types_selector
    {
        typedef typename std::conditional<map_type_check<is_map<T>::value,T>::value, typename map_type_check<is_map<T>::value,T>::type, T>::type T1;
        typedef typename std::conditional<container_type_check<is_container<T>::value,T>::value, typename container_type_check<is_container<T>::value, T>::type, T1>::type T2;
        typedef T2 type;
        static_assert(!std::is_same<type, void>::value,"No acceptable type selected");
        static size_t destination_id() {return typeid(type).hash_code();}
    };
    
    template<bool,typename T> struct map_type_check_for_trivial : std::integral_constant<bool, false>{};
    template<typename T> struct map_type_check_for_trivial<true,T> : std::integral_constant<bool, is_sql_acceptable<typename T::mapped_type>::value && is_sql_acceptable<typename T::key_type>::value> {};
    template<bool,typename T> struct containers_type_check_for_trivial : std::integral_constant<bool, false>{};
    template<typename T> struct containers_type_check_for_trivial<true,T> : std::integral_constant<bool, is_sql_acceptable<typename T::value_type>::value> {};
    
    template<typename T> struct is_trivial_map
        : std::integral_constant<bool,  map_type_check_for_trivial<is_map<T>::value,T>::value>
    {
    };

    template<typename T> struct is_trivial_container
        : std::integral_constant<bool, containers_type_check_for_trivial<is_container<T>::value,T>::value>
    {
    };

};

#endif /* sb_traits_h */
