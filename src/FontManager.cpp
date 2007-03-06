/***********************************************************************
** FontManager.cpp                                                    **
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

#include "DVIReader.h"
#include "FileFinder.h"
#include "FontManager.h"
#include "TFM.h"

using namespace std;


void VirtualFont::setChar (int c, DVIReader &dviReader) const {
	map<int,Byte*>::const_iterator it = charDef.find(c);
	if (it != charDef.end())
		dviReader.execute(it->second);
}



/////////////////////////////////////////////////////////

FontManager::FontManager () {
}


FontManager::~FontManager () {
	FORALL(fonts, vector<Font*>::iterator, i)
		delete *i;
	FORALL(tfms, vector<TFM*>::iterator, i)
		delete *i;
}


int FontManager::getIndex (int n) const {
	map<int,int>::iterator it = num2index.find(n);	
	if (it == num2index.end())
		return -1;
	return it->second;
}


void FontManager::addFont (int n, string name) {
	int index = getIndex(n);
	if (index < 0) {
		Font *newfont = 0;
		map<string,int>::iterator it = name2index.find(name);
		if (it != name2index.end()) {
			Font *font = fonts[it->second];
			newfont = font->clone();
		}
		else {
			if (fileFinder.lookup(name+".pfb"))
				newfont = new PhysicalFont(name, PhysicalFont::PFB);
			else if (fileFinder.lookup(name+".ttf"))
				newfont = new PhysicalFont(name, PhysicalFont::TTF);
			else if (fileFinder.lookup(name+".vf"))
				newfont = new VirtualFont(name);
			else if (fileFinder.lookup(name+".mf"))
				newfont = new PhysicalFont(name, PhysicalFont::MF);
			else 
				throw FontException("font " + name + " not found");
		}
		if (newfont) {
			int newindex = fonts.size();
			fonts.push_tail(newfont);
			tfms.push_tail(0);
			num2index[n] = newindex;
			name2index[name] = newindex;
		}
	}
}


const Font* FontManager::getFont (int n) const {
	int index = getIndex(n);
	if (index < 0)
		return 0;
	return fonts[index];
}



