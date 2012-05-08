/*********************************************************************/
// dar - disk archive - a backup/restoration program
// Copyright (C) 2002-2052 Denis Corbin
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// to contact the author : dar.linux@free.fr
/*********************************************************************/
// $Id: infinint.hpp,v 1.10 2003/02/11 22:01:53 edrusb Rel $
//
/*********************************************************************/

#ifndef INFININT_HPP
#define INFININT_HPP

#include <sys/types.h>
#include <typeinfo>

#include "storage.hpp"
#include "integers.hpp"

typedef unsigned char bitfield[8];

extern void swap_bytes(unsigned char &a, unsigned char &b) throw();
extern void swap_bytes(unsigned char *a, U_I size) throw();
extern void expand_byte(unsigned char a, bitfield &bit) throw();
extern void contract_byte(const bitfield &b, unsigned char & a) throw(Erange);

class generic_file;

class infinint
{
public :

    infinint(off_t a = 0) throw(Ememory, Erange, Ebug)
	{ E_BEGIN; infinint_from(a); E_END("infinint::infinint", "off_t"); };
#if time_t != off_t
    infinint(time_t a = 0) throw(Ememory, Erange, Ebug)
	{ E_BEGIN; infinint_from(a); E_END("infinint::infinint", "off_t"); };
#endif
    infinint(const infinint & ref) throw(Ememory, Ebug)
	{ E_BEGIN; copy_from(ref); E_END("infinint::infinint", "const infinint &"); };
    ~infinint() throw(Ebug)
	{ E_BEGIN; detruit(); E_END("infinint::~infinint",""); };


    infinint & operator = (const infinint & ref) throw(Ememory, Ebug)
	{ E_BEGIN; detruit(); copy_from(ref); return *this; E_END("infinint::operator =",""); };

    void read_from_file(int fd) throw(Erange, Ememory, Ebug); // read an infinint from a file
    void dump(int fd) const throw(Einfinint, Ememory, Erange, Ebug); // write byte sequence to file
    void read_from_file(generic_file &x) throw(Erange, Ememory, Ebug); // read an infinint from a file
    void dump(generic_file &x) const throw(Einfinint, Ememory, Erange, Ebug); // write byte sequence to file

    infinint & operator += (const infinint & ref) throw(Ememory, Erange, Ebug);
    infinint & operator -= (const infinint & ref) throw(Ememory, Erange, Ebug);
    infinint & operator *= (unsigned char arg) throw(Ememory, Erange, Ebug);
    infinint & operator *= (const infinint & ref) throw(Ememory, Erange, Ebug);
    inline infinint & operator /= (const infinint & ref) throw(Einfinint, Erange, Ememory, Ebug);
    inline infinint & operator %= (const infinint & ref) throw(Einfinint, Erange, Ememory, Ebug);
    infinint & operator >>= (U_32 bit) throw(Ememory, Erange, Ebug);
    infinint & operator >>= (infinint bit) throw(Ememory, Erange, Ebug);
    infinint & operator <<= (U_32 bit) throw(Ememory, Erange, Ebug);
    infinint & operator <<= (infinint bit) throw(Ememory, Erange, Ebug);
    infinint operator ++(int a) throw(Ememory, Erange, Ebug)
	{ E_BEGIN; infinint ret = *this; ++(*this); return ret; E_END("infinint::operator ++", "int"); };
    infinint operator --(int a) throw(Ememory, Erange, Ebug)
	{ E_BEGIN; infinint ret = *this; --(*this); return ret; E_END("infinint::operator --", "int"); };
    infinint & operator ++() throw(Ememory, Erange, Ebug)
	{ E_BEGIN; return *this += 1; E_END("infinint::operator ++", "()"); };
    infinint & operator --() throw(Ememory, Erange, Ebug)
	{ E_BEGIN; return *this -= 1; E_END("infinint::operator --", "()"); };

