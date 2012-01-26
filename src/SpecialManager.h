/*************************************************************************
** SpecialManager.h                                                     **
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

#ifndef SPECIALMANAGER_H
#define SPECIALMANAGER_H

#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "SpecialHandler.h"

struct SpecialActions;

class SpecialManager
{
	public:
		struct Listener
		{
			virtual ~Listener () {}
			virtual void beginSpecial (const char *prefix)=0;
			virtual void endSpecial (const char *prefix)=0;
		};

	private:
		typedef std::vector<SpecialHandler*> HandlerPool;
		typedef std::map<std::string,SpecialHandler*> HandlerMap;
		typedef HandlerMap::iterator Iterator;
		typedef HandlerMap::const_iterator ConstIterator;

   public:
		SpecialManager () {}
      virtual ~SpecialManager ();
		void registerHandler (SpecialHandler *handler);
		void registerHandlers (SpecialHandler **handlers, const char *ignorelist);
		void unregisterHandlers ();
		bool process (const std::string &special, SpecialActions *actions, Listener *listener=0) const;
		void notifyEndPage () const;
		void notifyPositionChange (double x, double y) const;
		void writeHandlerInfo (std::ostream &os) const;

	protected:
		SpecialManager (const SpecialManager &) {}
		void operator = (const SpecialManager &) {}
		SpecialHandler* findHandler (const std::string &prefix) const;

   private:
		HandlerPool _pool;     ///< stores pointers to all handlers
		HandlerMap _handlers;  ///< pointers to handlers for corresponding prefixes
		HandlerPool _endPageListeners;
		HandlerPool _positionListeners;
};

#endif
