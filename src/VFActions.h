/***********************************************************************
** VFActions.h                                                        **
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

#ifndef VFACTIONS_H
#define VFACTIONS_H

#include <string>
#include <vector>
#include "types.h"

using std::string;
using std::vector;


struct VFActions
{
	virtual ~VFActions () {}
	virtual void preamble (string comment, UInt32 checksum, double dsize) {}
	virtual void postamble () {}
	virtual void defineVFFont (UInt32 fontnum, string path, string name, UInt32 checksum, double dsize, double ssize) {}
	virtual void defineVFChar (UInt32 c, vector<UInt8> *dvi) {}
};

#endif
