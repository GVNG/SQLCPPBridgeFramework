//
//  example2.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 14/03/16.
//  Copyright © 2016, DataArt.
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

#include "example2.h"

DEFINE_SQL_TABLE(main,Case2)
{
    bind("ID",      &Case2::id_,          e_db_index_type::Unique),
    bind("NAME",    &Case2::name_),
    bind("VALUE",   &Case2::value_),
    bind("MEMBERS", &Case2::members_),
};

DEFINE_SQL_TABLE(member,MemberCase2)
{
    bind("VAL",     &MemberCase2::val_),
};

DEFINE_SQL_TABLE(par1,Parent1)
{
    bind("VAL",     &Parent1::val_),
};

DEFINE_SQL_TABLE(par2,Parent2)
{
    bind("VAL",     &Parent2::val_),
};

DEFINE_SQL_DATABASE(case2,1,Case2,MemberCase2,Parent1,Parent2)::upgrade_structure(size_t from, size_t to)
{
    // ------------------------------------------------------------------------------------
    // you can place here the upgrade script from the 'from' to the 'to' version
    // something like below, or whatever SQL statements you want
    //
    //if (from<=2 && to>2)
    //    execute("CREATE INDEX IF NOT EXISTS BLAH_BLAH_INDEX ON MY_TABLE (BLAH_BLAH_ID)");
    // ------------------------------------------------------------------------------------
}
