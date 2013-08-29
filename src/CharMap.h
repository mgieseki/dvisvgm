/*************************************************************************
** CharMap.h                                                            **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef CHARMAP_H
#define CHARMAP_H

#include <utility>
#include <vector>
#include "types.h"

class FontEngine;

class CharMap
{
	friend class FontEngine;
	typedef std::pair<UInt32, UInt32> UInt32Pair;
	public:
		CharMap () {}
		bool valueExists (UInt32 c) const;
		UInt32 operator [] (UInt32 c) const;
		UInt32 size () const {return _pairs.size();}
		bool empty () const  {return _pairs.empty();}
		void clear ()        {return _pairs.clear();}
		void invert ();

	protected:
		void append (UInt32 from, UInt32 to) {_pairs.push_back(UInt32Pair(from, to));}
		void sort ();

	private:
		std::vector<UInt32Pair> _pairs;
};

#endif
