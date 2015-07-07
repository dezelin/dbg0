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

#ifndef DWARFDIE_H
#define DWARFDIE_H

#include "die.h"

#include <memory>

namespace dbg0
{
namespace dwarf
{
namespace dies
{

using namespace interfaces::dies;

class DwarfDie : public Die
{
public:

    enum class Type : int {
        ArrayType = 0,
        ClassType,
        EntryPoint,
        EnumerationType,
        FormalParameter,
        ImportedDeclaration,
        Label,
        LexicalBlock,
        Member,
        PointerType,
        ReferenceType,
        CompileUnit,
        StringType,
        StructureType,
        SubroutineType,
        Typedef,
        UnionType,
        UnspecifiedParameters,
        Variant,
        CommonBlock,
        CommonInclusion,
        Inheritance,
        InlinedSubroutine,
        Module,
        PointerToMemberType,
        SetType,
        SubrangeType,
        WithStatement,
        AccessDeclaration,
        BaseType,
        CatchBlock,
        ConstType,
        Constant,
        Enumerator,
        FileType,
        Friend,
        Namelist,
        NamelistItem,
        PackedType,
        Subprogram,
        TemplateTypeParameter,
        TemplateValueParameter,
        ThrownType,
        TryBlock,
        VariantPart,
        Variable,
        VolatileType,
        DwarfProcedure,
        RestrictType,
        InterfaceType,
        Namespace,
        ImportedModule,
        UnspecifiedType,
        PartialUnit,
        ImportedUnit,
        MutableType,
        Condition,
        SharedType,
        TypeUnit,
        RValueReferenceType,
        TemplateAlias,
        LoUser,
        MIPSLoop,
        HPArrayDescriptor,
        FormatLabel,
        FunctionTemplate,
        ClassTemplate,
        GNUBINCL,
        GNUEINCL,
        GNUTemplateTemplateParameter,
        GNUTemplateParameterPack,
        GNUFormalParameterPack,
        GNUCallSite,
        GNUCallSiteParameter,
        ALTIUMCircType,
        ALTIUMMWACircType,
        ALTIUMRevCarryType,
        ALTIUMRom,
        UPCSharedType,
        UPCStrictType,
        UPCRelaxedType,
        PGIKanjiType,
        PGIInterfaceBlock,
        SUNFunctionTemplate,
        SUNClassTemplate,
        SUNStructTemplate,
        SUNUnionTemplate,
        SUNIndirectInheritance,
        SUNCodeFlags,
        SUNMemOpInfo,
        SUNOMPChildFunc,
        SUNRTTIDescriptor,
        SUNDtorInfo,
        SUNDtor,
        SUNf90Interface,
        SUNFortranVaxStructure,
        SUNHi,
        HiUser,

        UnknownType = -1
    };

    DwarfDie(DwarfDie::Type type = DwarfDie::Type::UnknownType);
    virtual ~DwarfDie();

    DwarfDie(const DwarfDie &die);
    DwarfDie(DwarfDie &&die);

    DwarfDie& operator= (DwarfDie die);

    void swap(DwarfDie &die);

    //
    // Interface
    //

    virtual void add(Attribute* attribute);

    virtual void add(Die* die);

    virtual const std::list<Attribute*>& attributes() const;

    virtual const std::list<Die*>& children() const;

    virtual int type() const;

private:
    class DwarfDiePrivate;
    std::unique_ptr<DwarfDiePrivate> _p;
};

} // namespace dies
} // namespace dwarf
} // namespace dbg0

#endif // DWARFDIE_H
