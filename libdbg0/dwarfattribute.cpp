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

#include "dwarfattribute.h"
#include "dwarfform.h"
#include "dwarfformfactory.h"

#include <assert.h>
#include <memory>

namespace dbg0
{
namespace dwarf
{
namespace attributes
{

using namespace factories;
using namespace forms;

class DwarfAttribute::DwarfAttributePrivate
{
public:
    DwarfAttributePrivate(DwarfAttribute::Type type, DwarfForm *form)
        : _form(form)
        , _type(type)
    {

    }

    DwarfAttributePrivate(const DwarfAttributePrivate &priv)
    {
        _form.reset(DwarfFormFactory::instance().clone(priv._form.get()));
        _type = priv._type;
    }

    //
    // Interface Attribute
    //

    Form* form() const
    {
        return _form.get();
    }

    DwarfAttribute::Type type() const
    {
        return _type;
    }

private:
    std::unique_ptr<DwarfForm> _form;
    DwarfAttribute::Type _type;
};

DwarfAttribute::DwarfAttribute(Type type, DwarfForm *from)
    : Attribute()
    , _p(new DwarfAttributePrivate(type, from))
{
}

DwarfAttribute::~DwarfAttribute()
{

}

DwarfAttribute::DwarfAttribute(const DwarfAttribute &attr)
{
    _p.reset(new DwarfAttributePrivate(*attr._p));
}

DwarfAttribute::DwarfAttribute(DwarfAttribute &&attr)
    : DwarfAttribute()
{
    std::swap(*this, attr);
}

DwarfAttribute &DwarfAttribute::operator= (DwarfAttribute attr)
{
    std::swap(*this, attr);
    return *this;
}

void DwarfAttribute::swap(DwarfAttribute &attr)
{
    std::swap(_p, attr._p);
}

//
// Interface Attribute
//

Form* DwarfAttribute::form() const
{
    assert(_p);
    return _p->form();
}

int DwarfAttribute::type() const
{
    assert(_p);
    return static_cast<int>(_p->type());
}

} // namespace attributes
} // namespace dwarf
} // namespace dbg0
