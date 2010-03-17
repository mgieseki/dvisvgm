/*************************************************************************
** Font.cpp                                                             **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Font.h"
#include "FontEngine.h"
#include "FileFinder.h"
#include "Glyph.h"
#include "Message.h"
#include "TFM.h"
#include "VFReader.h"
#include "macros.h"

using namespace std;


/** Returns true if 'unicode' is a valid unicode value in XML documents.
 *  XML version 1.0 doesn't allow various unicode character references
 *  (&#1; for example).  */
static bool valid_unicode (UInt32 unicode) {
	UInt32 ranges[] = {
		0x0000, 0x0020,
		0x007f, 0x0084,
		0x0086, 0x009f,
		0xfdd0, 0xfddf
	};
	for (int i=0; i < 4; i++)
		if (unicode >= ranges[2*i] && unicode <= ranges[2*i+1])
			return false;
	return true;
}


UInt32 Font::unicode (UInt32 c) const {
	// @@ this should be optimized :-)
	return valid_unicode(c) ? c : 0x3400+c;
}


///////////////////////////////////////////////////////////////////////////////////////


TFMFont::TFMFont (string name, UInt32 cs, double ds, double ss)
	: tfm(0), fontname(name), checksum(cs), dsize(ds), ssize(ss)
{
}


TFMFont::~TFMFont () {
	delete tfm;
}


const TFM* TFMFont::getTFM () const {
	if (!tfm) {
		tfm = TFM::createFromFile(fontname.c_str());
		if (!tfm)
			throw FontException("can't find "+fontname+".tfm");
	}
	return tfm;
}


double TFMFont::charWidth (int c) const  {return getTFM()->getCharWidth(c);}
double TFMFont::charDepth (int c) const  {return getTFM()->getCharDepth(c);}
double TFMFont::charHeight (int c) const {return getTFM()->getCharHeight(c);}
double TFMFont::italicCorr (int c) const {return getTFM()->getItalicCorr(c);}

//////////////////////////////////////////////////////////////////////////////

Font* PhysicalFont::create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type) {
	return new PhysicalFontImpl(name, checksum, dsize, ssize, type);
}


/** Extracts the glyph outlines of a given character.
 *  @param[in]  c character code of requested glyph 
 *  @param[out] glyph path segments of the glyph outline
 *  @return true if outline could be computed */
bool PhysicalFont::getGlyph (int c, GraphicPath<Int32> &glyph) const {
	if (type() == MF) {

	}
	else { // PFB or TTF
		FontEngine::instance().setFont(*this);
		if (FontEncoding *enc = encoding()) {
			if (const char *encname = enc->getEntry(c))
				return FontEngine::instance().traceOutline(encname, glyph, false);
		}
		else
			return FontEngine::instance().traceOutline((unsigned char)c, glyph, false);
	}
	return false;
}


Font* VirtualFont::create (string name, UInt32 checksum, double dsize, double ssize) {
	return new VirtualFontImpl(name, checksum, dsize, ssize);
}

//////////////////////////////////////////////////////////////////////////////

PhysicalFontImpl::PhysicalFontImpl (string name, UInt32 cs, double ds, double ss, PhysicalFont::Type type)
	: TFMFont(name, cs, ds, ss), _filetype(type), _charmap(0)
{
}


PhysicalFontImpl::~PhysicalFontImpl () {
	delete _charmap;
}


const char* PhysicalFontImpl::path () const {
	string ext;
	switch (_filetype) {
		case PFB: ext = "pfb"; break;
		case TTF: ext = "ttf"; break;
		case MF : ext = "mf";  break;
	}
	return FileFinder::lookup(name()+"."+ext);
}


UInt32 PhysicalFontImpl::unicode (UInt32 c) const {
	if (type() == MF)
		return Font::unicode(c);
	
	if (_charmap == 0) {
		FontEngine &fe = FontEngine::instance();
		if (fe.setFont(*this)) {
			_charmap = new map<UInt32,UInt32>;
			fe.buildTranslationMap(*_charmap);
		}
	}
	typedef map<UInt32,UInt32>::const_iterator ConstIterator;
	ConstIterator it = _charmap->find(c);
	if (it != _charmap->end())
		return it->second;

	// No unicode equivalent found in font file.
	// Now we should look for a smart alternative but at the moment
	// it's sufficient to simply choose a valid unused unicode value...
	map<UInt32,UInt32> reverse_map;
	FORALL(*_charmap, ConstIterator, it)
		reverse_map[it->second] = it->first;
	// can we use charcode itself as unicode replacement?
	if (valid_unicode(c) && (reverse_map.empty() || reverse_map.find(c) != reverse_map.end()))
		return c;
	return 0x3400+c;
}


//////////////////////////////////////////////////////////////////////////////

VirtualFontImpl::VirtualFontImpl (string name, UInt32 cs, double ds, double ss)
	: TFMFont(name, cs, ds, ss)
{
}


VirtualFontImpl::~VirtualFontImpl () {
	// delete dvi vectors received by VFReaderAction
	for (map<UInt32, DVIVector*>::iterator i=charDefs.begin(); i != charDefs.end(); ++i)
		delete i->second;
}


const char* VirtualFontImpl::path () const {
	return FileFinder::lookup(name()+".vf");
}


void VirtualFontImpl::assignChar (UInt32 c, DVIVector *dvi) {
	if (dvi) {
		if (charDefs.find(c) == charDefs.end())
			charDefs[c] = dvi;
		else
			delete dvi;
	}
}


const vector<UInt8>* VirtualFontImpl::getDVI (int c) const {
	map<UInt32,DVIVector*>::const_iterator it = charDefs.find(c);
	return (it == charDefs.end() ? 0 : it->second);
}

