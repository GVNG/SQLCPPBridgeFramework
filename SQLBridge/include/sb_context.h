//
//  sb_context.h
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

#pragma once
#ifndef sb_context_h
#define sb_context_h

#include "sb_context_engine.h"
#include "sb_ref_context.h"

namespace sql_bridge
{

    class context : public context_engine
    {
    private:
        template<typename T> class resolve_link_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            resolve_link_task(_t_base const& src,data_sections_ptr section, sql_context_references_container const& cn)
                : db_task(section)
                , data_(src)
                , ref_(cn)
                {};
            inline void run_task() override {ref_.push_back(section_->resolve_link(data_));}
            void error(base_sql_error const& err) override {throw err;}
            inline sql_context_references_container const& references() const {return ref_;}
        private:
            _t_base const& data_;
            sql_context_references_container ref_;
        };

        template<typename T> class save_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            save_task(_t_base const& src,data_sections_ptr section,range pg)
                : db_task(section)
                , page_size_(pg)
                , data_(src)
                {};
            save_task(_t_base&& src,data_sections_ptr section,range pg)
                : db_task(section)
                , page_size_(pg)
                , data_(std::move(src))
                {};
            
            inline void run_task() override {if (!page_size_.empty()) section_->save_page(page_size_,data_); else section_->save(data_);}
            void error(base_sql_error const& err) override {std::cerr << err.what() << std::endl;}
        private:
            range page_size_;
            _t_base data_;
        };
        
        template<typename T> class save_sync_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            save_sync_task(_t_base const& src,data_sections_ptr section,range pg)
                : db_task(section)
                , page_size_(pg)
                , data_(src)
                {};
            inline void run_task() override {if (!page_size_.empty()) section_->save_page(page_size_,data_); else section_->save(data_);}
            void error(base_sql_error const& err) override {throw err;}
        private:
            range page_size_;
            _t_base const& data_;
        };

        template<typename T> class replace_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            replace_task(_t_base const& src,data_sections_ptr section)
                : db_task(section)
                , data_(src)
                {};
            replace_task(_t_base&& src,data_sections_ptr section)
                : db_task(section)
                , data_(std::move(src))
                {};
            inline void run_task() override {section_->replace(data_);}
            void error(base_sql_error const& err) override {std::cerr << err.what() << std::endl;}
        private:
            _t_base data_;
        };
        
        template<typename T> class replace_sync_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            replace_sync_task(_t_base const& src,data_sections_ptr section)
                : db_task(section)
                , data_(src)
                {};
            inline void run_task() override {section_->replace(data_);}
            void error(base_sql_error const& err) override {throw err;}
        private:
            _t_base const& data_;
        };

        template<typename T> class load_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            load_task(_t_base& dst,
                      data_sections_ptr section,
                      std::string const& flt)
                : db_task(section)
                , filter_(flt)
                , data_(dst)
                , items_load_(0)
                {};
            inline void run_task() override {section_->load(data_,filter_,items_load_);}
            void error(base_sql_error const& err) override {throw err;}
            size_t items_load() const {return items_load_;}
        private:
            std::string filter_;
            _t_base& data_;
            size_t items_load_;
        };

        template<typename T> class async_load_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            using _fn_success_load = std::function<void(_t_base&&,size_t)>;
            using _fn_failed = std::function<void(base_sql_error const&)>;
            
            async_load_task(_t_base const& src,
                            data_sections_ptr section,
                            std::string const& flt,
                            _fn_failed fl,
                            _fn_success_load fs)
                : db_task(section)
                , filter_(flt)
                , data_(src)
                , fn_success_(fs)
                , fn_failed_(fl)
                , items_load_(0)
                {};
            void run_task() override
            {
                section_->load(data_,filter_,items_load_);
                if (fn_success_)
                    fn_success_(std::move(data_),items_load_);
            };
            void error(base_sql_error const& err) override
            {
                if (fn_failed_)
                    fn_failed_(err);
                else
                {
                    _t_base def;
                    if (fn_success_)
                        fn_success_(std::move(def),0);
                    std::cerr << err.what() << std::endl;
                }
            }
            inline _t_base&& data() {return std::move(data_);}
        private:
            std::string filter_;
            _t_base data_;
            _fn_success_load fn_success_;
            _fn_failed fn_failed_;
            size_t items_load_;
        };

        template<typename T> class load_page_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            load_page_task(_t_base& dst,
                           data_sections_ptr section,
                           range pgsz,
                           std::string const& flt)
                : db_task(section)
                , page_size_(pgsz)
                , filter_(flt)
                , data_(dst)
                , items_load_(0)
                {};
            inline void run_task() override {section_->load_page(page_size_,data_,filter_,items_load_);}
            void error(base_sql_error const& err) override {throw err;}
            size_t items_load() const {return items_load_;}
        private:
            range page_size_;
            std::string filter_;
            _t_base& data_;
            size_t items_load_;
        };

        template<typename T> class async_load_page_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            using _fn_success_load = std::function<void(_t_base&&,size_t)>;
            using _fn_failed = std::function<void(base_sql_error const&)>;
            
            async_load_page_task(_t_base const& src,
                                 data_sections_ptr section,
                                 range pgsz,
                                 std::string const& flt,
                                 _fn_failed fl,
                                 _fn_success_load fs)
                : db_task(section)
                , page_size_(pgsz)
                , filter_(flt)
                , data_(src)
                , fn_success_(fs)
                , fn_failed_(fl)
                , items_load_(0)
                {};
            void run_task() override
            {
                section_->load_page(page_size_,data_,filter_,items_load_);
                if (fn_success_)
                    fn_success_(std::move(data_),items_load_);
            };
            void error(base_sql_error const& err) override
            {
                if (fn_failed_)
                    fn_failed_(err);
                else
                {
                    _t_base def;
                    if (fn_success_)
                        fn_success_(std::move(def),0);
                    std::cerr << err.what() << std::endl;
                }
            }
            inline _t_base&& data() {return std::move(data_);}
        private:
            range page_size_;
            std::string filter_;
            _t_base data_;
            _fn_success_load fn_success_;
            _fn_failed fn_failed_;
            size_t items_load_;
        };

        template<typename T> class remove_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            remove_task(_t_base const& src,data_sections_ptr section)
                : db_task(section)
                , data_(src)
                {};
            remove_task(_t_base&& src,data_sections_ptr section)
                : db_task(section)
                , data_(std::move(src))
                {};
            inline void run_task() override {section_->remove(data_);}
            void error(base_sql_error const& err) override {std::cerr << err.what() << std::endl;}
        private:
            _t_base data_;
        };

        template<typename T> class remove_sync_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            remove_sync_task(_t_base const& src,data_sections_ptr section)
                : db_task(section)
                , data_(src)
                {};
            inline void run_task() override {section_->remove(data_);}
            void error(base_sql_error const& err) override {throw err;}
        private:
            _t_base const& data_;
        };

        template<typename T> class remove_by_key_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
            using _t_key = typename _t_base::key_type;
        public:
            remove_by_key_task(_t_key const& val, data_sections_ptr section)
                : db_task(section)
                , value_(val)
                {};
            inline void run_task() override {section_->remove_by_key<T>(value_);}
            void error(base_sql_error const& err) override {std::cerr << err.what() << std::endl;}
        private:
            _t_key const value_;
        };

        template<typename T> class remove_if_task : public db_task
        {
            using _t_base = typename std::decay<T>::type;
        public:
            remove_if_task(std::string const& flt, data_sections_ptr section)
                : db_task(section)
                , condition_(flt)
                {};
            inline void run_task() override {section_->remove_if<T>(condition_);}
            void error(base_sql_error const& err) override {std::cerr << err.what() << std::endl;}
        private:
            std::string const condition_;
        };

    protected:
        context(db_tasks_queue_interface_ptr q, data_sections_ptr ds)
            : context_engine(q, ds)
            {}
        context() = delete;
        context(context const&) = delete;
        
    public:
        context(context&& src)
            : context_engine(std::move(src))
            {}

