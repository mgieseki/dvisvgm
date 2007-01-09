/***********************************************************************
** DVIBBoxReader.cpp                                                  **
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

#include "DVIBBoxReader.h"
#include "FontInfo.h"

using namespace std;


DVIBBoxReader::DVIBBoxReader (istream &is) : DVIReader(is) {
}


void DVIBBoxReader::setChar (double x, double y, unsigned c) {
	if (const FontInfo* fi = getFontInfo()) {
		double w = fi->charWidth(c);
		double h = fi->charHeight(c);
		double d = fi->charDepth(c);
		BoundingBox charbox(getXPos(), getYPos()-h, getXPos()+w, getYPos()+d);
		bbox.embed(charbox);
	}
}


void DVIBBoxReader::setRule (double x, double y, double height, double width) {
	BoundingBox rect(getXPos(), getYPos()+height, getXPos()+width, getYPos());
	bbox.embed(rect);
}

