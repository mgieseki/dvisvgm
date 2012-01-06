/*************************************************************************
** PageRanges.h                                                         **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef PAGERANGES_H
#define PAGERANGES_H

#include <list>
#include <string>
#include <utility>

class PageRanges
{
	public:
		typedef std::pair<int,int> Range;
		typedef std::list<Range> Container;
		typedef Container::iterator Iterator;
		typedef Container::const_iterator ConstIterator;

   public:
		bool parse (std::string str, int max_page=0);
		void insert (int first, int last);
		size_t pages () const;
		size_t size () const             {return _ranges.size();}
		const Container& ranges () const {return _ranges;}
		ConstIterator begin () const     {return _ranges.begin();}
		ConstIterator end () const       {return _ranges.end();}

   private:
		Container _ranges;
		int _max;
};

#endif
