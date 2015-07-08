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

#include "dwarfaddressform.h"

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace forms
{

class DwarfAddressForm::DwarfAddressFormPrivate
{
public:
    DwarfAddressFormPrivate(void *address)
        : _address(address)
    {

    }

    DwarfAddressFormPrivate(const DwarfAddressFormPrivate &priv)
    {
        _address = priv.address();
    }

    void* address() const
    {
        return _address;
    }

    void setAddress(void *address)
    {
        _address = address;
    }

private:
    void *_address;
};

DwarfAddressForm::DwarfAddressForm(void *address)
    : DwarfForm(Class::Address)
    , _p(new DwarfAddressFormPrivate(address))
{
}

DwarfAddressForm::~DwarfAddressForm()
{

}

DwarfAddressForm::DwarfAddressForm(const DwarfAddressForm &form)
    : DwarfForm(form)
{
    assert(_p);
    assert(form._p);
    _p.reset(new DwarfAddressFormPrivate(*form._p));
}

DwarfAddressForm::DwarfAddressForm(DwarfAddressForm &&form)
    : DwarfAddressForm()
{
    std::swap(*this, form);
}

DwarfAddressForm& DwarfAddressForm::operator= (DwarfAddressForm form)
{
    std::swap(*this, form);
    return *this;
}

void DwarfAddressForm::swap(DwarfAddressForm &form)
{
    DwarfForm::swap(form);
    std::swap(_p, form._p);
}

//
// Propertes
//

void* DwarfAddressForm::address() const
{
    assert(_p);
    return _p->address();
}

void DwarfAddressForm::setAddress(void *address)
{
    assert(_p);
    _p->setAddress(address);
}

} // namespace forms
} // namespace dwarf
} // namespace dbg0
