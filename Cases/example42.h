//
//  example42.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 20/01/2023.
//  Copyright © 2023 DataArt. All rights reserved.
//

#ifndef example42_h
#define example42_h

#include "sqlcppbridge.h"

class Case42;
using Case42Container = std::vector<Case42>;

class Case42
{
    DECLARE_SQL_ACCESS(Case42);
    DECLARE_SQL_ACCESS(sql_bridge::recursion_ref<Case42>);
public:
    Case42()
        : db_id_(0)
        , val_(0)
        {};
    void make_children(size_t);
private:
    Case42Container children_;
    size_t db_id_;
    double val_;
};


#endif /* example42_h */
