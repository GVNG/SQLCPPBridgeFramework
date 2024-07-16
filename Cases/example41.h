//
//  example41.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 14/11/2022.
//  Copyright © 2022 DataArt. All rights reserved.
//

#ifndef example41_h
#define example41_h

#include "sqlcppbridge.h"

class Case41;
using Case41Container = std::vector<Case41>;

class Case41
{
    DECLARE_SQL_ACCESS(Case41);
public:
    Case41()
        : key_(0)
        {};
    Case41(size_t w)
        : key_(0)
    {
        data_.resize(w);
        memset(data_.data(), static_cast<sql_bridge::bytes_block::type>(w), w);
    };
    inline bool operator == (Case41 const& rv) const {return data_==rv.data_;}
    
    size_t key_;
    sql_bridge::bytes_block data_;
};


#endif /* example41_h */
