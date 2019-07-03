//
//  t_misc.cpp
//  Tests
//
//  Created by Roman Makhnenko on 02/07/2019.
//  Copyright © 2019 DataArt. All rights reserved.
//

#include "t_db_fixture.h"

TEST(Core,Misc)
{
    std::chrono::system_clock::time_point ts = std::chrono::system_clock::from_time_t(1554188000);

    std::string tt = sql_bridge::to_string() << ts;
    ASSERT_EQ(tt,"1554188000");
}
