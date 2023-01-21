//
//  sb_data_section_descriptor.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 14/03/2016.
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

#ifdef DEBUG_CORE
    #include <iostream>
#endif
#include "sb_data_section_descriptor.h"
#include "sb_exceptions.h"

namespace sql_bridge
{
    
#pragma mark - desctiptors
    
    data_section_descriptors& data_section_descriptors::instance()
    {
        static data_section_descriptors instance_;
        return instance_;
    }
    
    data_section_descriptors_ptr data_section_descriptors::operator[](std::string const& name)
    {
        proc_init_queue();
        protected_data_section_descriptors_map::access lck(index_);
        data_section_descriptors_map::const_iterator pos = lck.data().find(name);
        if (pos==lck.data().end())
            throw sql_bridge_error(to_string() << "The data section with the name \"" << name << "\" doesn't exist",
                                   to_string() << "You should use \"DEFINE_SQL_DATABASE(" << name << "...\" macro somewhere in one of yours compile units");
        return pos->second;
    }
    
    void data_section_descriptors::add_data_section(std::string const& name, data_section_descriptors_ptr sect)
    {
        static std::string const rec("Use the different name");
        protected_data_section_descriptors_map::access lck(index_);
        data_section_descriptors_map::const_iterator pos = lck.data().find(name);
        if (pos!=lck.data().end())
            throw sql_bridge_error(to_string() << "The data section with the name \"" << name << "\" is duplicated", rec);
        lck.mutable_data().insert({name,sect});
    }
  
    void data_section_descriptors::proc_init_queue()
    {
        while(!init_queue_.empty())
        {
            init_queue_.front()();
            init_queue_.pop();
        }
    }
    
#pragma mark - descriptor
    
    void data_section_descriptor::prepare_relations(class_links_container& dst,
                                                    std::string const& def_internal_type,
                                                    std::string const& def_recursive_type) const
    {
        for(class_descriptors_map::const_iterator src = classes_map_.begin(); src!=classes_map_.end(); ++src)
        {
            if (src->second->used_pointers() && src->second->has_unique_key())
            {
                dst.push_back(class_link(src->second->type_id(),
                                         src->second->table_name(),
                                         ""));
                continue;
            }
            class_descriptors_map::const_iterator trg = std::find_if(classes_map_.begin(),
                                                                     classes_map_.end(),
                                                                     [&src](class_descriptors_map::value_type const& vt)
            {
                if (vt.first==src->first) return false;
                return vt.second->has_child(src->second->type_id());
            });
            if (trg==classes_map_.end())
                dst.push_back(class_link(src->second->type_id(),
                                         src->second->table_name(),
                                         ""));
        }
        for(auto& dp : dst)
            dp.update_for_key_mode(lookup_for_key_mode(dp,def_internal_type,def_recursive_type,size_t_set()));
    }

