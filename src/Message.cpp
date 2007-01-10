/***********************************************************************
** Message.cpp                                                        **
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

#include <cstdarg>
#include <iostream>
#include "Message.h"

using namespace std;


struct NullStreamBuf : public streambuf
{
	streambuf::int_type overflow (int c)  {return 0;}
	streambuf::int_type underflow ()      {return traits_type::eof();}
	int sync ()                           {return traits_type::eof();}
};


struct NullStream : public ostream
{
	NullStream () : ostream(new NullStreamBuf) {}
	~NullStream () {delete rdbuf();}
};


static NullStream nullStream;

// maximal verbosity
int Message::level = Message::MESSAGES | Message::WARNINGS | Message::ERRORS;


/** Returns the stream for usual messages. */
ostream& Message::mstream (bool prefix) {
	ostream *os = (level & MESSAGES) ? &cerr : &nullStream;
	if (prefix)
		*os << "MESSAGE: ";
	return *os;
}


/** Returns the stream for warning messages. */
ostream& Message::wstream (bool prefix) {
	ostream *os = (level & WARNINGS) ? &cerr : &nullStream;
	if (prefix)
		*os << "WARNING: ";
	return *os;
}


/** Returns the stream for error messages. */
ostream& Message::estream (bool prefix) {
	ostream *os = (level & ERRORS) ? &cerr : &nullStream;
	if (prefix)
		*os << "ERROR: ";
	return *os;
}