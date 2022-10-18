//
//  sb_bytes_block.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 18/10/2022.
//  Copyright © 2022 DataArt. All rights reserved.
//

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
                if (allocated_)
                    std::memcpy(nb.get(),buffer_.get(),allocated_*sizeof(type));
                std::swap(buffer_,nb);
                allocated_ = newel;
                elements_ = newel;
            }
        }
    private:
        t_buffer buffer_;
        size_t elements_;
        size_t allocated_;
    };
    using bytes_block = _t_data_block<unsigned char>;
};

#endif /* sb_bytes_block_h */
