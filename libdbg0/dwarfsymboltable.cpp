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

#include "dwarfattributefactory.h"
#include "dwarfcompilationunit.h"
#include "dwarfdiefactory.h"
#include "dwarfform.h"
#include "dwarfformfactory.h"
#include "dwarfsymboltable.h"

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

#include "logger.h"

#include <assert.h>
#include <dwarf.h>
#include <libdwarf.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <initializer_list>
#include <list>

namespace dbg0
{
namespace dwarf
{
namespace symbols
{

using namespace attributes;
using namespace dies;
using namespace factories;
using namespace forms;

class DwarfSymbolTable::DwarfSymbolTablePrivate
{
public:
    DwarfSymbolTablePrivate()
    {

    }

    DwarfSymbolTablePrivate(const DwarfSymbolTablePrivate& priv)
    {

    }

    ~DwarfSymbolTablePrivate()
    {

    }

    int readSymbolTable(const std::string &fileName)
    {
        int fd = open(fileName.c_str(), O_RDONLY);
        if (fd < 0)
            return fd;

        int err = addCompilationUnits(fd);
        close(fd);
        return err;
    }

    const std::list<Die*>& compilationUnits() const
    {
        return _units;
    }

private:
    int addCompilationUnits(int fd)
    {
        Dwarf_Debug dbg;
        Dwarf_Error error;
        int err = dwarf_init(fd, DW_DLC_READ, nullptr, nullptr, &dbg, &error);
        if (err != DW_DLV_OK)
            return -1;

        Dwarf_Unsigned headerLength;
        Dwarf_Half version;
        Dwarf_Unsigned abbrevOffset;
        Dwarf_Half addressSize;
        Dwarf_Unsigned cuHeader, nextCuHeader = 0;

        while(err == DW_DLV_OK) {
            // Get next compilation unit
            cuHeader = nextCuHeader;

            if ((err = dwarf_next_cu_header(dbg, &headerLength, &version,
                &abbrevOffset, &addressSize, &nextCuHeader, &error)) != DW_DLV_OK)
            {
                // Error or DW_DLV_NO_ENTRY (last entry)
                break;
            }

            if ((err = addCompilationUnit(dbg, headerLength, version,
                abbrevOffset, addressSize, cuHeader)) != DW_DLV_OK)
            {
                // Error
                break;
            }
        }

        if (err != DW_DLV_OK && err != DW_DLV_NO_ENTRY) {
            dwarf_finish(dbg, &error);
            return -2;
        }

        err = dwarf_finish(dbg, &error);
        if (err != DW_DLV_OK)
            return -3;

        return 0;
    }

    void addAttributes(DwarfCompilationUnit *cu, DwarfDie *ddie,
        std::initializer_list<int> attributes, Dwarf_Die die)
    {
        assert(cu);
        assert(ddie);
        std::for_each(attributes.begin(), attributes.end(), [&](Dwarf_Half attribute) {
            DwarfAttribute *attr = getAttribute(cu, die, attribute);
            if (attr)
                ddie->add(attr);
        });
    }

    int addCompilationUnit(Dwarf_Debug dbg, Dwarf_Unsigned headerLength,
        Dwarf_Half version, Dwarf_Unsigned abbrevOffset, Dwarf_Half addressSize,
        Dwarf_Off cuHeaderOffset)
    {
        int err;
        Dwarf_Error error;
        Dwarf_Die noDie = 0, cuDie = 0;

        // Compilation will have a single die sibling
        if ((err = dwarf_siblingof(dbg, noDie, &cuDie, &error)) != DW_DLV_OK)
            return err;

        Dwarf_Off cuDieOffset;
        if ((err = dwarf_get_cu_die_offset_given_cu_header_offset(dbg, cuHeaderOffset,
            &cuDieOffset, &error)) != DW_DLV_OK)
        {
            return err;
        }

        std::unique_ptr<DwarfCompilationUnit> cu(
            DwarfDieFactory::instance().createCompileUnit());
        cu->setDieOffset(cuDieOffset);
        cu->setHeaderLength(headerLength);
        cu->setHeaderOffset(cuHeaderOffset);
        cu->setVersion(version);
        cu->setAbbrevOffset(abbrevOffset);
        cu->setAddressSize(addressSize);

        if ((err = addCompilationUnitDie(dbg, cu.get(), cuDie)) == DW_DLV_OK)
            _units.push_back(cu.release());

        dwarf_dealloc(dbg, cuDie, DW_DLA_DIE);
        return err;
    }

    int addCompilationUnitDie(Dwarf_Debug dbg, DwarfCompilationUnit *cu, Dwarf_Die die)
    {
        assert(cu);

        auto attributes = { DW_AT_low_pc, DW_AT_high_pc, DW_AT_ranges, DW_AT_name,
            DW_AT_language, DW_AT_stmt_list, DW_AT_macro_info, DW_AT_comp_dir,
            DW_AT_producer, DW_AT_identifier_case, DW_AT_base_types, DW_AT_use_UTF8,
            DW_AT_main_subprogram };
        addAttributes(cu, cu, attributes, die);
    }

