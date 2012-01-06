/*************************************************************************
** FontMap.h                                                            **
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

#ifndef FONTMAP_H
#define FONTMAP_H

#include <cstring>
#include <istream>
#include <map>
#include <ostream>
#include <string>


class FontMap
{
	struct MapEntry
	{
		std::string fontname; ///< target font name
		std::string encname;  ///< name of font encoding
	};

	typedef std::map<std::string,MapEntry>::const_iterator ConstIterator;

   public:
		FontMap () {}
		FontMap (const std::string &fname);
      FontMap (std::istream &is);
		bool read (const std::string &fname);
		void readdir (const std::string &dirname);
		void clear ()    {_fontMap.clear();}
		std::ostream& write (std::ostream &os) const;
		const char* lookup(const std::string &fontname) const;
		const char* encoding (const std::string &fontname) const;

	protected:
		void readPsMap (std::istream &is);
		void readPdfMap (std::istream &is);

   private:
		std::map<std::string,MapEntry> _fontMap;
};

#endif
