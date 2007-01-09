/***********************************************************************
** PSState.h                                                          **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id$

#ifndef PSSTATE_H
#define PSSTATE_H

#include <list>
#include <string>
#include "macros.h"

using std::list;
using std::string;


class PSObject;


struct PSException
{
	PSException (string msg, unsigned r=0, unsigned col=0) : message(msg), row(r), col(c) {}
	string message;
	unsigned row, col;
};


class PSOpStack
{
	public:		
		~PSOpStack ()             {deleteObjects();}		
		void push (PSObject *obj) {container.push_back(obj);}
		bool empty () const       {return container.empty();}
		void deleteObjects ();

		template <typename T>
		T* top () const {
			if (container.empty())
				throw PSException("stack underflow");
			
			T *obj = dynamic_cast<T*>(container.back());
			if (obj) 
				return obj;
			throw PSException("invalid operand type");
		}

		template <typename T>
		T* pop (PSObject *pso=0) {
			try {
				T *obj = top<T>();
				container.pop_back();
				return obj;
			}
			catch (PSException &e) {
				if (pso)
					push(pso);
				throw e;
			}
		}
	
	private:
		list<PSObject*> container;
};


struct PSState
{
	PSOpStack opStack;
};

#endif
