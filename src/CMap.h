/*************************************************************************
** CMap.h                                                               **
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

#ifndef CMAP_H
#define CMAP_H

#include <algorithm>
#include <ostream>
#include <vector>
#include "types.h"


struct CMap
{
	virtual ~CMap () {}
	virtual UInt32 decode (UInt32 c) const =0;
	virtual bool vertical () const=0;
};


struct IdentityCMap : public CMap
{
	UInt32 decode (UInt32 c) const {return c;}
};


struct IdentityHCMap : public IdentityCMap
{
	bool vertical () const {return false;}
};


struct IdentityVCMap : public IdentityCMap
{
	bool vertical () const {return true;}
};


class SegmentedCMap : public CMap
{
	friend class CMapReader;

	class Range {
		friend class SegmentedCMap;

		public:
			Range () : _min(0), _max(0), _cid(0) {}

			Range (UInt32 min, UInt32 max, UInt32 cid) : _min(min), _max(max), _cid(cid) {
				if (_min > _max)
					std::swap(_min, _max);
			}

			UInt32 min () const            {return _min;}
			UInt32 max () const            {return _max;}
			UInt32 cid () const            {return _cid;}
			UInt32 decode (UInt32 c) const {return c-_min+_cid;}
			bool operator < (const Range &r) const {return _min < r._min;}

		protected:
			bool join (const Range &r);
			void setMinAndAdaptCID (UInt32 c) {_cid = decode(c); _min = c;}

		private:
			UInt32 _min, _max;
			UInt32 _cid;
	};

	typedef std::vector<Range> Ranges;

   public:
      SegmentedCMap (const std::string &name) : _name(name), _basemap(0), _vertical(false) {}
		const std::string& name () const {return _name;}
		UInt32 decode (UInt32 c) const;
		void addRange (UInt32 first, UInt32 last, UInt32 cid);
		void write (std::ostream &os) const;
		bool vertical () const    {return _vertical;}
		size_t numRanges () const {return _ranges.size();}

	protected:
		void adaptNeighbors (Ranges::iterator it);
		int lookup (UInt32 c) const;
		int lowerBound (UInt32 c) const;

   private:
		std::string _name;
		CMap *_basemap;
		bool _vertical;
		Ranges _ranges;
};

#endif
