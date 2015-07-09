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
#include "dwarfform.h"
#include "form.h"

#include <memory>

namespace dbg0
{
namespace dwarf
{
namespace attributes
{

using namespace interfaces::attributes;
using namespace interfaces::forms;
using namespace forms;

class DwarfAttribute : public Attribute
{
public:

    enum class Type : int {
        Sibling = 0,
        Location,
        Name,
        Ordering,
        SubscrData,
        ByteSize,
        BitOffset,
        BitSize,
        ElementList,
        StatementList,
        LowPC,
        HighPC,
        Language,
        Member,
        Discr,
        DiscrValue,
        Visibility,
        Import,
        StringLength,
        CommonReference,
        CompDir,
        ConstValue,
        ContainingType,
        DefaultValue,
        Inline,
        IsOptional,
        LowerBound,
        Producer,
        Prototyped,
        ReturnAddr,
        StartScope,
        BitStride,
        StrideSize, // DWARF2 name
        UpperBound,
        AbstractOrigin,
        Accessibility,
        AddressClass,
        Artificial,
        BaseTypes,
        CallingConvention,
        Count,
        DataMemberLocation,
        DeclColumn,
        DeclFile,
        DeclLine,
        Declaration,
        DiscrList,
        Encoding,
        External,
        FrameBase,
        Friend,
        IdentifierCase,
        MacroInfo,
        NamelistItem,
        Priority,
        Segment,
        Specification,
        StaticLink,
        Type,
        UseLocation,
        VariableParameter,
        Virtuality,
        VTableElemLocation,
        Allocated,
        Associated,
        DataLocation,
        ByteStride,
        Stride, // DWARF3 (do not use)
        EntryPC,
        UseUTF8,
        Extension,
        Ranges,
        Trampoline,
        CallColumn,
        CallFile,
        CallLine,
        Description,
        BinaryScale,
        DecimalScale,
        Small,
        DecimalSign,
        DigitCount,
        PictureString,
        Mutable,
        ThreadsScaled,
        Explicit,
        ObjectPointer,
        Endianity,
        Elemental,
        Pure,
        Recursive,
        Signature,
        MainSubprogram,
        DataBitOffset,
        ConstExpr,
        EnumClass,
        LinkageName,

        // HP extensions
        HPBlockIndex,
        LoUser,
        MIPSfde,
        MIPSLoopBegin,
        MIPSTailLoopBegin,
        MIPSEpilogBegin,
        MIPSLoopUnrollFactor,
        MIPSSoftwarePipelineDepth,
        MIPSLinkageName,
        MIPSStride,
        MIPSAbstractName,
        MIPSCloneOrigin,
        MIPSHasInlines,
        MIPSStrideByte,
        MIPSStrideElem,
        MIPSPtrDopetype,
        MIPSAllocatableDopetype,
        MIPSAssumedShapeDopetype,
        MIPSAssumedSize,

        // HP extensions
        HPUnmodifiable,
        HPActualsStatementsList,
        HPProcPerSection,
        HPRawDataPtr,
        HPPassByReference,
        HPOptLevel,
        HPProfVersionId,
        HPOptFlags,
        HPColdRegionLowPC,
        HPColdRegionHighPC,
        HPAllVariablesModifiable,
        HPLinkageName,
        HPProfFlags,
        CPQDiscontigRanges,
        CPQSemanticEvents,
        CPQSplitLifetimesVar,
        CPQSplitLifetimesRtn,
        CPQPrologueLength,
        INTELOtherEndian,

        // GNU extensions
        GNUSrcFilesNames,
        GNUSrcInfo,
        GNUMacroInfo,
        GNUSrcCoords,
        GNUBodyBegin,
        GNUBodyEnd,
        GNUVector,
        GNUGuardedBy,
        GNUPtGuardedBy,
        GNUGuarded,
        GNUPtGuarded,
        GNULocksExcluded,
        GNUExclusiveLocksRequired,
        GNUSharedLocksRequired,
        GNUOdrSignature,
        GNUTemplateName,
        GNUCallSiteValue,
        GNUCallSiteDataValue,
        GNUCallSiteTarget,
        GNUCallSiteTargetClobbered,
        GNUTailCall,
        GNUAllTailCallSites,
        GNUAllCallSites,
        GNUAllSourceCallSites,

        // ALTIUM extension: ALTIUM Compliant location lists (flag)
        ALTIUMLoclist,

        // Sun extensions
        SUNTemplate,
        VMSRtnbegPdAddress,
        SUNAlignment,
        SUNvtable,
        SUNCountGuarantee,
        SUNCommandLine,
        SUNvbase,
        SUNCompileOptions,
        SUNLanguage,
        SUNBrowserFile,
        SUNvtableABI,
        SUNFuncOffsets,
        SUNCfKind,
        SUNvtableIndex,
        SUNOmpTprivAddr,
        SUNOmpChildFunc,
        SUNFuncOffset,
        SUNMemopTypeRef,
        SUNProfileId,
        SUNMemopSignature,
        SUNObjDir,
        SUNObjFile,
        SUNOriginalName,
        SUNHwcprofSignature,
        SUNamd64ParmDump,
        SUNPartLinkName,
        SUNLinkName,
        SUNPassWithConst,
        SUNReturnWithConst,
        SUNImportByName,
        SUNf90Pointer,
        SUNPassByRef,
        SUNf90Allocatable,
        SUNf90AssumedShapeArray,
        SUNCVla,
        SUNReturnValuePtr,
        SUNDtorStart,
        SUNDtorLength,
        SUNDtorStateInitial,
        SUNDtorStateFinal,
        SUNDtorStateDeltas,
        SUNImportByLname,
        SUNf90UseOnly,
        SUNNamelistSpec,
        SUNIsOmpChildFunc,
        SUNFortranMainAlias,
        SUNFortranBased,

        // GNAT extensions
        GNATUseDescriptiveType,
        GNATDescriptiveType,

        // UPC extension
        UPCThreadsScaled,

        // PGI (STMicroelectronics) extensions
        PGILocalBase,
        PGISectionOffset,
        PGILinearStride,

        // Apple extensions
        APPLEOptimized,
        APPLEFlags,
        APPLEISA,
        APPLEBlock,
        APPLEMajorRuntimeVers,
        APPLERuntimeClass,
        APPLEOmitFramePtr,
        APPLEClosure,

        HiUser,

        UnknownType = -1
    };

    DwarfAttribute(Type type = Type::UnknownType, DwarfForm *from = nullptr);
    virtual ~DwarfAttribute();

    DwarfAttribute(const DwarfAttribute &attr);
    DwarfAttribute(DwarfAttribute &&attr);

    DwarfAttribute &operator= (DwarfAttribute attr);

    void swap(DwarfAttribute &attr);

    //
    // Interface Attribute
    //

    virtual Form* form() const;

    virtual int type() const;

private:
    class DwarfAttributePrivate;
    std::unique_ptr<DwarfAttributePrivate> _p;
};

} // namespace attributes
} // namespace dwarf
} // namespace dbg0

#endif // DWARFATTRIBUTE_H
