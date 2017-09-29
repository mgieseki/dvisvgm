/*************************************************************************
** PdfSpecialHandler.cpp                                                **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <cstring>
#include <utility>
#include "InputReader.hpp"
#include "MapLine.hpp"
#include "PdfSpecialHandler.hpp"
#include "FontMap.hpp"
#include "Message.hpp"
#include "PapersizeSpecialHandler.hpp"
#include "SpecialActions.hpp"
#include "SpecialManager.hpp"

using namespace std;


PdfSpecialHandler::PdfSpecialHandler () : _maplineProcessed(false)
{
}


void PdfSpecialHandler::preprocess (const char*, istream &is, SpecialActions &actions) {
	StreamInputReader ir(is);
	ir.skipSpace();
	string cmd = ir.getWord();
	if (cmd != "pagesize")
		return;
	// add page sizes to collection of paper sizes in order to handle them equally
	SpecialHandler *handler = SpecialManager::instance().findHandlerByName("papersize");
	if (PapersizeSpecialHandler *papersizeHandler = dynamic_cast<PapersizeSpecialHandler*>(handler)) {
		try {
			Length width, height;
			// parse parameter sequence of the form (name length)+
			while (!ir.eof()) {
				string dimname = ir.getWord();
				string lenstr = ir.getString(" \t");
				// only consider width and height settings
				if (dimname == "width" && !lenstr.empty())
					width.set(lenstr);
				else if (dimname == "height" && !lenstr.empty())
					height.set(lenstr);
			}
			papersizeHandler->storePaperSize(actions.getCurrentPageNumber(), width, height);
		}
		catch (UnitException &e) { // ignore invalid length units for now
		}
	}
}


bool PdfSpecialHandler::process (const char*, istream &is, SpecialActions&) {
	StreamInputReader ir(is);
	ir.skipSpace();
	string cmd = ir.getWord();
	ir.skipSpace();
	if (cmd == "mapline" || cmd == "mapfile") {
		// read mode selector ('+', '-', or '=')
		char modechar = '+';           // default mode (append if new, do not replace existing mapping)
		if (strchr("=+-", ir.peek()))  // leading modifier given?
			modechar = char(ir.get());
		else if (!_maplineProcessed) { // no modifier given?
			// remove default map entries if this is the first mapline/mapfile special called
			FontMap::instance().clear();
		}

		if (cmd == "mapline") {
			try {
				MapLine mapline(is);
				FontMap::instance().apply(mapline, modechar);
			}
			catch (const MapLineException &ex) {
				Message::wstream(true) << "pdf:mapline: " << ex.what() << '\n';
			}
		}
		else { // mapfile
			string fname = ir.getString();
			if (!FontMap::instance().read(fname, modechar))
				Message::wstream(true) << "can't open map file " << fname << '\n';
		}
		_maplineProcessed = true;
	}
	return true;
}


const vector<const char*> PdfSpecialHandler::prefixes () const {
	const vector<const char*> pfx {"pdf:"};
	return pfx;
}
