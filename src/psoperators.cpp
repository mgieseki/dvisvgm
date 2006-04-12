/***********************************************************************
** psoperators.cpp                                                    **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2006 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/
// $Id$

#include <list>
#include <string>
#include "PSState.h"

using namespace std;

#include "macros.h"

#define push state.opStack.push
#define pop  state.opStack.pop
#define top  state.opStack.top

// math operators

void op_add (PSState &state) {
	PSNumber *op1 = pop<PSNumber>();
	*top<PSNumber>() += *op1;
	delete op1;
}

void op_sub (PSState &state) {
	PSNumber *op1 = pop<PSNumber>();	
	*top<PSNumber>() -= *op1;
	delete op1;
}

void op_mul (PSState &state) {
	PSNumber *op1 = pop<PSNumber>();	
	*top<PSNumber>() *= *op1;
	delete op1;
}

void op_div (PSState &state) {
	if (*top<PSNumber*> == 0)
		throw PSException("division by zero");
	PSNumber *op1 = pop<PSNumber>();
	*top<PSNumber>() /= *op1;
	delete op1;
}

void op_mod (PSState &state) {
	if (*top<PSInteger> == 0)
		throw PSException("division by zero");
	PSNumber *op1 = pop<PSInteger>();
	*top<PSInteger>() %= *op1;
	delete op1;
}

// stack operators

void op_dup (PSState &state) {
	push(top<PSObject>()->copy());
}

void op_clear (PSState &state) {
	state.opStack.deleteObjects();
}
