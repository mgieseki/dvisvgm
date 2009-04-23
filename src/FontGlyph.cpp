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
	if (n >= 0 && size_t(n) < _params.size())
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
	if (cmd.getSVGPathCommand() == getSVGPathCommand()) {
		_params.insert(_params.end(), cmd.params().begin(), cmd.params().end());
		return this;
	}
	return 0;
}


void GlyphHorizontalLineTo::writeSVGCommand (ostream &os, double sx, double sy) const {
	os << getSVGPathCommand() << _params[0].x()*sx;
}


void GlyphVerticalLineTo::writeSVGCommand (ostream &os, double sx, double sy) const {
	os << getSVGPathCommand() << _params[0].y()*sy;
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
	clear();
}


/** Removes all path commands from the glyph and frees the allocated objects. */
void Glyph::clear () {
	FORALL(_commands, Iterator, i)
		delete *i;
	_commands.clear();
}


/** Appends a new path command to the current glyph. 
 *  @param[in] cmd command to append */
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


/** Writes an SVG representation of the path commands to a stream. The parameters
 *  can be scaled by the given factors.
 *  @param[in] os output stream
 *  @param[in] sx horizontal scale factor 
 *  @param[in] sy vertical scale factor */
void Glyph::writeSVGCommands (ostream &os, double sx, double sy) const {
	FORALL (_commands, ConstIterator, i)
		(*i)->writeSVGCommand(os, sx, sy);
}


/** Calls a function for all path sections/commands. 
 *  @param[in] f function to be called
 *  @param[in] userParam parameter passed to function f */
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
 *  TrueType and Type1 fonts only support 3 drawing commands (moveto, lineto, conicto/cubicto).
 *  In the case of successive bezier curve sequences, control points or tangent slopes are often
 *  identical so that the path description contains redundant information. SVG provides shorthand 
 *  curve commands that need less parameters because they reuse previously given arguments.
 *  This method detects such command sequences and replaces them by their short form. It does not
 *  recompute the whole paths to reduce the number of necessary line/curve segments. */
void Glyph::optimizeCommands () {	
	LPair fp;            // first point of current path
	LPair cp;            // current point (where path drawing continues)
	ConstIterator prev;  // preceding command
	LPair pstore[2];
	FORALL(_commands, Iterator, it) {
		char cmd = (*it)->getSVGPathCommand();
		const vector<LPair> &params = (*it)->params();
		GlyphCommand *new_command = 0;
		switch (cmd) {
			case 'M':
				fp = params[0];  // record first point of path
				break;
			case 'Z':
				cp = fp;
				break;
			case 'L':
				if (params[0].x() == cp.x())
					new_command = new GlyphVerticalLineTo(params[0]);
				else if (params[0].y() == cp.y())
					new_command = new GlyphHorizontalLineTo(params[0]);
				break;
			case 'C':
				if ((*prev)->getSVGPathCommand() == 'C' || (*prev)->getSVGPathCommand() == 'S') {
					if (params[0] == pstore[1]*2-pstore[0])  // is first control point reflection of preceding second control point?
						new_command = new GlyphShortCubicTo(params[1], params[2]);
				}
				pstore[0] = params[1]; // store second control point and
				pstore[1] = params[2]; // curve endpoint
				break;
			case 'Q':
				if ((*prev)->getSVGPathCommand() == 'Q' || (*prev)->getSVGPathCommand() == 'T') {
					if (params[0] == pstore[1]*2-pstore[0])
						new_command = new GlyphShortConicTo(params[1]);
				}
				// [pass through]
			case 'S':
			case 'T':
				pstore[0] = params[0]; // store (second) control point and
				pstore[1] = params[1]; // curve endpoint
				break;
		}
		// update current point
		if (!(*it)->params().empty())
			cp = (*it)->param(-1);

		// replace current command by shorthand form
		if (new_command) {
			delete *it;
			*it = new_command;
			new_command = 0;
		}
		prev = it;
	}
}
