//
//  sb_sqlite_adapter.cpp
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 13/03/2016.
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

#include <iostream>
#include "sb_sqlite_adapter.h"
#include "sb_exceptions.h"
#include "sb_data_section.h"

namespace sql_bridge
{
#pragma mark - sql_file
    
    static std::string const g_root_database("_structure_settings");
    static std::string const g_versions_table_name("_SB_VERSIONS_");
    static std::string const g_file_suffix(".db");

    static std::string const g_err_cantopen("Can't open SQL-database");
    static std::string const g_err_cantupdate("Can't update the database");
    static std::string const g_err_cantread("Can't read the database");

    sqlite_adapter::sql_file::sql_file(std::string const& dbfname)
        : base_(nullptr)
        , file_name_(dbfname)
        , err_code_(0)
    {
        if (sqlite3_open_v2(dbfname.c_str(), &base_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE, NULL)!=SQLITE_OK)
            throw file_sql_error(g_err_cantopen,dbfname);
    }
    
    sqlite_adapter::sql_file::~sql_file()
    {
        reset_cache();
        if (base_) sqlite3_close_v2(base_);
        base_ = nullptr;
    }
    
    sqlite3_stmt* sqlite_adapter::sql_file::operator[](std::string const& txstm) const
    {
        if (!base_ || txstm.empty()) return nullptr;
        _t_statements_map::const_iterator pos = statements_cache_.find(txstm);
        if (pos!=statements_cache_.end())
        {
            sqlite3_reset(pos->second);
            return pos->second;
        }
        sqlite3_stmt* ret = nullptr;
        err_code_ = sqlite3_prepare_v2(base_, txstm.c_str(), (int)txstm.size(), &ret, NULL);
        if (err_code_!=SQLITE_OK) return nullptr;
        statements_cache_.insert({txstm,ret});
        return ret;
    }
    
    void sqlite_adapter::sql_file::reset_cache() const
    {
        for(auto& vk : statements_cache_)
            sqlite3_finalize(vk.second);
        statements_cache_.clear();
    }
    
    sqlite_adapter::sql_file const& sqlite_adapter::sql_file::execute(const std::string & cmd) const
    {
        reset_cache();
        char* errMsg = NULL;
        int code = sqlite3_exec(base_, cmd.c_str(), NULL, NULL, &errMsg);
        if (code!=SQLITE_OK)
            throw sql_error(errMsg,cmd,code);
        return *this;
    }
    
    sqlite_adapter::sql_file::transactions_lock::transactions_lock(sql_file const& src)
        : parent_(src.execute("BEGIN TRANSACTION"))
        , rollback_(false)
    {
    }
    
    sqlite_adapter::sql_file::transactions_lock::~transactions_lock()
    {
        try
        {
            if (rollback_)
                parent_.execute("ROLLBACK TRANSACTION");
            else
                parent_.execute("COMMIT TRANSACTION");
        }
        catch(std::exception& ex)
        {
            std::cerr << "Unexpected SQL exception: " << ex.what();
        }
    }

#pragma mark - sqlite_adapter::sql_updater

    sqlite_adapter::sql_updater::sql_updater(sql_file const& db,
                                             std::string const& sttm)
        : statement_(db[sttm])
        , need_step_(false)
        , txt_statement_(sttm)
        , fld_num_(1)
    {
        if (!statement_ && !sttm.empty())
            throw sql_error(g_err_cantupdate,txt_statement_,db.err_code());
    }
    
    sqlite_adapter::sql_updater::~sql_updater()
    {
        if (!statement_) return;
        if (need_step_)
            sqlite3_step(statement_);
    }
    
    bool sqlite_adapter::sql_updater::next()
    {
        if (!statement_ || !need_step_) return false;
        fld_num_ = 1;
        int code(SQLITE_OK);
        need_step_ = false;
        if ((code=sqlite3_step(statement_))!=SQLITE_DONE)
            throw sql_error(g_err_cantupdate,txt_statement_,code);
        return sqlite3_reset(statement_)==SQLITE_OK;
    }

#pragma mark - sqlite_adapter::sql_inserter_kv
    
