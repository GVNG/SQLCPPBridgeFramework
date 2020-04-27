//
//  sb_class_descriptor.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 27/03/2016.
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

#include "sb_class_descriptor.h"

namespace sql_bridge
{
    bool class_descriptor::has_child(size_t ch) const
    {
        for(auto const& mb : members())
            if (mb->has_child(ch))
                return true;
        for(auto const& inh : inheritances())
            if (inh->has_child(ch))
                return true;
        return type_id()==ch || reference_description().first==ch;
    }
    
    bool class_descriptor::has_unique_key() const
    {
        for(auto const& mb : members())
            if (mb->index_type()==e_db_index_type::Unique)
                return true;
        return false;
    }
    
    std::string const& class_descriptor::sql_type_for_unique_key() const
    {
        for(auto const& mb : members())
            if (mb->index_type()==e_db_index_type::Unique)
                return mb->sql_type();
        static std::string const def;
        return def;
    }
    
    sql_value class_descriptor::sql_value_for_unique_key() const
    {
        for(auto const& mb : members())
            if (mb->index_type()==e_db_index_type::Unique)
                return mb->try_cast();
        return sql_value();
    }
    
    std::string const& class_descriptor::field_name_for_unique_key() const
    {
        for(auto const& mb : members())
            if (mb->index_type()==e_db_index_type::Unique)
                return mb->field_name();
        static std::string const def;
        return def;
    }
    
};
