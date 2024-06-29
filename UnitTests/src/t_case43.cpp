//
//  t_case43.cpp
//  Tests
//
//  Created by Roman Makhnenko on 29/06/2024.
//  Copyright © 2024 DataArt. All rights reserved.
//

#include "t_db_fixture.h"
#include "example43.h"

TEST_F(DBFixture, Case43)
{
    sql_bridge::context cont(storage()["case43"]);
    Case43Container dst,src;
    for(int i=0; i<100; ++i)
        src.push_back(Case43(i));
    cont.replace(src);
    cont.load(dst);
    ASSERT_EQ(src,dst);
}
