/*************************************************************************
** MapLine.h                                                            **
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

#ifndef MAPLINE_H
#define MAPLINE_H

#include <istream>
#include <string>
#include "MessageException.h"

class InputReader;


struct MapLineException : MessageException
{
   MapLineException (const std::string &msg) : MessageException(msg) {}
};


class MapLine
{
   public:
      MapLine (std::istream &is);
		const std::string& texname () const   {return _texname;}
		const std::string& psname () const    {return _psname;}
		const std::string& fontfname () const {return _fontname;}
		const std::string& encname () const   {return _encname;}
      const std::string& sfname () const    {return _sfname;}
      int sfindex () const                  {return _sfindex;}
		double slant () const                 {return _slant;}
		double extend () const                {return _extend;}
      bool embed () const                   {return _embed;}

   protected:
      void init ();
      bool isDVIPSFormat (const char *line) const;
		void parse (const char *line);
      void parseDVIPSLine (InputReader &ir);
      void parseDVIPDFMLine (InputReader &ir);
      void parseFilenameOptions (std::string opt);

   private:
      std::string _texname;     ///< TeX font name
      std::string _psname;      ///< PS font name
      std::string _fontname;    ///< name of fontfile
      std::string _encname;     ///< name of encoding (without file suffix ".enc")
      std::string _sfname;      ///< subfont name
      int _sfindex;             ///< subfont index
      double _slant, _extend;
      bool _embed;
};


#endif

