/***********************************************************************
** FontMap.h                                                          **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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

#ifndef FONTMAP_H
#define FONTMAP_H

#include <cstring>
#include <istream>
#include <map>
#include <ostream>
#include <string>

using std::istream;
using std::map;
using std::ostream;

class FontMap
{
	struct MapEntry 
	{
		std::string fontname; ///< target font name
		std::string encname;  ///< name of font encoding
	};

	typedef map<std::string,MapEntry>::const_iterator ConstIterator;

   public:
//      FontMap (const string &fname, bool dir=false);
		FontMap () {}     
      FontMap (istream &is);
		void read (istream &is);
		void readdir (const std::string &dirname);
		void clear ()    {_fontMap.clear();}
		ostream& write (ostream &os) const;
//		bool readMapFile (const string &fname);
//		bool readMapDir (const string &dirname);
		const char* lookup(const std::string &fontname) const;
		const char* encoding (const std::string &fontname) const;

   private:
		map<std::string,MapEntry> _fontMap;
};

#endif
