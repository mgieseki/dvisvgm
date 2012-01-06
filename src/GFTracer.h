/*************************************************************************
** GFTracer.h                                                           **
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

#ifndef GFTRACER_H
#define GFTRACER_H

#include <istream>
#include "GFReader.h"

class GFTracer : public GFReader
{
   public:
      GFTracer (std::istream &is);
      GFTracer (std::istream &is, double upp);
      virtual ~GFTracer () {}
		virtual void moveTo (double x, double y) {}
		virtual void lineTo (double x, double y) {}
		virtual void curveTo (double c1x, double c1y, double c2x, double c2y, double x, double y) {}
		virtual void closePath () {}
		void beginChar (UInt32 c);
		void endChar (UInt32 c);

	protected:
		void unitsPerPoint(double upp) {_unitsPerPoint = upp;}

	private:
		double _unitsPerPoint; ///< target units per TeX point
};

#endif
