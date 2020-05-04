//
//  sb_local_storage.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 11/03/2016.
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
#ifndef sb_local_storage_h
#define sb_local_storage_h

#include "sb_tasks_queue.h"
#include "sb_context.h"
#include "sb_core_misc.h"

namespace sql_bridge
{
    template<typename TStrategy> class local_storage
    {
        class sections_keeper;
        typedef class _t_db_queue_entry<TStrategy> db_queue_entry;
        typedef std::shared_ptr<db_queue_entry> db_proc_queue_ptr;
        typedef std::weak_ptr<db_queue_entry> db_proc_queue_weak_ptr;
        typedef std::map<std::string,sections_keeper> sections_cache;
    private:
        
        template<typename T> class load_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            load_task(T const& src, std::string const& key, db_proc_queue_ptr const& trg)
                : db_task(data_sections_ptr())
                , kvdb_(trg)
                , def_value_(src)
                , key_(key)
                {};
            void run_task()
            {
                data_ = def_value_;
                db_proc_queue_ptr db(kvdb_.lock());
                if (db)
                    db->load(key_, data_);
            };
            inline T&& data() {return std::move(data_);}
        private:
            db_proc_queue_weak_ptr kvdb_;
            _t_base data_,def_value_;
            std::string key_;
        };
        
        template<typename T> class save_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            save_task(T const& src, std::string const& key, db_proc_queue_ptr const& trg)
                : db_task(data_sections_ptr())
                , kvdb_(trg)
                , data_(src)
                , key_(key)
                {};
            void run_task()
            {
                db_proc_queue_ptr db(kvdb_.lock());
                if (db)
                    db->save(key_,data_);
            };
        private:
            db_proc_queue_weak_ptr kvdb_;
            _t_base data_;
            std::string key_;
        };

        template<typename T,typename TKey> class remove_task : public db_task
        {
            typedef typename std::decay<T>::type _t_proc_type;
        public:
            remove_task(std::string const& tab, TKey const& key, db_proc_queue_ptr const& trg)
                : db_task(data_sections_ptr())
                , kvdb_(trg)
                , table_name_(tab)
                , key_(key)
                {};
            void run_task()
            {
                db_proc_queue_ptr db(kvdb_.lock());
                if (db)
                    db->template remove<_t_proc_type>(key_,table_name_);
            };
        private:
            db_proc_queue_weak_ptr kvdb_;
            std::string table_name_;
            TKey key_;
        };
        
        class create_task : public db_task
        {
        public:
            create_task(std::string const& name,
                        db_proc_queue_ptr const& trg,
                        std::string const& path,
                        fn_change_file_name fn_change)
                : db_task(data_sections_ptr())
                , name_(name)
                , path_(path)
                , kvdb_(trg)
                , fn_change_(fn_change)
                {};
            void run_task()
            {
                db_proc_queue_ptr db(kvdb_.lock());
                if (db)
                    section_ = db->create_section(name_,path_,fn_change_);
            };
            inline data_sections_ptr section() const {return section_;}
            bool out_of_band() const {return true;}
        private:
            db_proc_queue_weak_ptr kvdb_;
            std::string name_;
            std::string path_;
            fn_change_file_name fn_change_;
        };
        
        class sections_keeper
        {
        public:
            sections_keeper(data_sections_ptr sc)
                : section_(sc)
                , ts_(std::chrono::system_clock::now())
                {}
            inline bool is_expired() const {return ts_+std::chrono::seconds(30)<std::chrono::system_clock::now();}
        private:
            data_sections_ptr section_;
            std::chrono::time_point<std::chrono::system_clock> ts_;
        };
        
    public:
        local_storage(std::string const& path)
            : ready_(2)
            , root_path_(path)
            , proc_queue_(std::make_shared<db_queue_entry>(TStrategy::main_db_name(path)))
            , proc_thread_(std::bind(std::mem_fn(&local_storage::proc),this))
            , proc_flush_thread_(std::bind(std::mem_fn(&local_storage::proc_flush),this))
        
        {
            do {std::this_thread::yield();} while(ready_);
        }
        ~local_storage()
        {
            proc_queue_->shutdown();
            proc_thread_.join();
            shutdown_.fire();
            proc_flush_thread_.join();
        }
