/***********************************************************************
** SVGFontEmitter.cpp                                                 **
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
// $Id: SVGFontEmitter.cpp,v 1.7 2006/01/05 16:05:06 mgieseki Exp $

#include <sstream>
#include "macros.h"
#include "CharmapTranslator.h"
#include "FontGlyph.h"
#include "SVGFontEmitter.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;

SVGFontEmitter::SVGFontEmitter (const string &fname, const CharmapTranslator &cmt, XMLElementNode *root) 
	: charmapTranslator(cmt), rootNode(root)
{
	fontEngine.setFont(fname);
}


void SVGFontEmitter::readFontFile (const string &fname) {
	fontEngine.setFont(fname);
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
	fontNode->addAttribute("horiz-adv", XMLString(fontEngine.getHAdvance()));
	rootNode->append(fontNode);
	
	XMLElementNode *faceNode = new XMLElementNode("font-face");
	faceNode->addAttribute("font-family", id != "" ? id : fontEngine.getFamilyName());
	faceNode->addAttribute("units-per-em", XMLString(fontEngine.getUnitsPerEM()));
	faceNode->addAttribute("ascent", XMLString(fontEngine.getAscender()));
	faceNode->addAttribute("descent", XMLString(fontEngine.getDescender()));
	fontNode->append(faceNode);
	
#if 0
	// build panose-1 string
	vector<int> panose = fontEngine.getPanose();
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
		fontNode->append(glyphNode);
	}
	return usedChars->size();
}


bool SVGFontEmitter::emitGlyph (int c) const {
	glyphNode = new XMLElementNode("glyph");
	glyphNode->addAttribute("unicode", XMLString(charmapTranslator.unicode(c), false));
	glyphNode->addAttribute("glyph-name", fontEngine.getGlyphName(c));
	glyphNode->addAttribute("horiz-adv-x", XMLString(fontEngine.getHAdvance(c)));
	ostringstream path;
	Glyph glyph;
	glyph.read(c, fontEngine);
	glyph.closeOpenPaths();
//	glyph.optimizeCommands();
	glyph.writeSVGCommands(path);
	glyphNode->addAttribute("d", path.str());
	return true;
}

