/***********************************************************************
** SVGFontTraceEmitter.cpp                                            **
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
// $Id: SVGFontTraceEmitter.cpp,v 1.5 2006/01/05 16:05:06 mgieseki Exp $

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "FileFinder.h"
#include "GFTracer.h"
#include "macros.h"
#include "Message.h"
#include "MetafontWrapper.h"
#include "SVGFontTraceEmitter.h"
#include "TFM.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


SVGFontTraceEmitter::SVGFontTraceEmitter (const string &fname, const TFM *metrics,
		                                    const CharmapTranslator &cmt, XMLElementNode *n, const FileFinder *ff)
	: gfTracer(0), in(0), fontname(fname), tfm(metrics), mag(4.0), 
	  charmapTranslator(cmt), rootNode(n), glyphNode(0), fileFinder(ff)
{
}


SVGFontTraceEmitter::~SVGFontTraceEmitter () {
	delete gfTracer;
	delete in;
	MetafontWrapper::removeOutputFiles(fontname);
}


bool SVGFontTraceEmitter::checkTracer () const {
	if (!gfTracer) {
		MetafontWrapper mf(fontname, fileFinder);
		mf.make("ljfour", mag); // call Metafont if necessary
		if (mf.success() && tfm) {
			in = new ifstream((fontname+".gf").c_str(), ios_base::binary);
			gfTracer = new GFTracer(*in, 1000.0/tfm->getDesignSize()); // 1000 units per em
		}
		else 
			return false;  // Metafont failed
	}
	return true;
}


int SVGFontTraceEmitter::emitFont (string id) const {
	// @@ not needed at the moment
	return 0;
}


int SVGFontTraceEmitter::emitFont (const set<int> &usedChars, string id) const {
	return emitFont(&usedChars, id);
}


int SVGFontTraceEmitter::emitFont (const set<int> *usedChars, string id) const {
	if (!usedChars || usedChars->empty())
		return 0;

	if (!checkTracer()) {
		Message::wstream(true) << "unable to find " << fontname << ".mf, can't embed font\n";
		return 0;
	}

	Message::mstream() << "tracing glyphs of " << fontname << endl;
	XMLElementNode *fontNode = new XMLElementNode("font");
	if (id != "")
		fontNode->addAttribute("id", id);
//	fontNode->addAttribute("horiz-adv", XMLString(0));
	rootNode->append(fontNode);
	
	XMLElementNode *faceNode = new XMLElementNode("font-face");
	faceNode->addAttribute("font-family", id);
	faceNode->addAttribute("units-per-em", XMLString(1000));
//	faceNode->addAttribute("ascent", XMLString(0)); // @@
//	faceNode->addAttribute("descent", XMLString(0));  // @@
	fontNode->append(faceNode);

	FORALL(*usedChars, set<int>::const_iterator, i) {			
		emitGlyph(*i);  // create new glyphNode
		fontNode->append(glyphNode);
	}
	return usedChars->size();
}


bool SVGFontTraceEmitter::emitGlyph (int c) const {
	if (!checkTracer() || !tfm)
		return false;

	Message::mstream() << '[';
	if (c <= 32 || c >= 127)
		Message::mstream() << '#' << c;
	else
		Message::mstream() << char(c);
	gfTracer->executeChar(c);
	ostringstream path;
	const Glyph &glyph = gfTracer->getGlyph();
	glyphNode = new XMLElementNode("glyph");
	glyphNode->addAttribute("unicode", XMLString(charmapTranslator.unicode(c), false));
	glyphNode->addAttribute("horiz-adv-x", XMLString(1000.0*tfm->getCharWidth(c)/tfm->getDesignSize())); 
	glyph.writeSVGCommands(path);
	glyphNode->addAttribute("d", path.str());
	Message::mstream() << ']';
	return true;
}

