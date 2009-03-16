/***********************************************************************
** FontGlyph.cpp                                                      **
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

#include "FontEncoding.h"
#include "FontEngine.h"
#include "FontGlyph.h"
#include "Message.h"
#include "Pair.h"
#include "macros.h"

using namespace std;

GlyphCommand::GlyphCommand (const LPair &p) : _params(1) {
	_params[0] = p;
}


GlyphCommand::GlyphCommand (const LPair &p1, const LPair &p2) : _params(2) {
	_params[0] = p1;
	_params[1] = p2;
}


GlyphCommand::GlyphCommand (const LPair &p1, const LPair &p2, const LPair &p3) : _params(3) {
	_params[0] = p1;
	_params[1] = p2;
	_params[2] = p3;
}


LPair GlyphCommand::param (int n) const {
	if (n < 0)
		n = _params.size() + n;
	if (n >= 0 && unsigned(n) < _params.size())
		return _params[n];
	return LPair();
}


void GlyphCommand::writeSVGCommand (ostream &os, double sx, double sy) const {
	os << getSVGPathCommand();
	FORALL(_params, ConstIterator, i) {
		if (i != _params.begin())
			os << ' ';
		os << (i->x()*sx) << ' ' << (i->y()*sy);
	}
}


GlyphCommand* GlyphMoveTo::combine (GlyphCommand &cmd) {
	if (cmd.getSVGPathCommand() == 'M' && _params.size() == 1) 
		return &cmd;
	if (cmd.getSVGPathCommand() == 'L') {
		_params.insert(_params.end(), cmd.params().begin(), cmd.params().end());
		return this;
	}
	return 0;
}


GlyphCommand* GlyphLineTo::combine (GlyphCommand &cmd) {
	if (cmd.getSVGPathCommand() == 'L') {
		_params.insert(_params.end(), cmd.params().begin(), cmd.params().end());
		return this;
	}
	return 0;
}


GlyphCommand* GlyphCubicTo::combine (GlyphCommand &cmd) {
/*	if (cmd.getSVGPathCommand() == 'C' && cmd.getParams().size() == 3) {
		LPair c1 = getParam(-2);
		LPair e1 = getParam(-1);
		LPair b2 = cmd.getParam(0);
		LPair c2 = cmd.getParam(1);
		if (e1 == b2 && c2 == e1*2-c1) 
			return new GlyphShortCubicTo(cmd.getParam(-2), cmd.getParam(-1));	
		else {
			_params.insert(_params.end(), cmd.getParams().begin(), cmd.getParams().end());
			return this;		
		}
	}*/
	return 0;
}


///////////////////////////


class Commands : public FEGlyphCommands {
	public:
		Commands (Glyph &g) : glyph(g) {}
		
		void moveTo(long x, long y) {
			glyph.addCommand(new GlyphMoveTo(LPair(x, y)));
		}
		
		void lineTo(long x, long y) {
			glyph.addCommand(new GlyphLineTo(LPair(x, y)));
		}
		
		void conicTo(long x1, long y1, long x2, long y2)  {
			glyph.addCommand(new GlyphConicTo(LPair(x1, y1), LPair(x2, y2)));
		}
		
		void cubicTo(long x1, long y1, long x2, long y2, long x3, long y3) {
			glyph.addCommand(new GlyphCubicTo(LPair(x1, y1), LPair(x2, y2), LPair(x3, y3)));
		}

	private:
		Glyph &glyph;
};


Glyph::~Glyph () {
	FORALL(_commands, Iterator, i)
		delete *i;
}


void Glyph::clear () {
	FORALL(_commands, Iterator, i)
		delete *i;
	_commands.clear();
}


void Glyph::addCommand (GlyphCommand *cmd) {
	if (cmd)
		_commands.push_back(cmd);
}


void Glyph::read (unsigned char c, const FontEncoding *encoding, const FontEngine &fontEngine) {
	Commands _commands(*this);
	if (encoding) {
		if (const char *name = encoding->getEntry(c))
			fontEngine.traceOutline(name, _commands, false);
		else
			Message::wstream(true) << "no encoding for char #" << int(c) << " in '" << encoding->name() << "'\n";
	}
	else
		fontEngine.traceOutline(c, _commands, false);
}


void Glyph::writeSVGCommands (ostream &os, double sx, double sy) const {
	FORALL (_commands, ConstIterator, i)
		(*i)->writeSVGCommand(os, sx, sy);
}

void Glyph::forAllCommands (void (*f)(GlyphCommand*, void*), void *userParam) {
	FORALL(_commands, Iterator, i)
		f(*i, userParam);
}


/** Detects all open paths of the glyph's outline and closes them by adding a closePath command. 
 *	 Most font formats only support closed outline paths so there are no explicit closePath statements
 *	 in the glyph's outline description. All open paths are automatically closed by the renderer. 
 *	 This method detects all open paths and adds the missing closePath statement. */
void Glyph::closeOpenPaths () {
	GlyphCommand *prevCommand = 0;
	FORALL(_commands, Iterator, i) {
		if ((*i)->getSVGPathCommand() == 'M' && prevCommand && prevCommand->getSVGPathCommand() != 'Z') {
			prevCommand = *i;
			_commands.insert(i, new GlyphClosePath);
			++i; // skip inserted closePath command in next iteration step
		}
		else
			prevCommand = *i;
	}
	if (!_commands.empty())
		_commands.push_back(new GlyphClosePath);
}


/** Optimizes the glyph's outline description by using command sequences with less parameters.
 *  TrueType and Type1 fonts only support 3 drawing _commands (moveto, lineto, conicto/cubicto).
 *  In the case of successive bezier curve sequences, control points or tangent slopes are ofted 
 *  identical so the path description contains redundant information. SVG provides short form bezier 
 *  _commands that reuse previously given parameters. 
 *  This method detects such command sequences and replaces them by short form curve _commands. */
void Glyph::optimizeCommands () {
#if 0
	Iterator i1=_commands.begin();
	Iterator i2=_commands.begin();
	for (++i2; i2 != _commands.end(); ++i2) {
		if ((*i1)->getSVGPathCommand() == 'C' && (*i2)->getSVGPathCommand() == 'C') { // cubic bezier curve sequence?
			const LPair &p0 = i1->
			long x1, y1;    // end point of previous cubic bezier curve
			long x2, y2;    // start point of previous cubic bezier curve	
			long cx1, cy1; // second control point of previous cubic bezier curve
			long cx2, cy2; // first control point of current cubic bezier curve
			if (x1 == x2 && y1 == y2 && cx2 == rcx1 && cy2 == rcy1) {
				delete *i1;
				i1 = i2;
				i2 = _commands.erase(i2);
				i2.insert(new GlyphShortCubicTo(...));
			}
		}
		else if ((*i1)->getSVGPathCommand() == 'Q' && (*i2)->getSVGPathCommand() == 'Q') { // conic/quadratic bezier curve sequence?
		}
	}
#endif
}
