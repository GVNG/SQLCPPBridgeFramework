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

#include "sb_tasks_queue.h"
#include "sb_data_section.h"
#include "sb_stm_builder.h"

namespace sql_bridge
{

    class context
    {
    private:
        template<typename T> class save_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            save_task(T const& src,data_sections_ptr section)
                : section_(section)
                , data_(src)
                {};
            save_task(T&& src,data_sections_ptr section)
                : section_(section)
                , data_(std::move(src))
                {};
            
            inline void run_task() {section_->save(data_);};
            void error(base_sql_error const& err) {std::cerr << err.what() << std::endl;}
        private:
            data_sections_ptr section_;
            _t_base data_;
        };

        template<typename T> class replace_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            replace_task(T const& src,data_sections_ptr section)
                : section_(section)
                , data_(src)
                {};
            replace_task(T&& src,data_sections_ptr section)
                : section_(section)
                , data_(std::move(src))
                {};
            inline void run_task() {section_->replace(data_);};
            void error(base_sql_error const& err) {std::cerr << err.what() << std::endl;}
        private:
            data_sections_ptr section_;
            _t_base data_;
        };

        template<typename T> class load_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            load_task(data_sections_ptr section, std::string const& flt)
                : section_(section)
                , filter_(flt)
                {};
            inline void run_task() {section_->load(data_,filter_);};
            inline _t_base&& data() {return std::move(data_);}
            void error(base_sql_error const& err) {throw err;}
        private:
            data_sections_ptr section_;
            std::string filter_;
            _t_base data_;
        };

        template<typename T> class async_load_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            typedef std::function<void(_t_base&&)> _fn_success_load;
            typedef std::function<void(base_sql_error const&)> _fn_failed;
            
            async_load_task(_t_base const& src, data_sections_ptr section, std::string const& flt, _fn_failed fl, _fn_success_load fs)
                : section_(section)
                , filter_(flt)
                , data_(src)
                , fn_success_(fs)
                , fn_failed_(fl)
                {};
            void run_task()
            {
                section_->load(data_,filter_);
                if (fn_success_)
                    fn_success_(std::move(data_));
            };
            void error(base_sql_error const& err)
            {
                if (fn_failed_)
                    fn_failed_(err);
                else
                    std::cerr << err.what() << std::endl;
            }
            inline _t_base&& data() {return std::move(data_);}
        private:
            data_sections_ptr section_;
            std::string filter_;
            _t_base data_;
            _fn_success_load fn_success_;
            _fn_failed fn_failed_;
        };

        template<typename T> class remove_task : public db_task
        {
            typedef typename std::decay<T>::type _t_base;
        public:
            remove_task(T const& src,data_sections_ptr section)
                : section_(section)
                , data_(src)
                {};
            remove_task(T&& src,data_sections_ptr section)
                : section_(section)
                , data_(std::move(src))
                {};
            inline void run_task() {section_->remove(data_);};
            void error(base_sql_error const& err) {std::cerr << err.what() << std::endl;}
        private:
            data_sections_ptr section_;
            _t_base data_;
        };
        
    protected:
        context(db_tasks_queue_interface_ptr q, data_sections_ptr ds)
            : queue_(q)
            , data_(ds)
            {}
        context() = delete;
        context(context const&) = delete;
        
    public:
        context(context&& src)
        {
            std::swap(queue_, src.queue_);
            std::swap(data_, src.data_);
            std::swap(suffixes_, src.suffixes_);
        }

#pragma mark - public methods

        template<typename T> inline context& save(T const& src) {_save<T>(src);return *this;}
        template<typename T> inline context& save(T& src) {_save<T>(src);return *this;}
        template<typename T> inline context& save(T&& src) {_save_m<T>(std::move(src));return *this;}
        
        template<typename T> inline context& load(T& dst, std::string const& flt = "") {_load<T>(dst,build_suffix(flt));return *this;}
        template<typename T> inline context& load(T const& dst, std::string const& flt, typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) {_load<T>(dst,build_suffix(flt),fl,fs);return *this;}
        template<typename T> inline context& load(T const& dst, std::string const& flt, typename async_load_task<T>::_fn_success_load fs) {_load<T>(dst,build_suffix(flt),nullptr,fs);return *this;}

        template<typename T> inline context& remove(T const& src) {_remove<T>(src);return *this;}
        template<typename T> inline context& remove(T& src) {_remove<T>(src);return *this;}
        template<typename T> inline context& remove(T&& src) {_remove_m<T>(std::move(src));return *this;}
        
        template<typename T> inline context& replace(T const& src) {_replace<T>(src);return *this;}
        template<typename T> inline context& replace(T& src) {_replace<T>(src);return *this;}
        template<typename T> inline context& replace(T&& src) {_replace_m<T>(std::move(src));return *this;}

        inline context& limit(size_t count, size_t offset = 0) {_limit(count,offset);return *this;}

        template<typename T, typename TFn> inline context& order(TFn const T::*mem_ptr) {_order<T>(mem_ptr);return *this;}
        template<typename T, typename TFn> inline context& order_desc(TFn const T::*mem_ptr) {_order_desc<T>(mem_ptr);return *this;}

        template<typename T, typename TFn> inline context& where(TFn const T::*mem_ptr, std::string const& op, TFn const& val) {_where<T>(mem_ptr,op,val);return *this;}

    private:
        // methods
#pragma mark - save
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _save_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _save(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<save_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _save(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<save_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _save_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<save_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _save_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<save_task<T> >(std::move(src),data_));
        }
