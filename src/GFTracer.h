/***********************************************************************
** GFTracer.h                                                         **
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

#ifndef GFTRACER_H
#define GFTRACER_H

#include "FontGlyph.h"
#include "GFReader.h"

class FileFinder;
class TFM;

class GFTracer : public GFReader
{
   public:
      GFTracer (istream &is, double upp);
      ~GFTracer ();
		void beginChar (UInt32 c);
		void endChar (UInt32 c);
		const Glyph& getGlyph () const {return glyph;}
	private:
		Glyph glyph;
		double unitsPerPoint; // 
};

#endif
