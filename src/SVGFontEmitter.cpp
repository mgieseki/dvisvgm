/***********************************************************************
** SVGFontEmitter.cpp                                                 **
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

#include <sstream>
#include "macros.h"
#include "CharmapTranslator.h"
#include "FontEncoding.h"
#include "FontGlyph.h"
#include "SVGFontEmitter.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;

SVGFontEmitter::SVGFontEmitter (const string &fname, FontEncoding *enc, const CharmapTranslator &cmt, XMLElementNode *root) 
	: _charmapTranslator(cmt), _rootNode(root), _encoding(enc)
{
	_fontEngine.setFont(fname);
}


void SVGFontEmitter::readFontFile (const string &fname) {
	_fontEngine.setFont(fname);
}


int SVGFontEmitter::emitFont (string id) const {
	return emitFont(0, id);
}


int SVGFontEmitter::emitFont (const set<int> &usedChars, string id) const {
	return emitFont(&usedChars, id);
}


int SVGFontEmitter::emitFont (const set<int> *usedChars, string id) const {
	if (!usedChars || usedChars->empty())
		return 0;

	XMLElementNode *fontNode = new XMLElementNode("font");
	if (id != "")
		fontNode->addAttribute("id", id);
	fontNode->addAttribute("horiz-adv", XMLString(_fontEngine.getHAdvance()));
	_rootNode->append(fontNode);
	
	XMLElementNode *faceNode = new XMLElementNode("font-face");
	faceNode->addAttribute("font-family", id != "" ? id : _fontEngine.getFamilyName());
	faceNode->addAttribute("units-per-em", XMLString(_fontEngine.getUnitsPerEM()));
	faceNode->addAttribute("ascent", XMLString(_fontEngine.getAscender()));
	faceNode->addAttribute("descent", XMLString(_fontEngine.getDescender()));
	fontNode->append(faceNode);
	
#if 0
	// build panose-1 string
	vector<int> panose = _fontEngine.getPanose();
	if (panose.size() > 0) {
		string panstr;
		FORALL(panose, vector<int>::iterator, i) {
			panstr += XMLString(*i);
			if (i+1 != panose.end())
				panstr += " ";
		}
		faceNode->addAttribute("panose-1", panstr);
	}
#endif
	FORALL(*usedChars, set<int>::const_iterator, i) {			
		emitGlyph(*i);  // create new glyphNode
		fontNode->append(_glyphNode);
	}
	return usedChars->size();
}


bool SVGFontEmitter::emitGlyph (int c) const {
	_glyphNode = new XMLElementNode("glyph");
	_glyphNode->addAttribute("unicode", XMLString(_charmapTranslator.unicode(c), false));
	int advance;
	const char *name;
	if (_encoding && (name = _encoding->getEntry(c)))
		advance = _fontEngine.getHAdvance(name);
	else {
		advance = _fontEngine.getHAdvance(c);
	   name = _fontEngine.getGlyphName(c).c_str();
	}
	_glyphNode->addAttribute("horiz-adv-x", XMLString(advance));
	_glyphNode->addAttribute("glyph-name", name);
	ostringstream path;
	Glyph glyph;
	glyph.read(c, _encoding, _fontEngine);
	glyph.closeOpenPaths();
//	glyph.optimizeCommands();
	glyph.writeSVGCommands(path);
	_glyphNode->addAttribute("d", path.str());
	return true;
}

