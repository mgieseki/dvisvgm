/*************************************************************************
** DVIToSVGActions.h                                                    **
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
#include "DVIReader.h"


class  DVIToSVG;
struct FileFinder;
struct Font;
struct XMLNode;

class DVIToSVGActions : public DVIActions, public SpecialActions, public SpecialManager::Listener
{
	typedef std::map<const Font*, std::set<int> > CharMap;
	typedef std::set<const Font*> FontSet;
   typedef std::map<std::string,BoundingBox> BoxMap;

	public:
		DVIToSVGActions (DVIToSVG &dvisvg, SVGTree &svg);
		~DVIToSVGActions ();
      void reset ();
		void setChar (double x, double y, unsigned c, const Font *f);
		void setRule (double x, double y, double height, double width);
		void setBgColor (const Color &color);
		void setColor (const Color &color)              {_svg.setColor(color);}
		void setMatrix (const Matrix &m)                {_svg.setMatrix(m);}
		const Matrix& getMatrix () const                {return _svg.getMatrix();}
		Color getColor () const                         {return _svg.getColor();}
		void appendToPage (XMLNode *node)               {_svg.appendToPage(node);}
		void appendToDefs (XMLNode *node)               {_svg.appendToDefs(node);}
		void moveToX (double x);
		void moveToY (double y);
		void setFont (int num, const Font *font);
		void special (const std::string &s);
		void preamble (const std::string &cmt);
		void postamble ();
		void beginPage (unsigned n, Int32 *c);
		void endPage ();
		void beginSpecial (const char *prefix);
		void endSpecial (const char *prefix);
		void progress (size_t current, size_t total, const char *id=0);
		void progress (const char *id);
		CharMap& getUsedChars () const        {return _usedChars;}
		const FontSet& getUsedFonts () const  {return _usedFonts;}
		void setPageMatrix (const Matrix &tm);
		double getX() const   {return _dvisvg.getXPos();}
		double getY() const   {return _dvisvg.getYPos();}
		void setX (double x)  {_dvisvg.translateToX(x); _svg.setX(x);}
		void setY (double y)  {_dvisvg.translateToY(y); _svg.setY(y);}
      void finishLine ()    {_dvisvg.finishLine();}
		BoundingBox& bbox ()  {return _bbox;}
      BoundingBox& bbox (const std::string &name, bool reset=false);
      void embed (const BoundingBox &bbox);
      void embed (const DPair &p, double r=0);

	public:
		static double PROGRESSBAR_DELAY; ///< progress bar doesn't appear before this time has elapsed (in sec)
      static bool EXACT_BBOX;

	private:
		SVGTree &_svg;
		DVIToSVG &_dvisvg;
		BoundingBox _bbox;
		int _pageCount;
		int _currentFontNum;
		mutable CharMap _usedChars;
		FontSet _usedFonts;
		Matrix *_pageMatrix;  // transformation of whole page
		Color _bgcolor;
      BoxMap *_boxes;
};


#endif
