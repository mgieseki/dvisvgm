/*************************************************************************
** Message.h                                                            **
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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <algorithm>
#include <string>
#include <ostream>
#include <sstream>
#include "Terminal.h"


class Message;

class MessageStream
{
	friend class Message;
	public:
		MessageStream () : _os(0), _nl(false) {}
		MessageStream (std::ostream &os);
		~MessageStream ();

		template <typename T>
		MessageStream& operator << (const T &obj) {
			std::ostringstream oss;
			oss << obj;
			(*this) << oss.str();
			return *this;
		}

		MessageStream& operator << (const char *str);
		MessageStream& operator << (const char &c);
		MessageStream& operator << (const std::string &str) {return (*this) << str.c_str();}

		void indent (int level)        {_indent = std::max(0, level*2);}
		void indent (bool reset=false);
		void outdent (bool all=false);
		void clearline ();

	protected:
		void putChar (const char c, std::ostream &os);
		std::ostream* os () {return _os;}

	private:
		std::ostream *_os;
		bool _nl;     ///< true if previous character was a newline
		int _col;     ///< current terminal column
		int _indent;  ///< indentation width (number of columns/characters)
};


struct Message
{
	static MessageStream& mstream (bool prefix=false, int color=Terminal::DEFAULT, bool light=false);
	static MessageStream& estream (bool prefix=false);
	static MessageStream& wstream (bool prefix=false);

	enum {ERRORS=1, WARNINGS=2, MESSAGES=4};
	static int LEVEL;
	static bool COLORIZE;
};

#endif