    int addDie(Dwarf_Debug dbg, DwarfCompilationUnit *cu, Dwarf_Die die)
    {
        int err;
        Dwarf_Half tag;
        Dwarf_Error error;
        if ((err = dwarf_tag(die, &tag, &error)) != DW_DLV_OK)
            return err;

        switch(tag) {
        case DW_TAG_array_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_class_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_entry_point: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_enumeration_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_formal_parameter: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_imported_declaration: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_label: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_lexical_block: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_member: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_pointer_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_reference_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_compile_unit: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_string_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_structure_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_subroutine_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_typedef: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_union_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_unspecified_parameters: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_variant: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_common_block: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_common_inclusion: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_inheritance: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_inlined_subroutine: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_module: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_ptr_to_member_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_set_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_subrange_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_with_stmt: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_access_declaration: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_base_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_catch_block: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_const_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_constant: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_enumerator: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_file_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_friend: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_namelist: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_namelist_item: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_packed_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_subprogram: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_template_type_parameter: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_template_value_parameter: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_thrown_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_try_block: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_variant_part: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_variable: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_volatile_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_dwarf_procedure: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_restrict_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_interface_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_namespace: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_imported_module: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_unspecified_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_partial_unit: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_imported_unit: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_mutable_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_condition: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_shared_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_type_unit: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_rvalue_reference_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_template_alias: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_lo_user: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_MIPS_loop: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_HP_array_descriptor: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_format_label: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_function_template: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_class_template: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_GNU_BINCL: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_GNU_EINCL: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_GNU_template_template_parameter: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_GNU_template_parameter_pack: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_GNU_formal_parameter_pack: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_GNU_call_site: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_GNU_call_site_parameter: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_ALTIUM_circ_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_ALTIUM_mwa_circ_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_ALTIUM_rev_carry_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_ALTIUM_rom: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_upc_shared_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_upc_strict_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_upc_relaxed_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_PGI_kanji_type: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_PGI_interface_block: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_function_template: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_class_template: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_struct_template: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_union_template: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_indirect_inheritance: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_codeflags: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_memop_info: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_omp_child_func: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_rtti_descriptor: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_dtor_info: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_dtor: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_f90_interface: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_fortran_vax_structure: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_SUN_hi: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        case DW_TAG_hi_user: {
            err = addArrayTypeDie(dbg, cu, die);
            break;
        }
        default: {
            assert(!"Unknown die tag.");
            break;
        }
        }

        return err;
    }

    int addArrayTypeDie(Dwarf_Debug dbg, Die *parent, Dwarf_Die die)
    {
        int err;
        Dwarf_Half tag;
        Dwarf_Error error;
        if ((err = dwarf_tag(die, &tag, &error)) != DW_DLV_OK)
            return err;

        assert(tag == DW_TAG_array_type);
        if (tag != DW_TAG_array_type)
            return -1;

        //std::unique_ptr<DwarfArrayTypeDie> _d(new DwarfArrayTypeDie());

        return err;
    }

    DwarfAttribute* getAttribute(DwarfCompilationUnit *cu, Dwarf_Die die,
        Dwarf_Half attribute)
    {
        assert(cu);

        int err;
        Dwarf_Bool haveAttr;
        Dwarf_Error error;

        if ((err = dwarf_hasattr(die, attribute, &haveAttr, &error)) != DW_DLV_OK)
            return nullptr;

        if (!haveAttr)
            return nullptr;

        Dwarf_Attribute attr;
        if ((err = dwarf_attr(die, attribute, &attr, &error)) != DW_DLV_OK)
            return nullptr;

        DwarfForm::Class _class = getAttributeFormClass(attr, cu->dieOffset());
        if (_class == DwarfForm::Class::UnknownClass)
            return nullptr;

        DwarfAttribute::Type _type = getAttributeType(attribute);
        if (_type == DwarfAttribute::Type::UnknownType)
            return nullptr;

        std::unique_ptr<DwarfForm> _form;
        switch (_class)
        {
        case DwarfForm::Class::Address: {
            Dwarf_Addr addr;
            if ((err = dwarf_formaddr(attr, &addr, &error)) != DW_DLV_OK)
                return nullptr;

            // Address can be nullptr
            _form.reset(DwarfFormFactory::instance().createAddress(
                reinterpret_cast<void*>(addr)));
            break;
        }
        case DwarfForm::Class::Block: {
            Dwarf_Block *block;
            if ((err = dwarf_formblock(attr, &block, &error)) != DW_DLV_OK)
                return nullptr;

            assert(block);
            if (!block) return nullptr;

            _form.reset(DwarfFormFactory::instance().createBlock(block->bl_len,
                block->bl_data));
            break;
        }
        case DwarfForm::Class::Constant: {
            Dwarf_Half formType;
            if ((err = dwarf_whatform(attr, &formType, &error)) != DW_DLV_OK)
                return nullptr;

            if (formType == DW_FORM_data1 || formType == DW_FORM_data2 ||
                formType == DW_FORM_data4 || formType == DW_FORM_data8 ||
                formType == DW_FORM_udata)
            {
                Dwarf_Unsigned udata;
                if ((err = dwarf_formudata(attr, &udata, &error)) != DW_DLV_OK)
                    return nullptr;

                _form.reset(DwarfFormFactory::instance().createUnsignedConstant(udata));
            }
            else if (formType == DW_FORM_sdata) {
                Dwarf_Signed sdata;
                if ((err = dwarf_formsdata(attr, &sdata, &error)) != DW_DLV_OK)
                    return nullptr;

                _form.reset(DwarfFormFactory::instance().createSignedConstant(sdata));
            }
            else {
                assert(!"Unsupported form type.");
                return nullptr;
            }

            break;
        }
        case DwarfForm::Class::ExpressionLoc: {
            Dwarf_Unsigned len;
            Dwarf_Ptr block;
            if ((err = dwarf_formexprloc(attr, &len, &block, &error)) != DW_DLV_OK)
                return nullptr;

            assert(len > 0);
            assert(block);
            if (len < 1 || !block) return nullptr;

            _form.reset(DwarfFormFactory::instance().createExpressionLoc(len, block));
            break;
        }
        case DwarfForm::Class::Flag: {
            Dwarf_Bool flag;
            if ((err = dwarf_formflag(attr, &flag, &error)) != DW_DLV_OK)
                return nullptr;

            _form.reset(DwarfFormFactory::instance().createFlag(flag));
            break;
        }
        case DwarfForm::Class::LinePtr: {
            // Reference to .debug_line section
            Dwarf_Off offset;
            if ((err = dwarf_global_formref(attr, &offset, &error)) != DW_DLV_OK)
                return nullptr;

            _form.reset(DwarfFormFactory::instance().createLinePtr(offset));
            break;
        }
        case DwarfForm::Class::LocationListPtr: {
            // Reference to .debug_loc section
            Dwarf_Off offset;
            if ((err = dwarf_global_formref(attr, &offset, &error)) != DW_DLV_OK)
                return nullptr;

            _form.reset(DwarfFormFactory::instance().createLocationListPtr(offset));
            break;
        }
        case DwarfForm::Class::MacroPtr: {
            // Reference to .debug_macinfo section
            Dwarf_Off offset;
            if ((err = dwarf_global_formref(attr, &offset, &error)) != DW_DLV_OK)
                return nullptr;

            _form.reset(DwarfFormFactory::instance().createMacroPtr(offset));
            break;
        }
        case DwarfForm::Class::RangeListPtr: {
            // Reference to .debug_ranges section
            Dwarf_Off offset;
            if ((err = dwarf_global_formref(attr, &offset, &error)) != DW_DLV_OK)
                return nullptr;

            _form.reset(DwarfFormFactory::instance().createRangeListPtr(offset));
            break;
        }
        case DwarfForm::Class::Reference: {
            Dwarf_Half formType;
            if ((err = dwarf_whatform(attr, &formType, &error)) != DW_DLV_OK)
                return nullptr;

            if (formType == DW_FORM_ref1 || formType == DW_FORM_ref2 ||
                formType == DW_FORM_ref4 || formType == DW_FORM_ref8 ||
                formType == DW_FORM_ref_udata)
            {
                // Reference to the same CU
                Dwarf_Off offset;
                if ((err = dwarf_formref(attr, &offset, &error)) != DW_DLV_OK)
                    return nullptr;

                _form.reset(DwarfFormFactory::instance().createLocalReference(offset));
            }
            else if (formType == DW_FORM_ref_addr) {
                // Reference to .debug_info section of the same executable
                Dwarf_Off offset;
                if ((err = dwarf_global_formref(attr, &offset, &error)) != DW_DLV_OK)
                    return nullptr;

                _form.reset(DwarfFormFactory::instance().createGlobalReference(offset));
            }
            else if (formType == DW_FORM_ref_sig8 ) {
                // Reference to another symbol table using 8-byte hash
                Dwarf_Sig8 sig8; // 8-byte hash
                if ((err = dwarf_formsig8(attr, &sig8, &error)) != DW_DLV_OK)
                    return nullptr;

                _form.reset(DwarfFormFactory::instance().createSharedReference(
                    reinterpret_cast<u_int64_t>(sig8.signature)));
            }
            else {
                assert(!"Unsupported form type.");
                return nullptr;
            }

            break;
        }
        case DwarfForm::Class::String: {
            Dwarf_Half formType;
            if ((err = dwarf_whatform(attr, &formType, &error)) != DW_DLV_OK)
                return nullptr;

            char *string = nullptr;
            if ((err = dwarf_formstring(attr, &string, &error)) != DW_DLV_OK)
                return nullptr;

            _form.reset(DwarfFormFactory::instance().createString(string));
            break;
        }
        case DwarfForm::Class::FramePtr:
            // DW_FORM_CLASS_FRAMEPTR is MIPS/IRIX only, and refers
            // to the DW_AT_MIPS_fde attribute (a reference to the
            // .debug_frame section).
        case DwarfForm::Class::UnknownClass:
        default: {
            assert(!"Unknown attribute form class.");
            break;
        }
        }

        std::unique_ptr<DwarfAttribute> _attr(DwarfAttributeFactory::instance().create(
            _type, _form.release()));

        return _attr.release();
    }