#pragma mark - public methods

        template<typename T> inline context& save(T const& src) {_save<T>(range(),src);return *this;}
        template<typename T> inline context& save(T const* src) {_save_sync<T>(range(),*src);return *this;}
        template<typename T> inline context& save(T* src) {_save_sync<T>(range(),*src);return *this;}
        template<typename T> inline context& save(T& src) {_save<T>(range(),src);return *this;}
        template<typename T> inline context& save(T&& src) {_save_m<T>(range(),std::move(src));return *this;}

        template<typename T> inline context& save(size_t pgsz, T const& src) {_save_sync<T>(range(0,pgsz),src);return *this;}
        template<typename T> inline context& save(size_t pgsz, T const* src) {_save_sync<T>(range(0,pgsz),*src);return *this;}
        template<typename T> inline context& save(size_t pgsz, T* src) {_save_sync<T>(range(0,pgsz),*src);return *this;}
        template<typename T> inline context& save(size_t pgsz, T& src) {_save_sync<T>(range(0,pgsz),src);return *this;}
        template<typename T> inline context& save(size_t pgsz, T&& src) {_save_m<T>(range(0,pgsz),std::move(src));return *this;}

        template<typename T> inline context& load(T& dst, std::string const& flt = "", size_t* num = nullptr) {_load<T>(dst,build_suffix(flt),num);return *this;}
        template<typename T> inline context& load(T* dst, std::string const& flt = "", size_t* num = nullptr) {_load<T>(*dst,build_suffix(flt),num);return *this;}
        template<typename T> inline context& load(T const& dst, std::string const& flt, typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) {_load<T>(dst,build_suffix(flt),fl,fs);return *this;}
        template<typename T> inline context& load(T const& dst, std::string const& flt, typename async_load_task<T>::_fn_success_load fs) {_load<T>(dst,build_suffix(flt),nullptr,fs);return *this;}

        template<typename T> inline context& load(size_t pgsz, T& dst, std::string const& flt = "", size_t* num = nullptr) {_load_page<T>(range(0,pgsz),dst,build_suffix(flt),num);return *this;}
        template<typename T> inline context& load(size_t pgsz, T* dst, std::string const& flt = "", size_t* num = nullptr) {_load_page<T>(range(0,pgsz),*dst,build_suffix(flt),num);return *this;}
        template<typename T> inline context& load(size_t pgsz, T const& dst, std::string const& flt, typename async_load_page_task<T>::_fn_failed fl, typename async_load_page_task<T>::_fn_success_load fs) {_load_page<T>(range(0,pgsz),dst,build_suffix(flt),fl,fs);return *this;}
        template<typename T> inline context& load(size_t pgsz, T const& dst, std::string const& flt, typename async_load_page_task<T>::_fn_success_load fs) {_load_page<T>(range(0,pgsz),dst,build_suffix(flt),nullptr,fs);return *this;}

        template<typename T> inline context& remove(T const& src) {_remove<T>(src);return *this;}
        template<typename T> inline context& remove(T const* src) {_remove_sync<T>(*src);return *this;}
        template<typename T> inline context& remove(T* src) {_remove_sync<T>(*src);return *this;}
        template<typename T> inline context& remove(T& src) {_remove<T>(src);return *this;}
        template<typename T> inline context& remove(T&& src) {_remove_m<T>(std::move(src));return *this;}

        template<typename T> inline typename std::enable_if<is_map<T>::value,context&>::type remove(typename T::key_type const& val) {_remove_by_key<T>(val);return *this;}
        template<typename T> inline typename std::enable_if<is_map<T>::value,context&>::type remove(typename T::key_type const* val) {_remove_by_key<T>(*val);return *this;}
        template<typename T> inline typename std::enable_if<is_map<T>::value,context&>::type remove(typename T::key_type& val) {_remove_by_key<T>(val);return *this;}
        template<typename T> inline typename std::enable_if<is_map<T>::value,context&>::type remove(typename T::key_type* val) {_remove_by_key<T>(*val);return *this;}
        template<typename T> inline context& remove_if(std::string const& flt = "") {_remove_if<typename types_selector<T>::type>(build_suffix(flt));return *this;}

        template<typename T> inline context& replace(T const& src) {_replace<T>(src);return *this;}
        template<typename T> inline context& replace(T const* src) {_replace_sync<T>(*src);return *this;}
        template<typename T> inline context& replace(T* src) {_replace_sync<T>(*src);return *this;}
        template<typename T> inline context& replace(T& src) {_replace<T>(src);return *this;}
        template<typename T> inline context& replace(T&& src) {_replace_m<T>(std::move(src));return *this;}
        
        template<typename T> inline ref_context at(T const& src) {return _at<T>(src);};
        template<typename T> inline ref_context at(T const* src) {return _at<T>(*src);};
        template<typename T> inline ref_context at(T& src) {return _at<T>(src);};
        template<typename T> inline ref_context at(T* src) {return _at<T>(*src);};

        inline context& limit(size_t count, size_t offset = 0) {context_engine::limit(count,offset);return *this;}
        inline context& sql_or() {context_engine::sql_or();return *this;}
        inline context& sql_and() {context_engine::sql_and();return *this;}

        template<typename T, typename TFn> inline context& order(TFn const T::*mem_ptr) {context_engine::order<T,TFn>(mem_ptr);return *this;}
        template<typename T, typename TFn> inline context& order_desc(TFn const T::*mem_ptr) {context_engine::order_desc<T,TFn>(mem_ptr);return *this;}

        template<typename T, typename TFn> inline context& where(TFn const T::*mem_ptr, std::string const& op, TFn const& val) {context_engine::where<T,TFn>(mem_ptr,op,val);return *this;}
        template<typename T, typename TFn> inline context& where_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {context_engine::where_between<T,TFn>(mem_ptr,from,to);return *this;}
        template<typename T, typename TFn> inline context& where_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {context_engine::where_between<T,TFn>(val,mem_ptr_from,mem_ptr_to);return *this;}
        template<typename T, typename TFn> inline context& where_not_between(TFn const T::*mem_ptr, TFn const& from, TFn const& to) {context_engine::where_not_between<T,TFn>(mem_ptr,from,to);return *this;}
        template<typename T, typename TFn> inline context& where_not_between(TFn const& val, TFn const T::*mem_ptr_from, TFn const T::*mem_ptr_to) {context_engine::where_not_between<T,TFn>(val,mem_ptr_from,mem_ptr_to);return *this;}
        template<typename T, typename TFn, typename TCont> inline context& where_in(TFn const T::*mem_ptr, TCont const& cnt) {context_engine::where_in<T,TFn,TCont>(mem_ptr,cnt); return *this;}
        template<typename T, typename TFn, typename TCont> inline context& where_not_in(TFn const T::*mem_ptr, TCont const& cnt) {context_engine::where_not_in<T,TFn,TCont>(mem_ptr,cnt); return *this;}
        template<typename T> inline context& where_like(std::string const T::*mem_ptr, std::string const& val) {context_engine::where_like<T>(mem_ptr,val);return *this;}
        template<typename T> inline context& where_not_like(std::string const T::*mem_ptr, std::string const& val) {context_engine::where_not_like<T>(mem_ptr,val);return *this;}

    private:
        // methods

