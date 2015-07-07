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
#include "dwarfsymboltable.h"

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
        Dwarf_Unsigned nextCuHeader;

        while(err == DW_DLV_OK) {
            // Get next compilation unit
            if ((err = dwarf_next_cu_header(dbg, &headerLength, &version,
                &abbrevOffset, &addressSize, &nextCuHeader, &error)) != DW_DLV_OK)
            {
                // Error or DW_DLV_NO_ENTRY (last entry)
                break;
            }

            if ((err = addCompilationUnit(dbg, headerLength, version,
                abbrevOffset, addressSize, nextCuHeader)) != DW_DLV_OK)
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

    void addAttributes(DwarfDie *ddie, std::initializer_list<int> attributes,
        Dwarf_Die die)
    {
        assert(ddie);
        std::for_each(attributes.begin(), attributes.end(), [&](Dwarf_Half attribute) {
            DwarfAttribute *attr = getAttribute(die, attribute);
            if (attr)
                ddie->add(attr);
        });
    }

    int addCompilationUnit(Dwarf_Debug dbg, Dwarf_Unsigned headerLength,
        Dwarf_Half version, Dwarf_Unsigned abbrevOffset, Dwarf_Half addressSize,
        Dwarf_Unsigned /*nextCuHeader*/)
    {
        int err;
        Dwarf_Error error;
        Dwarf_Die noDie = 0, cuDie = 0;

        // Compilation will have a single die sibling
        if ((err = dwarf_siblingof(dbg, noDie, &cuDie, &error)) != DW_DLV_OK)
            return err;

        std::unique_ptr<DwarfCompilationUnit> cu(
            DwarfDieFactory::instance().createCompileUnit());
        cu->setHeaderLength(headerLength);
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
        addAttributes(cu, attributes, die);
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

    DwarfAttribute* getAttribute(Dwarf_Die die, Dwarf_Half attribute)
    {
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
