/***********************************************************************
** FontManager.h                                                      **
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

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <map>
#include <string>
#include <stack>
#include <vector>
#include "types.h"

using std::map;
using std::string;
using std::stack;
using std::vector;

class FileFinder;
class Font;
class TFM;
class VirtualFont;

/** This class provides methods for an easy DVI font handling. 
 *  DVI and VF files use local font numbers to reference fonts. For SVG output
 *  we need a single list with unique IDs of all physical fonts. Characters of
 *  virtual fonts are completely replaced by their DVI description so they don't
 *  appear anywhere in the output. */
class FontManager
{
   public:
      FontManager ();
      ~FontManager ();
		void registerFont (UInt32 fontnum, string fontname, UInt32 checksum, double dsize, double scale);
		const Font* selectFont (int n);
		const Font* getFont (int n) const;
		const Font* getFont (string name) const;
		int fontID (int n) const;
		int fontID (Font *font) const;
		int fontID (string name) const;
		void enterVF (const VirtualFont *vf);
		void leaveVF ();
		const vector<Font*>& getFonts () const {return fonts;}

   private:
		map<UInt32,int>  num2index;        ///< DVI font number -> fontID
		map<string, int> name2index;       ///< fontname -> fontID
		vector<Font*>    fonts;            ///< all registered Fonts (index == fontID)
		stack<const VirtualFont*> vfStack; ///< stack of currently processed virtual fonts
		int selectedFontID;                ///< fontID of active font
};




#endif
