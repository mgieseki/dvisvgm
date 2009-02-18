/***********************************************************************
** InputBuffer.cpp                                                    **
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

#include <cstring>
#include "InputBuffer.h"

using namespace std;

int InputBuffer::getInt () {
	int ret=0;
	int fac=1;
	skipSpace();
	if (peek() == '-') {
		fac = -1;
		get();
	}
	while (isdigit(peek()))
		ret = ret*10 + (get()-'0');
	return fac*ret;
}


double InputBuffer::getDouble () {
	double ret=getInt();
	if (peek() == '.') {
		get();
		double frac=0;
		for (double u=10; isdigit(peek()); u*=10) 
			frac += (get()-'0')/u;
		ret += frac;
	}
	return ret;
}


string InputBuffer::getWord () {
	string ret;
	skipSpace();
	while (isalpha(peek()))
		ret += get();
	return ret;
}

char InputBuffer::getPunct () {
	skipSpace();
	if (ispunct(peek()))
		return get();
	return 0;
}

//////////////////////////////////////////

StreamInputBuffer::StreamInputBuffer (istream &is, int bufsize)
	: _is(is), _bufsize(bufsize), _buf1(new char[_bufsize]), _buf2(new char[_bufsize]), _bufptr(_buf1), _line(1), _col(1)
{
	fillBuffer(_buf1);
	fillBuffer(_buf2);
}


StreamInputBuffer::~StreamInputBuffer () {
	delete [] _buf1;
	delete [] _buf2;
}


char StreamInputBuffer::get () {
	if (*_bufptr == 0) {
		if (*_buf2 == 0)
			return 0;
		swap(_buf1, _buf2);
		_bufptr = _buf1;
		fillBuffer(_buf2);
	}
	char c = *_bufptr++;
	if (c == '\n') {
		_line++;
		_col = 1;
	}
	else
		_col++;
	return c;
}


char StreamInputBuffer::peek () const {
	if (*_bufptr)
		return *_bufptr;
	return *_buf2;
}


char StreamInputBuffer::peek (int n) const {
	if (pos()+n < _bufsize-1)
		return *(_bufptr+n);
	return *(_buf2 + pos()+n-_bufsize+1);
}


bool StreamInputBuffer::check (const char *s, bool consume) {
	const char *bp = _bufptr;
	for (const char *p=s; *p; p++) {
		if (*bp == 0)
			bp = _buf2;
		if (*bp++ != *p)
			return false;
	}
	if (consume)
		skip(strlen(s));
	return true;
}


void StreamInputBuffer::skip (unsigned n) {
	while (n-- > 0) 
		get();
}


bool StreamInputBuffer::skipUntil (const char *s, bool consume) {
	bool found;
	while (!eof() && !(found = check(s, consume)))
		get();
	return found;
}


void StreamInputBuffer::skipSpace () {
	while (isspace(peek()))
		get();
}


void StreamInputBuffer::fillBuffer (char *buf) {
	streamsize ofs=0;
	if (_is && !_is.eof()) {
		_is.read(buf, _bufsize-1);
		ofs = _is.gcount();
	}
	memset(buf+ofs, 0, _bufsize-ofs);
}


///////////////////////////////////////////////


bool StringInputBuffer::check (const char *s, bool consume) {
	bool ret = (strcmp(_str.c_str()+_pos, s) == 0);
	if (ret && consume)
		_pos += strlen(s);
	return ret;
}

		
bool StringInputBuffer::skipUntil (const char *s, bool consume) {
	bool found;
	while (!eof() && !(found = check(s, consume)))
		get();
	return found;
}


void StringInputBuffer::skipSpace () {
	while (isspace(peek()))
		get();
}


#if 0
#include <fstream>
int main (int argc, char **argv) {
	if (argc > 1) {
		ifstream ifs(argv[1]);
		StreamInputBuffer b(ifs);
		int count = 0;
		
		bool ok = true;		
		while (ok) {
			ok = b.skipUntil("<!--", true);
			while (ok && !b.check("-->", true))
				cout << b.get();
		}
	}
	return 0;
}

#endif
