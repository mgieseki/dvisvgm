/***********************************************************************
** DVIBBoxReader.h                                                    **
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
// $Id: DVIBBoxReader.h,v 1.2 2006/01/05 16:05:05 mgieseki Exp $

#ifndef DVIBBOXREADER_H
#define DVIBBOXREADER_H

#include "BoundingBox.h"
#include "DVIReader.h"

class DVIBBoxReader : public DVIReader
{
   public:
      DVIBBoxReader (istream &is);
		void setChar (double x, double y, unsigned c);
		void setRule (double x, double y, double height, double width);
		const BoundingBox& getBoundingBox () const {return bbox;}

   private:
		BoundingBox bbox;
};

#endif