    sqlite_adapter::sql_inserter_kv::sql_inserter_kv(sql_file const& db, std::string const& table, bool kv)
        : state_(nullptr)
        , need_step_(false)
        , fld_num_(kv?2:1)
    {
        txt_statement_ = to_string() << "INSERT or REPLACE into " << table << (kv?" (KEY,VAL) values (?,?)":" (VAL) values (?)");
        state_ = db[txt_statement_];
        if (!state_)
            throw sql_error(g_err_cantupdate,txt_statement_,db.err_code());
    }
    
    bool sqlite_adapter::sql_inserter_kv::next()
    {
        if (!state_ || !need_step_) return false;
        int code(SQLITE_OK);
        need_step_ = false;
        if ((code=sqlite3_step(state_))!=SQLITE_DONE)
            throw sql_error(g_err_cantupdate,txt_statement_,code);
        return sqlite3_reset(state_)==SQLITE_OK;
    }
    
    sqlite_adapter::sql_inserter_kv::~sql_inserter_kv()
    {
        if (!state_) return;
        if (need_step_)
            sqlite3_step(state_);
    }

#pragma mark - sqlite_adapter::sql_remove_kv
    
    sqlite_adapter::sql_remove_kv::sql_remove_kv(sql_file const& db, std::string const& table)
        : state_(nullptr)
        , need_step_(false)
    {
        txt_statement_ = to_string() << "DELETE FROM " << table << " WHERE key=?";
        state_ = db[txt_statement_];
        if (!state_)
            throw sql_error(g_err_cantupdate,txt_statement_,db.err_code());
    }

    bool sqlite_adapter::sql_remove_kv::next()
    {
        if (!state_ || !need_step_) return false;
        int code(SQLITE_OK);
        need_step_ = false;
        if ((code=sqlite3_step(state_))!=SQLITE_DONE)
            throw sql_error(g_err_cantupdate,txt_statement_,code);
        return sqlite3_reset(state_)==SQLITE_OK;
    }
    
    sqlite_adapter::sql_remove_kv::~sql_remove_kv()
    {
        if (!state_) return;
        if (need_step_)
            sqlite3_step(state_);
    }

#pragma mark - sqlite_adapter::sql_reader

    sqlite_adapter::sql_reader::sql_reader(sql_file const& db,
                                           std::string const& stm,
                                           sql_value const& extkey)
        : statement_(db[stm])
        , valid_(statement_)
        , txt_statement_(stm)
        , fld_num_(0)
        , postponed_init_(true)
    {
        if (txt_statement_.empty()) return;
        if (!valid_)
            throw sql_error(g_err_cantread,txt_statement_,db.err_code());
        if (valid_)
        {
            if (!extkey.empty())
                bind(extkey);
        }
    }
    
    void sqlite_adapter::sql_reader::postponed_init()
    {
        if (!postponed_init_) return;
        fld_num_ = 0;
        if (valid_)
            valid_ = sqlite3_step(statement_)==SQLITE_ROW;
        postponed_init_ = false;
    }
    
    bool sqlite_adapter::sql_reader::next()
    {
        if (!valid_) return false;
        fld_num_ = 0;
        valid_ = sqlite3_step(statement_)==SQLITE_ROW;
        return valid_;
    }
        
#pragma mark - sqlite_adapter::sql_reader_kv
    
    sqlite_adapter::sql_reader_kv::sql_reader_kv(sql_file const& db,
                                                 std::string const& table,
                                                 std::string const& key)
        : state_(nullptr)
        , valid_(false)
    {
        txt_statement_ = to_string() << "SELECT VAL FROM " << table << " WHERE KEY='" << key << "'";
        state_ = db[txt_statement_];
        valid_ = state_!=nullptr;
        if (valid_)
            valid_ = sqlite3_step(state_)==SQLITE_ROW;
    }
    
