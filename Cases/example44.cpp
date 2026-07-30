//
//  example44.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 30/07/2026.
//  Copyright © 2026 DataArt. All rights reserved.
//

#include "example44.h"

DEFINE_SQL_TABLE(root,Case44)
{
    bind("id",                  &Case44::eid_,          sql_bridge::e_db_index_type::Unique),
    bind("ts",                  &Case44::optional_ts_),
    bind("val",                 &Case44::value_),
};

DEFINE_SQL_DATABASE(case44,1,Case44)::upgrade_structure(size_t from, size_t to)
{
    // ------------------------------------------------------------------------------------
    // you can place here the upgrade script from the 'from' to the 'to' version
    // something like below, or whatever SQL statements you want
    //
    //if (from<=2 && to>2)
    //    execute("CREATE INDEX IF NOT EXISTS BLAH_BLAH_INDEX ON MY_TABLE (BLAH_BLAH_ID)");
    // ------------------------------------------------------------------------------------
}
