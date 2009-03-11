/***********************************************************************
** SVGFontTraceEmitter.cpp                                            **
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

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "Font.h"
#include "FontManager.h"
#include "GFGlyphTracer.h"
#include "macros.h"
#include "Message.h"
#include "MetafontWrapper.h"
#include "SVGFontTraceEmitter.h"
#include "TFM.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


SVGFontTraceEmitter::SVGFontTraceEmitter (const Font *f, const FontManager &fm, const CharmapTranslator &cmt, XMLElementNode *n, bool uf)
	: _gfTracer(0), _in(0), _font(f), _fontManager(fm), _mag(4.0), 
	  _charmapTranslator(cmt), _rootNode(n), _glyphNode(0), _useFonts(uf)
{
}


SVGFontTraceEmitter::~SVGFontTraceEmitter () {
	delete _gfTracer;
	delete _in;
	MetafontWrapper::removeOutputFiles(_font->name());
}


bool SVGFontTraceEmitter::checkTracer () const {
	if (!_gfTracer) {
		MetafontWrapper mf(_font->name());
		mf.make("ljfour", _mag); // call Metafont if necessary
		if (mf.success() && _font->getTFM()) {
			_in = new ifstream((_font->name()+".gf").c_str(), ios_base::binary);
			_gfTracer = new GFGlyphTracer(*_in, 1000.0/_font->getTFM()->getDesignSize()); // 1000 units per em
		}
		else 
			return false;  // Metafont failed
	}
	return true;
}


int SVGFontTraceEmitter::emitFont (const char *id) const {
	// @@ not needed at the moment
	return 0;
}


int SVGFontTraceEmitter::emitFont (const set<int> &usedChars, const char *id) const {
	return emitFont(&usedChars, id);
}


int SVGFontTraceEmitter::emitFont (const set<int> *usedChars, const char *id) const {
	if (!usedChars || usedChars->empty())
		return 0;

	if (!checkTracer()) {
		Message::wstream(true) << "unable to find " << _font->name() << ".mf, can't embed font\n";
		return 0;
	}

	Message::mstream() << "tracing glyphs of " << _font->name() << endl;

	XMLElementNode *fontNode=0;
	if (_useFonts) {
		fontNode = new XMLElementNode("font");
		if (id && strlen(id) > 0)
			fontNode->addAttribute("id", id);
		_rootNode->append(fontNode);

		XMLElementNode *faceNode = new XMLElementNode("font-face");
		faceNode->addAttribute("font-family", id);
		faceNode->addAttribute("units-per-em", XMLString(1000));
		fontNode->append(faceNode);
	}
	else {
		fontNode = _rootNode;
#if 0
		if (usedChars && _font && !usedChars->empty()) {
			ostringstream oss;
			oss << _font->name() << ", " << _font->scaledSize() << "pt"; 
			fontNode->append(new XMLCommentNode(oss.str()));
		}
#endif
	}

	FORALL(*usedChars, set<int>::const_iterator, i) {			
		emitGlyph(*i);  // create new glyphNode
		fontNode->append(_glyphNode);
	}
	return usedChars->size();
}


/** Creates the SVG definition of a single glyph.
 *  @param[in] c character code of the glyph
 *  @param[in] duplicate true if the glyph is already included in different size */
bool SVGFontTraceEmitter::emitGlyph (int c) const {
	const TFM *tfm = _font->getTFM();
	if (!checkTracer() || !tfm)
		return false;

	Message::mstream() << '[';
	if (c <= 32 || c >= 127)
		Message::mstream() << '#' << c;
	else
		Message::mstream() << char(c);

	_gfTracer->executeChar(c);
	ostringstream path;
	const Glyph &glyph = _gfTracer->getGlyph();
	double sx=1.0, sy=1.0;
	if (_useFonts) {
		_glyphNode = new XMLElementNode("glyph");
		_glyphNode->addAttribute("unicode", XMLString(_charmapTranslator.unicode(c), false));
		_glyphNode->addAttribute("horiz-adv-x", XMLString(1000.0*tfm->getCharWidth(c)/tfm->getDesignSize())); 
	}
	else {
		ostringstream oss;
		oss << 'g' << _fontManager.fontID(_font) << c;
		_glyphNode = new XMLElementNode("path");		
		_glyphNode->addAttribute("id", oss.str());
		sx = _font->scaledSize()/1000.0; // 1000 units per em
		sy = -sx;
	}
	glyph.writeSVGCommands(path, sx, sy);
	_glyphNode->addAttribute("d", path.str());
	Message::mstream() << ']';
	return true;
}