    DwarfForm::Class getAttributeFormClass(Dwarf_Attribute attr, Dwarf_Half cuOffset)
    {
        int err;
        Dwarf_Half attrType;
        Dwarf_Half formType;
        Dwarf_Error error;
        if ((err = dwarf_whatform(attr, &formType, &error)) != DW_DLV_OK) {
            LOG(error) << "Unknown attribute form type: " << err;
            return DwarfForm::Class::UnknownClass;
        }

        if ((err = dwarf_whatattr(attr, &attrType, &error)) != DW_DLV_OK) {
            LOG(error) << "Unkwown attribute type: " << err;
            return DwarfForm::Class::UnknownClass;
        }

        Dwarf_Form_Class _class;
        if ((_class = dwarf_get_form_class(4 /*DWARF4*/, attrType, cuOffset,
            formType)) == DW_FORM_CLASS_UNKNOWN)
        {
            LOG(error) << "Unknown attribute class: " << attrType << " "
                       << cuOffset << " " << formType;
            return DwarfForm::Class::UnknownClass;
        }

        DwarfForm::Class classType = DwarfForm::Class::UnknownClass;
        switch (_class)
        {
        case DW_FORM_CLASS_ADDRESS: {
            classType = DwarfForm::Class::Address;
            break;
        }
        case DW_FORM_CLASS_BLOCK: {
            classType = DwarfForm::Class::Block;
            break;
        }
        case DW_FORM_CLASS_CONSTANT: {
            classType = DwarfForm::Class::Constant;
            break;
        }
        case DW_FORM_CLASS_EXPRLOC: {
            classType = DwarfForm::Class::ExpressionLoc;
            break;
        }
        case DW_FORM_CLASS_FLAG: {
            classType = DwarfForm::Class::Flag;
            break;
        }
        case DW_FORM_CLASS_LINEPTR: {
            classType = DwarfForm::Class::LinePtr;
            break;
        }
        case DW_FORM_CLASS_LOCLISTPTR: {
            classType = DwarfForm::Class::LocationListPtr;
            break;
        }
        case DW_FORM_CLASS_MACPTR: {
            classType = DwarfForm::Class::MacroPtr;
            break;
        }
        case DW_FORM_CLASS_RANGELISTPTR: {
            classType = DwarfForm::Class::RangeListPtr;
            break;
        }
        case DW_FORM_CLASS_REFERENCE: {
            classType = DwarfForm::Class::Reference;
            break;
        }
        case DW_FORM_CLASS_STRING: {
            classType = DwarfForm::Class::String;
            break;
        }
        case DW_FORM_CLASS_FRAMEPTR: {
            // DW_FORM_CLASS_FRAMEPTR is MIPS/IRIX only, and refers
            // to the DW_AT_MIPS_fde attribute (a reference to the
            // .debug_frame section).
            classType = DwarfForm::Class::FramePtr;
            break;
        }
        case DW_FORM_CLASS_UNKNOWN:
        default: {
            assert(!"Unknown attribute form class.");
            break;
        }
        }

        return classType;
    }

