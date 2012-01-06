/*************************************************************************
** PageSize.h                                                           **
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

#ifndef PAGESIZE_H
#define PAGESIZE_H

#include "MessageException.h"

struct PageSizeException : public MessageException
{
	PageSizeException (const std::string &msg) : MessageException(msg) {}
};

class PageSize
{
   public:
 		PageSize (double w=0, double h=0) : width(w), height(h) {}
		PageSize (std::string name);
		void resize (std:: string name);
		void resize (double w, double h);
		double widthInPT () const  {return width;}
		double heightInPT () const {return height;}
		double widthInMM () const  {return width*25.4/72.27;}
		double heightInMM () const {return height*25.4/72.27;}
		bool valid () const        {return width > 0 && height > 0;}

   private:
		double width, height; // in pt
};

#endif
