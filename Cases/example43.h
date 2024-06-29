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
    Case43()
        {};
private:
    std::chrono::system_clock::time_point ts_;
    std::chrono::seconds delta_;
};


#endif /* example43_hpp */
