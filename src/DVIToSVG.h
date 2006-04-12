/***********************************************************************
** DVIToSVG.h                                                         **
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
// $Id: DVIToSVG.h,v 1.18 2006/01/05 16:05:05 mgieseki Exp $

#ifndef DVITOSVG_H
#define DVITOSVG_H

#include <ostream>
#include "BoundingBox.h"
#include "DVIReader.h"

using std::istream;
using std::ostream;

class CharmapTranslator;
class FontMap;
class XMLDocument;
class XMLDocTypeNode;
class XMLElementNode;

class DVIToSVG : public DVIReader
{
   public:
      DVIToSVG (istream &is, ostream &os);
		~DVIToSVG ();
		int convert (unsigned firstPage, unsigned lastPage);
		void setFontMap (FontMap *fm)     {fontMap = fm;}
		void setMetafontMag (double m);
		void setPageSize (string name)    {pageSizeName = name;}
		void setProcessSpecials (bool ps);
		void setTransformation (const string &cmds) {transCmds = cmds;}
	
	protected:
		DVIToSVG (const DVIToSVG &);
		DVIToSVG operator = (const DVIToSVG &);
		void embedFonts (XMLElementNode *svgElement);
		bool computeBoundingBox (int page);

   private:
		ostream &out;
		double mag;              //< magnification factor of Metafont output
		FontMap *fontMap;
		XMLDocument *svgDocument;
		XMLDocTypeNode *doctypeNode;
		XMLElementNode *svgElement;
		string pageSizeName;
		string transCmds;
		bool processSpecials;
		BoundingBox boundingBox; //< bounding box of current page
};

#endif
