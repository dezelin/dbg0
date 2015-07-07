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

#ifndef DWARFATTRIBUTE_H
#define DWARFATTRIBUTE_H

#include "attribute.h"

#include <memory>

namespace dbg0
{
namespace dwarf
{

using namespace interfaces;

class DwarfAttribute : public Attribute
{
public:

    enum class Class : int {
        UnknownClass = -1
    };

    enum class Type : int {
        UnknownType = -1
    };

    DwarfAttribute(Class attrClass = Class::UnknownClass,
        Type type = Type::UnknownType);
    virtual ~DwarfAttribute();

    DwarfAttribute(const DwarfAttribute &attr);
    DwarfAttribute(DwarfAttribute &&attr);

    DwarfAttribute &operator= (DwarfAttribute attr);

    void swap(DwarfAttribute &attr);

    //
    // Interface Attribute
    //

    virtual int attrClass() const;
    virtual int type() const;

private:
    class DwarfAttributePrivate;
    std::unique_ptr<DwarfAttributePrivate> _p;
};

} // namespace dwarf
} // namespace dbg0

#endif // DWARFATTRIBUTE_H
