/*************************************************************************
** FontMap.h                                                            **
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

#ifndef FONTMAP_H
#define FONTMAP_H

#include <cstring>
#include <istream>
#include <map>
#include <ostream>
#include <string>

class MapLine;

class FontMap
{
	struct MapEntry
	{
		MapEntry () : locked(false) {}
		MapEntry (const std::string &fname, const std::string &ename) : fontname(fname), encname(ename), locked(false) {}
		std::string fontname; ///< target font name
		std::string encname;  ///< name of font encoding
		bool locked;
	};

	typedef std::map<std::string,MapEntry>::iterator Iterator;
	typedef std::map<std::string,MapEntry>::const_iterator ConstIterator;

	public:
		enum Mode {FM_APPEND, FM_REMOVE, FM_REPLACE};

		static FontMap& instance ();
		bool read (const std::string &fname, Mode mode=FM_REPLACE);
		bool read (const std::string &fname, char modechar);
		void readdir (const std::string &dirname);
		bool apply (const MapLine &mapline, Mode mode);
		bool apply (const MapLine &mapline, char modechar);
		bool append (const MapLine &mapline);
		bool replace (const MapLine &mapline);
		bool remove (const MapLine &mapline);
		void lockFont (const std::string &fontname);
		void clear ()    {_fontMap.clear();}
		std::ostream& write (std::ostream &os) const;
		const char* lookup(const std::string &fontname) const;
		const char* encoding (const std::string &fontname) const;

	protected:
		FontMap () {}

   private:
		std::map<std::string,MapEntry> _fontMap;
};

#endif
