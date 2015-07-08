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

#include "dwarfstringform.h"

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace forms
{

class DwarfStringForm::DwarfStringFormPrivate
{
public:
    DwarfStringFormPrivate()
    {

    }

    DwarfStringFormPrivate(const std::string &string)
        : _string(string)
    {

    }

    DwarfStringFormPrivate(const DwarfStringFormPrivate &priv)
    {
        _string = priv.string();
    }

    const std::string &string() const
    {
        return _string;
    }

private:
    std::string _string;
};

DwarfStringForm::DwarfStringForm()
    : DwarfForm(Class::String)
    , _p(new DwarfStringFormPrivate())
{
}

DwarfStringForm::DwarfStringForm(const std::string &string)
    : DwarfForm(Class::String)
    , _p(new DwarfStringFormPrivate(string))
{
}

DwarfStringForm::~DwarfStringForm()
{

}

DwarfStringForm::DwarfStringForm(const DwarfStringForm &form)
    : DwarfForm(form)
{
    assert(_p);
    assert(form._p);
    _p.reset(new DwarfStringFormPrivate(*form._p));
}

DwarfStringForm::DwarfStringForm(DwarfStringForm &&form)
    : DwarfStringForm()
{
    std::swap(*this, form);
}

DwarfStringForm& DwarfStringForm::operator= (DwarfStringForm form)
{
    std::swap(*this, form);
    return *this;
}

void DwarfStringForm::swap(DwarfStringForm &form)
{
    DwarfForm::swap(form);
    std::swap(_p, form._p);
}

//
// Properties
//

const std::string &DwarfStringForm::string() const
{
    assert(_p);
    return _p->string();
}

} // namespace forms
} // namespace dwarf
} // namespace dbg0
