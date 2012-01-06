/*************************************************************************
** SpecialManager.cpp                                                   **
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

#include <iomanip>
#include <sstream>
#include "SpecialActions.h"
#include "SpecialManager.h"
#include "macros.h"

using namespace std;


SpecialManager::~SpecialManager () {
	unregisterHandlers();
}


void SpecialManager::unregisterHandlers () {
	FORALL(_pool, vector<SpecialHandler*>::iterator, it)
		delete *it;
}


/** Registers a single special handler. This method doesn't check if a
 *  handler of the same class is already registered.
 *  @param[in] pointer to handler to be registered */
void SpecialManager::registerHandler (SpecialHandler *handler) {
	if (handler) {
		// get array of prefixes this handler is responsible for
		_pool.push_back(handler);
		for (const char **p=handler->prefixes(); *p; ++p)
			_handlers[*p] = handler;
		if (handler->isEndPageListener())
			_endPageListeners.push_back(handler);
		if (handler->isPositionListener())
			_positionListeners.push_back(handler);
	}
}


/** Registers a multiple special handlers.
 *  If ignorelist == 0, all given handlers are registered. To exclude selected sets of
 *  specials, the corresponding names can be given separated by non alpha-numeric characters,
 *  e.g. "color, ps, em" or "color: ps em" etc.
 *  @param[in] handlers pointer to zero-terminated array of handlers to be registered
 *  @param[in] ignorelist list of special names to be ignored */
void SpecialManager::registerHandlers (SpecialHandler **handlers, const char *ignorelist) {
	if (handlers) {
		string ign = ignorelist ? ignorelist : "";
		FORALL(ign, string::iterator, it)
			if (!isalnum(*it))
				*it = '%';
		ign = "%"+ign+"%";

		for (; *handlers; handlers++) {
			if (ign.find("%"+string((*handlers)->name())+"%") == string::npos)
				registerHandler(*handlers);
			else
				delete *handlers;
		}
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
 *  @param[in] actions actions the special handlers can perform
 *  @param[in] listener object that wants to be notified about the processing state
 *  @return true if a special handler was found
 *  @throw SpecialException in case of errors during special processing */
bool SpecialManager::process (const string &special, SpecialActions *actions, Listener *listener) const {
	istringstream iss(special);
	string prefix;
	int c;
	while (isalnum(c=iss.get()))
		prefix += c;
	if (ispunct(c)) // also add seperation character to identifying prefix
		prefix += c;
	if (prefix == "ps:" && iss.peek() == ':')
		prefix += iss.get();
	if (SpecialHandler *handler = findHandler(prefix)) {
		if (listener)
			listener->beginSpecial(prefix.c_str());
		bool ret = handler->process(prefix.c_str(), iss, actions);
		if (listener)
			listener->endSpecial(prefix.c_str());
		return ret;
	}
	return false;
}


void SpecialManager::notifyEndPage () const {
	FORALL(_endPageListeners, HandlerPool::const_iterator, it)
		(*it)->dviEndPage();
}


void SpecialManager::notifyPositionChange (double x, double y) const {
	FORALL(_positionListeners, HandlerPool::const_iterator, it)
		(*it)->dviMovedTo(x, y);
}


void SpecialManager::writeHandlerInfo (ostream &os) const {
	typedef map<string, SpecialHandler*> SortMap;
	SortMap m;
	FORALL(_handlers, ConstIterator, it)
		m[it->second->name()] = it->second;

	FORALL(m, SortMap::iterator, it) {
		os << setw(10) << left << it->second->name() << ' ';
		if (it->second->info())
			os << it->second->info();
		os << endl;
	}
}

