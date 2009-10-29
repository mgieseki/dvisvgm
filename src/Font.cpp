/*************************************************************************
** Font.cpp                                                             **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "FileFinder.h"
#include "Message.h"
#include "TFM.h"
#include "VFReader.h"
#include "macros.h"

using namespace std;


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
double TFMFont::italicCorr (int c) const  {return getTFM()->getItalicCorr(c);}

//////////////////////////////////////////////////////////////////////////////

Font* PhysicalFont::create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type) {
	return new PhysicalFontImpl(name, checksum, dsize, ssize, type);
}


Font* VirtualFont::create (string name, UInt32 checksum, double dsize, double ssize) {
	return new VirtualFontImpl(name, checksum, dsize, ssize);
}

//////////////////////////////////////////////////////////////////////////////

PhysicalFontImpl::PhysicalFontImpl (string name, UInt32 cs, double ds, double ss, PhysicalFont::Type type)
	: TFMFont(name, cs, ds, ss), filetype(type)
{
}


const char* PhysicalFontImpl::path () const {
	string ext;
	switch (filetype) {
		case PFB: ext = "pfb"; break;
		case TTF: ext = "ttf"; break;
		case MF : ext = "mf";  break;
	}
	return FileFinder::lookup(name()+"."+ext);
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