#pragma mark - at

        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value &&
                                                            !is_container<T>::value &&
                                                            !is_map<T>::value &&
                                                            !is_pointer<T>::value,ref_context>::type _at(T const& src) const
        {
            struct s_proxy : ref_context {s_proxy(db_tasks_queue_interface_ptr q, data_sections_ptr ds, sql_context_references_container const& ref,void const* src) : ref_context(q,ds,ref,src) {}};
            db_task_ptr task(std::make_shared< resolve_link_task<T> >(src,data_,sql_context_references_container()));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add( task );
                return s_proxy(qp,data_,static_cast<resolve_link_task<T>*>(task.get())->references(),&src);
            }
            else
                return s_proxy(qp,data_,sql_context_references_container(),&src);
        }
        
#pragma mark - save sync

        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save_sync(range, T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }

        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _save_sync(range pg, T const& src) const
        {
            db_task_ptr task(std::make_shared< save_sync_task<T> >(src,data_,pg));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( task );
        }

#pragma mark - save
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save(range, T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save_m(range, T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_pointer<T>::value>::type _save(range pg, T const& src) const {_save_sync<T>(pg,src);}
        template<typename T> inline typename std::enable_if<!is_pointer<T>::value &&
                                                            !is_sql_acceptable<T>::value &&
                                                            !is_container<T>::value &&
                                                            !is_any_map<T>::value>::type _save(range pg, T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(src,data_,pg));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _save(range pg, T const& src) const {_save_cont<T>(pg,src);}
        template<typename T> inline typename std::enable_if<is_map<T>::value>::type _save(range pg, T const& src) const {_save_map<T>(pg,src);}
        template<typename T> inline typename std::enable_if<is_pointer<typename T::value_type>::value>::type _save_cont(range pg, T const& src) const {_save_sync<T>(pg,src);}
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::value_type>::value>::type _save_cont(range pg, T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(src,data_,pg));
        }
        template<typename T> inline typename std::enable_if<is_pointer<typename T::mapped_type>::value>::type _save_map(range pg, T const& src) const {_save_sync<T>(pg,src);}
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::mapped_type>::value>::type _save_map(range pg, T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(src,data_,pg));
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value &&
                                                            !is_container<T>::value &&
                                                            !is_map<T>::value>::type _save_m(range pg, T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(std::move(src),data_,pg));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _save_m(range pg, T&& src) const {_save_cont_m<T>(pg,std::move(src));}
        template<typename T> inline typename std::enable_if<is_map<T>::value>::type _save_m(range pg, T&& src) const {_save_map_m<T>(pg,std::move(src));}
        template<typename T> inline typename std::enable_if<is_pointer<typename T::value_type>::value>::type _save_cont_m(range pg, T&& src) const
        {
            db_task_ptr task(std::make_shared< save_task<T> >(std::move(src),data_,pg));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( task );
        }
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::value_type>::value>::type _save_cont_m(range pg, T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(std::move(src),data_,pg));
        }
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::mapped_type>::value>::type _save_map_m(range pg, T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<save_task<T> >(std::move(src),data_,pg));
        }
        template<typename T> inline typename std::enable_if<is_pointer<typename T::mapped_type>::value>::type _save_map_m(range pg, T&& src) const
        {
            db_task_ptr task(std::make_shared< save_task<T> >(std::move(src),data_,pg));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( task );
        }
        
