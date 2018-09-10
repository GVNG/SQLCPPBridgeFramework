//
//  sb_inheritance_descriptor.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 06/05/2016.
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
#ifndef sb_inheritance_descriptor_h
#define sb_inheritance_descriptor_h

#include "sb_foundation.h"

namespace sql_bridge
{
    template<typename TStrategy, typename TChild, typename TParent> class _t_inheritance_descriptor
        : public class_descriptor
    {
    public:
        class_descriptors_container const& members() const {return description_->members();}
        class_descriptors_pair reference_description() const {return class_descriptors_pair(type_id(),description_);}
        class_descriptors_pair prefix_description() const {return empty_descriptors_pair;}
        std::string const& table_name() const {static std::string const def;return def;}
        std::string const& sql_type() const {static std::string const def;return def;}
        void bind(void const*, data_update_context&) {};
        void bind_comp(void const* val, data_update_context& cont, sql_value const& extkey) {_bind_comp<TParent>(*static_cast<TChild const*>(val),cont,extkey);};
        sql_value expand(void const*) {return sql_value();}
        void read(void* dst,data_update_context& cont) {}
        void read_comp(void* dst,data_update_context& cont,sql_value const& extkey) {_read_comp<TParent>(*static_cast<TChild*>(dst),cont,extkey);}

    protected:
        _t_inheritance_descriptor(class_descriptors_ptr desc)
            : class_descriptor(typeid(TParent).hash_code())
            , description_(desc)
            {}

    private:
        // methods
#pragma mark - bind
        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value>::type _bind_comp(TParent const& el, data_update_context& cont, sql_value const& extkey) {};
        template<typename TFn> inline typename std::enable_if<is_map<TFn>::value>::type _bind_comp(TParent const& el, data_update_context& cont, sql_value const& extkey) {};
        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value && !is_map<TFn>::value>::type _bind_comp(TParent const& el, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TFn).hash_code();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,std::string()));
            ncnt->bind_comp(&el, extkey);
        };
#pragma mark - read
        template<typename TFn> inline typename std::enable_if<is_container<TFn>::value>::type _read_comp(TParent& dst, data_update_context& cont, sql_value const& extkey) {};
        template<typename TFn> inline typename std::enable_if<is_map<TFn>::value>::type _read_comp(TParent& dst, data_update_context& cont, sql_value const& extkey) {};
        template<typename TFn> inline typename std::enable_if<!is_container<TFn>::value && !is_map<TFn>::value>::type _read_comp(TParent& dst, data_update_context& cont, sql_value const& extkey)
        {
            size_t elemt = typeid(TParent).hash_code();
            data_update_context_ptr ncnt(cont.context_for_member(elemt,extkey,std::string()));
            if(ncnt->is_ok())
                ncnt->read_comp(&dst, extkey);
        };

        // data
        class_descriptors_ptr description_;
    };

};

#endif /* sb_inheritance_descriptor_h */
