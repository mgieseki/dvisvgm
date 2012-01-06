/*************************************************************************
** FontEngine.cpp                                                       **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <iostream>
#include <sstream>
#include <ft2build.h>
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_TABLES_H
#include "Font.h"
#include "FontEngine.h"
#include "Message.h"
#include "macros.h"

using namespace std;


FontEngine::FontEngine () {
	_currentFace = 0;
	_currentChar = _currentGlyphIndex = 0;
	_horDeviceRes = _vertDeviceRes = 300;
	_ptSize = 0;
   if (FT_Init_FreeType(&_library))
      Message::estream(true) << "FontEngine: error initializing FreeType library\n";
}


FontEngine::~FontEngine () {
   if (_currentFace && FT_Done_Face(_currentFace))
      Message::estream(true) << "FontEngine: error removing glyph\n";
   if (FT_Done_FreeType(_library))
      Message::estream(true) << "FontEngine: error removing FreeType library\n";
}


/** Returns the singleton instance of this class. */
FontEngine& FontEngine::instance () {
	static FontEngine engine;
	return engine;
}


string FontEngine::version () {
	FT_Int major, minor, patch;
	FT_Library &lib = instance()._library;
	FT_Library_Version(lib, &major, &minor, &patch);
	ostringstream oss;
	oss << major << '.' << minor << '.' << patch;
	return oss.str();
}


void FontEngine::setDeviceResolution (int x, int y) {
	_horDeviceRes = x;
	_vertDeviceRes = y;
}


/** Builds a table that maps glyph indexes to char codes.
 * @param[in] face font face to be used
 * @param[out] reverseMap the resulting map */
static void build_reverse_map (FT_Face face, map<UInt32, UInt32> &reverseMap) {
	FT_UInt glyphIndex;
	UInt32 charcode = FT_Get_First_Char(face, &glyphIndex);
	while (glyphIndex) {
//		if (reverseMap.find(glyphIndex) == reverseMap.end())
		reverseMap[glyphIndex] = charcode;
		charcode = FT_Get_Next_Char(face, charcode, &glyphIndex);
	}
}


/** Sets the font to be used.
 * @param[in] fname path to font file
 * @param[in] ptSize font size in point units
 * @return true on success */
bool FontEngine::setFont (const string &fname, int ptSize) {
	if (FT_New_Face(_library, fname.c_str(), 0, &_currentFace)) {
		Message::estream(true) << "FontEngine: error reading file " << fname << '\n';
      return false;
   }
	if (ptSize && FT_Set_Char_Size(_currentFace, 0, ptSize*64, _horDeviceRes, _vertDeviceRes)) {
		Message::estream(true) << "FontEngine: error setting character size\n";
      return false;
   }
   // look for a custom character map
   for (int i=0; i < _currentFace->num_charmaps; i++) {
      FT_CharMap charmap = _currentFace->charmaps[i];
      if (charmap->encoding == FT_ENCODING_ADOBE_CUSTOM) {
         FT_Set_Charmap(_currentFace, charmap);
         break;
      }
   }
	_ptSize = ptSize;
	return true;
}


bool FontEngine::setFont (const Font &font) {
	if (fname != font.name()) {
		fname = font.name();
		return setFont(font.path());
	}
	return true;
}


void FontEngine::buildTranslationMap (map<UInt32, UInt32> &translationMap) const {
	FT_CharMap unicodeMap=0, customMap=0;
   for (int i=0; i < _currentFace->num_charmaps; i++) {
      FT_CharMap charmap = _currentFace->charmaps[i];
      if (charmap->encoding == FT_ENCODING_ADOBE_CUSTOM)
			customMap = charmap;
		else if (charmap->encoding == FT_ENCODING_UNICODE)
			unicodeMap = charmap;
	}
	if (unicodeMap == 0 || customMap == 0)
		return;

	map<UInt32,UInt32> reverseMap;
	build_reverse_map(_currentFace, reverseMap);

	FT_Set_Charmap(_currentFace, unicodeMap);
	FT_UInt glyphIndex;
	UInt32 charcode = FT_Get_First_Char(_currentFace, &glyphIndex);
	while (glyphIndex) {
		translationMap[reverseMap[glyphIndex]] = charcode;
		charcode = FT_Get_Next_Char(_currentFace, charcode, &glyphIndex);
	}
	FT_Set_Charmap(_currentFace, customMap);
}


