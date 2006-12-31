/***********************************************************************
** FontEngine.cpp                                                     **
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
// $Id$

#include <iostream>
#include <ft2build.h>
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_TABLES_H
#include "FontEngine.h"
#include "Message.h"
#include "macros.h"

using namespace std;


FontEngine::FontEngine () {
	currentFace = 0;
	currentChar = currentGlyphIndex = 0;
	horDeviceRes = vertDeviceRes = 300;
   if (FT_Init_FreeType(&library))
      Message::estream(true) << "FontEngine: error initializing FreeType library\n";
}


FontEngine::~FontEngine () {
   if (currentFace && FT_Done_Face(currentFace))
      Message::estream(true) << "FontEngine: error removing glyph\n";
   if (FT_Done_FreeType(library))
      Message::estream(true) << "FontEngine: error removing FreeType library\n";
}
   

void FontEngine::setDeviceResolution (int x, int y) {
	horDeviceRes = x;
	vertDeviceRes = y;
}


bool FontEngine::setFont (const string &fname, int ptSize) {
	if (FT_New_Face(library, fname.c_str(), 0, &currentFace)) {
		Message::estream(true) << "FontEngine: error reading file " << fname << endl;
      return false;
   }
	if (ptSize && FT_Set_Char_Size(currentFace, 0, ptSize*64, horDeviceRes, vertDeviceRes)) {
		Message::estream(true) << "FontEngine: error setting character size\n";
      return false;
   }  
   // look for a custom character map
   for (int i=0; i < currentFace->num_charmaps; i++) {
      FT_CharMap charmap = currentFace->charmaps[i];
      if (charmap->encoding == FT_ENCODING_ADOBE_CUSTOM) {
         FT_Set_Charmap(currentFace, charmap);
         break;
      }
   }
	return true;
}


void FontEngine::buildTranslationMap (map<UInt32, UInt32> &translationMap) const {
	FT_CharMap unicodeMap=0, customMap=0;
   for (int i=0; i < currentFace->num_charmaps; i++) {		
      FT_CharMap charmap = currentFace->charmaps[i];
      if (charmap->encoding == FT_ENCODING_ADOBE_CUSTOM) 
			customMap = charmap;
		else if (charmap->encoding == FT_ENCODING_UNICODE) 
			unicodeMap = charmap;
	}
	if (unicodeMap == 0 || customMap == 0)
		return;

	map<UInt32,UInt32> reverseMap;
	UInt32 glyphIndex;
	UInt32 charcode = FT_Get_First_Char(currentFace, &glyphIndex);
	while (glyphIndex) {
		if (reverseMap.find(glyphIndex) == reverseMap.end())
			reverseMap[glyphIndex] = charcode;
		charcode = FT_Get_Next_Char(currentFace, charcode, &glyphIndex);
	}

	FT_Set_Charmap(currentFace, unicodeMap);
	charcode = FT_Get_First_Char(currentFace, &glyphIndex);
	while (glyphIndex) {
		translationMap[reverseMap[glyphIndex]] = charcode;
		charcode = FT_Get_Next_Char(currentFace, charcode, &glyphIndex);
	}
	FT_Set_Charmap(currentFace, customMap);
}


const char* FontEngine::getFamilyName () const {
	return currentFace ? currentFace->family_name : 0;
}

const char* FontEngine::getStyleName () const {
	return currentFace ? currentFace->style_name : 0;
}

int FontEngine::getUnitsPerEM () const {
	return currentFace ? currentFace->units_per_EM : 0;
}

int FontEngine::getAscender () const {
	return currentFace ? currentFace->ascender : 0;
}

int FontEngine::getDescender () const {
	return currentFace ? currentFace->descender : 0;
}

int FontEngine::getHAdvance () const {
	if (currentFace) {
		TT_OS2 *table = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(currentFace, ft_sfnt_os2));
		return table ? table->xAvgCharWidth : 0;
	}
	return 0;
}

int FontEngine::getHAdvance (unsigned int c) const {
	if (currentFace) {
		int index = FT_Get_Char_Index(currentFace, (FT_ULong)c);
		FT_Load_Glyph(currentFace, index, FT_LOAD_NO_SCALE);
		return currentFace->glyph->metrics.horiAdvance;	
	}
	return 0;
}


/** Get first available character of the current font face. */
int FontEngine::getFirstChar () const {
	if (currentFace) 
		return currentChar = FT_Get_First_Char(currentFace, &currentGlyphIndex);
	return 0;
}


