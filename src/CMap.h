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
#include "FontEncoding.h"
#include "types.h"


struct CMap : public NamedFontEncoding
{
	virtual ~CMap () {}
	virtual bool vertical () const =0;
	virtual bool mapsToCID () const =0;
	virtual const char* path () const;
	virtual UInt32 cid (UInt32 c) const =0;
	virtual UInt32 bfcode (UInt32 cid) const =0;
	virtual std::string getROString () const =0;
	virtual const FontEncoding* findCompatibleBaseFontMap (const PhysicalFont *font, CharMapID &charmapID) const;
	virtual bool mapsToCharIndex () const {return mapsToCID();}

	Character decode (UInt32 c) const {
		if (mapsToCID())
			return Character(Character::INDEX, cid(c));
		return Character(Character::CHRCODE, bfcode(c));
	}
};


struct IdentityCMap : public CMap
{
	UInt32 cid (UInt32 c) const         {return c;}
	UInt32 bfcode (UInt32 cid) const    {return 0;}
	std::string getROString () const    {return "Adobe-Identity";}
	bool mapsToCID() const              {return true;}
};


struct IdentityHCMap : public IdentityCMap
{
	bool vertical () const    {return false;}
	const char* name () const {return "Identity-H";}
};


struct IdentityVCMap : public IdentityCMap
{
	bool vertical () const    {return true;}
	const char* name () const {return "Identity-V";}
};


struct UnicodeCMap : public CMap
{
	bool vertical () const           {return false;}
	const char* name () const        {return "unicode";}
	bool mapsToCID () const          {return false;}
	const char* path () const        {return 0;}
	UInt32 cid (UInt32 c) const      {return c;}
	UInt32 bfcode (UInt32 cid) const {return cid;}
	std::string getROString () const {return "";}
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
		SegmentedCMap (const std::string &name) : _name(name), _basemap(0), _vertical(false), _mapsToCID(true) {}
		const char* name () const {return _name.c_str();}
		UInt32 cid (UInt32 c) const;
		UInt32 bfcode (UInt32 cid) const;
		void addCIDRange (UInt32 first, UInt32 last, UInt32 cid)    {addRange(_cidranges, first, last, cid);}
		void addBFRange (UInt32 first, UInt32 last, UInt32 chrcode) {addRange(_bfranges, first, last, chrcode);}
		void write (std::ostream &os) const;
		bool vertical () const        {return _vertical;}
		bool mapsToCID () const       {return _mapsToCID;}
		size_t numCIDRanges () const  {return _cidranges.size();}
		size_t numBFRanges () const   {return _bfranges.size();}
		std::string getROString () const;

	protected:
		void addRange (Ranges &ranges, UInt32 first, UInt32 last, UInt32 cid);
		void adaptNeighbors (Ranges &ranges, Ranges::iterator it);
		int lookup (const Ranges &ranges, UInt32 c) const;

   private:
		std::string _name;
		std::string _registry;
		std::string _ordering;
		CMap *_basemap;
		bool _vertical;
		bool _mapsToCID;   // true: chrcode->CID, false: CID->charcode
		Ranges _cidranges;
		Ranges _bfranges;
};

#endif
