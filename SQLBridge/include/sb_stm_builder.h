//
//  sb_stm_builder.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 10/10/2018.
//  Copyright © 2018 DataArt.
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
#ifndef sb_stm_builder_h
#define sb_stm_builder_h

#include "sb_data_section.h"

namespace sql_bridge
{
    class suffix_bare;
    typedef std::shared_ptr<suffix_bare> suffix_bare_ptr;
    typedef std::vector<suffix_bare_ptr> suffixes_container;
    enum class e_weight {BASE, WHERE, ORDER, LIMIT};
    enum class e_simple_operator {OR,AND};

    class suffix_bare
    {
    public:
        inline bool operator < (suffix_bare const& rv) const {return weight_<rv.weight_;}
        inline e_weight weight() const {return weight_;}
        virtual ~suffix_bare() {};
        
        virtual std::string general(data_sections_ptr) const = 0;
        virtual std::string build(data_sections_ptr) const = 0;
        virtual std::string repeat_delimiter() const {return "";}
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
        
        std::string general(data_sections_ptr data) const override {return data->order_by(field_);}
        std::string build(data_sections_ptr data) const override {return desc_?data->order_desc(field_):data->order_asc(field_);}
        std::string repeat_delimiter() const override {return ",";}
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
        std::string general(data_sections_ptr data) const override {return data->limit(count_);}
        std::string build(data_sections_ptr data) const override {return offset_?data->limit_offset(offset_):"";}
    private:
        size_t count_;
        size_t offset_;
    };
    
    class suffix_where : public suffix_bare
    {
    public:
        suffix_where(std::string const& fld, std::string const& cond)
            : suffix_bare(fld,e_weight::WHERE)
            , condition_(cond)
            {}
        std::string general(data_sections_ptr data) const override {return data->where();}
        std::string build(data_sections_ptr data) const override {return data->where(field_,condition_);}
    private:
        std::string condition_;
    };
    
    class suffix_simple_operator : public suffix_bare
    {
    public:
        suffix_simple_operator(e_simple_operator op)
            : suffix_bare("",e_weight::WHERE)
            , op_(op)
            {}
        std::string general(data_sections_ptr) const override {return "";}
        std::string build(data_sections_ptr data) const override
        {
            switch (op_)
            {
                case e_simple_operator::OR:     return data->operator_or();
                case e_simple_operator::AND:    return data->operator_and();
            }
            return "";
        }
    private:
        e_simple_operator op_;
    };
  
    class suffix_between : public suffix_bare
    {
    public:
        suffix_between(std::string const& fld, std::string const& from, std::string const& to, bool nf)
            : suffix_bare(fld, e_weight::WHERE)
            , from_(from)
            , to_(to)
            , flag_not_(nf)
            {}
        std::string general(data_sections_ptr data) const override {return data->where();}
        std::string build(data_sections_ptr data) const override
        {
            return flag_not_
                ? data->where_not_between(field_, from_, to_)
                : data->where_between(field_, from_, to_);
        }
    private:
        std::string from_,to_;
        bool flag_not_;
    };

    class suffix_between_rev : public suffix_bare
    {
    public:
        suffix_between_rev(std::string const& fld, std::string const& fld2, std::string const& val, bool nf)
            : suffix_bare(fld, e_weight::WHERE)
            , field2_(fld2)
            , val_(val)
            , flag_not_(nf)
            {}
        std::string general(data_sections_ptr data) const override {return data->where();}
        std::string build(data_sections_ptr data) const override
        {
            return flag_not_
                ? data->where_not_between(val_, field_, field2_)
                : data->where_between(val_, field_, field2_);
        }
    private:
        std::string field2_,val_;
        bool flag_not_;
    };

    class suffix_where_in : public suffix_bare
    {
    public:
        suffix_where_in(std::string const& fld, std::string const& val, bool nf)
            : suffix_bare(fld,e_weight::WHERE)
            , values_(val)
            , flag_not_(nf)
            {}
        std::string general(data_sections_ptr data) const override {return data->where();}
        std::string build(data_sections_ptr data) const override
        {
            return flag_not_
                ? data->where_not_in(field_,values_)
                : data->where_in(field_,values_);
        }
    private:
        std::string values_;
        bool flag_not_;
    };

};

#endif /* sb_stm_builder_h */
