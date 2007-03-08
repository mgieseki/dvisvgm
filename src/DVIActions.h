/***********************************************************************
** DVIActions.h                                                       **
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

#ifndef DVIACTIONS_H
#define DVIACTIONS_H

#include <string>
#include "Message.h"
#include "types.h"

using std::string;

class Font;

struct DVIActions
{
	virtual ~DVIActions () {}
	virtual void setChar (double x, double y, unsigned c, const Font *f) {}
	virtual void setRule (double x, double y, double height, double width) {}
	virtual void moveToX (double x) {}
	virtual void moveToY (double y) {}
	virtual void defineFont (int num, const string &path, const string &name, double ds, double sc) {}
	virtual void setFont (int num) {}
	virtual void special (const string &s) {}
	virtual void preamble (const string &cmt) {}
	virtual void postamble () {}
	virtual void beginPage (Int32 *c) {}
	virtual void endPage () {}
};

#endif
