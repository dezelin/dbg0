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
#include "dwarfsymboltable.h"

#include <assert.h>
#include <dwarf.h>
#include <libdwarf.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <list>


namespace dbg0
{
namespace dwarf
{

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

    const std::list<CompilationUnit*>& compilationUnits() const
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

    int addCompilationUnit(Dwarf_Debug dbg, Dwarf_Unsigned headerLength,
        Dwarf_Half version, Dwarf_Unsigned abbrevOffset, Dwarf_Half addressSize,
        Dwarf_Unsigned nextCuHeader)
    {
        int err;
        Dwarf_Error error;
        Dwarf_Die noDie = 0, cuDie = 0;

        // Compilation will have a single die sibling
        if ((err = dwarf_siblingof(dbg, noDie, &cuDie, &error)) != DW_DLV_OK)
            return err;

        std::unique_ptr<DwarfCompilationUnit> cu(new DwarfCompilationUnit());
        cu->setHeaderLength(headerLength);
        cu->setVersion(version);
        cu->setAbbrevOffset(abbrevOffset);
        cu->setAddressSize(addressSize);

        if ((err = addDie(dbg, cu.get(), cuDie)) == DW_DLV_OK)
            _units.push_back(cu.release());

        dwarf_dealloc(dbg, cuDie, DW_DLA_DIE);
        return err;
    }

    int addDie(Dwarf_Debug dbg, DwarfCompilationUnit *cu, Dwarf_Die die)
    {
        return 0;
    }

private:
    std::list<CompilationUnit*> _units;
};

DwarfSymbolTable::DwarfSymbolTable()
    : interfaces::SymbolTable()
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

const std::list<CompilationUnit*>& DwarfSymbolTable::compilationUnits() const
{
    assert(_p);
    return _p->compilationUnits();
}

} // namespace dwarf
} // namespace dbg0
