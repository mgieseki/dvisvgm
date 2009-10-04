/*************************************************************************
** DVIToSVGActions.h                                                    **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef DVITOSVGACTIONS_H
#define DVITOSVGACTIONS_H

#include <map>
#include <set>
#include "BoundingBox.h"
#include "DVIActions.h"
#include "Matrix.h"
#include "SpecialActions.h"
#include "SpecialManager.h"
#include "SVGTree.h"


class CharmapTranslator;
class DVIToSVG;
class FileFinder;
class Font;
class XMLNode;

class DVIToSVGActions : public DVIActions, public SpecialActions, public SpecialManager::Listener
{
	typedef std::map<const Font*, CharmapTranslator*> CharmapTranslatorMap;
	typedef std::map<const Font*, std::set<int> > UsedCharsMap;
	
	public:
		DVIToSVGActions (DVIToSVG &dvisvg, SVGTree &svg);
		~DVIToSVGActions ();
		void setChar (double x, double y, unsigned c, const Font *f);
		void setRule (double x, double y, double height, double width);
		void setBgColor (const Color &color);
		void setColor (const Color &color)              {_svg.setColor(color);}
		void setMatrix (const Matrix &m)                {_svg.setMatrix(m);}
		const Matrix& getMatrix () const                {return _svg.getMatrix();}
		Color getColor () const                         {return _svg.getColor();}
		void appendToPage (XMLNode *node)               {_svg.appendToPage(node);}
		void appendToDefs (XMLNode *node)               {_svg.appendToDefs(node);}
		void moveToX (double x)                         {_svg.setX(x);}
		void moveToY (double y)                         {_svg.setY(y);}
		void defineFont (int num, const Font *font);
		void setFont (int num, const Font *font);
		void special (const string &s);
		void preamble (const string &cmt);
		void postamble ();
		void beginPage (Int32 *c);
		void endPage ();
		void beginSpecial (const char *prefix);
		void endSpecial (const char *prefix);
		UsedCharsMap& getUsedChars () const  {return _usedCharsMap;}
		void setPageMatrix (const Matrix &tm);
		CharmapTranslator* getCharmapTranslator (const Font *font) const;
		double getX() const  {return _dvisvg.getXPos();}
		double getY() const  {return _dvisvg.getYPos();}
		void setX (double x) {_dvisvg.setXPos(x);}
		void setY (double y) {_dvisvg.setYPos(y);}
		BoundingBox& bbox () {return _bbox;}

	public:
		static unsigned PROGRESSBAR; ///< disabled if 0, indicates skip value otherwise

	private:
		SVGTree &_svg;
		DVIToSVG &_dvisvg;
		BoundingBox _bbox;
		int _pageCount;
		int _currentFontNum;
		CharmapTranslatorMap _charmapTranslatorMap;
		mutable UsedCharsMap _usedCharsMap;
		Matrix *_pageMatrix;  // transformation of whole page
		Color _bgcolor;
};


#endif
