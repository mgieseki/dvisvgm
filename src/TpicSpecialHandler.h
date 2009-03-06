/***********************************************************************
** TpicSpecialHandler.h                                               **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#ifndef TPICSPECIALHANDLER_H
#define TPICSPECIALHANDLER_H

#include "Pair.h"
#include "SpecialHandler.h"

class TpicSpecialHandler : public SpecialHandler
{
	public:
		TpicSpecialHandler ();
		const char* prefix () const {return 0;}
		const char* info () const   {return "TPIC specials";}
		const char* name () const   {return "tpic";}
		bool process (const char *prefix, std::istream &is, SpecialActions *actions);
		void endPage ();

	protected:
		void reset ();
		void drawLines (bool fill, double ddist, SpecialActions *actions);
		void drawArc (double cx, double cy, double rx, double ry, double angle1, double angle2, SpecialActions *actions);

   private:
		double _penwidth; ///< pen width in TeX point units
		double _fill;     ///< fill intensity [0,1]; if < 0, we don't fill anything
		std::list<DPair> _points;
};

#endif