    DwarfForm::Class xxxgetAttributeFormClass(Dwarf_Attribute attr)
    {
        int err;
        Dwarf_Half formType;
        Dwarf_Error error;
        if ((err = dwarf_whatform(attr, &formType, &error)) != DW_DLV_OK)
            return DwarfForm::Class::UnknownClass;

        DwarfForm::Class _class = DwarfForm::Class::UnknownClass;

        switch(formType) {
        case DW_FORM_addr: {
            _class = DwarfForm::Class::Address;
            break;
        }
        case DW_FORM_block:
        case DW_FORM_block1:
        case DW_FORM_block2:
        case DW_FORM_block4: {
            _class = DwarfForm::Class::Block;
            break;
        }
        case DW_FORM_data1:
        case DW_FORM_data2:
        case DW_FORM_data4:
        case DW_FORM_data8:
        case DW_FORM_sdata:
        case DW_FORM_udata: {
            _class = DwarfForm::Class::Constant;
            break;
        }
        case DW_FORM_exprloc: {
            _class = DwarfForm::Class::ExpressionLoc;
            break;
        }
        case DW_FORM_flag:
        case DW_FORM_flag_present: {
            _class = DwarfForm::Class::Flag;
            break;
        }
        case DW_FORM_ref_addr:
        case DW_FORM_ref1:
        case DW_FORM_ref2:
        case DW_FORM_ref4:
        case DW_FORM_ref8:
        case DW_FORM_ref_udata:
        case DW_FORM_ref_sig8: {
            _class = DwarfForm::Class::Reference;
            break;
        }
        case DW_FORM_strp:
        case DW_FORM_string: {
            _class = DwarfForm::Class::String;
            break;
        }

        case DW_FORM_indirect:
        case DW_FORM_sec_offset:

        default: {
            assert(!"Unknown form type.");
            break;
        }
        }

        return _class;
    }

