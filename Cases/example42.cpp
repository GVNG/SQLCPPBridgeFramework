//
//  example42.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 20/01/2023.
//  Copyright © 2023 DataArt. All rights reserved.
//

#include "example42.h"

DEFINE_SQL_TABLE(root,Case42)
{
    bind("id",                  &Case42::db_id_,        sql_bridge::e_db_index_type::PrimaryKey),
    bind_recursive("children",  &Case42::children_),
    bind("val",                 &Case42::val_),
};

DEFINE_SQL_TABLE(rec,sql_bridge::recursion_ref<Case42>)
{
    bind("id",                  &sql_bridge::recursion_ref<Case42>::db_id_,        sql_bridge::e_db_index_type::PrimaryKey),
    bind_recursive("children",  &sql_bridge::recursion_ref<Case42>::children_),
    bind("val",                 &sql_bridge::recursion_ref<Case42>::val_),
};

DEFINE_SQL_DATABASE(case42,1,Case42)::upgrade_structure(size_t from, size_t to)
{
    // ------------------------------------------------------------------------------------
    // you can place here the upgrade script from the 'from' to the 'to' version
    // something like below, or whatever SQL statements you want
    //
    //if (from<=2 && to>2)
    //    execute("CREATE INDEX IF NOT EXISTS BLAH_BLAH_INDEX ON MY_TABLE (BLAH_BLAH_ID)");
    // ------------------------------------------------------------------------------------
}

void Case42::make_children(size_t nsz)
{
    val_ = nsz;
    children_ = Case42Container(nsz);
}
