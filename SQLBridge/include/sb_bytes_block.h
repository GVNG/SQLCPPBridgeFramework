//
//  sb_bytes_block.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 18/10/2022.
//  Copyright © 2022, DataArt.
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
#ifndef sb_bytes_block_h
#define sb_bytes_block_h

#include <memory>
#include <cassert>
#include <cstring>

namespace sql_bridge
{
    template<typename T> class _t_data_block
    {
    public:
        using type = T;
        using t_buffer = std::shared_ptr<type>;
        
        _t_data_block()
            : elements_(0)
            , allocated_(0)
            {};
        explicit _t_data_block(size_t eln) // elements number
            : buffer_(eln?new type[eln]:nullptr)
            , elements_(buffer_?eln:0)
            , allocated_(elements_)
            {}
        _t_data_block(void const* src, size_t sz) // memory buffer, size in bytes(!!!)
            : _t_data_block(sz/sizeof(type))
        {
            assert(elements_*sizeof(type)==sz);
            if (buffer_!=nullptr)
                std::memcpy(buffer_.get(),src,sz);
        }
        inline bool empty() const {return elements_==0 || buffer_==nullptr;}
        inline type const* data() const {return buffer_.get();}
        inline type* data() {return buffer_.get();}
        inline size_t elements() const {return elements_;}
        inline size_t size() const {return elements_*sizeof(type);}
        inline size_t allocated() const {return allocated_*sizeof(type);}
        
        inline bool operator < (_t_data_block const& rv) const
        {
            if (elements_!=rv.elements_) return elements_<rv.elements_;
            if (!elements_) return false;
            return std::memcmp(buffer_.get(), rv.buffer_.get(), size())<0;
        }
        inline bool operator == (_t_data_block const& rv) const
        {
            if (elements_!=rv.elements_) return false;
            if (!elements_) return true;
            return std::memcmp(buffer_.get(), rv.buffer_.get(), size())==0;
        }
        
        void resize(size_t newel) // elements number
        {
            assert(newel);
            if (newel<=allocated_)
                elements_ = newel;
            else
            {
                t_buffer nb(new type[newel]);
                if (elements_)
                    std::memcpy(nb.get(),buffer_.get(),elements_*sizeof(type));
                std::swap(buffer_,nb);
                allocated_ = newel;
                elements_ = newel;
            }
        }
        
        void append(void const* src, size_t sz) // memory buffer, size in bytes(!!!)
        {
            size_t newel = elements_+sz/sizeof(type);
            assert(newel*sizeof(type)==size()+sz);
            if (newel<=allocated_)
            {
                std::memcpy(data()+elements(),src,sz);
                elements_ = newel;
            }
            else
            {
                size_t newalloc = newel*2;
                t_buffer nb(new type[newalloc]);
                if (elements_)
                    std::memcpy(nb.get(),buffer_.get(),elements_*sizeof(type));
                std::memcpy(nb.get()+elements(),src,sz);
                std::swap(buffer_,nb);
                allocated_ = newalloc;
                elements_ = newel;
            }
        }
        
        void trim()
        {
            if (elements_==allocated_) return;
            if (elements_)
            {
                t_buffer nb(new type[elements_]);
                std::memcpy(nb.get(),buffer_.get(),elements_*sizeof(type));
                std::swap(buffer_,nb);
            }
            else
                buffer_ = t_buffer();
            allocated_ = elements_;
        }
        
    private:
        t_buffer buffer_;
        size_t elements_;
        size_t allocated_;
    };
    using bytes_block = _t_data_block<unsigned char>;
};

#endif /* sb_bytes_block_h */
