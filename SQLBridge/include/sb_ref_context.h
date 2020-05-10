//
//  sb_ref_context.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 09/05/2020.
//  Copyright © 2020, DataArt.
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
#ifndef sb_ref_context_h
#define sb_ref_context_h

#include "sb_context_engine.h"

namespace sql_bridge
{
    class ref_context : public context_engine
    {
    private:
        template<typename T> class save_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            save_task(_t_base const& src,data_sections_ptr section,size_t pg,sql_context_references_container const& ref)
                : db_task(section)
                , page_size_(pg)
                , data_(src)
                , references_(ref)
                {};
            save_task(_t_base&& src,data_sections_ptr section,size_t pg,sql_context_references_container const& ref)
                : db_task(section)
                , page_size_(pg)
                , data_(std::move(src))
                , references_(ref)
                {};
            
            inline void run_task() override {if (page_size_) section_->save_page(page_size_,data_,references_); else section_->save(data_,references_);}
            void error(base_sql_error const& err) override {std::cerr << err.what() << std::endl;}
        private:
            size_t page_size_;
            _t_base data_;
            sql_context_references_container references_;
        };

        template<typename T> class save_sync_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            save_sync_task(_t_base const& src,data_sections_ptr section,size_t pg, sql_context_references_container const& ref)
                : db_task(section)
                , page_size_(pg)
                , data_(src)
                , references_(ref)
                {};
            inline void run_task() override {if (page_size_) section_->save_page(page_size_,data_,references_); else section_->save(data_,references_);}
            void error(base_sql_error const& err) override {std::cerr << err.what() << std::endl;}
        private:
            size_t page_size_;
            _t_base const& data_;
            sql_context_references_container references_;
        };
        
        sql_context_references_container references_;
        
    protected:
        ref_context(db_tasks_queue_interface_ptr q, data_sections_ptr ds, sql_context_references_container const& ref)
            : context_engine(q,ds)
            , references_(ref)
            {}
        ref_context() = delete;
        ref_context(ref_context const&) = delete;
        
    public:
        ref_context(ref_context&& src)
            : context_engine(std::move(src))
        {
            std::swap(references_,src.references_);
        }
#pragma mark - public methods
        template<typename T> inline ref_context& save(T const& src) {_save<T>(0,src);return *this;}
        template<typename T> inline ref_context& save(T const* src) {_save_sync<T>(0,*src);return *this;}
        template<typename T> inline ref_context& save(T* src) {_save_sync<T>(0,*src);return *this;}
        template<typename T> inline ref_context& save(T& src) {_save<T>(0,src);return *this;}
        template<typename T> inline ref_context& save(T&& src) {_save_m<T>(0,std::move(src));return *this;}

        template<typename T> inline ref_context& save(size_t pgsz, T const& src) {_save_sync<T>(pgsz,src);return *this;}
        template<typename T> inline ref_context& save(size_t pgsz, T const* src) {_save_sync<T>(pgsz,*src);return *this;}
        template<typename T> inline ref_context& save(size_t pgsz, T* src) {_save_sync<T>(pgsz,*src);return *this;}
        template<typename T> inline ref_context& save(size_t pgsz, T& src) {_save_sync<T>(pgsz,src);return *this;}
        template<typename T> inline ref_context& save(size_t pgsz, T&& src) {_save_m<T>(pgsz,std::move(src));return *this;}


        
        inline ref_context& limit(size_t count, size_t offset = 0) {context_engine::limit(offset,count);return *this;}
        inline ref_context& sql_or() {context_engine::sql_or();return *this;}
        inline ref_context& sql_and() {context_engine::sql_and();return *this;}

        template<typename T, typename TFn> inline ref_context& order(TFn const T::*mem_ptr) {context_engine::order<T,TFn>(mem_ptr);return *this;}
        template<typename T, typename TFn> inline ref_context& order_desc(TFn const T::*mem_ptr) {context_engine::order_desc<T,TFn>(mem_ptr);return *this;}

        template<typename T, typename TFn> inline ref_context& where(TFn const T::*mem_ptr, std::string const& op, TFn const& val) {context_engine::where<T,TFn>(mem_ptr,op,val);return *this;}
        template<typename T, typename TFn> inline ref_context& where_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {context_engine::where_between<T,TFn>(mem_ptr,from,to);return *this;}
        template<typename T, typename TFn> inline ref_context& where_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {context_engine::where_between<T,TFn>(val,mem_ptr_from,mem_ptr_to);return *this;}
        template<typename T, typename TFn> inline ref_context& where_not_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {context_engine::where_not_between<T,TFn>(mem_ptr,from,to);return *this;}
        template<typename T, typename TFn> inline ref_context& where_not_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {context_engine::where_not_between<T,TFn>(val,mem_ptr_from,mem_ptr_to);return *this;}
        template<typename T, typename TFn, typename TCont> inline ref_context& where_in(TFn const T::*mem_ptr, TCont const& cnt) {context_engine::where_in<T,TFn,TCont>(mem_ptr,cnt); return *this;}
        template<typename T, typename TFn, typename TCont> inline ref_context& where_not_in(TFn const T::*mem_ptr, TCont const& cnt) {context_engine::where_not_in<T,TFn,TCont>(mem_ptr,cnt); return *this;}

    private:
