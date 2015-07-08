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

#ifndef DWARFFORMFACTORY_H
#define DWARFFORMFACTORY_H

#include "dwarfform.h"

namespace dbg0
{
namespace dwarf
{
namespace factories
{

using namespace forms;

class DwarfFormFactory
{
public:

    // Singleton instance
    static DwarfFormFactory& instance();

    DwarfForm* clone(DwarfForm *form) const;
    DwarfForm* create(DwarfForm::Class _class) const;
    DwarfForm* createAddress(void *address) const;
    DwarfForm* createBlock(size_t len, void *blockData) const;
    DwarfForm* createExpressionLoc(size_t len, void *blockData) const;
    DwarfForm* createFlag(bool flag) const;
    DwarfForm* createLinePtr(size_t offset) const;
    DwarfForm* createLocationListPtr(size_t offset) const;
    DwarfForm* createMacroPtr(size_t offset) const;
    DwarfForm* createRangeListPtr(size_t offset) const;
    DwarfForm* createString(char *s) const;

    // References
    DwarfForm* createLocalReference(size_t offset) const;
    DwarfForm* createGlobalReference(size_t offset) const;
    DwarfForm* createSharedReference(u_int64_t offset) const;

    // Constants
    DwarfForm* createSignedConstant(int64_t sconst) const;
    DwarfForm* createUnsignedConstant(u_int64_t uconst) const;

private:
    // Non-copyable
    DwarfFormFactory();
    DwarfFormFactory(const DwarfFormFactory&) = delete;
    DwarfFormFactory &operator= (const DwarfFormFactory&) = delete;
};

} // namespace factories
} // namespace dwarf
} // namespace dbg0

#endif // DWARFFORMFACTORY_H