/** Get the next available character of the current font face. */
int FontEngine::getNextChar () const {
	if (currentFace && currentGlyphIndex) 
		return currentChar = FT_Get_Next_Char(currentFace, currentChar, &currentGlyphIndex);
	return getFirstChar();		
}


string FontEngine::getGlyphName (unsigned int c) const {
	if (currentFace) {
		char buf[256];
		int index = FT_Get_Char_Index(currentFace, c);
		FT_Get_Glyph_Name(currentFace, index, buf, 256);
		return string(buf);
	}
	return "";
}


vector<int> FontEngine::getPanose () const {
	vector<int> panose(10);
	if (currentFace) {
		TT_OS2 *table = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(currentFace, ft_sfnt_os2));
		if (table) 
			for (int i=0; i < 10; i++)
				panose[i] = table->panose[i];
	}
	return panose;
}


bool FontEngine::setCharSize (int ptSize) {
	if (currentFace) {
		if (FT_Set_Char_Size(currentFace, 0, ptSize*64, horDeviceRes, vertDeviceRes)) {
			Message::estream(true) << "FontEngine: error setting character size\n";
		   return false;
      }
      return true;
	}
	Message::wstream(true) << "FontEngine: can't set char size, no font face selected\n";
	return false;
}


// handle API change in freetype version 2.2.1
#if FREETYPE_MAJOR >= 2 && FREETYPE_MINOR >= 2 && FREETYPE_PATCH >= 1
	typedef const FT_Vector *FTVectorPtr;
#else
	typedef FT_Vector *FTVectorPtr;
#endif

// Callback functions used by traceOutline 
static int moveto (FTVectorPtr to, void *user) {
	FEGlyphCommands *commands = static_cast<FEGlyphCommands*>(user);
	commands->moveTo(to->x, to->y);
	return 0;
}


static int lineto (FTVectorPtr to, void *user) {
	FEGlyphCommands *commands = static_cast<FEGlyphCommands*>(user);
	commands->lineTo(to->x, to->y);
	return 0;
}


static int conicto (FTVectorPtr control, FTVectorPtr to, void *user) {
	FEGlyphCommands *commands = static_cast<FEGlyphCommands*>(user);
	commands->conicTo(control->x, control->y, to->x, to->y);
	return 0;
}


static int cubicto (FTVectorPtr control1, FTVectorPtr control2, FTVectorPtr to, void *user) {
	FEGlyphCommands *commands = static_cast<FEGlyphCommands*>(user);
	commands->cubicTo(control1->x, control1->y, control2->x, control2->y, to->x, to->y);
	return 0;
}


/** Traces the outline of a glyph by calling the corresponding "drawing" functions.
 *  Each glyph is composed of straight lines, quadratic (conic) or cubic Bézier curves.
 *  This function takes all these outline segments and processes them by calling
 *  the corresponding functions. The functions must be provided in form of a 
 *  FEGlyphCommands object. 
 *  @param chr the glyph of this character will be traced 
 *  @param commands the drawing commands to be executed
 *  @param scale if true the current pt size will be considered otherwise
 *               the plain TrueType units are used. 
 *  @return false on errors */
bool FontEngine::traceOutline (unsigned char chr, FEGlyphCommands &commands, bool scale) const {
	if (currentFace) {
		int index = FT_Get_Char_Index(currentFace, (FT_ULong)chr);
		if (FT_Load_Glyph(currentFace, index, scale ? FT_LOAD_DEFAULT : FT_LOAD_NO_SCALE)) {
			Message::estream(true) << "can't load glyph " << int(chr) << endl;
         return false;
      }

		if (currentFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
			Message::estream(true) << "no outlines found in glyph " << int(chr) << endl;
			return false;
		}
		FT_Outline outline = currentFace->glyph->outline;
		const FT_Outline_Funcs funcs = {moveto, lineto, conicto, cubicto, 0, 0};
		FT_Outline_Decompose(&outline, &funcs, &commands);
		return true;
	}
	Message::wstream(true) << "FontEngine: can't trace outline, no font face selected\n";
	return false;
}

