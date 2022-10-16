//
//  example40.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 16/10/2022.
//  Copyright © 2022 DataArt. All rights reserved.
//

#ifndef example40_h
#define example40_h

#include "sqlcppbridge.h"

class Case40;
typedef std::vector<Case40> Case40Container;

class Case40
{
    DECLARE_SQL_ACCESS(Case40);
public:
    Case40()
        : key_(0)
        {};
    Case40(size_t w)
        : key_(0)
        , ts_(std::chrono::system_clock::now())
        , log_string_(sql_bridge::to_string() << "Log from " << w << " at " << ts_)
    {};
    size_t key_;
    std::chrono::time_point<std::chrono::system_clock> ts_;
    std::string log_string_;
};

#endif /* example40_h */
