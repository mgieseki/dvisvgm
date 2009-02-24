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

#include <iomanip>
#include <sstream>
#include "SpecialManager.h"
#include "macros.h"

using namespace std;


UnprefixedSpecialHandler::~UnprefixedSpecialHandler () {
	FORALL(_handlers, Iterator, it)
		delete *it;
}


bool UnprefixedSpecialHandler::process (const char *prefix, istream &in, SpecialActions *actions) {
	bool processed=false;
	for (Iterator it=_handlers.begin(); it != _handlers.end() && !processed; ++it)
		processed = (*it)->process(prefix, in, actions);
	return processed;
}


void UnprefixedSpecialHandler::endPage () {
	FORALL(_handlers, Iterator, it)
		(*it)->endPage();
}


//////////////////////////////////////////////////////////////////////////


SpecialManager::SpecialManager () : _uphandler(0)
{
}


SpecialManager::~SpecialManager () {
	delete _uphandler;
	FORALL(_handlers, Iterator, it)
		delete it->second;
}


/** Registers a single special handler.
 *  @param[in] pointer to handler to be registered */
void SpecialManager::registerHandler (SpecialHandler *handler) {
	if (handler) {
		if (handler->prefix()) {
			delete findHandler(handler->prefix());
			_handlers[handler->prefix()] = handler;
		}
		else 
			unprefixedHandler()->registerHandler(handler);
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
 *  @return true if a special handler was found 
 *  @throw SpecialException in case of errors during special processing */
bool SpecialManager::process (const string &special, SpecialActions *actions) {
	istringstream iss(special);
	string prefix;
	int c;
	while (isalnum(c=iss.get()))
		prefix += c;
	if (ispunct(c)) // also add seperation character to identifying prefix
		prefix += c;
	if (SpecialHandler *handler = findHandler(prefix))
		return handler->process(prefix.c_str(), iss, actions);
	return unprefixedHandler()->process(prefix.c_str(), iss, actions);
}


void SpecialManager::notifyEndPage () {
	FORALL(_handlers, Iterator, it)
		it->second->endPage();
}


void SpecialManager::writeHandlerInfo (ostream &os) const {
	typedef map<string, SpecialHandler*> SortMap;
	SortMap m;
	FORALL(_handlers, ConstIterator, it)
		m[it->second->name()] = it->second;
	FORALL(unprefixedHandler()->handlers(), list<SpecialHandler*>::const_iterator, it)
		m[(*it)->name()] = *it;	
	
	FORALL(m, SortMap::iterator, it) {
		os << setw(10) << left << it->second->name() << ' ';
		if (it->second->info())
			os << it->second->info();
		os << endl;
	}
}


UnprefixedSpecialHandler* SpecialManager::unprefixedHandler () const {
	if (!_uphandler)
		_uphandler = new UnprefixedSpecialHandler;
	return _uphandler;
}
