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

#include "dwarfexpressionlocform.h"

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace forms
{

class DwarfExpressionLocForm::DwarfExpressionLocFormPrivate
{
public:
    DwarfExpressionLocFormPrivate()
    {

    }

    template<class InputIterator>
    DwarfExpressionLocFormPrivate(InputIterator first, InputIterator last)
        : _exprloc(first, last)
    {

    }

    DwarfExpressionLocFormPrivate(const DwarfExpressionLocFormPrivate &priv)
    {
        _exprloc = priv.exprloc();
    }

    const std::vector<char> &exprloc() const
    {
        return _exprloc;
    }

private:
    std::vector<char> _exprloc;
};

DwarfExpressionLocForm::DwarfExpressionLocForm()
    : DwarfForm(Class::ExpressionLoc)
    , _p(new DwarfExpressionLocFormPrivate())
{
}

DwarfExpressionLocForm::DwarfExpressionLocForm(char *first, char *last)
    : DwarfForm(Class::ExpressionLoc)
    , _p(new DwarfExpressionLocFormPrivate(first, last))
{
}

DwarfExpressionLocForm::~DwarfExpressionLocForm()
{

}

DwarfExpressionLocForm::DwarfExpressionLocForm(const DwarfExpressionLocForm &form)
    : DwarfForm(form)
{
    assert(_p);
    assert(form._p);
    _p.reset(new DwarfExpressionLocFormPrivate(*form._p));
}

DwarfExpressionLocForm::DwarfExpressionLocForm(DwarfExpressionLocForm &&form)
    : DwarfExpressionLocForm()
{
    std::swap(*this, form);
}

DwarfExpressionLocForm& DwarfExpressionLocForm::operator= (DwarfExpressionLocForm form)
{
    std::swap(*this, form);
    return *this;
}

void DwarfExpressionLocForm::swap(DwarfExpressionLocForm &form)
{
    DwarfForm::swap(form);
    std::swap(_p, form._p);
}

//
// Properties
//

const std::vector<char> &DwarfExpressionLocForm::exprloc() const
{
    assert(_p);
    return _p->exprloc();
}

} // namespace forms
} // namespace dwarf
} // namespace dbg0
