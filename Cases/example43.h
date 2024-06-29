//
//  example43.hpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 29/06/2024.
//  Copyright © 2024 DataArt. All rights reserved.
//

#ifndef example43_hpp
#define example43_hpp

#include "sqlcppbridge.h"

class Case43;

typedef std::vector<Case43> Case43Container;

class Case43
{
    DECLARE_SQL_ACCESS(Case43);
public:
    Case43(size_t i = 0)
        : ts_(std::chrono::system_clock::now())
        , delta_(i)
        , st_ts_(std::chrono::steady_clock::now()+delta_)
        {};
private:
    size_t db_id_;
    std::chrono::system_clock::time_point ts_;
    std::chrono::seconds delta_;
    std::chrono::steady_clock::time_point st_ts_;
};


#endif /* example43_hpp */