#pragma mark - save sync
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save_sync(size_t, T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }

        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _save_sync(size_t pg, T const& src) const
        {
            db_task_ptr task(std::make_shared< save_sync_task<T> >(src,data_,pg,references_));
            std::future<void> ret(task->get_future());
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add( task );
                ret.get();
            }
        }
#pragma mark - save
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save(size_t, T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save_m(size_t, T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_pointer<T>::value>::type _save(size_t pg, T const& src) const {_save_sync<T>(pg,src);}
        template<typename T> inline typename std::enable_if<!is_pointer<T>::value &&
                                                            !is_sql_acceptable<T>::value &&
                                                            !is_container<T>::value &&
                                                            !is_any_map<T>::value>::type _save(size_t pg, T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(src,data_,pg,references_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _save(size_t pg, T const& src) const {_save_cont<T>(pg,src);}
        template<typename T> inline typename std::enable_if<is_map<T>::value>::type _save(size_t pg, T const& src) const {_save_map<T>(pg,src);}
        template<typename T> inline typename std::enable_if<is_pointer<typename T::value_type>::value>::type _save_cont(size_t pg, T const& src) const {_save_sync<T>(pg,src);}
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::value_type>::value>::type _save_cont(size_t pg, T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(src,data_,pg,references_));
        }
        template<typename T> inline typename std::enable_if<is_pointer<typename T::mapped_type>::value>::type _save_map(size_t pg, T const& src) const {_save_sync<T>(pg,src);}
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::mapped_type>::value>::type _save_map(size_t pg, T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(src,data_,pg,references_));
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value &&
                                                            !is_container<T>::value &&
                                                            !is_map<T>::value>::type _save_m(size_t pg, T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(std::move(src),data_,pg,references_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _save_m(size_t pg, T&& src) const {_save_cont_m<T>(pg,std::move(src));}
        template<typename T> inline typename std::enable_if<is_map<T>::value>::type _save_m(size_t pg, T&& src) const {_save_map_m<T>(pg,std::move(src));}
        template<typename T> inline typename std::enable_if<is_pointer<typename T::value_type>::value>::type _save_cont_m(size_t pg, T&& src) const
        {
            db_task_ptr task(std::make_shared< save_task<T> >(std::move(src),data_,pg,references_));
            std::future<void> ret(task->get_future());
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add( task );
                ret.get();
            }
        }
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::value_type>::value>::type _save_cont_m(size_t pg, T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(std::move(src),data_,pg,references_));
        }
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::mapped_type>::value>::type _save_map_m(size_t pg, T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(std::move(src),data_,pg,references_));
        }
        template<typename T> inline typename std::enable_if<is_pointer<typename T::mapped_type>::value>::type _save_map_m(size_t pg, T&& src) const
        {
            db_task_ptr task(std::make_shared< save_task<T> >(std::move(src),data_,pg,references_));
            std::future<void> ret(task->get_future());
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add( task );
                ret.get();
            }
        }

        
        
        
    };
};

#endif /* sb_ref_context_h */
