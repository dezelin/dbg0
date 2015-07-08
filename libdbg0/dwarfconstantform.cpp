//
// Copyright (c) 2015, Aleksandar Dezelin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of dbg0 nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "dwarfconstantform.h"

#include <assert.h>
#include <algorithm>

namespace dbg0
{
namespace dwarf
{
namespace forms
{

class DwarfConstantForm::DwarfConstantFormPrivate
{
public:
    DwarfConstantFormPrivate(DwarfConstantForm::Type type)
        : _type(type)
    {
        _constant.u = 0;
    }

    DwarfConstantFormPrivate(DwarfConstantForm::Type type, int64_t c)
        : _type(type)
    {
        _constant.s = c;
    }

    DwarfConstantFormPrivate(DwarfConstantForm::Type type, u_int64_t c)
        : _type(type)
    {
        _constant.u = c;
    }

    DwarfConstantFormPrivate(const DwarfConstantFormPrivate &priv)
    {
        _type = priv.type();
        _constant = priv.constant();
    }

    DwarfConstantForm::Constant constant() const
    {
        return _constant;
    }

    DwarfConstantForm::Type type() const
    {
        return _type;
    }

private:
    DwarfConstantForm::Constant _constant;
    DwarfConstantForm::Type _type;
};

DwarfConstantForm::DwarfConstantForm(Type type)
    : DwarfForm(Class::Constant)
    , _p(new DwarfConstantFormPrivate(type))
{
}

DwarfConstantForm::DwarfConstantForm(int64_t sconst)
    : DwarfForm(Class::Constant)
    , _p(new DwarfConstantFormPrivate(Type::Signed, sconst))
{
}

DwarfConstantForm::DwarfConstantForm(u_int64_t uconst)
    : DwarfForm(Class::Constant)
    , _p(new DwarfConstantFormPrivate(Type::UnknownType, uconst))
{
}

DwarfConstantForm::~DwarfConstantForm()
{

}

DwarfConstantForm::DwarfConstantForm(const DwarfConstantForm &form)
    : DwarfForm(form)
{
    assert(_p);
    assert(form._p);
    _p.reset(new DwarfConstantFormPrivate(*form._p));
}

DwarfConstantForm::DwarfConstantForm(DwarfConstantForm &&form)
    : DwarfConstantForm()
{
    std::swap(*this, form);
}

DwarfConstantForm& DwarfConstantForm::operator= (DwarfConstantForm form)
{
    std::swap(*this, form);
    return *this;
}

void DwarfConstantForm::swap(DwarfConstantForm &form)
{
    DwarfForm::swap(form);
    std::swap(_p, form._p);
}

DwarfConstantForm::Constant DwarfConstantForm::constant() const
{
    assert(_p);
    return _p->constant();
}

DwarfConstantForm::Type DwarfConstantForm::type() const
{
    assert(_p);
    return _p->type();
}

} // namespace forms
} // namespace dwarf
} // namespace dbg0
