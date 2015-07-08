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

#include "dwarfflagform.h"

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace forms
{

class DwarfFlagForm::DwarfFlagFormPrivate
{
public:
    DwarfFlagFormPrivate(bool flag)
        : _flag(flag)
    {

    }

    DwarfFlagFormPrivate(const DwarfFlagFormPrivate &priv)
    {
        _flag = priv.flag();
    }

    bool flag() const
    {
        return _flag;
    }

private:
    bool _flag;
};

DwarfFlagForm::DwarfFlagForm(bool flag)
    : DwarfForm(Class::Flag)
    , _p(new DwarfFlagFormPrivate(flag))
{
}

DwarfFlagForm::~DwarfFlagForm()
{

}

DwarfFlagForm::DwarfFlagForm(const DwarfFlagForm &form)
    : DwarfForm(form)
{
    assert(_p);
    assert(form._p);
    _p.reset(new DwarfFlagFormPrivate(*form._p));
}

DwarfFlagForm::DwarfFlagForm(DwarfFlagForm &&form)
    : DwarfFlagForm()
{
    std::swap(*this, form);
}

DwarfFlagForm& DwarfFlagForm::operator= (DwarfFlagForm form)
{
    std::swap(*this, form);
    return *this;
}

void DwarfFlagForm::swap(DwarfFlagForm &form)
{
    DwarfForm::swap(form);
    std::swap(_p, form._p);
}

//
// Properties
//

bool DwarfFlagForm::flag() const
{
    assert(_p);
    return _p->flag();
}

} // namespace forms
} // namespace dwarf
} // namespace dbg0
