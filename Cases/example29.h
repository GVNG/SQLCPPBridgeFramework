//
//  example29.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 08/01/2020.
//  Copyright © 2020 DataArt.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//      * Neither the name of the DataArt nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL DataArt BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef example29_h
#define example29_h

#include "sqlcppbridge.h"
#include <cmath>

class Case29;
typedef std::vector<Case29> Case29Container;

class Case29
{
    DECLARE_SQL_ACCESS(Case29);
public:
    Case29() {};
    Case29(int i)
        : opt_i_(i)
    {
        if (i%3==1)
            opt_f_ = M_PI*i;
        if (i%3==0)
            opt_t_ = sql_bridge::to_string() << "Ts: " << i;
    }
    inline bool operator == (Case29 const& rv) const {return opt_i_==rv.opt_i_ && opt_f_==rv.opt_f_ && opt_t_==rv.opt_t_;}
private:
    sql_bridge::optional_value<int> opt_i_;
    sql_bridge::optional_value<float> opt_f_;
    sql_bridge::optional_value<std::string> opt_t_;
};

#endif /* example29_h */
