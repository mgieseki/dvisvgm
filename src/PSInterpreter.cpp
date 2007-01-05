/***********************************************************************
** PSInterpreter.cpp                                                  **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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

#include "PSInterpreter.h"

using namespace std;


PSInterpreter::PSInterpreter () {
}


void PSInterpreter::read (istream &is) {
	struct ObjectPattern {
		char *pattern;
		void (PSInterpreter::*handler)(void);
	};
	ObjectPattern patterns[] = {
		{"-?\\d+", PSInterpreter::doInteger},
		{"\\[.*?\\]", PSInterpreter::doArray},
		
		
	while (!is.eof) {
		char c = is.peekg();
		if (c == '-' || (c >= '0' && c <= 9))
			opStack.push(readNumber());
		else if (isalpha(c))
			doOperator(readOperator());
		else if (c == '/')
			readName();
		else if (c == '[')
			readArray();
		else if (c == '(')
			opStack.push(readString());
		else
			throw PSException("unexpected token", line, col);
	}		
}
