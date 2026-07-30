//
//  t_case44.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 30/07/2026.
//  Copyright © 2026 DataArt. All rights reserved.
//

#include "t_db_fixture.h"
#include "example44.h"

TEST_F(DBFixture, Case44)
{
    sql_bridge::context cont(storage()["case44"]);
    Case44Container dst,src;
    for(int i=0; i<100; ++i)
        src.push_back(Case44(i));
    cont.save(src);
    cont.load(dst);
    ASSERT_EQ(src,dst);
}
