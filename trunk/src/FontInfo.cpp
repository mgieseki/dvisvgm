/***********************************************************************
** FontInfo.cpp                                                       **
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

#include "FontEmitter.h"
#include "FontInfo.h"
#include "Message.h"
#include "TFM.h"

using namespace std;

FontInfo::FontInfo (string n, UInt32 cs, double ds, double sc) 
	: fontname(n), checksum(cs), designSize(ds), scaledSize(sc), tfm(0)
{
}


FontInfo::~FontInfo () {
	delete tfm;
}


void FontInfo::readTFM (FileFinder *fileFinder) {
	if (!tfm)
		tfm = TFM::createFromFile(fontname.c_str(), fileFinder);
}


/*
int FontInfo::emitUsedGlyphs (const FontEmitter &emitter) const {
	return emitter.emitFont(usedChars, fontname);
}


void FontInfo::mergeUsedGlyphs (const FontInfo &fi) {
	usedChars.insert(fi.usedChars.begin(), fi.usedChars.end());
}*/