    sqlite_adapter::sql_reader_kv::sql_reader_kv(sql_file const& db,
                                                 std::string const& table,
                                                 bool keep_order)
        : state_(nullptr)
        , valid_(false)
    {
        if (keep_order)
            txt_statement_ = to_string() << "SELECT VAL FROM " << table << " " << sqlite_adapter::sql_order_by("") << "KEY";
        else
            txt_statement_ = to_string() << "SELECT * FROM " << table;
        state_ = db[txt_statement_];
        valid_ = state_!=nullptr;
        if (valid_)
            valid_ = sqlite3_step(state_)==SQLITE_ROW;
    }
    
    bool sqlite_adapter::sql_reader_kv::next()
    {
        if (!valid_) return false;
        return sqlite3_step(state_)==SQLITE_ROW;
    }
    
#pragma mark - sqlite_adapter - methods
    
    sqlite_adapter::sql_inserter_kv sqlite_adapter::create_table(sql_file const& db,
                                                                 std::string const& type)
    {
        if (created_tables_.find(type)==created_tables_.end())
        {
            db.execute(to_string() << "CREATE TABLE IF NOT EXISTS " << type << "_VAL (key TEXT, val " << type << ")");
            db.execute(to_string() << "CREATE UNIQUE INDEX IF NOT EXISTS ix_KEY_" << type << " ON " << type << "_VAL (key)");
            created_tables_.insert(type);
        }
        return sql_inserter_kv(db,to_string() << type << "_VAL",true);
    }

    sqlite_adapter::sql_remove_kv sqlite_adapter::create_statement_for_remove_value(sql_file const& db,
                                                                                    std::string const& type)
    {
        if (created_tables_.find(type)==created_tables_.end())
        {
            db.execute(to_string() << "CREATE TABLE IF NOT EXISTS " << type << "_VAL (key TEXT, val " << type << ")");
            db.execute(to_string() << "CREATE UNIQUE INDEX IF NOT EXISTS ix_KEY_" << type << " ON " << type << "_VAL (key)");
            created_tables_.insert(type);
        }
        return sql_remove_kv(db,to_string() << type << "_VAL");
    }
    
    sqlite_adapter::sql_remove_kv sqlite_adapter::create_statement_for_remove_in_map(sql_file const& db,std::string const& table, std::string const& typekey, std::string const& typeval)
    {
        return sql_remove_kv(db,to_string() << table << "_" << typekey << "_to_" << typeval << "_table");
    }

    sqlite_adapter::sql_inserter_kv sqlite_adapter::create_table_for_array(sql_file const& db,std::string const& table, std::string const& type)
    {
        db.execute(to_string() << "DROP TABLE IF EXISTS " << table);
        db.execute(to_string() << "CREATE TABLE " << table << " (key INTEGER PRIMARY KEY AUTOINCREMENT, val " << type << ")");
        return sql_inserter_kv(db,table,false);
    }
    
    sqlite_adapter::sql_inserter_kv sqlite_adapter::create_table_for_map(sql_file const& db,std::string const& table, std::string const& typekey, std::string const& typeval)
    {
        db.execute(to_string() << "DROP TABLE IF EXISTS " << table);
        db.execute(to_string() << "CREATE TABLE " << table << " (key " << typekey << ", val " << typeval << ")");
        return sql_inserter_kv(db,table,true);
    }

    size_t sqlite_adapter::create_table_for_versions(sql_file const& db, std::string const& name)
    {
        if (created_tables_.find(g_versions_table_name)==created_tables_.end())
        {
            db.execute(to_string() << "CREATE TABLE IF NOT EXISTS " << g_versions_table_name << " (KEY TEXT, VAL INTEGER DEFAULT 0)");
            db.execute(to_string() << "CREATE UNIQUE INDEX IF NOT EXISTS ix_KEY" << g_versions_table_name << " ON " << g_versions_table_name << " (KEY)");
            created_tables_.insert(g_versions_table_name);
        }
        sql_reader_kv rd(db,g_versions_table_name,name);
        size_t ret(0);
        if (rd.is_valid())
            rd.read_value(ret, 0);
        return ret;
    }
    
    void sqlite_adapter::create_statements(class_links_container& cont)
    {
        for(auto& cn : cont)
            create_statements(cn);
    }
    
