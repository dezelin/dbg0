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

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace attributes
{

class DwarfAttribute::DwarfAttributePrivate
{
public:
    DwarfAttributePrivate(DwarfAttribute::Class attrClass, DwarfAttribute::Type type)
        : _class(attrClass)
        , _type(type)
    {

    }

    DwarfAttributePrivate(const DwarfAttributePrivate &priv)
    {
        _class = priv._class;
        _type = priv._type;
    }

    DwarfAttribute::Class attrClass() const
    {
        return _class;
    }

    DwarfAttribute::Type type() const
    {
        return _type;
    }

private:
    DwarfAttribute::Class _class;
    DwarfAttribute::Type _type;
};

DwarfAttribute::DwarfAttribute(Class attrClass, Type type)
    : Attribute()
    , _p(new DwarfAttributePrivate(attrClass, type))
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

int DwarfAttribute::attrClass() const
{
    assert(_p);
    return static_cast<int>(_p->attrClass());
}

int DwarfAttribute::type() const
{
    assert(_p);
    return static_cast<int>(_p->type());
}

} // namespace attributes
} // namespace dwarf
} // namespace dbg0
