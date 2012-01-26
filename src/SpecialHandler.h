/*************************************************************************
** SpecialHandler.h                                                     **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#ifndef SPECIALHANDLER_H
#define SPECIALHANDLER_H

#include <istream>
#include <list>
#include "MessageException.h"


struct SpecialActions;
class  SpecialManager;


struct SpecialException : public MessageException
{
	SpecialException (const std::string &msg) : MessageException(msg) {}
};


class SpecialHandler
{
	friend class SpecialManager;

	public:
		virtual ~SpecialHandler () {}
		virtual const char** prefixes () const=0;
		virtual const char* info () const=0;
		virtual const char* name () const=0;
		virtual bool process (const char *prefix, std::istream &is, SpecialActions *actions)=0;
		virtual void dviMovedTo (double x, double y) {}
		virtual void dviEndPage () {}

	protected:
		virtual bool isEndPageListener () const  {return false;}
		virtual bool isPositionListener () const {return false;}
};


#endif
