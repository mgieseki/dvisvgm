/*************************************************************************
** FontEncoding.h                                                       **
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

#ifndef FONTENCODING_H
#define FONTENCODING_H

#include <istream>
#include <map>
#include <string>
#include <vector>
#include "types.h"


class FontEncoding
{
   public:
      FontEncoding (const std::string &name);
		void read ();
		void read (std::istream &is);
		int size () const                  {return _table.size();}
		std::string name () const          {return _encname;}
		const char* getEntry (int c) const;
		const char* path () const;
		static FontEncoding* encoding (const std::string &fontname);

   private:
		std::string _encname;
		std::vector<std::string> _table;
};

#endif
