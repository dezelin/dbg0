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

#include "dwarfformfactory.h"

#include "dwarfaddressform.h"
#include "dwarfblockform.h"
#include "dwarfconstantform.h"
#include "dwarfexpressionlocform.h"
#include "dwarfflagform.h"
#include "dwarflineptrform.h"
#include "dwarflocationlistptrform.h"
#include "dwarfmacroptrform.h"
#include "dwarfrangelistptrform.h"
#include "dwarfreferenceform.h"
#include "dwarfstringform.h"

#include <assert.h>

namespace dbg0
{
namespace dwarf
{
namespace factories
{

DwarfFormFactory::DwarfFormFactory()
{

}

DwarfFormFactory& DwarfFormFactory::instance()
{
    // C++11 doesn't need double-check locking.
    // This is enough to be thread-safe
    static DwarfFormFactory factory;
    return factory;
}

DwarfForm* DwarfFormFactory::clone(DwarfForm *form) const
{
    assert(form);
    if (!form)
        return nullptr;

    DwarfForm *cloned = nullptr;
    DwarfForm::Class _class = static_cast<DwarfForm::Class>(form->formClass());
    switch (_class) {
    case DwarfForm::Class::Address: {
        cloned = new DwarfAddressForm(*static_cast<DwarfAddressForm*>(form));
        break;
    }
    case DwarfForm::Class::Block: {
        cloned = new DwarfBlockForm(*static_cast<DwarfBlockForm*>(form));
        break;
    }
    case DwarfForm::Class::Constant: {
        cloned = new DwarfConstantForm(*static_cast<DwarfConstantForm*>(form));
        break;
    }
    case DwarfForm::Class::ExpressionLoc: {
        cloned = new DwarfExpressionLocForm(*static_cast<DwarfExpressionLocForm*>(form));
        break;
    }
    case DwarfForm::Class::Flag: {
        cloned = new DwarfFlagForm(*static_cast<DwarfFlagForm*>(form));
        break;
    }
    case DwarfForm::Class::LinePtr: {
        cloned = new DwarfLinePtrForm(*static_cast<DwarfLinePtrForm*>(form));
        break;
    }
    case DwarfForm::Class::LocationListPtr: {
        cloned = new DwarfLocationListPtrForm(*static_cast<DwarfLocationListPtrForm*>(form));
        break;
    }
    case DwarfForm::Class::MacroPtr: {
        cloned = new DwarfMacroPtrForm(*static_cast<DwarfMacroPtrForm*>(form));
        break;
    }
    case DwarfForm::Class::RangeListPtr: {
        cloned = new DwarfRangeListPtrForm(*static_cast<DwarfRangeListPtrForm*>(form));
        break;
    }
    case DwarfForm::Class::Reference: {
        cloned = new DwarfReferenceForm(*static_cast<DwarfReferenceForm*>(form));
        break;
    }
    case DwarfForm::Class::String: {
        cloned = new DwarfStringForm(*static_cast<DwarfStringForm*>(form));
        break;
    }
    default: {
        assert(!"Unknown form class.");
        break;
    }
    }

    return cloned;
}

DwarfForm* DwarfFormFactory::create(DwarfForm::Class _class) const
{
    DwarfForm *form = nullptr;
    switch (_class) {
    case DwarfForm::Class::Address: {
        form = new DwarfAddressForm();
        break;
    }
    case DwarfForm::Class::Block: {
        form = new DwarfBlockForm();
        break;
    }
    case DwarfForm::Class::Constant: {
        form = new DwarfConstantForm();
        break;
    }
    case DwarfForm::Class::ExpressionLoc: {
        form = new DwarfExpressionLocForm();
        break;
    }
    case DwarfForm::Class::Flag: {
        form = new DwarfFlagForm();
        break;
    }
    case DwarfForm::Class::LinePtr: {
        form = new DwarfLinePtrForm();
        break;
    }
    case DwarfForm::Class::LocationListPtr: {
        form = new DwarfLocationListPtrForm();
        break;
    }
    case DwarfForm::Class::MacroPtr: {
        form = new DwarfMacroPtrForm();
        break;
    }
    case DwarfForm::Class::RangeListPtr: {
        form = new DwarfRangeListPtrForm();
        break;
    }
    case DwarfForm::Class::Reference: {
        form = new DwarfReferenceForm();
        break;
    }
    case DwarfForm::Class::String: {
        form = new DwarfStringForm();
        break;
    }
    default: {
        assert(!"Unknown form class.");
        break;
    }
    }

    return form;
}

DwarfForm* DwarfFormFactory::createAddress(void *address) const
{
    // Address can be nullptr
    return new DwarfAddressForm(address);
}

DwarfForm* DwarfFormFactory::createBlock(size_t len, void *blockData) const
{
    assert(len > 0);
    assert(blockData);
    if (len < 1 || !blockData)
        return nullptr;

    return new DwarfBlockForm(static_cast<char *>(blockData),
                              static_cast<char *>(blockData) + len);
}

DwarfForm* DwarfFormFactory::createExpressionLoc(size_t len, void *blockData) const
{
    assert(len > 0);
    assert(blockData);
    if (len < 1 || !blockData)
        return nullptr;

    return new DwarfExpressionLocForm(static_cast<char *>(blockData),
                                      static_cast<char *>(blockData) + len);
}

DwarfForm* DwarfFormFactory::createFlag(bool flag) const
{
    return new DwarfFlagForm(flag);
}

DwarfForm* DwarfFormFactory::createLinePtr(size_t offset) const
{
    return new DwarfLinePtrForm(offset);
}

DwarfForm* DwarfFormFactory::createLocationListPtr(size_t offset) const
{
    return new DwarfLocationListPtrForm(offset);
}

DwarfForm* DwarfFormFactory::createMacroPtr(size_t offset) const
{
    return new DwarfMacroPtrForm(offset);
}

DwarfForm* DwarfFormFactory::createRangeListPtr(size_t offset) const
{
    return new DwarfRangeListPtrForm(offset);
}

DwarfForm* DwarfFormFactory::createString(char *s) const
{
    return new DwarfStringForm(s);
}

DwarfForm* DwarfFormFactory::createLocalReference(size_t offset) const
{
    return new DwarfReferenceForm(DwarfReferenceForm::Type::ReferenceLocal, offset);
}

DwarfForm* DwarfFormFactory::createGlobalReference(size_t offset) const
{
    return new DwarfReferenceForm(DwarfReferenceForm::Type::ReferenceGlobal, offset);
}

DwarfForm* DwarfFormFactory::createSharedReference(u_int64_t offset) const
{
    return new DwarfReferenceForm(DwarfReferenceForm::Type::ReferenceShared, offset);
}

DwarfForm* DwarfFormFactory::createSignedConstant(int64_t sconst) const
{
    return new DwarfConstantForm(sconst);
}

DwarfForm* DwarfFormFactory::createUnsignedConstant(u_int64_t uconst) const
{
    return new DwarfConstantForm(uconst);
}


} // namespace factories
} // namespace dwarf
} // namespace dbg0
