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

#include <string>
#include <ostream>

class MessageStream
{
	public:
		MessageStream () : _os(0), _nl(false) {}
		MessageStream (std::ostream &os) : _os(&os), _nl(false) {}

		template <typename T>
		MessageStream& operator << (const T &obj) {
			if (_os)
				(*_os) << obj;
			return *this;
		}

		MessageStream& operator << (const char *str);
		MessageStream& operator << (const char &c);
		MessageStream& operator << (const std::string &str) {return (*this) << str.c_str();}

	private:
		std::ostream *_os;
		bool _nl;
};



struct Message
{
	static MessageStream& mstream (bool prefix=false);
	static MessageStream& estream (bool prefix=false);
	static MessageStream& wstream (bool prefix=false);

	enum {ERRORS=1, WARNINGS=2, MESSAGES=4};
	static int level;
};

#endif
