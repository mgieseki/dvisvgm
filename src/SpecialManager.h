/***********************************************************************
** SpecialManager.h                                                   **
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

#ifndef SPECIALMANAGER_H
#define SPECIALMANAGER_H

#include <map>
#include <ostream>
#include <string>
#include "SpecialHandler.h"

class SpecialActions;


class UnprefixedSpecialHandler : public SpecialHandler
{
	friend class SpecialManager;
	typedef std::list<SpecialHandler*> HandlerList;
	typedef HandlerList::iterator Iterator;
	typedef HandlerList::const_iterator ConstIterator;

	public:
		~UnprefixedSpecialHandler ();
		const char* prefix () const {return 0;}
		const char* info () const   {return 0;}
		const char* name () const   {return 0;}
		void registerHandler (SpecialHandler *handler) {_handlers.push_back(handler);}
		bool process (const char *prefix, std::istream &in, SpecialActions *actions);
		void endPage ();		

	protected:
		UnprefixedSpecialHandler () {}
		const HandlerList& handlers () const {return _handlers;}

	private:
		HandlerList _handlers;
};


class SpecialManager
{
	typedef std::map<string,SpecialHandler*> HandlerMap;
	typedef HandlerMap::iterator Iterator;
	typedef HandlerMap::const_iterator ConstIterator;

   public:
		SpecialManager ();
      ~SpecialManager ();
		void registerHandler (SpecialHandler *handler);
		void registerHandlers (SpecialHandler **handlers, const char *ignorelist);
		bool process (const std::string &special, SpecialActions *actions);
		void notifyEndPage ();
		void writeHandlerInfo (std::ostream &os) const;

	protected:
		SpecialManager (const SpecialManager &) {}
		void operator = (const SpecialManager &) {}
		SpecialHandler* findHandler (const std::string &prefix) const;
		UnprefixedSpecialHandler* unprefixedHandler ();

   private:
		HandlerMap _handlers;
		UnprefixedSpecialHandler *_uphandler;
};

#endif
