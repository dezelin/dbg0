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

#ifndef DWARFCOMPILATIONUNIT_H
#define DWARFCOMPILATIONUNIT_H

#include "dwarfdie.h"

#include <cstddef>
#include <list>
#include <memory>

namespace dbg0
{
namespace dwarf
{
namespace dies
{

using namespace interfaces::dies;

class DwarfCompilationUnit : public DwarfDie
{
public:
    DwarfCompilationUnit();
    virtual ~DwarfCompilationUnit();

    DwarfCompilationUnit(const DwarfCompilationUnit &cu);
    DwarfCompilationUnit(DwarfCompilationUnit &&cu);

    DwarfCompilationUnit &operator =(DwarfCompilationUnit cu);

    void swap(DwarfCompilationUnit &cu);

    //
    // Properties
    //

    size_t dieOffset() const;
    size_t headerLength() const;
    size_t headerOffset() const;
    int version() const;
    size_t abbrevOffset() const;
    int addressSize() const;

    void setDieOffset(size_t offset);
    void setHeaderLength(size_t length);
    void setHeaderOffset(size_t offset);
    void setVersion(int version);
    void setAbbrevOffset(size_t offset);
    void setAddressSize(int size);

private:
    class DwarfCompilationUnitPrivate;
    std::unique_ptr<DwarfCompilationUnitPrivate> _p;
};

} // namespace dies
} // namespace dwarf
} // namespace dbg0

#endif // DWARFCOMPILATIONUNIT_H
