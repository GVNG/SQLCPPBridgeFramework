//
//  t_case41.cpp
//  Tests
//
//  Created by Roman Makhnenko on 15/11/2022.
//  Copyright © 2022 DataArt. All rights reserved.
//

#include "t_db_fixture.h"
#include "example41.h"

TEST_F(DBFixture, Case41)
{
    sql_bridge::context cont(storage()["case41"]);
    Case41Container dst,src(10,Case41(200)),dst2;
    cont.replace(src);
    cont.load(dst);
    ASSERT_EQ(src,dst);
    cont.save(Case41());
    cont.load(dst2);
    ASSERT_EQ(dst2.back().data_.empty(), true);
}

