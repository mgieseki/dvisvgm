/***********************************************************************
** InputBuffer.h                                                      **
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
// $Id$

#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

#include <algorithm>
#include <istream>
#include <iostream>
#include <string>
#include <ostream>

using std::istream;
using std::string;


struct InputBuffer
{
	virtual ~InputBuffer () {}
	virtual int pos () const =0;
	virtual char get () =0;
	virtual char peek () const =0;
	virtual char peek (int n) const =0;
	virtual bool check (char c) const {return peek() == c;}
	virtual bool check (const char *s, bool consume=true) =0;
	virtual void skip (unsigned n) =0;
	virtual bool skipUntil (const char *s, bool consume=true) =0;
	virtual void skipSpace () =0;
	virtual bool eof () const =0;
//	void write (std::ostream &os) const;
};


class StreamInputBuffer : public InputBuffer
{
	public:
		StreamInputBuffer (istream &is, int bufsize=1024);
		~StreamInputBuffer ();
		int pos () const {return _bufptr-_buf1;}
		int line () const {return _line;}
		int col () const  {return _col;}
		char get ();
		char peek () const;
		char peek (int n) const;
		bool check (char c) const {return peek() == c;}
		bool check (const char *s, bool consume=true);
		void skip (unsigned n);
		bool skipUntil (const char *s, bool consume=true);
		void skipSpace ();
		bool eof () const {return *_bufptr == 0 && *_buf2 == 0;}
		void write (std::ostream &os) const;

	protected:
		void fillBuffer (char *buf);

	private:
		istream &_is;
		const int _bufsize;
		char *_buf1;
		char *_buf2;
		char *_bufptr;
		int _line, _col;
};


class StringInputBuffer : public InputBuffer
{
	public:
		StringInputBuffer (string &str);
		int pos () const        {return _pos;}
		char get ()             {return _pos < _str.length() ? _str[_pos++] : 0;}
		char peek () const      {return _pos < _str.length() ? _str[_pos] : 0;}
		char peek (int n) const {return _pos+n < _str.length() ? _str[_pos+n] : 0;} 
		bool check (const char *s, bool consume=true);
		void skip (unsigned n)  {_pos += std::min(n, _str.length()-_pos);}
		bool skipUntil (const char *s, bool consume=true);
		void skipSpace ();
		bool eof () const       {return _pos >= _str.length();}

	private:
		string &_str;
		size_t _pos;
};

#endif
