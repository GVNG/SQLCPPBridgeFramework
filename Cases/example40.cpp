//
//  example40.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 16/10/2022.
//  Copyright © 2022 DataArt. All rights reserved.
//

#include "example40.h"

DEFINE_SQL_TABLE(root,Case40)
{
    bind("ID",      &Case40::key_,           e_db_index_type::PrimaryKey),
    bind("TS",      &Case40::ts_),
    bind("TX",      &Case40::log_string_),
};

DEFINE_SQL_DATABASE(case40,1,Case40)::upgrade_structure(size_t from, size_t to)
{
    // ------------------------------------------------------------------------------------
    // you can place here the upgrade script from the 'from' to the 'to' version
    // something like below, or whatever SQL statements you want
    //
    //if (from<=2 && to>2)
    //    execute("CREATE INDEX IF NOT EXISTS BLAH_BLAH_INDEX ON MY_TABLE (BLAH_BLAH_ID)");
    // ------------------------------------------------------------------------------------
}
