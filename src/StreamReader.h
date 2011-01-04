/*************************************************************************
** StreamReader.h                                                       **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2011 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef STREAMREADER_H
#define STREAMREADER_H

#include <istream>
#include <string>
#include <vector>
#include "MessageException.h"
#include "types.h"


class StreamReader
{
   public:
		StreamReader (std::istream &s);
		virtual ~StreamReader () {}
		std::istream& replaceStream (std::istream &s);
		UInt32 readUnsigned (int n);
		Int32 readSigned (int n);
		std::string readString (int length);
		std::vector<UInt8>& readBytes (int n, std::vector<UInt8> &bytes);
		int readByte ()     {return is->get();}

	protected:
		std::istream& in () {return *is;}

   private:
		std::istream *is;
};


struct StreamReaderException : public MessageException
{
	StreamReaderException (const std::string &msg) : MessageException(msg) {}
};

#endif
