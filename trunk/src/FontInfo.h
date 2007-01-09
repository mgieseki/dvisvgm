/***********************************************************************
** FontInfo.h                                                         **
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

#ifndef FONTINFO_H
#define FONTINFO_H

#include <set>
#include <string>
#include "TFM.h"
#include "types.h"

using std::set;
using std::string;

class FileFinder;
class FontEmitter;

class FontInfo
{
   public:
      FontInfo (string n, UInt32 cs, double ds, double sc);
		~FontInfo ();
		void readTFM (FileFinder *fileFinder);
		UInt32 getChecksum () const          {return checksum;}
		double charWidth (int c) const       {return tfm ? tfm->getCharWidth(c) : 0;}
		double charHeight (int c) const      {return tfm ? tfm->getCharHeight(c) : 0;}
		double charDepth (int c) const       {return tfm ? tfm->getCharDepth(c) : 0;}
		const string& getFontName () const   {return fontname;}
		const TFM* getTFM () const           {return tfm;}
		double getDesignSize () const        {return designSize;}
		double getScaledSize () const        {return scaledSize;}
		double scaleFactor () const          {return scaledSize/designSize;}		

   private:
		string fontname;
		UInt32 checksum;
		double designSize;
		double scaledSize;
		TFM *tfm;
};

#endif

