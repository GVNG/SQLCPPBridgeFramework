//
//  example13.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 27/07/16.
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
#ifndef example13_hpp
#define example13_hpp

#include "sqlcppbridge.h"

class Case13Element
{
    DECLARE_SQL_ACCESS(Case13Element);
public:
    Case13Element(float i = 0)
        : val_(i+1)
        {};
    inline bool operator == (Case13Element const& rv) const {return val_==rv.val_;}
private:
    float val_;
};

enum class EBase {Normal,Alt};

class Case13
{
    DECLARE_SQL_ACCESS(Case13);
public:
    Case13(EBase b=EBase::Normal)
        : base_(b)
        , elements_(10,Case13Element((float)b))
        , elements_alt_(5,Case13Element((float)b))
        {}
    inline bool operator == (Case13 const& rv) const {return base_==rv.base_ && elements_==rv.elements_ && elements_alt_==rv.elements_alt_;}
private:
    EBase base_;
    std::vector<Case13Element> elements_;
    std::vector<Case13Element> elements_alt_;
};


#endif /* example13_hpp */
