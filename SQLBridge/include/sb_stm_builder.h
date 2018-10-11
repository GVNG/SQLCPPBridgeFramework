//
//  sb_stm_builder.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 10/10/2018.
//  Copyright © 2018 DataArt. All rights reserved.
//

#pragma once
#ifndef sb_stm_builder_h
#define sb_stm_builder_h

#include "sb_data_section.h"

namespace sql_bridge
{
    class suffix_bare;
    typedef std::shared_ptr<suffix_bare> suffix_bare_ptr;
    typedef std::vector<suffix_bare_ptr> suffixes_container;
    enum class e_weight {BASE, WHERE, GROUP, ORDER, LIMIT};

    class suffix_bare
    {
    public:
        inline bool operator < (suffix_bare const& rv) const {return weight_<rv.weight_;}
        inline e_weight weight() const {return weight_;}
        virtual ~suffix_bare() {};
        
        virtual std::string general(data_sections_ptr) const = 0;
        virtual std::string build(data_sections_ptr) const = 0;
    protected:
        suffix_bare(std::string const& fld, e_weight wh)
            : field_(fld)
            , weight_(wh)
            {}
        std::string field_;
        e_weight weight_;
    };
    
    class suffix_order : public suffix_bare
    {
    public:
        suffix_order(std::string const& fld, bool ds)
            : suffix_bare(fld, e_weight::ORDER)
            , desc_(ds)
            {}
        
        std::string general(data_sections_ptr data) const {return data->order_by(field_);}
        std::string build(data_sections_ptr data) const {return desc_?data->order_desc(field_):data->order_asc(field_);}
    private:
        bool desc_;
    };
    
    class suffix_limit : public suffix_bare
    {
    public:
        suffix_limit(size_t cnt, size_t ofs)
            : suffix_bare("",e_weight::LIMIT)
            , count_(cnt)
            , offset_(ofs)
            {}
        std::string general(data_sections_ptr data) const {return data->limit(count_);}
        std::string build(data_sections_ptr data) const {return offset_?data->limit_offset(offset_):"";}
    private:
        size_t count_;
        size_t offset_;
    };
    
};

#endif /* sb_stm_builder_h */
