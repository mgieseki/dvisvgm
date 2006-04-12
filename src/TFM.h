/***********************************************************************
** TFM.h                                                              **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2006 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id: TFM.h,v 1.8 2006/01/05 16:05:06 mgieseki Exp $

#ifndef TFM_H
#define TFM_H

#include <istream>
#include <vector>
#include "types.h"

class FileFinder;

class TFM
{
   public:
		TFM ();
		TFM (std::istream &is);
		static TFM* createFromFile (const char *fname, FileFinder *ff=0);
		static void setMetafontMag (double m) {mag = m;}
		UInt16 getChecksum () const        {return checksum;}
		double getDesignSize () const;
		double getCharWidth (int c) const;
		double getCharHeight (int c) const;
		double getCharDepth (int c) const;
		
	protected:
		bool readFromStream (std::istream &is);

   private:
		UInt16 checksum;
		UInt16 firstChar, lastChar;		
		FixWord designSize; // design size of the font in TeX points (7227 pt = 254 cm)
		std::vector<UInt32>  charInfoTable; // 
		std::vector<FixWord> widthTable;    // character widths in design size units
		std::vector<FixWord> heightTable;   // character widths in design size units
		std::vector<FixWord> depthTable;    // character widths in design size units
		static double mag;  // magnification used when Metafont is called
};

#endif