    member_for_index_ref data_section_descriptor::lookup_for_key_mode(class_link& dp,
                                                                      std::string const& def_internal_type,
                                                                      std::string const& def_recursive_type,
                                                                      size_t_set tids_in_proc) const
    {
        class_descriptors_map::const_iterator cur = classes_map_.find(dp.source_id());
        if (cur==classes_map_.end())
            throw sql_bridge_error(to_string() << "Unspecified table with name \"" << dp.table_name() <<"\"",
                                   to_string() << "You should use \"DEFINE_SQL_TABLE(" << dp.table_name() << "...\" macro somewhere in one of yours compile units");

        member_for_index_ref ret;
        fields_definition chld_index_ref{"","",false,false};

        if (cur->second->used_pointers() &&
            cur->second->has_unique_key() &&
            !dp.ref_field_name().empty())
        {
            fields_definition fld{"key_for_ref",cur->second->sql_type_for_unique_key(),false,false};
            dp.add_field(fld);
            return ret;
        }
        
        for(auto& mb : cur->second->members())
        {
            fields_definition fld{mb->field_name(),mb->sql_type(),false,false};
            if (mb->index_type()==e_db_index_type::Unique && ret.empty() && dp.ref_field_name().empty())
            {
                ret = member_for_index_ref(e_db_key_mode::Unique,mb->field_name());
                chld_index_ref = fields_definition{to_string() << dp.table_name() << "_" << mb->field_name(),mb->sql_type(),false,true};
                dp.add_index_to_create(index_to_field_pair(mb->index_type(),mb->field_name()));
            }
            else
            if (mb->index_type()==e_db_index_type::PrimaryKey)
            {
                fld.autoincremented_ = true;
                ret = member_for_index_ref(e_db_key_mode::PrimaryKey,mb->field_name());
                chld_index_ref = fields_definition{to_string() << dp.table_name() << "_" << mb->field_name(),mb->sql_type(),false,true};
            }
            else
            if (mb->index_type()==e_db_index_type::Basic ||
                mb->index_type()==e_db_index_type::OrderAsc ||
                mb->index_type()==e_db_index_type::OrderDesc)
            {
                if (ret.empty())
                    ret = member_for_index_ref(mb->field_name());
                dp.add_index_to_create(index_to_field_pair(mb->index_type(),mb->field_name()));
            }
            else
            if (mb->index_type()==e_db_index_type::Unique)
            {
                if (ret.empty() && !dp.ref_field_name().empty())
                    ret = member_for_index_ref(mb->field_name());
                dp.add_index_to_create(index_to_field_pair(dp.ref_field_name().empty()?mb->index_type():e_db_index_type::Basic,mb->field_name()));
            }
            
            if (!fld.type_.empty())
                dp.add_field(fld);

            class_descriptors_pair cbp = mb->reference_description();
            class_descriptors_pair pref = mb->prefix_description();
            if (cbp.second!=nullptr)
            {
                if (tids_in_proc.find(cbp.first)==tids_in_proc.end())
                {
                    dp.add_target(class_link(cbp.first, to_string() << dp.table_name() << "_" << mb->field_name() << "_" << cbp.second->table_name(),mb->field_name()));
                    if (pref.second)
                        dp.target().back().add_prefix_field(fields_definition{pref.second->field_name(),pref.second->sql_type(),false,false});
                }
            }
            else
            if (!cbp.second && cbp.first)
            {
                class_descriptors_map::const_iterator trg = classes_map_.find(cbp.first);
                if (trg!=classes_map_.end())
                    dp.add_target(class_link(cbp.first, to_string() << dp.table_name() << "_as_value_" << trg->second->table_name(),trg->second->table_name()));
            }
        }
        for(auto& inh : cur->second->inheritances())
        {
            class_descriptors_pair cbp = inh->reference_description();
            dp.add_target(class_link(cbp.first,to_string() << dp.table_name() << "_" << cbp.second->table_name(),""));
        }
        if (ret.empty() && !dp.target().empty())
        {
            if (cur->second->is_target_map())
                dp.switch_to_trivial_key();
            ret = member_for_index_ref(e_db_key_mode::ExternalPrimaryKey,to_string() << "sqlcpp_internal_field");
            dp.add_field({ret.name(),def_internal_type,true,false});
            chld_index_ref = fields_definition{to_string() << dp.table_name() << "_" << ret.name(),def_internal_type,false,true};
        }
        tids_in_proc.insert(dp.source_id());
        for(auto& chl : dp.target())
        {
            if (chld_index_ref.type_.empty())
                throw sql_bridge_error(g_internal_error_text,g_architecture_error_text);
            if (tids_in_proc.find(chl.source_id())!=tids_in_proc.end())
            {
//                throw sql_bridge_error("Recursive dependencies don't support yet", "You should wait the next releases or simplificate your object model.");
                
//                ret = member_for_index_ref(e_db_key_mode::None,to_string() << "sqlcpp_recursive_field");
//                dp.add_field({ret.name(),def_recursive_type,false,true});
//                chld_index_ref = fields_definition{to_string() << dp.table_name() << "_" << ret.name(),def_recursive_type,false,true};
//                return ret;
            }
            chl.update_for_key_mode(lookup_for_key_mode(chl,def_internal_type,def_recursive_type,tids_in_proc));
            chl.add_field(chld_index_ref);
        }
        return ret;
    }
    
    void data_section_descriptor::apply_links(class_links_container const& src)
    {
        for(auto const& cn : src)
        {
            class_descriptors_map::iterator pos = classes_map_.find(cn.source_id());
            if (pos==classes_map_.end())
                throw sql_bridge_error(to_string() << "Unspecified table with name \"" << cn.table_name() <<"\"","You should use the DEFINE_SQL_DATABASE macro somewhere in your code");
            pos->second->update_for_depends(cn);
        }
    }
    
