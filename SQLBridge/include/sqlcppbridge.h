//
//  sqlcppbridge.h
//  this is the root file for the SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 11/03/2016.
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
#ifndef sql_cpp_bridge_h
#define sql_cpp_bridge_h

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4068)
#endif

#include "sb_exceptions.h"
#include "sb_local_storage.h"
#include "sb_context.h"
#include "sb_data_section_descriptor.h"
#include "sb_adapter_sqlite.h"

#define DEFINE_SQL_DATABASE_EXT(STRAT,NAME,VERSION,...)\
    struct __t_sect_name_##NAME##_selector{};\
    static sql_bridge::_t_data_section_descriptors_creator<STRAT,__t_sect_name_##NAME##_selector,__VA_ARGS__> const g_db_##NAME##_init(#NAME,VERSION);\
    template<> void sql_bridge::_t_data_section_descriptor<STRAT,__t_sect_name_##NAME##_selector,__VA_ARGS__>::updater

#define DEFINE_SQL_TABLE_EXT(STRAT,NAME,CLASSNAME)\
    template<> std::string const sql_bridge::_t_class_descriptor<STRAT,CLASSNAME>::table_name_ = #NAME;\
    template<> sql_bridge::class_descriptors_container const sql_bridge::_t_class_descriptor<STRAT,CLASSNAME>::members_ =

#define DEFINE_SQL_TRIVIAL_TABLE_EXT(STRAT,NAME,CLASSNAME)\
    template<> std::string const sql_bridge::_t_container_descriptor<STRAT,CLASSNAME>::table_name_ = #NAME;\
    template<> sql_bridge::class_descriptors_container const sql_bridge::_t_container_descriptor<STRAT,CLASSNAME>::members_ =\
    sql_bridge::_t_container_descriptor<STRAT,CLASSNAME>::create_members();

#define DECLARE_SQL_ACCESS_EXT(STRAT,CLASSNAME)\
    friend class sql_bridge::_t_class_descriptor< STRAT,CLASSNAME >;

#define DECLARE_SQL_INHERITANCE_ACCESS_EXT(STRAT,CLASSNAME,CLASSNAME_PARENT)\
    friend class sql_bridge::_t_inheritance_descriptor< STRAT,CLASSNAME,CLASSNAME_PARENT >;

// defines for the default strategy (sqlite)

#define DEFINE_SQL_DATABASE(NAME,VERSION,...)\
    DEFINE_SQL_DATABASE_EXT(sql_bridge::sqlite_adapter,NAME,VERSION,__VA_ARGS__)

#define DEFINE_SQL_TABLE(NAME,CLASSNAME)\
    DEFINE_SQL_TABLE_EXT(sql_bridge::sqlite_adapter,NAME,CLASSNAME)

#define DEFINE_SQL_TRIVIAL_TABLE(NAME,CLASSNAME)\
    DEFINE_SQL_TRIVIAL_TABLE_EXT(sql_bridge::sqlite_adapter,NAME,CLASSNAME)

#define DECLARE_SQL_ACCESS(CLASSNAME)\
    DECLARE_SQL_ACCESS_EXT(sql_bridge::sqlite_adapter,CLASSNAME)

#define DECLARE_SQL_INHERITANCE_ACCESS(CLASSNAME,CLASSNAME_PARENT)\
    DECLARE_SQL_INHERITANCE_ACCESS_EXT(sql_bridge::sqlite_adapter,CLASSNAME,CLASSNAME_PARENT)

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif /* sql_cpp_bridge_h */
