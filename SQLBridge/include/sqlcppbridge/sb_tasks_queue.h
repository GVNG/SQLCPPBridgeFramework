//
//  sb_tasks_queue.h
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
#ifndef sb_tasks_queue_h
#define sb_tasks_queue_h

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include "sb_exceptions.h"
#include "sb_kv_storage.h"
#include "sb_adapter.h"

namespace sql_bridge
{
    
    class db_task;
    class db_tasks_queue_interface;
    using db_task_ptr = std::shared_ptr<db_task>;
    using db_tasks_queue = std::list<db_task_ptr>; // use list instead of the queue because
                                                   // processing out-of-bound tasks are possible
    using db_tasks_queue_interface_ptr = std::shared_ptr<db_tasks_queue_interface>;
    using db_tasks_queue_interface_weak_ptr = std::weak_ptr<db_tasks_queue_interface>;
    
    class db_tasks_queue_interface
    {
    public:
        virtual ~db_tasks_queue_interface() {};
        virtual void add(db_task_ptr) = 0;
        virtual void add_for_sync(db_task_ptr) = 0;
    };
    
    class db_task : public std::packaged_task<void()>
    {
        friend class context_bare;
    public:
        db_task(data_sections_ptr sc)
            : std::packaged_task<void()>(std::bind(std::mem_fn(&db_task::execute),this))
            , section_(sc)
            {}
        virtual ~db_task() {};
        virtual void run_task() = 0;
        virtual bool out_of_queue() const {return false;}
    protected:
        data_sections_ptr section_;
    private:
        void execute();
        virtual bool error(base_sql_error const&) {return true;}
    };

    template<typename TStrategy> class _t_db_queue_entry
        : public key_value_storage<TStrategy>
        , public db_tasks_queue_interface
    {
    public:
        _t_db_queue_entry(std::string const& fn)
            : key_value_storage<TStrategy>(fn)
            , shutdown_(false)
            {}
        
        void add(db_task_ptr tsk) override
        {
            tasks_queue_access_.under_guard_and_fire([this,tsk]()
            {
                if (tsk->out_of_queue() && !tasks_queue_.empty())
                    tasks_queue_.insert(std::find_if(tasks_queue_.begin(),
                                                     tasks_queue_.end(),
                                                     [](db_task_ptr const& ts){return !ts->out_of_queue();}),
                                        std::move(tsk));
                else
                    tasks_queue_.push_back(std::move(tsk));
            });
        }
        
        void add_for_sync(db_task_ptr tsk) override
        {
            auto chk(tsk->get_future().share());
            add(tsk);
            chk.get();
        }
        
        void do_proc()
        {
            while(!shutdown_)
            {
                for(;;)
                {
                    db_task_ptr task;
                    tasks_queue_access_.wait_if([this,&task]()
                    {
                        if (tasks_queue_.empty()) return !shutdown_;
                        task = std::move(tasks_queue_.front());
                        tasks_queue_.pop_front();
                        return false;
                    });
                    if (!task) break;
                    (*task)();
                }
            }
        }
        
        void shutdown()
        {
            tasks_queue_access_.under_guard_and_fire([this](){shutdown_ = true;});
        }
    private:
        bool shutdown_;
        mt_event tasks_queue_access_;
        db_tasks_queue tasks_queue_;
    };
    
};


#endif /* sb_tasks_queue_h */
