//
//  sb_ref_context.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 09/05/2020.
//  Copyright © 2020, DataArt.
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
#ifndef sb_ref_context_h
#define sb_ref_context_h

#include "sb_context_engine.h"

namespace sql_bridge
{
    class ref_context : private context_engine
    {
    private:
        sql_context_references_container references_;
        
    protected:
        ref_context(db_tasks_queue_interface_ptr q, data_sections_ptr ds, sql_context_references_container const& ref)
            : context_engine(q,ds)
            , references_(ref)
            {}
        ref_context() = delete;
        ref_context(ref_context const&) = delete;
        
    public:
        ref_context(ref_context&& src)
            : context_engine(std::move(src))
        {
            std::swap(references_,src.references_);
        }
        
    };
};

#endif /* sb_ref_context_h */