const char* FontEngine::getFamilyName () const {
	return _currentFace ? _currentFace->family_name : 0;
}


const char* FontEngine::getStyleName () const {
	return _currentFace ? _currentFace->style_name : 0;
}


int FontEngine::getUnitsPerEM () const {
	return _currentFace ? _currentFace->units_per_EM : 0;
}


int FontEngine::getAscender () const {
	return _currentFace ? _currentFace->ascender : 0;
}


int FontEngine::getDescender () const {
	return _currentFace ? _currentFace->descender : 0;
}


int FontEngine::getHAdvance () const {
	if (_currentFace) {
		TT_OS2 *table = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(_currentFace, ft_sfnt_os2));
		return table ? table->xAvgCharWidth : 0;
	}
	return 0;
}


int FontEngine::getHAdvance (unsigned c) const {
	if (_currentFace) {
		int index = FT_Get_Char_Index(_currentFace, (FT_ULong)c);
		FT_Load_Glyph(_currentFace, index, FT_LOAD_NO_SCALE);
		return _currentFace->glyph->metrics.horiAdvance;
	}
	return 0;
}


int FontEngine::getHAdvance (const char *name) const {
	if (_currentFace && name) {
		int index = FT_Get_Name_Index(_currentFace, (FT_String*)name);
		FT_Load_Glyph(_currentFace, index, FT_LOAD_NO_SCALE);
		return _currentFace->glyph->metrics.horiAdvance;
	}
	return 0;
}


/** Get first available character of the current font face. */
int FontEngine::getFirstChar () const {
	if (_currentFace)
		return _currentChar = FT_Get_First_Char(_currentFace, &_currentGlyphIndex);
	return 0;
}


/** Get the next available character of the current font face. */
int FontEngine::getNextChar () const {
	if (_currentFace && _currentGlyphIndex)
		return _currentChar = FT_Get_Next_Char(_currentFace, _currentChar, &_currentGlyphIndex);
	return getFirstChar();
}


/** Returns the glyph name for a given charater code.
 * @param[in] c char code
 * @return glyph name */
string FontEngine::getGlyphName (unsigned c) const {
	if (_currentFace && FT_HAS_GLYPH_NAMES(_currentFace)) {
		char buf[256];
		int index = FT_Get_Char_Index(_currentFace, c);
		FT_Get_Glyph_Name(_currentFace, index, buf, 256);
		return string(buf);
	}
	return "";
}


/* Returns the character code for a given glyph name.
 * @param name glyph name
 * @return char code or 0 if name couldn't be found
int FontEngine::getCharByGlyphName (const char *name) const {
	if (_currentFace && FT_HAS_GLYPH_NAMES(_currentFace))	{
		int index = FT_Get_Name_Index(_currentFace, (FT_String*)name);
		map<UInt32, UInt32>::const_iterator it = _reverseMap.find(index);
		if (it != _reverseMap.end())
			return it->second;
	}
	return 0;
}*/


vector<int> FontEngine::getPanose () const {
	vector<int> panose(10);
	if (_currentFace) {
		TT_OS2 *table = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(_currentFace, ft_sfnt_os2));
		if (table)
			for (int i=0; i < 10; i++)
				panose[i] = table->panose[i];
	}
	return panose;
}


bool FontEngine::setCharSize (int ptSize) {
	if (_currentFace) {
		if (FT_Set_Char_Size(_currentFace, 0, ptSize*64, _horDeviceRes, _vertDeviceRes)) {
			Message::estream(true) << "FontEngine: error setting character size\n";
		   return false;
      }
		_ptSize = ptSize;
      return true;
	}
	Message::wstream(true) << "FontEngine: can't set char size, no font face selected\n";
	return false;
}