    void data_section_descriptor::get_drop_all_statements(string_container &dst) const
    {
        for(auto const& cn : classes_map_)
        {
            if (cn.second->depends().empty()) continue;
            dst.push_back(cn.second->depends().statements().drop_table_);
            
            for(auto const& tr : cn.second->depends().target())
                get_drop_all_statements(tr, dst);
        }
    }
    
    void data_section_descriptor::get_drop_all_statements(class_link const& lnk, string_container &dst) const
    {
        dst.push_back(lnk.statements().drop_table_);
        for(auto const& tr : lnk.target())
            get_drop_all_statements(tr, dst);
    }
    
    void data_section_descriptor::get_create_statements(string_container &dst) const
    {
        for(auto const& cn : classes_map_)
        {
            if (cn.second->depends().empty()) continue;
            dst.push_back(cn.second->depends().statements().create_);
            std::copy(cn.second->depends().statements().trigger_.begin(),
                      cn.second->depends().statements().trigger_.end(),
                      std::back_inserter(dst));
            std::copy(cn.second->depends().statements().indexes_.begin(),
                      cn.second->depends().statements().indexes_.end(),
                      std::back_inserter(dst));
            
            for(auto const& tr : cn.second->depends().target())
                get_create_statements(tr, dst);
        }
    }
    
    void data_section_descriptor::get_create_statements(class_link const& lnk, string_container &dst) const
    {
        dst.push_back(lnk.statements().create_);
        std::copy(lnk.statements().trigger_.begin(),
                  lnk.statements().trigger_.end(),
                  std::back_inserter(dst));
        std::copy(lnk.statements().indexes_.begin(),
                  lnk.statements().indexes_.end(),
                  std::back_inserter(dst));
        for(auto const& tr : lnk.target())
            get_create_statements(tr, dst);
    }
    
    void data_section_descriptor::dump_statements(class_links_container const& src) const
    {
#ifdef DEBUG_CORE
        std::cerr << "*** The data section '" << section_name_ << "'" << std::endl;
        for(size_t i=0; i<src.size(); ++i)
        {
            std::cerr   << " -" << (src[i].target().empty()?"-":"+")
                        << " \"" << src[i].table_name() << "\"" << std::endl;
            dump_statements(src[i],to_string() << "  " << (src[i].target().empty()?" ":"|"));
            std::cerr << std::endl;
        }
#endif
    }
    
    void data_section_descriptor::dump_statements(class_link const& src, std::string const& pref) const
    {
#ifdef DEBUG_CORE
        
        std::cerr << pref << std::endl;
        statements_set const& stm(src.statements());

        std::cerr << pref << "       init: " << stm.create_ << std::endl;
        for(auto const& idx : stm.indexes_)
            std::cerr << pref << "      index: " << idx << std::endl;
        for(auto const& idx : stm.trigger_)
            std::cerr << pref << "    trigger: " << idx << std::endl;
        std::cerr << pref << "     insert: " << stm.insert_ << std::endl;
        if (!stm.update_.empty())
            std::cerr << pref << "     update: " << stm.update_ << std::endl;
        if (!stm.remove_.empty())
            std::cerr << pref << "     remove: " << stm.remove_ << std::endl;
        if (!stm.remove_all_.empty())
            std::cerr << pref << " remove all: " << stm.remove_all_ << std::endl;
        if (!stm.remove_rel_.empty())
            std::cerr << pref << " remove rel: " << stm.remove_rel_ << std::endl;
        std::cerr << pref << "     select: " << stm.select_;
        if (stm.select_app_.empty())
            std::cerr << std::endl;
        else
            std::cerr << "  // optional: " << stm.select_app_ << std::endl;
        std::cerr << pref << "       drop: " << stm.drop_table_ << std::endl;

        std::cerr << pref << std::endl;
        for(size_t i=0; i<src.target().size(); ++i)
        {
            std::cerr   << pref << "--" << (src.target()[i].target().empty()?"-":"+")
                        << " \"" << src.target()[i].table_name() << "\"" << std::endl;
            dump_statements(src.target()[i], to_string() << pref << "  " << (src.target()[i].target().empty()?" ":"|"));
        }
#endif
    }
};
