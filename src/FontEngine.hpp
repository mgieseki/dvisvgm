/*************************************************************************
** FontEngine.hpp                                                       **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2025 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef FONTENGINE_HPP
#define FONTENGINE_HPP

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CID_H
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Character.hpp"
#include "CharMapID.hpp"
#include "Glyph.hpp"
#include "NumericRanges.hpp"
#include "RangeMap.hpp"

class Font;

/** This class provides methods to handle font files and font data.
 *  It's a wrapper for the Freetype font library. */
class FontEngine {
	using CodepointRanges = NumericRanges<uint32_t>;
	public:
		~FontEngine ();
		static FontEngine& instance ();
		static std::string version ();
		bool setFont (const Font &font);
		const Font* currentFont () const {return _currentFont;}
		bool isCIDFont() const;
		bool hasVerticalMetrics () const;
		bool traceOutline (const Character &c, Glyph &glyph, bool scale=true) const;
		const char* getFamilyName () const;
		const char* getStyleName () const;
		const char* getPSName () const;
		std::string getPSName (const std::string &fname) const;
		int getUnitsPerEM () const;
		int getAscender () const;
		int getDescender () const;
		int getHAdvance () const;
		int getHAdvance (const Character &c) const;
		int getVAdvance (const Character &c) const;
		int getWidth (const Character &c) const;
		int getHeight (const Character &c) const;
		int getDepth (const Character &c) const;
		int getCharMapIDs (std::vector<CharMapID> &charmapIDs) const;
		int getNumGlyphs () const;
		CharMapID setUnicodeCharMap () const;
		CharMapID setCustomCharMap () const;
		std::vector<int> getPanose () const;
		std::string getGlyphName (const Character &c) const;
		int getCharIndexByGlyphName (const char *name) const;
		bool setCharMap (const CharMapID &charMapID) const;
		RangeMap buildGidToCharCodeMap () const;
		std::unique_ptr<const RangeMap> createCustomToUnicodeMap () const;

	protected:
		FontEngine ();
		bool setFont (const std::string &fname, int fontindex, const CharMapID &charmapID);
		int charIndex (const Character &c) const;
		void addCharsByGlyphNames (uint32_t minGID, uint32_t maxGID, RangeMap &charmap, CodepointRanges &ucp) const;

	private:
		FT_Face _currentFace = nullptr;
		FT_Library _library = nullptr;
		const Font *_currentFont = nullptr;
};

#endif
