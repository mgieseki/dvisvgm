/***********************************************************************
** FontGlyph.h                                                        **
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
		virtual const vector<LPair>& getParams () const    {return params;}
		virtual LPair getParam (int n) const;
		virtual void writeSVGCommand (ostream &os) const;
	protected:
		typedef vector<LPair>::iterator Iterator;
		typedef vector<LPair>::const_iterator ConstIterator;
		vector<LPair> params;
};


class GlyphMoveTo : public GlyphCommand
{
	public:
		GlyphMoveTo (const LPair &p) : GlyphCommand(p) {}
		char getSVGPathCommand () const {return 'M';}
		GlyphCommand* combine (GlyphCommand &cmd);
};


class GlyphLineTo : public GlyphCommand
{	
	public:
		GlyphLineTo (const LPair &p) : GlyphCommand(p) {}
		char getSVGPathCommand () const {return 'L';}
		GlyphCommand* combine (GlyphCommand &cmd);
};


class GlyphConicTo : public GlyphCommand
{
	public:
		GlyphConicTo (const LPair &p1, const LPair &p2) : GlyphCommand(p1, p2) {}
		char getSVGPathCommand () const {return 'Q';}
		GlyphCommand* combine (GlyphCommand &cmd);
};


class GlyphShortConicTo : public GlyphCommand
{
	public:
		GlyphShortConicTo (const LPair &p) : GlyphCommand(p) {}
		char getSVGPathCommand () const {return 'T';}
		GlyphCommand* combine (GlyphCommand &cmd);
};


class GlyphCubicTo : public GlyphCommand
{
	public:
		GlyphCubicTo (const LPair &p1, const LPair &p2, const LPair &p3) : GlyphCommand(p1, p2, p3) {}
		char getSVGPathCommand () const {return 'C';}
		GlyphCommand* combine (GlyphCommand &cmd);
};


class GlyphShortCubicTo : public GlyphCommand
{
	public:
		GlyphShortCubicTo (const LPair &p1, const LPair &p2) : GlyphCommand(p1, p2) {}
		char getSVGPathCommand () const {return 'S';}
		GlyphCommand* combine (GlyphCommand &cmd);
};


class GlyphClosePath : public GlyphCommand
{
	public:
		char getSVGPathCommand () const {return 'Z';}
};


class FontEngine;

class Glyph
{
	typedef list<GlyphCommand*>::iterator Iterator;
	typedef list<GlyphCommand*>::const_iterator ConstIterator;
	public:
		~Glyph ();
		void addCommand(GlyphCommand *cmd);
		void clear ();
		void closeOpenPaths ();
		void optimizeCommands ();
		void read (char c, const FontEngine &fontEngine);
		void writeSVGCommands (ostream &os) const;
		void forAllCommands (void (*f)(GlyphCommand*, void*), void *userParam=0);
	private:
		list<GlyphCommand*> commands;
};

#endif
