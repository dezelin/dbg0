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

#include "dwarfcompilationunit.h"

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace dies
{

class DwarfCompilationUnit::DwarfCompilationUnitPrivate
{
public:
    DwarfCompilationUnitPrivate()
        : _headerLength(0)
        , _version(0)
        , _abbrevOffset(0)
        , _addressSize(0)
    {
    }

    DwarfCompilationUnitPrivate(const DwarfCompilationUnitPrivate &priv)
    {
        _headerLength = priv.headerLength();
        _version = priv.version();
        _abbrevOffset = priv.abbrevOffset();
        _addressSize = priv.addressSize();
    }

    size_t headerLength() const
    {
        return _headerLength;
    }

    int version() const
    {
        return _version;
    }

    size_t abbrevOffset() const
    {
        return _abbrevOffset;
    }

    int addressSize() const
    {
        return _addressSize;
    }

    void setHeaderLength(size_t length)
    {
        _headerLength = length;
    }

    void setVersion(int version)
    {
        _version = version;
    }

    void setAbbrevOffset(size_t offset)
    {
        _abbrevOffset = offset;
    }

    void setAddressSize(int size)
    {
        _addressSize = size;
    }

private:
    size_t _headerLength;
    int _version;
    size_t _abbrevOffset;
    int _addressSize;
};

DwarfCompilationUnit::DwarfCompilationUnit()
    : DwarfDie(DwarfDie::Type::CompileUnit)
    , _p(new DwarfCompilationUnitPrivate())
{
}

DwarfCompilationUnit::~DwarfCompilationUnit()
{
}

DwarfCompilationUnit::DwarfCompilationUnit(const DwarfCompilationUnit &cu)
    : DwarfDie(cu)
{
    assert(_p);
    _p.reset(new DwarfCompilationUnitPrivate(*cu._p));
}

DwarfCompilationUnit::DwarfCompilationUnit(DwarfCompilationUnit &&cu)
    : DwarfDie()
{
    std::swap(*this, cu);
}

DwarfCompilationUnit &DwarfCompilationUnit::operator =(DwarfCompilationUnit cu)
{
    std::swap(*this, cu);
    return *this;
}

void DwarfCompilationUnit::swap(DwarfCompilationUnit &cu)
{
    DwarfDie::swap(cu);
    std::swap(_p, cu._p);
}

//
// Properties
//

size_t DwarfCompilationUnit::headerLength() const
{
    assert(_p);
    return _p->headerLength();
}

int DwarfCompilationUnit::version() const
{
    assert(_p);
    return _p->version();
}

size_t DwarfCompilationUnit::abbrevOffset() const
{
    assert(_p);
    return _p->abbrevOffset();
}

int DwarfCompilationUnit::addressSize() const
{
    assert(_p);
    return _p->addressSize();
}

void DwarfCompilationUnit::setHeaderLength(size_t length)
{
    assert(_p);
    _p->setHeaderLength(length);
}

void DwarfCompilationUnit::setVersion(int version)
{
    assert(_p);
    _p->setVersion(version);
}

void DwarfCompilationUnit::setAbbrevOffset(size_t offset)
{
    assert(_p);
    _p->setAbbrevOffset(offset);
}

void DwarfCompilationUnit::setAddressSize(int size)
{
    assert(_p);
    _p->setAddressSize(size);
}

} // namespace dies
} // namespace dwarf
} // namespace dbg0
