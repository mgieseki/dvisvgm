/***********************************************************************
** DVIBBoxActions.h                                                   **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#include "BoundingBox.h"
#include "DVIActions.h"
#include "Font.h"

#ifndef DVIBBOXACTIONS_H
#define DVIBBOXACTIONS_H

class DVIBBoxActions : public DVIActions
{
   public:
      DVIBBoxActions (BoundingBox &bb) : bbox(bb) {}
		
		void setChar (double x, double y, unsigned c, const Font *font) {
			if (font) {
			/*	x *= BP;
				y *= BP;*/
				double s = font->scaleFactor(); // * BP;
				double w = s*font->charWidth(c);
				double h = s*font->charHeight(c);
				double d = s*font->charDepth(c);
				BoundingBox charbox(x, y-h, x+w, y+d);
				bbox.embed(charbox);
			}
		}
		
		void setRule (double x, double y, double height, double width) {
/*			x *= BP; // TeX points to PostScript points ("big points")
			y *= BP;
			height *= BP;
			width  *= BP; */
			BoundingBox rect(x, y+height, x+width, y);
			bbox.embed(rect);
		}
		
		const BoundingBox& getBoundingBox () const {return bbox;}

   private:
		BoundingBox &bbox;
};

#endif
