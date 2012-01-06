/*************************************************************************
** DVIToSVG.h                                                           **
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

#ifndef DVITOSVG_H
#define DVITOSVG_H

#include <iostream>
#include <string>
#include <utility>
#include "DVIReader.h"
#include "SpecialManager.h"
#include "SVGTree.h"


class DVIToSVG : public DVIReader
{
   public:
      /** DVIToSVG uses this interface to get the output streams for the SVG files. */
      struct Output {
         virtual ~Output () {}
			virtual std::ostream& getPageStream (int page, int numPages) const =0;
			virtual std::string filename (int page, int numPages) const =0;
      };

   public:
      DVIToSVG (std::istream &is, Output &out);
		~DVIToSVG ();
		void convert (unsigned firstPage, unsigned lastPage, std::pair<int,int> *pageinfo=0);
      void convert (const std::string &range, std::pair<int,int> *pageinfo=0);
		const SpecialManager* setProcessSpecials (const char *ignorelist=0);
		const SpecialManager& specialManager () const    {return _specialManager;}
		void setPageSize (const std::string &name)       {_bboxString = name;}
		void setTransformation (const std::string &cmds) {_transCmds = cmds;}

   public:
      static char TRACE_MODE;

	protected:
		DVIToSVG (const DVIToSVG &);
		DVIToSVG operator = (const DVIToSVG &);
		void beginPage (unsigned n, Int32 *c);
		void endPage ();
		void embedFonts (XMLElementNode *svgElement);

   private:
		SVGTree _svg;
		Output &_out;
		std::string _bboxString;
		std::string _transCmds;
		SpecialManager _specialManager;
};

#endif
