/***********************************************************************
** DVIToSVG.h                                                         **
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

#ifndef DVITOSVG_H
#define DVITOSVG_H

#include <iostream>
#include "DVIReader.h"
#include "SVGTree.h"


class SpecialManager;


class DVIToSVG : public DVIReader
{
   public:
      DVIToSVG (std::istream &is, std::ostream &os);
		~DVIToSVG ();
		int convert (unsigned firstPage, unsigned lastPage);
		void setPageSize (string name)    {_pageSizeName = name;}
		const SpecialManager* setProcessSpecials (const char *ignorelist=0);
		void setTransformation (const std::string &cmds) {_transCmds = cmds;}
	
	protected:
		DVIToSVG (const DVIToSVG &);
		DVIToSVG operator = (const DVIToSVG &);
		void beginPage (Int32 *c);
		void endPage ();
		void embedFonts (XMLElementNode *svgElement);

   private:
		SVGTree _svg;
		std::ostream &_out;       ///< DVI output is written to this stream
		std::string _pageSizeName;
		std::string _transCmds;

	public:
		static bool CREATE_STYLE; ///< should <style>...</style> and class attributes be used to reference fonts?
		static bool USE_FONTS;    ///< if true, create font references and don't draw paths directly
};

#endif
