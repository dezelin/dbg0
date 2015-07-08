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

#include "dwarfrangelistptrform.h"

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace forms
{

class DwarfRangeListPtrForm::DwarfRangeListPtrFormPrivate
{
public:
    DwarfRangeListPtrFormPrivate(size_t rangelistptr)
        : _rangelistptr(rangelistptr)
    {

    }

    DwarfRangeListPtrFormPrivate(const DwarfRangeListPtrFormPrivate &priv)
    {
        _rangelistptr = priv.rangelistptr();
    }

    size_t rangelistptr() const
    {
        return _rangelistptr;
    }

private:
    size_t _rangelistptr;
};

DwarfRangeListPtrForm::DwarfRangeListPtrForm(size_t rangelistptr)
    : DwarfForm(Class::RangeListPtr)
    , _p(new DwarfRangeListPtrFormPrivate(rangelistptr))
{
}

DwarfRangeListPtrForm::~DwarfRangeListPtrForm()
{

}

DwarfRangeListPtrForm::DwarfRangeListPtrForm(const DwarfRangeListPtrForm &form)
    : DwarfForm(form)
{
    assert(_p);
    assert(form._p);
    _p.reset(new DwarfRangeListPtrFormPrivate(*form._p));
}

DwarfRangeListPtrForm::DwarfRangeListPtrForm(DwarfRangeListPtrForm &&form)
    : DwarfRangeListPtrForm()
{
    std::swap(*this, form);
}

DwarfRangeListPtrForm& DwarfRangeListPtrForm::operator= (DwarfRangeListPtrForm form)
{
    std::swap(*this, form);
    return *this;
}

void DwarfRangeListPtrForm::swap(DwarfRangeListPtrForm &form)
{
    DwarfForm::swap(form);
    std::swap(_p, form._p);
}

//
// Properties
//

size_t DwarfRangeListPtrForm::rangelistptr() const
{
    assert(_p);
    return _p->rangelistptr();
}

} // namespace forms
} // namespace dwarf
} // namespace dbg0
