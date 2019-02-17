//
//  t_db_fixture.h
//  UnitTests
//
//  Created by Roman Makhnenko on 31/12/2018.
//  Copyright © 2018, DataArt.
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

#pragma once
#ifndef t_db_fixture_h
#define t_db_fixture_h

#include "gtest/gtest.h"
#include "sqlcppbridge.h"
#include <ftw.h>
#include <cmath>

static char const* db_path = "./DBTest";

class DBFixture : public ::testing::Test
{
    typedef sql_bridge::local_storage<sql_bridge::sqlite_adapter> _t_storage;
protected:
    DBFixture()
    {
        rmrf(db_path);
		#ifndef _WIN32
            mkdir(db_path, 0777);
        #else
            mkdir(db_path);
        #endif
        storage_ = std::make_unique<_t_storage>(db_path);
    }
    ~DBFixture() override
    {
        storage_.reset();
        rmrf(db_path);
    }
    inline _t_storage& storage() {return *storage_.get();}
    
private:
    std::unique_ptr<_t_storage> storage_;

    static int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
    {
        int rv = remove(fpath);
        if (rv) perror(fpath);
        return rv;
    }
    
    int rmrf(char const* path) {return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);}
    
};

#endif /* t_db_fixture_h */