    DwarfAttribute::Type getAttributeType(Dwarf_Half attribute)
    {
        // FIXME: There are duplicate attribute types as per defined in
        // DWARF4 standard. Resolve them!
        DwarfAttribute::Type type = DwarfAttribute::Type::UnknownType;
        switch (attribute)
        {
        case DW_AT_sibling: {
            type = DwarfAttribute::Type::Sibling;
            break;
        }
        case DW_AT_location: {
            type = DwarfAttribute::Type::Location;
            break;
        }
        case DW_AT_name: {
            type = DwarfAttribute::Type::Name;
            break;
        }
        case DW_AT_ordering: {
            type = DwarfAttribute::Type::Ordering;
            break;
        }
        case DW_AT_subscr_data: {
            type = DwarfAttribute::Type::SubscrData;
            break;
        }
        case DW_AT_byte_size: {
            type = DwarfAttribute::Type::ByteSize;
            break;
        }
        case DW_AT_bit_offset: {
            type = DwarfAttribute::Type::BitOffset;
            break;
        }
        case DW_AT_bit_size: {
            type = DwarfAttribute::Type::BitSize;
            break;
        }
        case DW_AT_element_list: {
            type = DwarfAttribute::Type::ElementList;
            break;
        }
        case DW_AT_stmt_list: {
            type = DwarfAttribute::Type::StatementList;
            break;
        }
        case DW_AT_low_pc: {
            type = DwarfAttribute::Type::LowPC;
            break;
        }
        case DW_AT_high_pc: {
            type = DwarfAttribute::Type::HighPC;
            break;
        }
        case DW_AT_language: {
            type = DwarfAttribute::Type::Language;
            break;
        }
        case DW_AT_member: {
            type = DwarfAttribute::Type::Member;
            break;
        }
        case DW_AT_discr: {
            type = DwarfAttribute::Type::Discr;
            break;
        }
        case DW_AT_discr_value: {
            type = DwarfAttribute::Type::DiscrValue;
            break;
        }
        case DW_AT_visibility: {
            type = DwarfAttribute::Type::Visibility;
            break;
        }
        case DW_AT_import: {
            type = DwarfAttribute::Type::Import;
            break;
        }
        case DW_AT_string_length: {
            type = DwarfAttribute::Type::StringLength;
            break;
        }
        case DW_AT_common_reference: {
            type = DwarfAttribute::Type::CommonReference;
            break;
        }
        case DW_AT_comp_dir: {
            type = DwarfAttribute::Type::CompDir;
            break;
        }
        case DW_AT_const_value: {
            type = DwarfAttribute::Type::ConstValue;
            break;
        }
        case DW_AT_containing_type: {
            type = DwarfAttribute::Type::ContainingType;
            break;
        }
        case DW_AT_default_value: {
            type = DwarfAttribute::Type::DefaultValue;
            break;
        }
        case DW_AT_inline: {
            type = DwarfAttribute::Type::Inline;
            break;
        }
        case DW_AT_is_optional: {
            type = DwarfAttribute::Type::IsOptional;
            break;
        }
        case DW_AT_lower_bound: {
            type = DwarfAttribute::Type::LowerBound;
            break;
        }
        case DW_AT_producer: {
            type = DwarfAttribute::Type::Producer;
            break;
        }
        case DW_AT_prototyped: {
            type = DwarfAttribute::Type::Prototyped;
            break;
        }
        case DW_AT_return_addr: {
            type = DwarfAttribute::Type::ReturnAddr;
            break;
        }
        case DW_AT_start_scope: {
            type = DwarfAttribute::Type::StartScope;
            break;
        }
        //case DW_AT_stride_size:
        case DW_AT_bit_stride: {
            type = DwarfAttribute::Type::BitStride;
            break;
        }
        case DW_AT_upper_bound: {
            type = DwarfAttribute::Type::UpperBound;
            break;
        }
        case DW_AT_abstract_origin: {
            type = DwarfAttribute::Type::AbstractOrigin;
            break;
        }
        case DW_AT_accessibility: {
            type = DwarfAttribute::Type::Accessibility;
            break;
        }
        case DW_AT_address_class: {
            type = DwarfAttribute::Type::AddressClass;
            break;
        }
        case DW_AT_artificial: {
            type = DwarfAttribute::Type::Artificial;
            break;
        }
        case DW_AT_base_types: {
            type = DwarfAttribute::Type::BaseTypes;
            break;
        }
        case DW_AT_calling_convention: {
            type = DwarfAttribute::Type::CallingConvention;
            break;
        }
        case DW_AT_count: {
            type = DwarfAttribute::Type::Count;
            break;
        }
        case DW_AT_data_member_location: {
            type = DwarfAttribute::Type::DataMemberLocation;
            break;
        }
        case DW_AT_decl_column: {
            type = DwarfAttribute::Type::DeclColumn;
            break;
        }
        case DW_AT_decl_file: {
            type = DwarfAttribute::Type::DeclFile;
            break;
        }
        case DW_AT_decl_line: {
            type = DwarfAttribute::Type::DeclLine;
            break;
        }
        case DW_AT_declaration: {
            type = DwarfAttribute::Type::Declaration;
            break;
        }
        case DW_AT_discr_list: {
            type = DwarfAttribute::Type::DiscrList;
            break;
        }
        case DW_AT_encoding: {
            type = DwarfAttribute::Type::Encoding;
            break;
        }
        case DW_AT_external: {
            type = DwarfAttribute::Type::External;
            break;
        }
        case DW_AT_frame_base: {
            type = DwarfAttribute::Type::FrameBase;
            break;
        }
        case DW_AT_friend: {
            type = DwarfAttribute::Type::Friend;
            break;
        }
        case DW_AT_identifier_case: {
            type = DwarfAttribute::Type::IdentifierCase;
            break;
        }
        case DW_AT_macro_info: {
            type = DwarfAttribute::Type::MacroInfo;
            break;
        }
        case DW_AT_namelist_item: {
            type = DwarfAttribute::Type::NamelistItem;
            break;
        }
        case DW_AT_priority: {
            type = DwarfAttribute::Type::Priority;
            break;
        }
        case DW_AT_segment: {
            type = DwarfAttribute::Type::Segment;
            break;
        }
        case DW_AT_specification: {
            type = DwarfAttribute::Type::Specification;
            break;
        }
        case DW_AT_static_link: {
            type = DwarfAttribute::Type::StaticLink;
            break;
        }
        case DW_AT_type: {
            type = DwarfAttribute::Type::Type;
            break;
        }
        case DW_AT_use_location: {
            type = DwarfAttribute::Type::UseLocation;
            break;
        }
        case DW_AT_variable_parameter: {
            type = DwarfAttribute::Type::VariableParameter;
            break;
        }
        case DW_AT_virtuality: {
            type = DwarfAttribute::Type::Virtuality;
            break;
        }
        case DW_AT_vtable_elem_location: {
            type = DwarfAttribute::Type::VTableElemLocation;
            break;
        }
        case DW_AT_allocated: {
            type = DwarfAttribute::Type::Allocated;
            break;
        }
        case DW_AT_associated: {
            type = DwarfAttribute::Type::Associated;
            break;
        }
        case DW_AT_data_location: {
            type = DwarfAttribute::Type::DataLocation;
            break;
        }
        //case DW_AT_stride:
        case DW_AT_byte_stride: {
            type = DwarfAttribute::Type::ByteStride;
            break;
        }
        case DW_AT_entry_pc: {
            type = DwarfAttribute::Type::EntryPC;
            break;
        }
        case DW_AT_use_UTF8: {
            type = DwarfAttribute::Type::UseUTF8;
            break;
        }
        case DW_AT_extension: {
            type = DwarfAttribute::Type::Extension;
            break;
        }
        case DW_AT_ranges: {
            type = DwarfAttribute::Type::Ranges;
            break;
        }
        case DW_AT_trampoline: {
            type = DwarfAttribute::Type::Trampoline;
            break;
        }
        case DW_AT_call_column: {
            type = DwarfAttribute::Type::CallColumn;
            break;
        }
        case DW_AT_call_file: {
            type = DwarfAttribute::Type::CallFile;
            break;
        }
        case DW_AT_call_line: {
            type = DwarfAttribute::Type::CallLine;
            break;
        }
        case DW_AT_description: {
            type = DwarfAttribute::Type::Description;
            break;
        }
        case DW_AT_binary_scale: {
            type = DwarfAttribute::Type::BinaryScale;
            break;
        }
        case DW_AT_decimal_scale: {
            type = DwarfAttribute::Type::DecimalScale;
            break;
        }
        case DW_AT_small: {
            type = DwarfAttribute::Type::Small;
            break;
        }
        case DW_AT_decimal_sign: {
            type = DwarfAttribute::Type::DecimalSign;
            break;
        }
        case DW_AT_digit_count: {
            type = DwarfAttribute::Type::DigitCount;
            break;
        }
        case DW_AT_picture_string: {
            type = DwarfAttribute::Type::PictureString;
            break;
        }
        case DW_AT_mutable: {
            type = DwarfAttribute::Type::Mutable;
            break;
        }
        case DW_AT_threads_scaled: {
            type = DwarfAttribute::Type::ThreadsScaled;
            break;
        }
        case DW_AT_explicit: {
            type = DwarfAttribute::Type::Explicit;
            break;
        }
        case DW_AT_object_pointer: {
            type = DwarfAttribute::Type::ObjectPointer;
            break;
        }
        case DW_AT_endianity: {
            type = DwarfAttribute::Type::Endianity;
            break;
        }
        case DW_AT_elemental: {
            type = DwarfAttribute::Type::Elemental;
            break;
        }
        case DW_AT_pure: {
            type = DwarfAttribute::Type::Pure;
            break;
        }
        case DW_AT_recursive: {
            type = DwarfAttribute::Type::Recursive;
            break;
        }
        case DW_AT_signature: {
            type = DwarfAttribute::Type::Signature;
            break;
        }
        case DW_AT_main_subprogram: {
            type = DwarfAttribute::Type::MainSubprogram;
            break;
        }
        case DW_AT_data_bit_offset: {
            type = DwarfAttribute::Type::DataBitOffset;
            break;
        }
        case DW_AT_const_expr: {
            type = DwarfAttribute::Type::ConstExpr;
            break;
        }
        case DW_AT_enum_class: {
            type = DwarfAttribute::Type::EnumClass;
            break;
        }
        case DW_AT_linkage_name: {
            type = DwarfAttribute::Type::LinkageName;
            break;
        }
        /* HP extensions. */
        //case DW_AT_lo_user:
        case DW_AT_HP_block_index: {
            type = DwarfAttribute::Type::HPBlockIndex;
            break;
        }
        //case DW_AT_HP_unmodifiable:
        //case DW_AT_CPQ_discontig_ranges:
        case DW_AT_MIPS_fde: {
            type = DwarfAttribute::Type::MIPSfde;
            break;
        }
        //case DW_AT_CPQ_split_lifetimes_var:
        case DW_AT_MIPS_tail_loop_begin: {
            type = DwarfAttribute::Type::MIPSTailLoopBegin;
            break;
        }
        //case DW_AT_CPQ_split_lifetimes_rtn:
        case DW_AT_MIPS_epilog_begin: {
            type = DwarfAttribute::Type::MIPSEpilogBegin;
            break;
        }
        //case DW_AT_CPQ_prologue_length:
        case DW_AT_MIPS_loop_unroll_factor: {
            type = DwarfAttribute::Type::MIPSLoopUnrollFactor;
            break;
        }
        case DW_AT_MIPS_software_pipeline_depth: {
            type = DwarfAttribute::Type::MIPSSoftwarePipelineDepth;
            break;
        }
        case DW_AT_MIPS_linkage_name: {
            type = DwarfAttribute::Type::MIPSLinkageName;
            break;
        }
        case DW_AT_MIPS_stride: {
            type = DwarfAttribute::Type::MIPSStride;
            break;
        }
        case DW_AT_MIPS_abstract_name: {
            type = DwarfAttribute::Type::MIPSAbstractName;
            break;
        }
        case DW_AT_MIPS_clone_origin: {
            type = DwarfAttribute::Type::MIPSCloneOrigin;
            break;
        }
        case DW_AT_MIPS_has_inlines: {
            type = DwarfAttribute::Type::MIPSHasInlines;
            break;
        }
        case DW_AT_MIPS_stride_byte: {
            type = DwarfAttribute::Type::MIPSStrideByte;
            break;
        }
        case DW_AT_MIPS_stride_elem: {
            type = DwarfAttribute::Type::MIPSStrideElem;
            break;
        }
        case DW_AT_MIPS_ptr_dopetype: {
            type = DwarfAttribute::Type::MIPSPtrDopetype;
            break;
        }
        case DW_AT_MIPS_allocatable_dopetype: {
            type = DwarfAttribute::Type::MIPSAllocatableDopetype;
            break;
        }
        //case DW_AT_HP_actuals_stmt_list:
        case DW_AT_MIPS_assumed_shape_dopetype: {
            type = DwarfAttribute::Type::MIPSAssumedShapeDopetype;
            break;
        }
        //case DW_AT_HP_proc_per_section:
        case DW_AT_MIPS_assumed_size: {
            type = DwarfAttribute::Type::MIPSAssumedSize;
            break;
        }
        case DW_AT_HP_raw_data_ptr: {
            type = DwarfAttribute::Type::HPRawDataPtr;
            break;
        }
        case DW_AT_HP_pass_by_reference: {
            type = DwarfAttribute::Type::HPPassByReference;
            break;
        }
        case DW_AT_HP_opt_level: {
            type = DwarfAttribute::Type::HPOptLevel;
            break;
        }
        case DW_AT_HP_prof_version_id: {
            type = DwarfAttribute::Type::HPProfVersionId;
            break;
        }
        case DW_AT_HP_opt_flags: {
            type = DwarfAttribute::Type::HPOptFlags;
            break;
        }
        case DW_AT_HP_cold_region_low_pc: {
            type = DwarfAttribute::Type::HPColdRegionLowPC;
            break;
        }
        case DW_AT_HP_cold_region_high_pc: {
            type = DwarfAttribute::Type::HPColdRegionHighPC;
            break;
        }
        case DW_AT_HP_all_variables_modifiable: {
            type = DwarfAttribute::Type::HPAllVariablesModifiable;
            break;
        }
        case DW_AT_HP_linkage_name: {
            type = DwarfAttribute::Type::HPLinkageName;
            break;
        }
        case DW_AT_HP_prof_flags: {
            type = DwarfAttribute::Type::HPProfFlags;
            break;
        }
        case DW_AT_CPQ_semantic_events: {
            type = DwarfAttribute::Type::CPQSemanticEvents;
            break;
        }
        case DW_AT_INTEL_other_endian: {
            type = DwarfAttribute::Type::INTELOtherEndian;
            break;
        }
        // GNU extensions.
        case DW_AT_sf_names: {
            type = DwarfAttribute::Type::GNUSrcFilesNames;
            break;
        }
        case DW_AT_src_info: {
            type = DwarfAttribute::Type::GNUSrcInfo;
            break;
        }
        case DW_AT_mac_info: {
            type = DwarfAttribute::Type::GNUMacroInfo;
            break;
        }
        case DW_AT_src_coords: {
            type = DwarfAttribute::Type::GNUSrcCoords;
            break;
        }
        case DW_AT_body_begin: {
            type = DwarfAttribute::Type::GNUBodyBegin;
            break;
        }
        case DW_AT_body_end: {
            type = DwarfAttribute::Type::GNUBodyEnd;
            break;
        }
        case DW_AT_GNU_vector: {
            type = DwarfAttribute::Type::GNUVector;
            break;
        }
        case DW_AT_GNU_guarded_by: {
            type = DwarfAttribute::Type::GNUGuardedBy;
            break;
        }
        case DW_AT_GNU_pt_guarded_by: {
            type = DwarfAttribute::Type::GNUPtGuardedBy;
            break;
        }
        case DW_AT_GNU_guarded: {
            type = DwarfAttribute::Type::GNUGuarded;
            break;
        }
        case DW_AT_GNU_pt_guarded: {
            type = DwarfAttribute::Type::GNUPtGuarded;
            break;
        }
        case DW_AT_GNU_locks_excluded: {
            type = DwarfAttribute::Type::GNULocksExcluded;
            break;
        }
        case DW_AT_GNU_exclusive_locks_required: {
            type = DwarfAttribute::Type::GNUExclusiveLocksRequired;
            break;
        }
        case DW_AT_GNU_shared_locks_required: {
            type = DwarfAttribute::Type::GNUSharedLocksRequired;
            break;
        }
        case DW_AT_GNU_odr_signature: {
            type = DwarfAttribute::Type::GNUOdrSignature;
            break;
        }
        case DW_AT_GNU_template_name: {
            type = DwarfAttribute::Type::GNUTemplateName;
            break;
        }
        //  The GNU call site extension.
        case DW_AT_GNU_call_site_value: {
            type = DwarfAttribute::Type::GNUCallSiteValue;
            break;
        }
        case DW_AT_GNU_call_site_data_value: {
            type = DwarfAttribute::Type::GNUCallSiteDataValue;
            break;
        }
        case DW_AT_GNU_call_site_target: {
            type = DwarfAttribute::Type::GNUCallSiteTarget;
            break;
        }
        case DW_AT_GNU_call_site_target_clobbered: {
            type = DwarfAttribute::Type::GNUCallSiteTargetClobbered;
            break;
        }
        case DW_AT_GNU_tail_call: {
            type = DwarfAttribute::Type::GNUTailCall;
            break;
        }
        case DW_AT_GNU_all_tail_call_sites: {
            type = DwarfAttribute::Type::GNUAllTailCallSites;
            break;
        }
        case DW_AT_GNU_all_call_sites: {
            type = DwarfAttribute::Type::GNUAllCallSites;
            break;
        }
        case DW_AT_GNU_all_source_call_sites: {
            type = DwarfAttribute::Type::GNUAllSourceCallSites;
            break;
        }
        // ALTIUM extension: ALTIUM Compliant location lists (flag)
        case DW_AT_ALTIUM_loclist: {
            type = DwarfAttribute::Type::ALTIUMLoclist;
            break;
        }
        // Sun extensions
        //case DW_AT_VMS_rtnbeg_pd_address:
        case DW_AT_SUN_template: {
            type = DwarfAttribute::Type::SUNTemplate;
            break;
        }
        case DW_AT_SUN_alignment: {
            type = DwarfAttribute::Type::SUNAlignment;
            break;
        }
        case DW_AT_SUN_vtable: {
            type = DwarfAttribute::Type::SUNvtable;
            break;
        }
        case DW_AT_SUN_count_guarantee: {
            type = DwarfAttribute::Type::SUNCountGuarantee;
            break;
        }
        case DW_AT_SUN_command_line: {
            type = DwarfAttribute::Type::SUNCommandLine;
            break;
        }
        case DW_AT_SUN_vbase: {
            type = DwarfAttribute::Type::SUNvbase;
            break;
        }
        case DW_AT_SUN_compile_options: {
            type = DwarfAttribute::Type::SUNCompileOptions;
            break;
        }
        case DW_AT_SUN_language: {
            type = DwarfAttribute::Type::SUNLanguage;
            break;
        }
        case DW_AT_SUN_browser_file: {
            type = DwarfAttribute::Type::SUNBrowserFile;
            break;
        }
        case DW_AT_SUN_vtable_abi: {
            type = DwarfAttribute::Type::SUNvtableABI;
            break;
        }
        case DW_AT_SUN_func_offsets: {
            type = DwarfAttribute::Type::SUNFuncOffsets;
            break;
        }
        case DW_AT_SUN_cf_kind: {
            type = DwarfAttribute::Type::SUNCfKind;
            break;
        }
        case DW_AT_SUN_vtable_index: {
            type = DwarfAttribute::Type::SUNvtableIndex;
            break;
        }
        case DW_AT_SUN_omp_tpriv_addr: {
            type = DwarfAttribute::Type::SUNOmpTprivAddr;
            break;
        }
        case DW_AT_SUN_omp_child_func: {
            type = DwarfAttribute::Type::SUNOmpChildFunc;
            break;
        }
        case DW_AT_SUN_func_offset: {
            type = DwarfAttribute::Type::SUNFuncOffset;
            break;
        }
        case DW_AT_SUN_memop_type_ref: {
            type = DwarfAttribute::Type::SUNMemopTypeRef;
            break;
        }
        case DW_AT_SUN_profile_id: {
            type = DwarfAttribute::Type::SUNProfileId;
            break;
        }
        case DW_AT_SUN_memop_signature: {
            type = DwarfAttribute::Type::SUNMemopSignature;
            break;
        }
        case DW_AT_SUN_obj_dir: {
            type = DwarfAttribute::Type::SUNObjDir;
            break;
        }
        case DW_AT_SUN_obj_file: {
            type = DwarfAttribute::Type::SUNObjFile;
            break;
        }
        case DW_AT_SUN_original_name: {
            type = DwarfAttribute::Type::SUNOriginalName;
            break;
        }
        case DW_AT_SUN_hwcprof_signature: {
            type = DwarfAttribute::Type::SUNHwcprofSignature;
            break;
        }
        case DW_AT_SUN_amd64_parmdump: {
            type = DwarfAttribute::Type::SUNamd64ParmDump;
            break;
        }
        case DW_AT_SUN_part_link_name: {
            type = DwarfAttribute::Type::SUNPartLinkName;
            break;
        }
        case DW_AT_SUN_link_name: {
            type = DwarfAttribute::Type::SUNLinkName;
            break;
        }
        case DW_AT_SUN_pass_with_const: {
            type = DwarfAttribute::Type::SUNPassWithConst;
            break;
        }
        case DW_AT_SUN_return_with_const: {
            type = DwarfAttribute::Type::SUNReturnWithConst;
            break;
        }
        case DW_AT_SUN_import_by_name: {
            type = DwarfAttribute::Type::SUNImportByName;
            break;
        }
        case DW_AT_SUN_f90_pointer: {
            type = DwarfAttribute::Type::SUNf90Pointer;
            break;
        }
        case DW_AT_SUN_pass_by_ref: {
            type = DwarfAttribute::Type::SUNPassByRef;
            break;
        }
        case DW_AT_SUN_f90_allocatable: {
            type = DwarfAttribute::Type::SUNf90Allocatable;
            break;
        }
        case DW_AT_SUN_f90_assumed_shape_array: {
            type = DwarfAttribute::Type::SUNf90AssumedShapeArray;
            break;
        }
        case DW_AT_SUN_c_vla: {
            type = DwarfAttribute::Type::SUNCVla;
            break;
        }
        case DW_AT_SUN_return_value_ptr: {
            type = DwarfAttribute::Type::SUNReturnValuePtr;
            break;
        }
        case DW_AT_SUN_dtor_start: {
            type = DwarfAttribute::Type::SUNDtorStart;
            break;
        }
        case DW_AT_SUN_dtor_length: {
            type = DwarfAttribute::Type::SUNDtorLength;
            break;
        }
        case DW_AT_SUN_dtor_state_initial: {
            type = DwarfAttribute::Type::SUNDtorStateInitial;
            break;
        }
        case DW_AT_SUN_dtor_state_final: {
            type = DwarfAttribute::Type::SUNDtorStateFinal;
            break;
        }
        case DW_AT_SUN_dtor_state_deltas: {
            type = DwarfAttribute::Type::SUNDtorStateDeltas;
            break;
        }
        case DW_AT_SUN_import_by_lname: {
            type = DwarfAttribute::Type::SUNImportByLname;
            break;
        }
        case DW_AT_SUN_f90_use_only: {
            type = DwarfAttribute::Type::SUNf90UseOnly;
            break;
        }
        case DW_AT_SUN_namelist_spec: {
            type = DwarfAttribute::Type::SUNNamelistSpec;
            break;
        }
        case DW_AT_SUN_is_omp_child_func: {
            type = DwarfAttribute::Type::SUNIsOmpChildFunc;
            break;
        }
        case DW_AT_SUN_fortran_main_alias: {
            type = DwarfAttribute::Type::SUNFortranMainAlias;
            break;
        }
        case DW_AT_SUN_fortran_based: {
            type = DwarfAttribute::Type::SUNFortranBased;
            break;
        }
        case DW_AT_use_GNAT_descriptive_type: {
            type = DwarfAttribute::Type::GNATUseDescriptiveType;
            break;
        }
        case DW_AT_GNAT_descriptive_type: {
            type = DwarfAttribute::Type::GNATDescriptiveType;
            break;
        }
        // UPC extension
        case DW_AT_upc_threads_scaled: {
            type = DwarfAttribute::Type::UPCThreadsScaled;
            break;
        }
        // PGI (STMicroelectronics) extensions
        case DW_AT_PGI_lbase: {
            type = DwarfAttribute::Type::PGILocalBase;
            break;
        }
        case DW_AT_PGI_soffset: {
            type = DwarfAttribute::Type::PGISectionOffset;
            break;
        }
        case DW_AT_PGI_lstride: {
            type = DwarfAttribute::Type::PGILinearStride;
            break;
        }
        // Apple extensions
        case DW_AT_APPLE_optimized: {
            type = DwarfAttribute::Type::APPLEOptimized;
            break;
        }
        case DW_AT_APPLE_flags: {
            type = DwarfAttribute::Type::APPLEFlags;
            break;
        }
        case DW_AT_APPLE_isa: {
            type = DwarfAttribute::Type::APPLEISA;
            break;
        }
        //case DW_AT_APPLE_closure:
        case DW_AT_APPLE_block: {
            type = DwarfAttribute::Type::APPLEBlock;
            break;
        }
        //case DW_AT_APPLE_major_runtime_vers:
        case DW_AT_APPLE_major_runtime_vers: {
            type = DwarfAttribute::Type::APPLEMajorRuntimeVers;
            break;
        }
        //case DW_AT_APPLE_runtime_class:
        case DW_AT_APPLE_runtime_class: {
            type = DwarfAttribute::Type::APPLERuntimeClass;
            break;
        }
        case DW_AT_APPLE_omit_frame_ptr: {
            type = DwarfAttribute::Type::APPLEOmitFramePtr;
            break;
        }
        case DW_AT_hi_user: {
            type = DwarfAttribute::Type::HiUser;
            break;
        }
        default: {
            assert(!"Unknown attribute.");
            break;
        }
        }

        return type;
    }

private:
    std::list<Die*> _units;
};

DwarfSymbolTable::DwarfSymbolTable()
    : SymbolTable()
    , _p(new DwarfSymbolTablePrivate())
{
}

DwarfSymbolTable::~DwarfSymbolTable()
{

}

DwarfSymbolTable::DwarfSymbolTable(const DwarfSymbolTable& symbolTable)
{
    _p.reset(new DwarfSymbolTablePrivate(*symbolTable._p));
}

DwarfSymbolTable::DwarfSymbolTable(DwarfSymbolTable &&symbolTable)
    : DwarfSymbolTable()
{
    std::swap(*this, symbolTable);
}

DwarfSymbolTable& DwarfSymbolTable::operator= (DwarfSymbolTable symbolTable)
{
    std::swap(*this, symbolTable);
    return *this;
}

void DwarfSymbolTable::swap(DwarfSymbolTable& symbolTable)
{
    std::swap(_p, symbolTable._p);
}

//
// Interfaces SymbolTable
//

int DwarfSymbolTable::readSymbolTable(const std::string &fileName)
{
    assert(_p);
    return _p->readSymbolTable(fileName);
}

const std::list<Die*>& DwarfSymbolTable::compilationUnits() const
{
    assert(_p);
    return _p->compilationUnits();
}

} // namespace symbols
} // namespace dwarf
} // namespace dbg0
