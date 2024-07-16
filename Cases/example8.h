//
//  example8.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 07/07/16.
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

#pragma once
#ifndef example8_h
#define example8_h

#include "sqlcppbridge.h"
#include <chrono>
#include <array>

using Case8Member = std::deque<float>;
using Case8Static = std::array<int,20>;

class Case8
{
    DECLARE_SQL_ACCESS(Case8);
public:
    Case8() {};
    Case8(int i)
        : item_id_(sql_bridge::to_string() << "Test: " << i)
        , member_(3,i+5.0f)
        , time_stamp_(std::chrono::system_clock::now())
    {
        buffer_.fill(i);
    }
    inline bool operator < (Case8 const& rv) const {return item_id_<rv.item_id_;}
    inline bool operator == (Case8 const& rv) const
    {
        return  item_id_==rv.item_id_ &&
                member_==rv.member_ &&
				(std::max(time_stamp_,rv.time_stamp_)-std::min(time_stamp_,rv.time_stamp_))<std::chrono::microseconds(1) &&
                buffer_==rv.buffer_;
    }

private:
    std::string item_id_;
    Case8Member member_;
    std::chrono::system_clock::time_point time_stamp_;
    Case8Static buffer_;
};

#endif /* example8_hpp */
