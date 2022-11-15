//
//  example41.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 14/11/2022.
//  Copyright © 2022 DataArt. All rights reserved.
//

#include "example41.h"

DEFINE_SQL_TABLE(root,Case41)
{
    bind("ID",      &Case41::key_,           e_db_index_type::PrimaryKey),
    bind("DAT",     &Case41::data_),
};

DEFINE_SQL_DATABASE(case41,1,Case41)::upgrade_structure(size_t from, size_t to)
{
    // ------------------------------------------------------------------------------------
    // you can place here the upgrade script from the 'from' to the 'to' version
    // something like below, or whatever SQL statements you want
    //
    //if (from<=2 && to>2)
    //    execute("CREATE INDEX IF NOT EXISTS BLAH_BLAH_INDEX ON MY_TABLE (BLAH_BLAH_ID)");
    // ------------------------------------------------------------------------------------
}
