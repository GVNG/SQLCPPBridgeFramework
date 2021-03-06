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

#ifndef sb_tasks_queue_h
#define sb_tasks_queue_h

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include "sb_exceptions.h"
#include "sb_kv_storage.h"
#include "sb_sqlite_adapter.h"

namespace sql_bridge
{
    
    class db_task;
    class db_tasks_queue_interface;
    typedef std::shared_ptr<db_task> db_task_ptr;
    typedef std::list<db_task_ptr> db_tasks_queue;
    typedef std::shared_ptr<db_tasks_queue_interface> db_tasks_queue_interface_ptr;
    typedef std::weak_ptr<db_tasks_queue_interface> db_tasks_queue_interface_weak_ptr;
    
    class db_tasks_queue_interface
    {
    public:
        virtual void add(db_task_ptr) = 0;
    };
    
    class db_task : public std::packaged_task<void(db_task*)>
    {
        friend class context_bare;
    public:
        db_task(data_sections_ptr sc)
            : std::packaged_task<void(db_task*)>(execute)
            , section_(sc)
            {}
        virtual ~db_task() {};
        virtual void run_task() = 0;
        virtual bool out_of_band() const {return false;}
    protected:
        data_sections_ptr section_;
    private:
        static void execute(db_task* trg);
        virtual void ready() {}
        virtual void error(base_sql_error const&) {}
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
            std::lock_guard<std::mutex> lck(access_);
            if (tsk->out_of_band() && !tasks_queue_.empty())
            {
                db_tasks_queue::iterator pos = std::find_if(tasks_queue_.begin(),
                                                            tasks_queue_.end(),
                                                            [](db_task_ptr ts){return !ts->out_of_band();});
                tasks_queue_.insert(pos,tsk);
            }
            else
                tasks_queue_.push_back(tsk);
            new_data_.fire();
        }
        void do_proc(interlocked<size_t>& ready)
        {
            ready--;
            new_data_.wait();
            for(;;)
            {
                db_task_ptr task;
                {
                    std::lock_guard<std::mutex> lck(access_);
                    if (!tasks_queue_.empty())
                    {
                        task = tasks_queue_.front();
                        tasks_queue_.pop_front();
                    }
                    else
                        if (shutdown_) break;
                }
                if (task!=nullptr)
                    task->operator()(task.get());
                else
                    new_data_.wait();
            }
        }
        
        void shutdown()
        {
            shutdown_ = true;
            new_data_.fire();
        }
    private:
        bool shutdown_;
        db_tasks_queue tasks_queue_;
        std::mutex access_;
        mt_event new_data_;
    };
    
};


#endif /* sb_tasks_queue_h */
