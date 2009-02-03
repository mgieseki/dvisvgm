/***********************************************************************
** SpecialManager.cpp                                                 **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#include <sstream>
#include "SpecialHandler.h"
#include "SpecialManager.h"
#include "macros.h"

using namespace std;



SpecialManager::~SpecialManager () {
	FORALL(_handlers, Iterator, it)
		delete it->second;
}


void SpecialManager::registerHandler (SpecialHandler *handler) {
	if (handler) {
		delete findHandler(handler->prefix());
		_handlers[handler->prefix()] = handler;
	}
}


/** Looks for an appropriate handler for a given special prefix.
 *  @param[in] prefix the special prefix, e.g. "color" or "em"
 *  @return in case of success: pointer to handler, 0 otherwise */
SpecialHandler* SpecialManager::findHandler (const string &prefix) const {
	ConstIterator it = _handlers.find(prefix);
	if (it != _handlers.end())
		return it->second;
	return 0;
}


/** Executes a special command. 
 *  @param[in] special the special expression 
 *  @return true if a special handler was found 
 *  @throw SpecialException in case of errors during special processing */
bool SpecialManager::process (const string &special) {
	istringstream iss(special);
	string prefix;
	iss >> prefix;
	if (SpecialHandler *handler = findHandler(prefix)) {
		handler->process(iss);
		return true;
	}
	return false;
}
