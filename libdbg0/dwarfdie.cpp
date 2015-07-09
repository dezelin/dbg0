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

#include "dwarfdie.h"

#include <assert.h>
#include <list>

namespace dbg0
{
namespace dwarf
{
namespace dies
{

class DwarfDie::DwarfDiePrivate
{
public:
    DwarfDiePrivate(DwarfDie::Type type)
        : _type(type)
        , _offset(0)
        , _offsetCU(0)
    {

    }

    DwarfDiePrivate(const DwarfDiePrivate &priv)
    {
        _type = priv._type;
    }

    size_t offset() const
    {
        return _offset;
    }

    size_t offsetCU() const
    {
        return _offsetCU;
    }

    void setOffset(size_t offset)
    {
        _offset = offset;
    }

    void setOffsetCU(size_t offset)
    {
        _offsetCU = offset;
    }


    //
    // Interface
    //

    void add(Attribute* attribute)
    {
        assert(attribute);
        _attributes.push_back(attribute);
    }

    void add(Die* die)
    {
        assert(die);
        _children.push_back(die);
    }

    const std::list<Attribute*> &attributes() const
    {
        return _attributes;
    }

    const std::list<Die*> &children() const
    {
        return _children;
    }

    DwarfDie::Type type() const
    {
        return _type;
    }

private:
    DwarfDie::Type _type;
    std::list<Attribute*> _attributes;
    std::list<Die*> _children;
    size_t _offset;
    size_t _offsetCU;
};

DwarfDie::DwarfDie(DwarfDie::Type type)
    : Die()
    , _p(new DwarfDiePrivate(type))
{
}

DwarfDie::~DwarfDie()
{

}

DwarfDie::DwarfDie(const DwarfDie &die)
{
    assert(_p);
    _p.reset(new DwarfDiePrivate(*die._p));
}

DwarfDie::DwarfDie(DwarfDie &&die)
    : DwarfDie()
{
    std::swap(*this, die);
}

DwarfDie& DwarfDie::operator= (DwarfDie die)
{
    std::swap(*this, die);
    return *this;
}

void DwarfDie::swap(DwarfDie &die)
{
    std::swap(_p, die._p);
}

//
// Properties
//

size_t DwarfDie::offset() const
{
    assert(_p);
    return _p->offset();
}

size_t DwarfDie::offsetCU() const
{
    assert(_p);
    return _p->offsetCU();
}

void DwarfDie::setOffset(size_t offset)
{
    assert(_p);
    _p->setOffset(offset);
}

void DwarfDie::setOffsetCU(size_t offset)
{
    assert(_p);
    _p->setOffsetCU(offset);
}

//
// Interface Die
//

void DwarfDie::add(Attribute* attribute)
{
    assert(_p);
    _p->add(attribute);
}

void DwarfDie::add(Die* die)
{
    assert(_p);
    _p->add(die);
}

const std::list<Attribute*>& DwarfDie::attributes() const
{
    assert(_p);
    return _p->attributes();
}

const std::list<Die*>& DwarfDie::children() const
{
    assert(_p);
    return _p->children();
}

int DwarfDie::type() const
{
    assert(_p);
    return static_cast<int>(_p->type());
}

} // namespace dies
} // namespace dwarf
} // namespace dbg0