    std::string sqlite_adapter::main_db_name(std::string const& path)
    {
        return to_string() << path << "/" << g_root_database << g_file_suffix;
    }
    
    void sqlite_adapter::create_statements(class_link& dp, std::string const& relfrom)
    {
        // create
        to_string crts;
        crts << "CREATE TABLE IF NOT EXISTS " << dp.table_name() << " (";
        for(auto const& fl : dp.prefix_fields())
        {
            crts << fl.name_ << " " << fl.type_;
            if (fl.autoincremented_) crts << " PRIMARY KEY AUTOINCREMENT";
            crts << ",";
        }
        for(auto const& fl : dp.fields())
        {
            crts << fl.name_ << " " << fl.type_;
            if (fl.autoincremented_) crts << " PRIMARY KEY AUTOINCREMENT";
            crts << ",";
        }
        crts.remove_from_tail(1);
        crts << ")";
        dp.update_for_create_statement(crts);
        
        dp.update_for_drop_table_statement(to_string() << "DROP TABLE " << dp.table_name());
        
        for(auto const& isr : dp.should_create_indexes())
        {
            to_string idts;
            idts << "CREATE ";
            if (isr.first==e_db_index_type::Unique)
                idts << "UNIQUE ";
            idts << "INDEX IF NOT EXISTS ix_" << dp.table_name() << "_" << isr.second << " ON "
                 << dp.table_name() << " (" << isr.second << ")";
            dp.update_for_index_statement(idts);
        }

        for(auto const& fl : dp.prefix_fields())
            if (fl.simple_indexed_)
        {
            to_string idts;
            idts << "CREATE ";
            idts << "INDEX IF NOT EXISTS ix_" << dp.table_name() << "_" << fl.name_ << " ON "
                 << dp.table_name() << " (" << fl.name_ << ")";
                dp.update_for_index_statement(idts);
        }

        for(auto const& fl : dp.fields())
            if (fl.simple_indexed_)
        {
            to_string idts;
            idts << "CREATE ";
            idts << "INDEX IF NOT EXISTS ix_" << dp.table_name() << "_" << fl.name_ << " ON "
                 << dp.table_name() << " (" << fl.name_ << ")";
            dp.update_for_index_statement(idts);
        }
        
        if (!dp.target().empty())
        {
            to_string trsts,uptr;
            trsts << "CREATE TRIGGER IF NOT EXISTS tr_" << dp.table_name();
            trsts << " BEFORE DELETE ON " << dp.table_name() << " BEGIN ";
            uptr << "CREATE TRIGGER IF NOT EXISTS trup_" << dp.table_name();
            uptr << " BEFORE UPDATE ON " << dp.table_name() << " BEGIN ";
            for(auto const& tr : dp.target())
            {
                trsts << "DELETE FROM " << tr.table_name() << sql_where() << dp.table_name() << "_" << dp.index_ref().name();
                trsts << "=old." << dp.index_ref().name() << ";";
                uptr << "DELETE FROM " << tr.table_name() << sql_where() << dp.table_name() << "_" << dp.index_ref().name();
                uptr << "=old." << dp.index_ref().name() << ";";
            }
            trsts << "END";
            uptr << "END";
            dp.update_for_trigger_statement(trsts);
            dp.update_for_trigger_statement(uptr);
        }

        to_string insts,qsts;
        insts << "INSERT INTO " << dp.table_name() << " (";
        for(auto const& fn : dp.prefix_fields())
            if (!fn.autoincremented_)
        {
            insts << fn.name_ << ",";
            qsts << "?,";
        }
        for(auto const& fn : dp.fields())
            if (!fn.autoincremented_)
        {
            insts << fn.name_ << ",";
            qsts << "?,";
        }
        insts.remove_from_tail(1);
        if (qsts.str().empty())
            insts << "VALUES (NULL)";
        else
        {
            insts << ") VALUES (" << qsts.str();
            insts.remove_from_tail(1);
            insts << ")";
        }
        dp.update_for_insert_statement(insts);

        if (!dp.index_ref().empty() && relfrom.empty())
        {
            to_string updsts;
            updsts << "UPDATE " << dp.table_name() << " SET ";
            for(auto const& fn : dp.prefix_fields())
                if (!fn.autoincremented_)
                    updsts << fn.name_ << "=?,";
            for(auto const& fn : dp.fields())
                if (!fn.autoincremented_)
                    updsts << fn.name_ << "=?,";
                else
                    updsts << fn.name_ << "=" << fn.name_ << ",";
            updsts.remove_from_tail(1);
            if (dp.is_trivial_key())
                updsts << sql_where() << dp.fields().front().name_ << "=?";
            else
                updsts << sql_where() << dp.index_ref().name() << "=?";
            dp.update_for_update_statement(updsts);
        }
        
        if (!dp.index_ref().empty() && relfrom.empty())
        {
            to_string delsts;
            if (dp.is_trivial_key())
                delsts << "DELETE FROM " << dp.table_name() << sql_where() << dp.fields().front().name_ << "=?";
            else
                delsts << "DELETE FROM " << dp.table_name() << sql_where() << dp.index_ref().name() << "=?";
            dp.update_for_remove_statement(delsts);
        }
        if (relfrom.empty())
            dp.update_for_remove_all_statement(to_string() << "DELETE FROM " << dp.table_name());
        else
            dp.update_for_remove_rel_statement(to_string() << "DELETE FROM " << dp.table_name() << sql_where() << relfrom << "=?");
            
        
        to_string selsts;
        selsts << "SELECT ";
        for(auto const& mm : dp.prefix_fields())
            if (mm.name_!=relfrom)
                selsts << mm.name_ << ",";
        for(auto const& mm : dp.fields())
            if (mm.name_!=relfrom)
                selsts << mm.name_ << ",";
        selsts.remove_from_tail(1);
        selsts << " FROM " << dp.table_name();
        if (!relfrom.empty())
            selsts << sql_where() << relfrom << "=?";
        to_string selapp;
        for(auto const& pr : dp.should_create_indexes())
        {
            if (pr.first==e_db_index_type::OrderAsc)
            {
                selapp << sql_order_by("") << sql_order_asc(pr.second);
                break;
            }
            else
            if (pr.first==e_db_index_type::OrderDesc)
            {
                selapp << sql_order_by("") << sql_order_desc(pr.second);
                break;
            }
        }
        if (selapp.str().empty())
            if (dp.index_ref().type()==e_db_key_mode::PrimaryKey ||
                dp.index_ref().type()==e_db_key_mode::ExternalPrimaryKey)
                    selapp << sql_order_by("") << sql_order_asc(dp.index_ref().name());

        dp.update_for_select_statement(selsts,selapp);
        
        for(auto& trg : dp.target())
            create_statements(trg, to_string() << dp.table_name() << "_" << dp.index_ref().name());
    }
    
    std::string sqlite_adapter::create_section(sql_file const& db, std::string const& name, std::string const& path, fn_change_file_name fnch)
    {
        size_t ver = create_table_for_versions(db,name);
        data_section_descriptors_ptr desc = data_section_descriptors::instance()[name];
        std::string fname = fnch?(to_string() << path << "/" << fnch(to_string() << name << g_file_suffix))
                                :(to_string() << path << "/" << name << g_file_suffix);
        if (ver<desc->version())
        { // lets create/upgrate the tables
#ifdef DEBUG_CORE
            std::cerr << "Create(upgrade) structure for the table \"" << name << "\" from the version: " << ver << std::endl;
#endif
            if (ver) desc->update(fname, ver, desc->version());
            desc->create_tables(fname);
            if (!ver) desc->update(fname, ver, desc->version());
            sql_inserter_kv ins(db, g_versions_table_name, true);
            ins.bind_key(name).bind_value(desc->version());
        }
        else
        if (ver>desc->version())
            throw sql_bridge_error("The \"downgrade\" of database is impossible", to_string() << "Requested the version " << desc->version() << " but the version " << ver << " already exists for the database \"" << name << "\"");
        return fname;
    }
    

};
