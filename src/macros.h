/***********************************************************************
** macros.h                                                           **
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
// $Id: macros.h,v 1.2 2006/01/05 16:05:06 mgieseki Exp $

#ifndef MACROS_H
#define MACROS_H

#define FORALL(container, itertype, itervar) \
	for (itertype itervar=(container).begin(); itervar != (container).end(); ++itervar)

#define IMPLEMENT_ARITHMETIC_OPERATOR(class, op)        \
	inline class operator op (class a, const class &b) { \
		return a op##= b;                                 \
	}

#define IMPLEMENT_ARITHMETIC_OPERATOR2(class, scalar, op) \
	inline class operator op (class a, scalar b) { \
		return a op##= b;                           \
	}

#define IMPLEMENT_OUTPUT_OPERATOR(class)                            \
	inline std::ostream& operator << (std::ostream &os, class obj) { \
		return obj.write(os);                                         \
	}

#endif
