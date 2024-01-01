/*************************************************************************
** NameTable.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2024 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <array>
#include "NameTable.hpp"
#include "TTFWriter.hpp"
#include "../Font.hpp"
#include "../version.hpp"

using namespace std;
using namespace ttf;

enum NameID {
	COPYRIGHT_NOTICE = 0,
	FONT_FAMILY = 1,
	FONT_SUBFAMILY = 2,
	FONT_ID = 3,
	FULL_FONT_NAME = 4,
	VERSION_STRING = 5,
	PS_NAME = 6,
	TRADEMARK = 7,
	MANUFACTURER_NAME = 8,
	DESIGNER = 9,
	DESCRIPTION = 10,
	URL_VENDOR = 11,
	URL_DESIGNER = 12,
	LICENSE_DESCRIPTION = 13,
	LICENSE_INFO_URL = 14,
	TYPOGRAPHIC_FAMILY_NAME = 16,
	TYPOGRAPHIC_SUBFAMILY_NAME = 17,
	COMPATIBLE_FULL = 18,
	SAMPLE_TEXT = 19,
	PS_CID_FINDFONT_NAME = 20,
	WWS_FAMILY_NAME = 21,
	WWS_SUBFAMILY_NAME = 22,
	LIGHT_BG_PALLETTE = 23,
	DARK_BG_PALLETTE = 24,
	VAR_PS_NAME_PREFIX = 25
};


void NameTable::write (ostream &os) const {
	struct NameEntry {
		NameID nameID;
		std::string name;
	};
	array<NameEntry,5> nameEntries {{
		{FONT_FAMILY, ttfWriter()->getFont().familyName()},
		{FULL_FONT_NAME, ttfWriter()->getFont().name()},
		{VERSION_STRING, string("Version 1.0")},
		{PS_NAME, ttfWriter()->getFont().name()},
		{DESCRIPTION, string("Generated by ")+PROGRAM_NAME+" "+PROGRAM_VERSION}
	}};
	writeUInt16(os, 0);  // table format
	writeUInt16(os, 2*nameEntries.size());  // number of name records
	writeUInt16(os, 6+2*12*nameEntries.size());  // offset to start of string storage
	size_t offset=0;
	// write name entry records ordered by platform ID
	for (const NameEntry &entry : nameEntries) {
		writeUInt16(os, 1);  // Mac
		writeUInt16(os, 0);  // Roman, UTF-8 encoding
		writeUInt16(os, 0);  // English
		writeUInt16(os, entry.nameID);
		writeUInt16(os, entry.name.length());
		writeUInt16(os, offset);
		offset += entry.name.length();
	}
	for (const NameEntry &entry : nameEntries) {
		writeUInt16(os, 3);  // Windows
		writeUInt16(os, 1);  // UCS-2 encoding
		writeUInt16(os, 0x0409);  // US English
		writeUInt16(os, entry.nameID);
		writeUInt16(os, 2*entry.name.length());
		writeUInt16(os, offset);
		offset += 2*entry.name.length();
	}
	// write string data
	for (const NameEntry &entry : nameEntries) {
		for (char c : entry.name)
			writeUInt8(os, c);
	}
	for (const NameEntry &entry : nameEntries) {
		for (char c : entry.name) {
			writeUInt8(os, 0);
			writeUInt8(os, c);
		}
	}
}
