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

#include <cmath>
#include <cstring>
#include "InputBuffer.h"
#include "debug.h"

using namespace std;


/** Skips n characters. */
void InputBuffer::skip (unsigned n) {
	while (n-- > 0) 
		get();
}


/** Moves the buffer pointer to the next non-space character. A following call
 *  of get() returns this character. */
void InputBuffer::skipSpace () {
	while (isspace(peek()))
		get();
}


/** Tries to find a given string and skips all characters preceding that string. 
 *  @param[in] s string to look for (must not be longer than the maximal buffer size)
 *  @param[in] consume if true, the buffer pointer is moved to the first charater following string s
 *  @return true if s was found */
bool InputBuffer::skipUntil (const char *s, bool consume) {
	bool found;
	while (!eof() && !(found = check(s, consume)))
		get();
	return found;
}


/** Checks if the next characters to be read match a given string. 
 *  @param[in] s string to be matched 
 *  @param[in] consume if true, the characters of the matched string are skipped 
 *  @return true if s matches */
bool InputBuffer::check (const char *s, bool consume) {
	size_t count = 0;
	for (const char *p=s; *p; p++) {
		if (peek(count++) != *p)
			return false;
	}
	if (consume)
		skip(count);
	return true;
}


int InputBuffer::compare (const char *s, bool consume) {
	size_t count = 0;
	for (const char *p=s; *p; p++) {
		int c = peek(count++);
		if (c != *p)
			return *p < c ? -1 : 1;
	}
	if (consume)
		skip(count);
	return 0;
}


/** Reads an integer from the buffer. All characters that are part of
 *  the read integer constant are skipped. If this function returns false,
 *  the buffer pointer points to the same position as before the function call.
 *  @param[out] val contains the read integer value on success
 *  @param[in] accept_sign if false, only positive integers (without sign) are accepted 
 *  @return true if integer could be read */
bool InputBuffer::parseInt (int &val, bool accept_sign) {
	val = 0;
	int fac=1;
	char sign;    // explicitly given sign
	if (accept_sign && ((sign = peek()) == '+' || sign == '-')) {
		if (isdigit(peek(1))) {
			get();  // skip sign
			if (sign == '-')
				fac = -1;
		}
		else
			return false;
	}
	else if (!isdigit(peek()))
		return false;
	
	while (isdigit(peek()))
		val = val*10 + (get()-'0');
	val *= fac;
	return true;
}


bool InputBuffer::parseInt (int base, int &val) {
	if (base < 2 || base > 32)
		return false;

	const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	const char maxdigit = digits[base-1];
	char c;
	if (!isalnum(c = tolower(peek())) || c > maxdigit)
		return false;

	val = 0;
	while (isalnum(c = tolower(peek())) && c <= maxdigit) {
		get();
		int digit = c - (c <= '9' ? '0' : 'a'-10);
		val = val*base + digit;
	}
	return true;	
}


/** Reads a double from the buffer. All characters that are part of
 *  the read double constant are skipped. If this function returns false,
 *  the buffer pointer points to the same position as before the function call.
 *  @param[out] val contains the read double value on success
 *  @return number details: 0=no number, 'i'=integer, 'f'=floating point number */
char InputBuffer::parseDouble (double &val) {
	int fac=1;
	int int_part=0;
	bool is_float = false;
	if (parseInt(int_part)) { // match [+-]?[0-9]+\.?
		if (peek() == '.') {
			get();
			is_float = true;
		}
		if (int_part < 0) {
			fac = -1;
			int_part = -int_part;
		}
	}
	else {  // match [+-]?\.
		char sign;   // explicitly given sign
		if ((sign = peek()) == '+' || sign == '-') { // match [+-]?\.[0-9]
			if (peek(1) != '.' || !isdigit(peek(2)))
				return 0;
			if (sign == '-')
				fac = -1;
			skip(2);  // skip sign and dot
		}
		else if (peek() == '.' && isdigit(peek(1)))
			get();
		else
			return 0;
		is_float = true;
	}
	// parse fractional part
	double frac_part=0.0;
	for (double u=10; isdigit(peek()); u*=10)
		frac_part += (get()-'0')/u;
	val = (int_part + frac_part) * fac;
	// parse exponent
	char c;
	if (tolower(peek()) == 'e' && (isdigit(c=peek(1)) || ((c == '+' || c == '-') && isdigit(peek(2))))) {
		get(); // skip 'e'
		int exp;
		parseInt(exp);
		val *= pow10(exp);
		is_float = true;
	}
	return is_float ? 'f' : 'i';
}


/** Reads an integer value from the buffer. If no valid integer constant
 *  could be found at the current position 0 is returned. */
int InputBuffer::getInt () {
	skipSpace();
	int val;
	return parseInt(val) ? val : 0;
}


/** Reads an double value from the buffer. If no valid double constant
 *  could be found at the current position 0 is returned. */
