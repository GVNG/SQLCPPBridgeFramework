//
//  sb_core.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 01/08/16.
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
#ifndef sb_core_h
#define sb_core_h

#include <cassert>
#include "sb_core_misc.h"
#include "sb_traits.h"

namespace sql_bridge
{
    
    template<typename T> class _t_to_string
    {
    public:
        using type = std::decay_t<T>;
        using type_elem = typename T::value_type;
        using type_stream = std::basic_ostringstream<type_elem>;

        struct chrono_formatter
        {
            chrono_formatter(type const& fmt = type(), bool lc = false) : format_(fmt),local_(lc) {}
            inline bool empty() const {return format_.empty();}
            type format_;
            bool local_;
        };
        
        template<typename TFn> inline _t_to_string& operator << (TFn const& src) {_write(src);return *this;}
        inline operator type() const {return str();}
        inline type str() const {return buf_.str().substr(0,(size_t)buf_.tellp());}
        inline void remove_from_tail(long offs) {buf_.seekp(-offs, std::ios_base::cur);buf_.put(0);buf_.seekp(-1, std::ios_base::cur);}
        inline operator type_stream&() {return buf_;}
    private:
        mutable type_stream buf_;
        chrono_formatter chrono_format_;
        template<typename TFn> inline std::enable_if_t<is_chrono<TFn>::value> _write(TFn const& src)
        {
            if (chrono_format_.empty())
            {
                double pr = static_cast<double>(src.time_since_epoch().count()) / TFn::period::den * TFn::period::num;
                buf_ << std::setprecision(15) << pr;
            }
            else
            {
                auto const& tmput = std::use_facet < std::time_put<type_elem> > (std::locale::classic());
                std::time_t tt = TFn::clock::to_time_t(src);
                tmput.put(buf_,
                          buf_,
                          ' ',
                          chrono_format_.local_ ? std::localtime(&tt) : std::gmtime(&tt),
                          chrono_format_.format_.data(),
                          chrono_format_.format_.data()+chrono_format_.format_.length());
            }
        }
        template<typename TFn> inline std::enable_if_t<std::is_enum<TFn>::value> _write(TFn const& src) {buf_ << static_cast< std::underlying_type_t<TFn> >(src);}
        template<typename TFn> inline std::enable_if_t<std::is_same<TFn, chrono_formatter>::value> _write(TFn const& src) {chrono_format_ = src;}
        template<typename TFn> inline std::enable_if_t<!std::is_enum<TFn>::value &&
                                                       !is_chrono<TFn>::value &&
                                                       !std::is_same<TFn,chrono_formatter>::value> _write(TFn const& src) {buf_ << src;}
    };
    
    using to_string = _t_to_string<std::string>;
    using to_wstring = _t_to_string<std::wstring>;

    template<typename T> class protected_section
    {
    public:
        using type = std::decay_t<T>;
    private:
        template<bool,typename TInt> struct check_for_container {using value_type = TInt;};
        template<typename TInt> struct check_for_container<true,TInt> {using value_type = typename TInt::value_type;};
        using init_type = typename check_for_container<is_any_container<type>::value,type>::value_type;
        using pointer_type = std::unique_ptr<type>;
        using lock_guard = std::lock_guard<std::mutex>;
    public:
        class access
        {
            friend protected_section;
        public:
            access(protected_section const& src)
                : guard_(src.access_)
                , value_(src.pointer_.get())
                , mt_value_(nullptr)
                {}
            access(protected_section& src)
                : guard_(src.access_)
                , value_(src.pointer_.get())
                , mt_value_(src.pointer_.get())
                {}
            inline bool empty() const {return !value_;}
            inline type const& data() const {return *value_;}
            inline type& mutable_data() {assert(mt_value_);return *mt_value_;}
        private:
            lock_guard guard_;
            type const* value_;
            type* mt_value_;
        };
        inline protected_section()
            : pointer_(std::make_unique<type>())
            {};
        protected_section(protected_section const&) = delete;
        protected_section(protected_section&&) = delete;
        
        explicit inline protected_section(type&& src)
            : pointer_(std::make_unique<type>(std::forward(src)))
            {}
        inline protected_section(std::initializer_list<init_type> src)
            : pointer_(std::make_unique<type>(src))
            {}
        ~protected_section() {lock_guard lk(access_);pointer_.reset();}
    private:
        mutable std::mutex access_;
        pointer_type pointer_;
    };

    template<typename T> class _t_event
    {
        using type = T;
        using locker = std::unique_lock<type>;
        using lock_guard = std::lock_guard<type>;
        using guarded_function = std::function<void()>;
        using guarded_if_function = std::function<bool()>;
    public:
        _t_event() {};
        _t_event(_t_event const&) = delete;
        _t_event(_t_event&& v) = delete;

        inline void fire() {lock_guard lk(mtx_);var_.notify_one();}
        inline void fire_all() {lock_guard lk(mtx_);var_.notify_all();}
        template<typename _tfn> inline std::enable_if_t<is_duration<_tfn>::value,bool> wait_for(_tfn const& dl) {locker lk(mtx_);return var_.wait_for(lk, dl)==std::cv_status::no_timeout;}
        template<typename _tfn> inline std::enable_if_t<is_duration<_tfn>::value,bool> wait_for_if(_tfn const& dl,guarded_if_function fn) {locker lk(mtx_);if (fn()) return var_.wait_for(lk, dl)==std::cv_status::no_timeout; else {lk.unlock();return true;}}
        inline void wait(guarded_function fn=nullptr) {locker lk(mtx_);if (fn) fn(); var_.wait(lk);}
        inline void wait_if(guarded_if_function fn) {locker lk(mtx_);if (fn()) var_.wait(lk); else lk.unlock();}
        inline void under_guard(guarded_function fn) {lock_guard lk(mtx_);fn();}
        inline void under_guard_and_fire(guarded_function fn) {lock_guard lk(mtx_);fn();var_.notify_one();}
        inline void under_guard_and_fire_all(guarded_function fn) {lock_guard lk(mtx_);fn();var_.notify_all();}
    private:
        type mtx_;
        std::condition_variable var_;
    };
    
    template<typename T, typename TCounter> class _t_countdown
    {
        using type = T;
        using t_counter = TCounter;
        using locker = std::unique_lock<type>;
        using lock_guard = std::lock_guard<type>;
    public:
        _t_countdown(t_counter tc)
            : counter_(tc)
            {}
        _t_countdown(_t_countdown const&) = delete;
        _t_countdown(_t_countdown&& v) = delete;

        inline void wait() {locker lk(mtx_);if (counter_) var_.wait(lk);}
        inline void fire() {lock_guard lk(mtx_);if (!counter_) var_.notify_one(); else counter_--;}
    private:
        t_counter counter_;
        type mtx_;
        std::condition_variable var_;
    };
    
    using mt_event = _t_event<std::mutex>;
    using mt_countdown = _t_countdown<std::mutex,size_t>;
};

#endif /* sb_core_h */