    U_32 operator % (U_32 arg) const throw(Einfinint, Ememory, Erange, Ebug)
	{ E_BEGIN; return modulo(arg); E_END("infinint::operator %",""); };

	// increment the argument up to a legal value for its storage type and decrement the object in consequence
	// note that the initial value of the argument is not ignored !
	// when the object is null the value of the argument stays the same as before
    template <class T>void unstack(T &v) throw(Ememory, Erange, Ebug)
	{ E_BEGIN; infinint_unstack_to(v); E_END("infinint::unstack", typeid(v).name()); };

    friend bool operator < (const infinint &, const infinint &) throw(Erange, Ebug);
    friend bool operator == (const infinint &, const infinint &) throw(Erange, Ebug);
    friend bool operator > (const infinint &, const infinint &) throw(Erange, Ebug);
    friend bool operator <= (const infinint &, const infinint &) throw(Erange, Ebug);
    friend bool operator != (const infinint &, const infinint &) throw(Erange, Ebug);
    friend bool operator >= (const infinint &, const infinint &) throw(Erange, Ebug);
    friend void euclide(infinint a, const infinint &b, infinint &q, infinint &r) throw(Einfinint, Erange, Ememory, Ebug);

private :
    static const int TG = 4;

    enum endian { big_endian, little_endian, not_initialized };
    typedef unsigned char group[TG];

    storage *field;

    bool is_valid() const throw();
    void reduce() throw(Erange, Ememory, Ebug); // put the object in canonical form : no leading byte equal to zero
    void copy_from(const infinint & ref) throw(Ememory, Ebug);
    void detruit() throw(Ebug);
    void make_at_least_as_wider_as(const infinint & ref) throw(Erange, Ememory, Ebug);
    template <class T> void infinint_from(T a) throw(Ememory, Erange, Ebug);
    template <class T> void infinint_unstack_to(T &a) throw(Ememory, Erange, Ebug);
    template <class T> T modulo(T arg) const throw(Einfinint, Ememory, Erange, Ebug);
    signed int difference(const infinint & b) const throw(Erange, Ebug); // gives the sign of (*this - arg) but only the sign !

	/////////////////////////
	// static statments
	//
    static endian used_endian;
    static void setup_endian();
};

#define OPERATOR(OP) inline bool operator OP (const infinint &a, const infinint &b) throw(Erange, Ebug) \
{ \
    E_BEGIN; \
    return a.difference(b) OP 0; \
    E_END("operator OP", "infinint, infinint"); \
}

OPERATOR(<);
OPERATOR(>);
OPERATOR(<=);
OPERATOR(>=);
OPERATOR(==);
OPERATOR(!=);

infinint operator + (const infinint &, const infinint &) throw(Erange, Ememory, Ebug);
infinint operator - (const infinint &, const infinint &) throw(Erange, Ememory, Ebug);
infinint operator * (const infinint &, const infinint &) throw(Erange, Ememory, Ebug);
infinint operator * (const infinint &, const unsigned char) throw(Erange, Ememory, Ebug);
infinint operator * (const unsigned char, const infinint &) throw(Erange, Ememory, Ebug);
infinint operator / (const infinint &, const infinint &) throw(Einfinint, Erange, Ememory, Ebug);
infinint operator % (const infinint &, const infinint &) throw(Einfinint, Erange, Ememory, Ebug);
infinint operator >> (const infinint & a, U_32 bit) throw(Erange, Ememory, Ebug);
infinint operator >> (const infinint & a, const infinint & bit) throw(Erange, Ememory, Ebug);
infinint operator << (const infinint & a, U_32 bit) throw(Erange, Ememory, Ebug);
infinint operator << (const infinint & a, const infinint & bit) throw(Erange, Ememory, Ebug);
void euclide(infinint a, const infinint &b, infinint &q, infinint &r) throw(Einfinint, Erange, Ememory, Ebug);
template <class T> inline void euclide(T a, T b, T & q, T &r)
{
    E_BEGIN;
    q = a/b; r = a%b;
    E_END("euclide", "");
};