#pragma mark - save
        
        template<typename T> inline void save(std::string const& key, T const& val) const
        {
            proc_queue_->add(std::make_shared< save_task<T> >(val,key,proc_queue_));
        }
        
#pragma mark - load
        
        template<typename T> inline T load(std::string const& key, T const& def) const
        {
            db_task_ptr task(std::make_shared< load_task<T> >(def,key,proc_queue_));
            std::future<void> ret(task->get_future());
            proc_queue_->add(task);
            ret.wait();
            ret.get(); // exceptions check
            return static_cast<load_task<T>*>(task.get())->data();
        }
        inline std::string load(std::string const& key) const {return load(key,std::string());}

#pragma mark - remove
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type remove(std::string const& key) const
        {
            proc_queue_->add(std::make_shared< remove_task<T,std::string> >("",key,proc_queue_));
        }
        template<typename T> inline typename std::enable_if<is_trivial_map<T>::value>::type remove(typename T::key_type const& key, std::string const& tab) const
        {
            proc_queue_->add(std::make_shared< remove_task<T,typename T::key_type> >(tab,key,proc_queue_));
        }

#pragma mark - contexts load
        
        inline context operator[](std::string const& nm) {return context_with_alt_file(nm,nullptr);}

        context context_with_alt_file(std::string const& nm, fn_change_file_name fn)
        {
            struct _t_context_creator : context {_t_context_creator(db_tasks_queue_interface_ptr q, data_sections_ptr ds) : context(q,ds) {}};
            {
                std::lock_guard<std::mutex> lk(data_section_access_);
                data_sections_map::iterator pos(data_sections_.find(nm));
                if (pos!=data_sections_.end() && !pos->second.expired())
                {
                    data_sections_ptr sect = pos->second.lock();
                    section_keepers_.erase(nm);
                    section_keepers_.insert({nm,sections_keeper(sect)});
                    return _t_context_creator(proc_queue_,sect);
                }
                data_sections_.erase(nm);
                section_keepers_.erase(nm);
            }
            db_task_ptr task(std::make_shared< create_task >(nm,proc_queue_,root_path_,fn));
            std::future<void> ret(task->get_future());
            proc_queue_->add(task);
            ret.wait();
            ret.get(); // exceptions check
            data_sections_ptr sect(static_cast<create_task*>(task.get())->section());
            {
                std::lock_guard<std::mutex> lk(data_section_access_);
                data_sections_map::iterator pos(data_sections_.find(nm));
                section_keepers_.erase(nm);
                if (pos!=data_sections_.end() && !pos->second.expired())
                {
                    data_sections_ptr ls = pos->second.lock();
                    section_keepers_.insert({nm,sections_keeper(ls)});
                    return _t_context_creator(proc_queue_,ls);
                }
                else
                {
                    data_sections_.erase(nm);
                    data_sections_.insert({nm,sect});
                    section_keepers_.insert({nm,sections_keeper(sect)});
                    return _t_context_creator(proc_queue_,sect);
                }
            }
        }
    private:
        // data
        interlocked<size_t> ready_;
        mt_event shutdown_;
        std::string root_path_;
        data_sections_map data_sections_;
        sections_cache section_keepers_;
        std::mutex data_section_access_;
        db_proc_queue_ptr proc_queue_;
        std::thread proc_thread_,proc_flush_thread_;
        // methods
        void proc() {proc_queue_->do_proc(ready_);}
        void proc_flush()
        {
            ready_--;
            while(!shutdown_.wait_for(std::chrono::seconds(10)))
            {
                std::lock_guard<std::mutex> lk(data_section_access_);
                for(typename sections_cache::iterator pos = section_keepers_.begin(); pos!=section_keepers_.end();)
                {
                    if (pos->second.is_expired())
                        pos = section_keepers_.erase(pos);
                    else
                        pos++;
                }
            }
        }
    };
};

#endif /* sb_local_storage_h */
