//
//  example44.hpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 30/07/2026.
//  Copyright © 2026 DataArt. All rights reserved.
//

#pragma once
#ifndef example44_hpp
#define example44_hpp

#include "sqlcppbridge.h"

class Case44;
using Case44Container = std::vector<Case44>;
using Case44OptionalInternal = sql_bridge::optional_value< std::chrono::time_point<std::chrono::system_clock> >;
using Case44OptionalStd = std::optional<std::size_t>;

class Case44
{
    DECLARE_SQL_ACCESS(Case44);
public:
    Case44(std::size_t i = 0)
        : eid_(i)
    {
        if (i%2)
            optional_ts_ = std::chrono::system_clock::now();
        else
            value_ = i;
    }
    inline bool operator == (Case44 const& rv) const
    {
        return  eid_==rv.eid_ &&
                optional_ts_==rv.optional_ts_ &&
                value_==rv.value_;
    }
private:
    std::size_t eid_;
    Case44OptionalInternal optional_ts_;
    Case44OptionalStd value_;
};

#endif /* example44_hpp */
