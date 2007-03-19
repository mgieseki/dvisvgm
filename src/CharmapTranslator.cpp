/***********************************************************************
** CharmapTranslator.cpp                                              **
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

#include <fstream>
#include "CharmapTranslator.h"
#include "FontEngine.h"
#include "KPSFileFinder.h"
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


CharmapTranslator::CharmapTranslator (const char *fontname) {
	setFont(fontname);
}


CharmapTranslator::CharmapTranslator (const FontEngine &fe) {
	fe.buildTranslationMap(translationMap);
}


void CharmapTranslator::setFont (const char *fontname) {
	translationMap.clear();
	
	string filename = string(fontname) + ".pfb";
	if (const char *path = KPSFileFinder::lookup(filename)) {
		FontEngine fe;
		fe.setFont(path);
		fe.buildTranslationMap(translationMap);
	}
}


UInt32 CharmapTranslator::unicode (UInt32 customCode) const {
	ConstIterator it = translationMap.find(customCode);	
	if (it != translationMap.end())
		return it->second;
	
	// No unicode equivalent found in font file.
	// Now we should look for a smart alternative but at the moment
	// it's sufficient to simply choose a valid unused unicode value...	
	map<UInt32,UInt32> reverseMap;
	FORALL(translationMap, ConstIterator, i)
		reverseMap[i->second] = i->first;
	// can we use charcode itself as unicode replacement?
	if (valid_unicode(customCode) && (reverseMap.empty() || reverseMap.find(customCode) != reverseMap.end()))
		return customCode;
	// @@ this should be optimized :-)
	return 0x3400+customCode;
}

