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

#include "elfexecutable.h"
#include "dwarfsymboltable.h"

#include <assert.h>

namespace dbg0
{
namespace elf
{

using namespace dwarf;

class ElfExecutable::ElfExecutablePrivate
{
public:
    ElfExecutablePrivate(ElfExecutable::SymbolTableReader reader)
        : _readerType(reader)
    {
        switch(_readerType) {
        case DWARF: {
            _symbolTable.reset(new DwarfSymbolTable());
            break;
        }
        default: {
            assert(!"Unknown symbol table reader type.");
            break;
        }
        }
    }

    ~ElfExecutablePrivate()
    {
    }

    ElfExecutablePrivate(const ElfExecutablePrivate& priv)
    {
        _fileName = priv.fileName();
        _readerType = priv.readerType();
        _symbolTable.reset(new DwarfSymbolTable(*static_cast<DwarfSymbolTable*>(
            priv.symbolTable())));
    }

    ElfExecutable::SymbolTableReader readerType() const
    {
        return _readerType;
    }

    int readSymbolTable(const std::string &fileName)
    {
        assert(_symbolTable);
        int err;
        if ((err =_symbolTable->readSymbolTable(fileName)) < 0)
            return err;

        _fileName = fileName;
    }

    std::string fileName() const
    {
        return _fileName;
    }

    SymbolTable* symbolTable() const
    {
        return _symbolTable.get();
    }

private:

private:
    std::string _fileName;
    ElfExecutable::SymbolTableReader _readerType;
    std::unique_ptr<SymbolTable> _symbolTable;
};


ElfExecutable::ElfExecutable(SymbolTableReader reader)
    : interfaces::Executable()
    , _p(new ElfExecutablePrivate(reader))
{
}

ElfExecutable::~ElfExecutable()
{
}

ElfExecutable::ElfExecutable(const ElfExecutable &exec)
{
    _p.reset(new ElfExecutablePrivate(*exec._p));
}

ElfExecutable::ElfExecutable(ElfExecutable &&exec)
    : ElfExecutable()
{
    std::swap(*this, exec);
}

ElfExecutable& ElfExecutable::operator=(ElfExecutable exec)
{
    std::swap(*this, exec);
    return *this;
}

void ElfExecutable::swap(ElfExecutable& exec)
{
    std::swap(_p, exec._p);
}

//
// Properties
//

ElfExecutable::SymbolTableReader ElfExecutable::readerType() const
{
    assert(_p);
    return _p->readerType();
}

//
// Interface Executable
//

int ElfExecutable::readSymbolTable(const std::string &fileName)
{
    assert(_p);
    return _p->readSymbolTable(fileName);
}

std::string ElfExecutable::fileName() const
{
    assert(_p);
    return _p->fileName();
}

SymbolTable* ElfExecutable::symbolTable() const
{
    assert(_p);
    return _p->symbolTable();
}

} // namespace elf
} // namespace dbg0