inline infinint & infinint::operator /= (const infinint & ref) throw(Einfinint, Erange, Ememory, Ebug)
{
    E_BEGIN;
    *this = *this / ref;
    return *this;
    E_END("infinint::operator /=", "");
}

inline infinint & infinint::operator %= (const infinint & ref) throw(Einfinint, Erange, Ememory, Ebug)
{
    E_BEGIN;
    *this = *this % ref;
    return *this;
    E_END("infinint::operator %=", "");
}

template <class T> T infinint::modulo(T arg) const throw(Einfinint, Ememory, Erange, Ebug)
{
    E_BEGIN;
    infinint tmp = *this % infinint(arg);
    T ret = 0;
    unsigned char *debut = (unsigned char *)(&ret);
    unsigned char *ptr = debut + sizeof(T) - 1;
    storage::iterator it = tmp.field->rbegin();

    while(it != tmp.field->rend() && ptr >= debut)
	*(ptr--) = *(it--);

    if(it != tmp.field->rend())
	throw SRC_BUG; // could not put all the data in the returned value !

    if(used_endian == big_endian)
	swap_bytes(debut, sizeof(T));

    return ret;
    E_END("infinint::modulo", "");
}

template <class T> void infinint::infinint_from(T a) throw(Ememory, Erange, Ebug)
{
    E_BEGIN;
    U_I size = sizeof(a);
    S_I direction = +1;
    unsigned char *ptr, *fin;

    if(used_endian == not_initialized)
	setup_endian();

    if(used_endian == big_endian)
    {
	direction = -1;
	ptr = (unsigned char *)(&a) + (size - 1);
	fin = (unsigned char *)(&a) - 1;
    }
    else
    {
	direction = +1;
	ptr = (unsigned char *)(&a);
	fin = (unsigned char *)(&a) + size;
    }

    while(ptr != fin && *ptr == 0)
    {
	ptr += direction;
	size--;
    }

    if(size == 0)
    {
	size = 1;
	ptr -= direction;
    }

    field = new storage(size);
    if(field != NULL)
    {
	storage::iterator it = field->begin();

	while(ptr != fin)
	{
	    *(it++) = *ptr;
	    ptr += direction;
	}
	if(it != field->end())
	    throw SRC_BUG; // size mismatch in this algorithm
    }
    else
	throw Ememory("template infinint::infinint_from");

    E_END("infinint::infinint_from", "");
}

    // argument must be a regular interger (a bit field).
template <class T> T rotate_right_one_bit(T v)
{
    bool retenue = (v & 1) != 0;

    v >>= 1;
    if(retenue)
	v |= 1 << sizeof(v);

    return v;
}


template <class T> void infinint::infinint_unstack_to(T &a) throw(Ememory, Erange, Ebug)
{
    E_BEGIN;
	// T is supposed to be an unsigned "integer"
	// (ie.: sizeof returns the width of the storage bit field  and no sign bit is present)
	// Note : static here avoids the recalculation of max_T at each call
//    static const T max_T = ~ T(0); // negation bit by bit of zero gives max possible value of T (all bits set)
    static const T max_T = ~T(0) > 0 ? ~ T(0) : ~rotate_right_one_bit(T(1));
    infinint step = max_T - a;

    if(*this < step)
    {
	T transfert = 0;
	unsigned char *debut = (unsigned char *)&transfert;
	unsigned char *ptr = debut + sizeof(transfert) - 1;
	storage::iterator it = field->rbegin();

	while(ptr >= debut && it != field->rend())
	    *(ptr--) = *(it--);

	if(used_endian == big_endian)
	    swap_bytes(debut, sizeof(transfert));
	a += transfert;
	*this = 0;
    }
    else
    {
	*this -= step;
	a = max_T;
    }
    E_END("infinint::infinint_unstack_to", "");
}


#endif
