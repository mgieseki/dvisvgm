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
#include <fstream>
#include "DVIReader.h"
#include "Font.h"
#include "FontManager.h"
#include "KPSFileFinder.h"
#include "Message.h"
#include "TFM.h"
#include "VFReader.h"
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
		Num2IdMap::const_iterator it = num2id.find(n);
		return (it == num2id.end()) ? -1 : it->second;
	}
	const VirtualFont *vf = vfStack.top();
	VfNum2IdMap::const_iterator vit = vfnum2id.find(vfStack.top());
	if (vit == vfnum2id.end())
		return -1;
	const Num2IdMap &num2id = vit->second;
	Num2IdMap::const_iterator it = num2id.find(n);
	return (it == num2id.end()) ? -1 : it->second;
}


int FontManager::fontID (Font *font) const {
	for (unsigned i=0; i < fonts.size(); i++)
		if (fonts[i] == font)
			return i;
	return -1;
}


int FontManager::fontID (string name) const {
	map<string,int>::const_iterator it = name2id.find(name);
	if (it == name2id.end())
		return -1;
	return it->second;
}


int FontManager::fontnum (int id) const {
	if (id < 0 || id > fonts.size())
		return -1;
	Font *font = fonts[id];
	if (vfStack.empty()) {
		FORALL(num2id, Num2IdMap::const_iterator, i)
			if (i->second == id)
				return i->first;
	}
	else {
		VfNum2IdMap::const_iterator it = vfnum2id.find(vfStack.top());
		if (it == vfnum2id.end())
			return -1;
		const Num2IdMap &num2id = it->second;
		FORALL(num2id, Num2IdMap::const_iterator, i)
			if (i->second == id)
				return i->first;
	}
	return -1;
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
 *  @param ssize scaled size in TeX point units 
 *  @return id of registered font */
int FontManager::registerFont (UInt32 fontnum, string name, UInt32 checksum, double dsize, double ssize) {
	int id = fontID(fontnum);
	if (id >= 0)
		return id;

	Font *newfont = 0;
	int newid = fonts.size();      // the new font gets this ID
	const char *fontpath=0;
	Name2IdMap::iterator it = name2id.find(name);
	if (it != name2id.end()) {  // font with same name already registered?
		Font *font = fonts[it->second];
		newfont = font->clone(dsize, ssize);
	}
	else {
		if ((fontpath = KPSFileFinder::lookup(name+".pfb")))
			newfont = PhysicalFont::create(name, checksum, dsize, ssize, PhysicalFont::PFB);
		else if ((fontpath = KPSFileFinder::lookup(name+".ttf")))
			newfont = PhysicalFont::create(name, checksum, dsize, ssize, PhysicalFont::TTF);
		else if ((fontpath = KPSFileFinder::lookup(name+".vf")))
			newfont = VirtualFont::create(name, checksum, dsize, ssize);
		else if ((fontpath = KPSFileFinder::lookup(name+".mf")))
			newfont = PhysicalFont::create(name, checksum, dsize, ssize, PhysicalFont::MF);
		else
			newfont = new EmptyFont(name);
		//				throw FontException("font " + name + " not found");
		name2id[name] = newid;
	}
	fonts.push_back(newfont);
	if (vfStack.empty())  // register font referenced in dvi file?
		num2id[fontnum] = newid;
	else {  // register font referenced in vf file
		VirtualFont *vf = const_cast<VirtualFont*>(vfStack.top());
		vfnum2id[vf][fontnum] = newid;
	}
	if (VirtualFont *vf = dynamic_cast<VirtualFont*>(newfont)) {
		// read vf file and register its fonts
		enterVF(vf);
		ifstream ifs(fontpath, ios::binary);
		VFReader vfReader(ifs, this);
		vf->read(vfReader);
		leaveVF();
	}
	return newid;
}


//@@ do we need this method?
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


ostream& FontManager::write (ostream &os, Font *font, int level) {
#if 1
	if (font) {
		int id = -1;
		for (int i=0; i < fonts.size() && id < 0; i++)
			if (fonts[i] == font)
				id = i;

		VirtualFont *vf = dynamic_cast<VirtualFont*>(font);
		for (int j=0; j < level+1; j++)
			os << "  ";
		os << "id " << id 
			<< " fontnum " << fontnum(id) << " "
			<< (vf ? "VF" : "PF") << " " 
			<< font->name() 
			<< endl;
		
		if (vf) {
			enterVF(vf);
			const Num2IdMap &num2id = vfnum2id.find(vf)->second;
			FORALL(num2id, Num2IdMap::const_iterator, i) {
				Font *font = fonts[i->second];
				write(os, font, level+1);
			}
			leaveVF();
		}
	}
	else {
		for (int i=0; i < fonts.size(); i++)
			write(os, fonts[i], level);
		os << endl;
	}
#endif
	return os;
}
