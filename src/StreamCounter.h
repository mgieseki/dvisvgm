/*************************************************************************
** StreamCounter.h                                                      **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef STREAMCOUNTER_H
#define STREAMCOUNTER_H

#include <ostream>
#include "types.h"


/** This class template is used to count the number of characters
 *  written to a stream buffer. It simply counts the characters written 
 *  and forwards them to the target buffer. */
template <typename C, typename T=std::char_traits<C> >
class CountingStreamBuf : public std::basic_streambuf<C,T>
{
   public:
      explicit CountingStreamBuf (std::basic_streambuf<C,T> *b) 
			: targetBuf(b), counter(0) {}
		
		UInt64 count () const {return counter;}
		void reset ()         {counter = 0;}
		
	protected:
		CountingStreamBuf (const CountingStreamBuf &csb);
		CountingStreamBuf& operator = (const CountingStreamBuf &csb);
		
		typename T::int_type overflow (typename T::int_type c=T::eof()) {
			typename T::int_type n = targetBuf->sputc(c);
			if (!T::eq_int_type(c, T::eof()) && !T::eq_int_type(n, T::eof())) 
				counter++;
			return n;
		}
		
		std::streamsize xsputn (const C *buf, std::streamsize bufsize) {
			std::streamsize n = targetBuf->sputn(buf, bufsize);
         counter += n;
         return n; 
		}
		
   private:
		std::basic_streambuf<C,T> *targetBuf;
		UInt64 counter;
};


/** This class template assigns a counting buffer to a stream. It makes
 *  the use of CountingStreamBuf more comfortable. */
template <typename C, typename T=std::char_traits<C> >
class StreamCounter
{
	public:
		explicit StreamCounter (std::basic_ios<C,T> &stream) 
			: targetStream(stream), 
		     targetBuf(stream.rdbuf()), 
			  countingBuf(new CountingStreamBuf<C,T>(stream.rdbuf()))
		{
			stream.rdbuf(countingBuf);
			valid = true;
		}
		
		~StreamCounter () {
			if (valid)
				targetStream.rdbuf(targetBuf);
			delete countingBuf;
		}

		UInt64 count () const {return countingBuf->count();}
		void reset ()         {countingBuf->reset();}
		void invalidate ()    {valid = false;}
	
	protected:
		StreamCounter (const StreamCounter &csf);
		StreamCounter operator = (const StreamCounter &csf);
			
	private:
		bool                      valid; ///< true if target stream is valid
		std::basic_ios<C,T>       &targetStream;
		std::basic_streambuf<C,T> *targetBuf;
		CountingStreamBuf<C,T>    *countingBuf;
};

#endif
