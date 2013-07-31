/*************************************************************************
** CMapManager.cpp                                                      **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <sstream>
#include "CMap.h"
#include "CMapManager.h"
#include "CMapReader.h"
#include "Message.h"

using namespace std;


CMapManager::~CMapManager () {
	for (CMaps::iterator it=_cmaps.begin(); it != _cmaps.end(); ++it)
		delete it->second;
}


CMapManager& CMapManager::instance () {
	static CMapManager cmm;
	return cmm;
}


/** Loads a cmap and returns the corresponding object. */
CMap* CMapManager::lookup (const string &name) {
	CMaps::iterator it = _cmaps.find(name);
	if (it != _cmaps.end())
		return it->second;

	if (_includedCMaps.find(name) != _includedCMaps.end()) {
		_level = 0;
		ostringstream oss;
		oss << "circular reference of cmap " << name;
		throw CMapReaderException(oss.str());
	}

	CMap *cmap=0;
	if (name == "Identity-H")
		cmap = new IdentityHCMap;
	else if (name == "Identity-V")
		cmap = new IdentityVCMap;
	if (cmap) {
		_cmaps[name] = cmap;
		return cmap;
	}
	// Load cmap data of file <name> and also process all cmaps referenced by operator "usecmap".
	// This can lead to a sequence of further calls of lookup(). In order to prevent infinite loops
	// due to (disallowed) circular cmap inclusions, we keep track of all cmaps processed during
	// a sequence of inclusions.
	_includedCMaps.insert(name);  // save name of current cmap being processed
	_level++;                     // increase nesting level
	try {
		CMapReader reader;
		if ((cmap = reader.read(name)))
			_cmaps[name] = cmap;
		else
			_level = 1;
	}
	catch (const CMapReaderException &e) {
		Message::estream(true) << "CMap file " << name << ": " << e.what() << "\n";
	}
	if (--_level == 0)            // back again at initial nesting level?
		_includedCMaps.clear();    // => names of included cmaps are no longer needed
	return cmap;
}
