/*************************************************************************
** Message.cpp                                                          **
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

#include <cstdarg>
#include <iostream>
#include "Message.h"
#include "macros.h"

using namespace std;


MessageStream& MessageStream::operator << (const char *str) {
	if (_os) {
		for (const char *p=str; *p; ++p) {
			if (!_nl || *p != '\n') {
				(*_os) << *p;
				_nl = (*p == '\n');
			}
		}
	}
	return *this;
}


MessageStream& MessageStream::operator << (const char &c) {
	if (_os) {
		if (!_nl || c != '\n') {
			(*_os) << c;
			_nl = (c == '\n');
		}
	}
	return *this;
}


static MessageStream nullStream;
static MessageStream messageStream(cerr);

// maximal verbosity
int Message::level = Message::MESSAGES | Message::WARNINGS | Message::ERRORS;


/** Returns the stream for usual messages. */
MessageStream& Message::mstream (bool prefix) {
	MessageStream *os = (level & MESSAGES) ? &messageStream : &nullStream;
	if (prefix)
		*os << "MESSAGE: ";
	return *os;
}


/** Returns the stream for warning messages. */
MessageStream& Message::wstream (bool prefix) {
	MessageStream *os = (level & MESSAGES) ? &messageStream : &nullStream;
	if (prefix)
		*os << "WARNING: ";
	return *os;
}


/** Returns the stream for error messages. */
MessageStream& Message::estream (bool prefix) {
	MessageStream *os = (level & MESSAGES) ? &messageStream : &nullStream;
	if (prefix)
		*os << "ERROR: ";
	return *os;
}

