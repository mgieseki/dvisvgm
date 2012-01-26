/*************************************************************************
** TFM.h                                                                **
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

#ifndef TFM_H
#define TFM_H

#include <istream>
#include <vector>
#include "types.h"

struct FileFinder;

class TFM
{
   public:
		TFM ();
		TFM (std::istream &is);
		static TFM* createFromFile (const char *fname);
		double getDesignSize () const;
		double getCharWidth (int c) const;
		double getCharHeight (int c) const;
		double getCharDepth (int c) const;
		double getItalicCorr (int c) const;
		UInt32 getChecksum () const {return _checksum;}
		UInt16 firstChar () const   {return _firstChar;}
		UInt16 lastChar () const    {return _lastChar;}

	protected:
		bool readFromStream (std::istream &is);

   private:
		UInt32 _checksum;
		UInt16 _firstChar, _lastChar;
		FixWord _designSize;  ///< design size of the font in TeX points (7227 pt = 254 cm)
		std::vector<UInt32>  _charInfoTable;
		std::vector<FixWord> _widthTable;    ///< character widths in design size units
		std::vector<FixWord> _heightTable;   ///< character height in design size units
		std::vector<FixWord> _depthTable;    ///< character depth in design size units
		std::vector<FixWord> _italicTable;   ///< italic corrections in design size units
};

#endif
