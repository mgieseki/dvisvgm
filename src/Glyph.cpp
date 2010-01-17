/*************************************************************************
** Glyph.cpp                                                            **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "FontEncoding.h"
#include "FontEngine.h"
#include "Glyph.h"
#include "Message.h"
#include "macros.h"

using namespace std;


void Glyph::read (unsigned char c, const FontEncoding *encoding, const FontEngine &fontEngine) {
	if (encoding) {
		if (const char *name = encoding->getEntry(c))
			fontEngine.traceOutline(name, *this, false);
		else
			Message::wstream(true) << "no encoding for char #" << int(c) << " in '" << encoding->name() << "'\n";
	}
	else
		fontEngine.traceOutline(c, *this, false);
}
