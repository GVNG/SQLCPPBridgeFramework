//
//  example1.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 12/03/16.
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

#include "example1.h"


DEFINE_SQL_TABLE(main, Case1)
{
    bind("ID",          &Case1::id_,            e_db_index_type::Unique),
    bind("NAME",        &Case1::name_),
    bind("VALUE",       &Case1::value_),
    bind("RELATIVE",    &Case1::rel_info_),
};

DEFINE_SQL_TABLE(related, Case1Related)
{
    bind("VAL",         &Case1Related::test_),
    bind("ATTACH",      &Case1Related::attaches_),
};

DEFINE_SQL_TABLE(attaches, Case1Attach)
{
    bind("VAL",         &Case1Attach::attached_value_),
};

DEFINE_SQL_DATABASE(case1 /*name of context*/, 1 /*current version*/, Case1, Case1Related, Case1Attach)::upgrade_structure(size_t from, size_t to)
{
    // ------------------------------------------------------------------------------------
    // you can place here the upgrade script from the 'from' to the 'to' version
    // something like below, or whatever SQL statements you want
    //
    //if (from<=2 && to>2)
    //    execute("CREATE INDEX IF NOT EXISTS BLAH_BLAH_INDEX ON MY_TABLE (BLAH_BLAH_ID)");
    // ------------------------------------------------------------------------------------
}
