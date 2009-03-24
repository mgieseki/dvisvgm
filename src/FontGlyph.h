/***********************************************************************
** FontGlyph.h                                                        **
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

#ifndef FONTGLYPH_H
#define FONTGLYPH_H

#include <ostream>
#include <list>
#include <vector>
#include "Pair.h"

using std::ostream;
using std::list;
using std::vector;

class GlyphCommand
{
	public:
		GlyphCommand () {}
		GlyphCommand (const LPair &p);
		GlyphCommand (const LPair &p1, const LPair &p2);
		GlyphCommand (const LPair &p1, const LPair &p2, const LPair &p3);
		virtual ~GlyphCommand () {}
		virtual char getSVGPathCommand () const =0;
		virtual GlyphCommand* combine (GlyphCommand &cmd) {return 0;}
		virtual const vector<LPair>& params () const      {return _params;}
		virtual LPair param (int n) const;
		virtual void writeSVGCommand (ostream &os, double sx=1, double sy=1) const;

	protected:
		typedef vector<LPair>::iterator Iterator;
		typedef vector<LPair>::const_iterator ConstIterator;
		vector<LPair> _params;
};


struct GlyphMoveTo : public GlyphCommand
{
	GlyphMoveTo (const LPair &p) : GlyphCommand(p) {}
	char getSVGPathCommand () const {return 'M';}
	GlyphCommand* combine (GlyphCommand &cmd);
};


struct GlyphLineTo : public GlyphCommand
{	
	GlyphLineTo (const LPair &p) : GlyphCommand(p) {}
	char getSVGPathCommand () const {return 'L';}
	GlyphCommand* combine (GlyphCommand &cmd);
};


struct GlyphHorizontalLineTo : public GlyphLineTo
{
	GlyphHorizontalLineTo (const LPair &p) : GlyphLineTo(LPair(p.x(), 0)) {}
	char getSVGPathCommand () const {return 'H';}
	void writeSVGCommand (ostream &os, double sx=1, double sy=1) const;
};


struct GlyphVerticalLineTo : public GlyphLineTo
{
	GlyphVerticalLineTo (const LPair &p) : GlyphLineTo(LPair(0, p.y())) {}
	char getSVGPathCommand () const {return 'V';}
	void writeSVGCommand (ostream &os, double sx=1, double sy=1) const;
};


struct GlyphConicTo : public GlyphCommand
{
	GlyphConicTo (const LPair &p1, const LPair &p2) : GlyphCommand(p1, p2) {}
	char getSVGPathCommand () const {return 'Q';}
};


struct GlyphShortConicTo : public GlyphCommand
{
	GlyphShortConicTo (const LPair &p) : GlyphCommand(p) {}
	char getSVGPathCommand () const {return 'T';}
};


struct GlyphCubicTo : public GlyphCommand
{
	GlyphCubicTo (const LPair &p1, const LPair &p2, const LPair &p3) : GlyphCommand(p1, p2, p3) {}
	char getSVGPathCommand () const {return 'C';}
	GlyphCommand* combine (GlyphCommand &cmd);
};


struct GlyphShortCubicTo : public GlyphCommand
{
	GlyphShortCubicTo (const LPair &p1, const LPair &p2) : GlyphCommand(p1, p2) {}
	char getSVGPathCommand () const {return 'S';}
};


struct GlyphClosePath : public GlyphCommand
{
	char getSVGPathCommand () const {return 'Z';}
};


class FontEncoding;
class FontEngine;

class Glyph
{
	typedef list<GlyphCommand*> CommandList;
	typedef CommandList::iterator Iterator;
	typedef CommandList::const_iterator ConstIterator;
	public:
		~Glyph ();
		void addCommand(GlyphCommand *cmd);
		void clear ();
		void closeOpenPaths ();
		void optimizeCommands ();
		void read (unsigned char c, const FontEncoding *encoding, const FontEngine &fontEngine);
		void writeSVGCommands (ostream &os, double sx, double sy) const;
		void forAllCommands (void (*f)(GlyphCommand*, void*), void *userParam=0);
		bool empty () const {return _commands.empty();}
		const CommandList& commands () const {return _commands;}

	private:
		CommandList _commands;
};

#endif
