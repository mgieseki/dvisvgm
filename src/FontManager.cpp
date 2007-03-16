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

#include <cstdlib>
#include "DVIReader.h"
#include "Font.h"
#include "FontManager.h"
#include "KPSFileFinder.h"
#include "Message.h"
#include "TFM.h"
#include "macros.h"

using namespace std;


FontManager::FontManager () 
	: selectedFontID(-1)
{
}


FontManager::~FontManager () {
	FORALL(fonts, vector<Font*>::iterator, i)
		delete *i;
}


/** Returns a unique ID that identifies the font. 
 *  @param n local font number, as used in DVI and VF files 
 *  @return non-negative font ID if font was found, -1 otherwise */
int FontManager::fontID (int n) const {
	if (vfStack.empty()) {
		map<UInt32,int>::const_iterator it = num2index.find(n);
		return (it == num2index.end()) ? -1 : it->second;
	}
	return vfStack.top()->fontID(n);
}


int FontManager::fontID (Font *font) const {
	for (int i=0; i < fonts.size(); i++)
		if (fonts[i] == font)
			return i;
	return -1;
}


int FontManager::fontID (string name) const {
	map<string,int>::const_iterator it = name2index.find(name);
	if (it == name2index.end())
		return -1;
	return it->second;
}


/** Returns a previously registered font.
 *  @param n local font number, as used in DVI and VF files 
 *  @return pointer to font if font was found, 0 otherwise */
const Font* FontManager::getFont (int n) const {
	int id = fontID(n);
	return (id < 0) ? 0 : fonts[id];
}


const Font* FontManager::getFont (string name) const {
	int id = fontID(name);
	if (id < 0)
		return 0;
	return fonts[id];
}


/** Registers a new font to be managed by the FontManager. If there is
 *  already a registered font assigned to number n, nothing happens.
 *  @param fontnum local font number, as used in DVI and VF files 
 *  @param name fontname, e.g. cmr10 
 *  @param checksum checksum to be compared with TFM checksum
 *  @param dsize design size in TeX point units
 *  @param ssize scaled size in TeX point units */
void FontManager::registerFont (UInt32 fontnum, string name, UInt32 checksum, double dsize, double ssize) {
/*	SHOW(name);
	SHOW(checksum);
	SHOW(dsize);
	SHOW(ssize); */
	int id = fontID(fontnum);
	if (id < 0) {
		Font *newfont = 0;
		int newid = fonts.size();      // the new font gets this ID
		map<string,int>::iterator it = name2index.find(name);
		if (it != name2index.end()) {  // font with same name already registered?
			Font *font = fonts[it->second];
			newfont = font->clone(dsize, ssize);
		}
		else {
			if (KPSFileFinder::find(name+".pfb"))
				newfont = PhysicalFont::create(name, checksum, dsize, ssize, PhysicalFont::PFB);
			else if (KPSFileFinder::find(name+".ttf"))
				newfont = PhysicalFont::create(name, checksum, dsize, ssize, PhysicalFont::TTF);
			else if (KPSFileFinder::find(name+".vf"))
				newfont = VirtualFont::create(name, checksum, dsize, ssize);
			else if (KPSFileFinder::find(name+".mf"))
				newfont = PhysicalFont::create(name, checksum, dsize, ssize, PhysicalFont::MF);
			else {
#ifdef MIKTEX
				const string MKTEXMF = "makemf";
#else
				const string MKTEXMF = "mktexmf";
#endif
				Message::mstream() << "running " << MKTEXMF << " for " << name << endl;
				system((MKTEXMF + " " + name + ".mf").c_str());
				if (KPSFileFinder::find(name+".mf"))
					newfont = PhysicalFont::create(name, checksum, dsize, ssize, PhysicalFont::MF);
				else					
					throw FontException("font " + name + " not found");
			}
			name2index[name] = newid;
		}
		if (newfont) {
			fonts.push_back(newfont);
			num2index[fontnum] = newid;
		}
	}
}


//@@ do we need this?
const Font* FontManager::selectFont (int n) {
	int id = fontID(n);
	if (id < 0)
		return 0;
	selectedFontID = id;
	return fonts[id];
}


/** Enters a new virtual font frame. 
 *  This method must be called before processing a VF character.
 *  @param vf virtual font */
void FontManager::enterVF (const VirtualFont *vf) {
	if (vf)
		vfStack.push(vf);
}

/** Leaves a previously entered virtual font frame. 
 *  This method must be called after processing a VF character.
 *  @throw FontException if there is no VF frame to leave */
void FontManager::leaveVF () {
	if (vfStack.empty())
		throw FontException(""); // @@
	vfStack.pop();
}