#pragma mark - load
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _load(T const&,std::string const&,typename async_load_task<T>::_fn_failed, typename async_load_task<T>::_fn_success_load) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _load(T const& dst,std::string const& flt,typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) const {_load_container<T>(dst,flt,fl,fs);}
        template<typename T> inline typename std::enable_if<is_trivial_container<T>::value || is_trivial_map<T>::value>::type _load_container(T const&,std::string const&,typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _load(T const& dst,std::string const& flt,typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) const
        {
            db_task_ptr task(std::make_shared< async_load_task<T> >(dst,data_,flt,fl,fs));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add(task);
        }
        template<typename T> inline typename std::enable_if<!is_trivial_container<T>::value && !is_trivial_map<T>::value>::type _load_container(T const& dst,std::string const& flt,typename async_load_task<T>::_fn_failed fl, typename async_load_task<T>::_fn_success_load fs) const
        {
            db_task_ptr task(std::make_shared< async_load_task<T> >(dst,data_,flt,fl,fs));
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add(task);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _load(T&,std::string const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _load(T& dst,std::string const& flt) const
        {
            db_task_ptr task(std::make_shared< load_task<T> >(data_,flt));
            std::future<void> ret(task->get_future());
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add(task);
                ret.get();
                dst = static_cast<load_task<T>*>(task.get())->data();
            }
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _load(T& dst,std::string const& flt) const
        {
            db_task_ptr task(std::make_shared< load_task<T> >(data_,flt));
            std::future<void> ret(task->get_future());
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr)
            {
                qp->add(task);
                ret.get();
                dst = static_cast<load_task<T>*>(task.get())->data();
            }
        }
#pragma mark - remove
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _remove(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _remove(T const& src) const {_remove_container<T>(src);}
        template<typename T> inline typename std::enable_if<is_trivial_container<T>::value || is_trivial_map<T>::value>::type _remove_container(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _remove_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _remove_m(T&& src) const {_remove_container_m<T>(std::move(src));}
        template<typename T> inline typename std::enable_if<is_trivial_container<T>::value || is_trivial_map<T>::value>::type _remove_container_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _remove(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<!is_trivial_container<T>::value && !is_trivial_map<T>::value>::type _remove_container(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _remove_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<!is_trivial_container<T>::value && !is_trivial_map<T>::value>::type _remove_container_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<remove_task<T> >(std::move(src),data_));
        }

#pragma mark - replace
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _replace(T const&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<is_sql_acceptable<T>::value>::type _replace_m(T&&) const
        {
            throw sql_bridge_error(g_internal_error_text, g_architecture_error_text);
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _replace(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<replace_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _replace(T const& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<replace_task<T> >(src,data_));
        }
        template<typename T> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _replace_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<replace_task<T> >(std::move(src),data_));
        }
        template<typename T> inline typename std::enable_if<is_container<T>::value || is_map<T>::value>::type _replace_m(T&& src) const
        {
            db_tasks_queue_interface_ptr qp = queue_.lock();
            if (qp!=nullptr) qp->add( std::make_shared<replace_task<T> >(std::move(src),data_));
        }
        
#pragma mark - order
        template<typename T,typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _order(TFn const T::*mem_ptr)
        {
            std::string field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_order>(field,false));
        }

        template<typename T,typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _order_desc(TFn const T::*mem_ptr)
        {
            std::string field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_order>(field,true));
        }

#pragma mark - limit

        void _limit(size_t count, size_t offset)
        {
            suffixes_.erase(std::remove_if(suffixes_.begin(),
                                           suffixes_.end(),
                                           [](suffix_bare_ptr sfx){return sfx->weight()==e_weight::LIMIT;}),suffixes_.end());
            suffixes_.push_back(std::make_shared<suffix_limit>(count,offset));
        }

#pragma mark - where
        
        template<typename T,typename TFn> inline typename std::enable_if<!is_sql_acceptable<T>::value && !is_container<T>::value && !is_map<T>::value>::type _where(TFn const T::*mem_ptr, std::string const& op, TFn const& val) {_where_def<T,TFn>(mem_ptr,op,val);}
        template<typename T,typename TFn> inline typename std::enable_if<is_convertible_to_text<TFn>::value>::type _where_def(TFn const T::*mem_ptr, std::string const& op, TFn const& val)
        {
            std::string field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_where>(field,to_string() << op << "'" << val << "'"));
        }
        template<typename T,typename TFn> inline typename std::enable_if<!is_convertible_to_text<TFn>::value>::type _where_def(TFn const T::*mem_ptr, std::string const& op, TFn const& val)
        {
            std::string field = data_->field_name(mem_ptr);
            suffixes_.push_back(std::make_shared<suffix_where>(field,to_string() << op << val));
        }

#pragma mark - members
        // members
        db_tasks_queue_interface_weak_ptr queue_;
        data_sections_ptr data_;
        suffixes_container suffixes_;
        
        std::string build_suffix(std::string const& usr)
        {
            if (usr.empty())
            {
                to_string ret;
                std::stable_sort(suffixes_.begin(),suffixes_.end(),[](suffix_bare_ptr lv,suffix_bare_ptr rv){return *lv < *rv;});
                e_weight cw = e_weight::BASE;
                bool repeat = false;
                for(auto const& sfx : suffixes_)
                {
                    if (sfx->weight()!=cw)
                    {
                        ret << " " << sfx->general(data_) << " " << sfx->build(data_);
                        repeat = false;
                        cw = sfx->weight();
                    }
                    else
                    {
                        if (repeat) ret << ",";
                        repeat = true;
                        ret << " " << sfx->build(data_);
                    }
                }
                suffixes_.clear();
                return ret;
            }
            else
            {
                suffixes_.clear();
                return usr;
            }
        }
    };
    
};

#endif /* sb_context_h */