#pragma mark - load page
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _load_page(range,T const&,std::string const&,typename async_load_task<T>::_fn_failed, typename async_load_task<T>::_fn_success_load) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _load_page(range pgsz, T const& dst,std::string const& flt,typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) const
        {
            db_task_ptr task(std::make_shared< async_load_page_task<T> >(dst,data_,pgsz,flt,fl,fs));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add(task);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _load_page(range,T&,std::string const&,size_t*) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _load_page(range pgsz, T& dst,std::string const& flt,size_t* num) const
        {
            db_task_ptr task(std::make_shared< load_page_task<T> >(dst,data_,pgsz,flt));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add(task);
                if (num)
                    *num = static_cast<load_page_task<T>*>(task.get())->items_load();
            }
        }

#pragma mark - load
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _load(T const&,std::string const&,typename async_load_task<T>::_fn_failed, typename async_load_task<T>::_fn_success_load) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _load(T const& dst,std::string const& flt,typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) const
        {
            db_task_ptr task(std::make_shared< async_load_task<T> >(dst,data_,flt,fl,fs));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add(task);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _load(T&,std::string const&,size_t*) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _load(T& dst,std::string const& flt,size_t* num) const
        {
            db_task_ptr task(std::make_shared< load_task<T> >(dst,data_,flt));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add(task);
                if (num)
                    *num = static_cast<load_task<T>*>(task.get())->items_load();
            }
        }