// handle API change in freetype version 2.2.1
#if FREETYPE_MAJOR > 2 || (FREETYPE_MAJOR == 2 && (FREETYPE_MINOR > 2 || (FREETYPE_MINOR == 2 && FREETYPE_PATCH >= 1)))
	typedef const FT_Vector *FTVectorPtr;
#else
	typedef FT_Vector *FTVectorPtr;
#endif

// Callback functions used by traceOutline
static int moveto (FTVectorPtr to, void *user) {
	Glyph *glyph = static_cast<Glyph*>(user);
	glyph->moveto(to->x, to->y);
	return 0;
}


static int lineto (FTVectorPtr to, void *user) {
	Glyph *glyph = static_cast<Glyph*>(user);
	glyph->lineto(to->x, to->y);
	return 0;
}


static int conicto (FTVectorPtr control, FTVectorPtr to, void *user) {
	Glyph *glyph = static_cast<Glyph*>(user);
	glyph->conicto(control->x, control->y, to->x, to->y);
	return 0;
}


static int cubicto (FTVectorPtr control1, FTVectorPtr control2, FTVectorPtr to, void *user) {
	Glyph *glyph = static_cast<Glyph*>(user);
	glyph->cubicto(control1->x, control1->y, control2->x, control2->y, to->x, to->y);
	return 0;
}


/** Traces the outline of a glyph by calling the corresponding "drawing" functions.
 *  Each glyph is composed of straight lines, quadratic (conic) or cubic B�zier curves.
 *  This function takes all these outline segments and processes them by calling
 *  the corresponding functions. The functions must be provided in form of a
 *  FEGlyphCommands object.
 *  @param[in] index index of the glyph that will be traced
 *  @param[in] commands the drawing commands to be executed
 *  @param[in] scale if true the current pt size will be considered otherwise the plain TrueType units are used.
 *  @return false on errors */
static bool trace_outline (FT_Face face, int index, Glyph &glyph, bool scale) {
	if (face) {
		if (FT_Load_Glyph(face, index, scale ? FT_LOAD_DEFAULT : FT_LOAD_NO_SCALE)) {
			Message::estream(true) << "can't load glyph " << int(index) << '\n';
         return false;
      }

		if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
			Message::estream(true) << "no outlines found in glyph " << int(index) << '\n';
			return false;
		}
		FT_Outline outline = face->glyph->outline;
		const FT_Outline_Funcs funcs = {moveto, lineto, conicto, cubicto, 0, 0};
		FT_Outline_Decompose(&outline, &funcs, &glyph);
		return true;
	}
	Message::wstream(true) << "FontEngine: can't trace outline, no font face selected\n";
	return false;
}



/** Traces the outline of a glyph by calling the corresponding "drawing" functions.
 *  Each glyph is composed of straight lines, quadratic (conic) or cubic B�zier curves.
 *  This function takes all these outline segments and processes them by calling
 *  the corresponding functions. The functions must be provided in form of a
 *  FEGlyphCommands object.
 *  @param[in] chr the glyph of this character will be traced
 *  @param[in] commands the drawing commands to be executed
 *  @param[in] scale if true the current pt size will be considered otherwise
 *                   the plain TrueType units are used.
 *  @return false on errors */
bool FontEngine::traceOutline (unsigned char chr, Glyph &glyph, bool scale) const {
	if (_currentFace) {
		int index = FT_Get_Char_Index(_currentFace, chr);
		return trace_outline(_currentFace, index, glyph, scale);
	}
	Message::wstream(true) << "FontEngine: can't trace outline, no font face selected\n";
	return false;
}


bool FontEngine::traceOutline (const char *name, Glyph &glyph, bool scale) const {
	if (_currentFace) {
		int index = FT_Get_Name_Index(_currentFace, (FT_String*)name);
		return trace_outline(_currentFace, index, glyph, scale);
	}
	Message::wstream(true) << "FontEngine: can't trace outline, no font face selected\n";
	return false;
}