double InputBuffer::getDouble () {
	skipSpace();
	double val;
	return parseDouble(val) ? val : 0.0;
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


string InputBuffer::getString () {
	string ret;
	skipSpace();
	while (!eof() && !isspace(peek()))
		ret += get();
	return ret;
}

//////////////////////////////////////////

StreamInputBuffer::StreamInputBuffer (istream &is, size_t bufsize)
	: _is(is), _bufsize(bufsize), _buf1(new UInt8[_bufsize]), _buf2(new UInt8[_bufsize]), _bufptr(_buf1)
{
	_size1 = fillBuffer(_buf1);
	_size2 = fillBuffer(_buf2);
}


StreamInputBuffer::~StreamInputBuffer () {
	delete [] _buf1;
	delete [] _buf2;
}


int StreamInputBuffer::get () {
	if (pos() == _size1) {
		if (_size2 == 0)
			return -1;
		swap(_buf1, _buf2);
		_size1 = _size2;
		_bufptr = _buf1;
		_size2 = fillBuffer(_buf2);
	}
	UInt8 c = *_bufptr++;
	return c;
}


/** Returns the next character to be read without skipping it. 
 *  Same as peek(0). */
int StreamInputBuffer::peek () const {
	if (pos() < _size1)
		return *_bufptr;
	return _size2 > 0 ? *_buf2 : -1;
}


/** Returns the n-th next character without skipping it. */
int StreamInputBuffer::peek (size_t n) const {
	if (pos()+n < _size1)
		return *(_bufptr+n);
	if (pos()+n < _size1+_size2)
		return *(_buf2 + pos()+n-_size1);
	return -1;
}


/** Fills the buffer by reading a sequence of characters from the assigned
 *  input stream. 
 *  @param[in] buf pointer to character buffer to be filled 
 *  @return number of characters read */
int StreamInputBuffer::fillBuffer (UInt8 *buf) {
	if (_is && !_is.eof()) {
		_is.read((char*)buf, _bufsize);
		return  _is.gcount();
	}
	return 0;
}


///////////////////////////////////////////////


bool StringInputBuffer::check (const char *s, bool consume) {
	bool ret = (strcmp(_str.c_str()+_pos, s) == 0);
	if (ret && consume)
		_pos += strlen(s);
	return ret;
}

///////////////////////////////////////////////

bool CharInputBuffer::check (const char *s, bool consume) {
	const char *p1=s, *p2=_pos; 
	size_t n = strlen(s);
	if (n < _size)
		return false;

	while (n-- > 0 && *p1 == *p2) {
		p1++;
		p2++;
	}
	if (consume && n == 0)
		_pos += strlen(s);
	return n == 0;
}

///////////////////////////////////////////////

SplittedCharInputBuffer::SplittedCharInputBuffer (const char *buf1, size_t s1, const char *buf2, size_t s2) {
	_buf[0] = buf1;
	_buf[1] = buf2;
	_size[0] = buf1 ? s1 : 0;
  	_size[1] = buf2 ? s2 : 0;
	_index = _size[0] ? 0 : 1;
}


int SplittedCharInputBuffer::get () {
	if (_size[_index] == 0)
		return -1;
	int ret = *_buf[_index]++;
	_size[_index]--;
	if (_index == 0 && _size[0] == 0)
		_index++;
	return ret;
}


int SplittedCharInputBuffer::peek () const {
	return _size[_index] ? *_buf[_index] : -1;
}


int SplittedCharInputBuffer::peek (size_t n) const {
	if (n < _size[_index])
		return _buf[_index][n];
	n -= _size[_index];
	if (_index == 0 && n < _size[1])
		return _buf[1][n];
	return -1;
}


void SplittedCharInputBuffer::skip (size_t n) {
	if (n < _size[_index]) {
		_size[_index] -= n;
		_buf[_index] += n;
	}
	else if (_index == 0) {
		n -= _size[_index];
		_index++;
		_size[1] -= n;
		_buf[1] += n;
	}
	else
		_size[1] = 0;
}


bool SplittedCharInputBuffer::check (const char *str, bool consume) {
	const char *p1 = _buf[_index];
	const char *p2 = str;
	int index = _index;
	size_t size = _size[index];
	while (size > 0 && *p2 && *p1 == *p2) {
		p1++;
		p2++;
		if (--size == 0 && index == 0) {
			size = _size[1];
			p1 = _buf[1];
			index++;
		}		
	}
	if (consume && *p2 == 0) {
		_index = index;
		_size[index] = size;
		_buf[index] = p1;
	}
	return *p2 == 0;
}
///////////////////////////////////////////////


int TextStreamInputBuffer::get () {
	int c = StreamInputBuffer::get();
	if (c == '\n') {
		_line++;
		_col = 1;
	}
	else
		_col++;
	return c;
}


