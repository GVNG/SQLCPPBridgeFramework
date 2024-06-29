//
//  example43.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 29/06/2024.
//  Copyright © 2024 DataArt. All rights reserved.
//

#include "example43.h"

DEFINE_SQL_TABLE(root,Case43)
{
    bind("id",                  &Case43::db_id_,        sql_bridge::e_db_index_type::PrimaryKey),
    bind("ts",                  &Case43::ts_),
    bind("st",                  &Case43::st_ts_),
    bind("delta",               &Case43::delta_),
};

DEFINE_SQL_DATABASE(case43,1,Case43)::upgrade_structure(size_t from, size_t to)
{
    // ------------------------------------------------------------------------------------
    // you can place here the upgrade script from the 'from' to the 'to' version
    // something like below, or whatever SQL statements you want
    //
    //if (from<=2 && to>2)
    //    execute("CREATE INDEX IF NOT EXISTS BLAH_BLAH_INDEX ON MY_TABLE (BLAH_BLAH_ID)");
    // ------------------------------------------------------------------------------------
}
