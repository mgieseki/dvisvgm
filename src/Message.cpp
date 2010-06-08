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
#include <cstring>
#include <iostream>
#include "Message.h"
#include "Terminal.h"
#include "macros.h"

using namespace std;

MessageStream::MessageStream (std::ostream &os)
	: _os(&os), _nl(true), _col(1), _indent(0)
{
	Terminal::init(os);
}


MessageStream::~MessageStream () {
	if (_os)
		Terminal::finish(*_os);
}


void MessageStream::putchar (const char c, ostream &os) {
	switch (c) {
		case '\r':
			os << '\r';
			_nl = true;
			_col = 1;
			return;
		case '\n':
			if (!_nl) {
				_col = 1;
				_nl = true;
				os << '\n';
			}
			return;
		default:
			if (_nl) {
				os << string(_indent, ' ');
				_col += _indent;
			}
			else {
				const int cols = Terminal::columns();
				if (cols > 0 && _col >= cols) {
					os << '\n' << string(_indent, ' ');
					_col = _indent+1;
				}
				else
					_col++;
			}
			_nl = false;
			if (!_nl || c != '\n')
				os << c;
	}
}


MessageStream& MessageStream::operator << (const char *str) {
	if (_os) {
		const int len = strlen(str);
		const int cols = Terminal::columns();
		if (cols > 0 && _col+len > cols && _indent+len <= cols)
			putchar('\n', *_os);
		for (const char *p=str; *p; ++p)
			putchar(*p, *_os);
	}
	return *this;
}


MessageStream& MessageStream::operator << (const char &c) {
	if (_os)
		putchar(c, *_os);
	return *this;
}


void MessageStream::indent (bool reset) {
	if (reset)
		_indent = 0;
	_indent += 2;
}


void MessageStream::outdent (bool all) {
	if (all)
		_indent = 0;
	else if (_indent > 0)
		_indent -= 2;
}


void MessageStream::clearline () {
	if (_os) {
		int cols = Terminal::columns();
		*_os << '\r' << string(cols ? cols-1 : 79, ' ') << '\r';
		_nl = true;
		_col = 1;
	}
}

static MessageStream nullStream;
static MessageStream messageStream(cerr);

// maximal verbosity
int Message::LEVEL = Message::MESSAGES | Message::WARNINGS | Message::ERRORS;
bool Message::COLORIZE = false;


/** Returns the stream for usual messages. */
MessageStream& Message::mstream (bool prefix, int color, bool light) {
	MessageStream *ms = (LEVEL & MESSAGES) ? &messageStream : &nullStream;
	if (COLORIZE && ms && ms->os())
		Terminal::color(color, light, *ms->os());
	if (prefix)
		*ms << "\nMESSAGE: ";
	return *ms;
}


/** Returns the stream for warning messages. */
MessageStream& Message::wstream (bool prefix) {
	MessageStream *ms = (LEVEL & WARNINGS) ? &messageStream : &nullStream;
	if (COLORIZE && ms && ms->os())
		Terminal::color(Terminal::YELLOW, false, *ms->os());
	if (prefix)
		*ms << "\nWARNING: ";
	return *ms;
}


/** Returns the stream for error messages. */
MessageStream& Message::estream (bool prefix) {
	MessageStream *ms = (LEVEL & ERRORS) ? &messageStream : &nullStream;
	if (COLORIZE && ms && ms->os())
		Terminal::color(Terminal::RED, true, *ms->os());
	if (prefix)
		*ms << "\nERROR: ";
	return *ms;
}