#pragma mark - remove sync
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value ||
                                                            is_trivial_map<T>::value ||
                                                            is_trivial_container<T>::value>::type _remove_sync(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value &&
                                                            !is_trivial_map<T>::value &&
                                                            !is_trivial_container<T>::value>::type _remove_sync(T const& src) const
        {
            db_task_ptr task(std::make_shared< remove_sync_task<T> >(src,data_));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( task );
        }

#pragma mark - remove
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _remove(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _remove(T const& src) const {_remove_cont<T>(src);}
        template<typename T> inline typename std::enable_if<is_any_map<T>::value>::type _remove(T const& src) const {_remove_map<T>(src);}
        template<typename T> inline typename std::enable_if<is_trivial_container<T>::value>::type _remove_cont(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_trivial_map<T>::value>::type _remove_map(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _remove_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _remove_m(T&& src) const {_remove_cont_m<T>(std::move(src));}
        template<typename T> inline typename std::enable_if<is_any_map<T>::value>::type _remove_m(T&& src) const {_remove_map_m<T>(std::move(src));}
        template<typename T> inline typename std::enable_if<is_trivial_container<T>::value>::type _remove_cont_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_trivial_map<T>::value>::type _remove_map_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_pointer<T>::value>::type _remove(T const& src) const {_remove_sync<T>(src);}
        template<typename T> inline typename std::enable_if<!is_pointer<T>::value && !is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _remove(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<!is_trivial_container<T>::value && !is_pointer<typename T::value_type>::value>::type _remove_cont(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<!is_trivial_map<T>::value && !is_pointer<typename T::mapped_type>::value>::type _remove_map(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _remove_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<!is_trivial_container<T>::value && !is_pointer<typename T::value_type>::value>::type _remove_cont_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<!is_trivial_map<T>::value && !is_pointer<typename T::mapped_type>::value>::type _remove_map_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _remove_if(std::string const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _remove_if(std::string const& flt) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<remove_if_task<T> >(flt,data_));
        }
        template<typename T> inline typename std::enable_if<is_map<T>::value>::type _remove_by_key(typename T::key_type const& val) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<remove_by_key_task<T> >(val,data_));
        }

#pragma mark - replace sync
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _replace_sync(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value>::type _replace_sync(T const& src) const
        {
            db_task_ptr task(std::make_shared< replace_sync_task<T> >(src,data_));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( task );
        }

#pragma mark - replace
        
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _replace(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _replace_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_incorrect_operation_err_text);
        }
        template<typename T> inline typename std::enable_if<!is_pointer<T>::value && !is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _replace(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<replace_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _replace(T const& src) const {_replace_cont<T>(src);}
        template<typename T> inline typename std::enable_if<is_map<T>::value>::type _replace(T const& src) const {_replace_map<T>(src);}
        template<typename T> inline typename std::enable_if<is_pointer<typename T::value_type>::value>::type _replace_cont(T const& src) const {_replace_sync<T>(src);}
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::value_type>::value>::type _replace_cont(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<replace_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<is_pointer<typename T::mapped_type>::value>::type _replace_map(T const& src) const {_replace_sync<T>(src);}
        template<typename T> inline typename std::enable_if<!is_pointer<typename T::mapped_type>::value>::type _replace_map(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<replace_task<T> >(src,data_));
        }

        template<typename T> inline typename std::enable_if<!is_pointer<T>::value && !is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _replace_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<replace_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value>::type _replace_m(T&& src) const {_replace_cont_m<T>(std::move(src));}
        template<typename T> inline typename std::enable_if<is_map<T>::value>::type _replace_m(T&& src) const {_replace_map_m<T>(std::move(src));}
        template<typename T> inline typename std::enable_if<is_pointer<T>::value>::type _replace_cont_m(T&& src) const
        {
            db_task_ptr task(std::make_shared< replace_task<T> >(std::move(src),data_));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( task );
        }
        template<typename T> inline typename std::enable_if<!is_pointer<T>::value>::type _replace_cont_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<replace_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<is_pointer<T>::value>::type _replace_map_m(T&& src) const
        {
            db_task_ptr task(std::make_shared< replace_task<T> >(std::move(src),data_));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( task );
        }
        template<typename T> inline typename std::enable_if<!is_pointer<T>::value>::type _replace_map_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
                qp->add( std::make_shared<replace_task<T> >(std::move(src),data_));
        }

    };
    
};

#endif /* sb_context_h */
