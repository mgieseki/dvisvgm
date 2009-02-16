/***********************************************************************
** DVIToSVGActions.h                                                  **
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

#ifndef DVITOSVGACTIONS_H
#define DVITOSVGACTIONS_H

#include <map>
#include <set>
#include "DVIActions.h"
#include "SpecialActions.h"
#include "TransformationMatrix.h"

using std::map;
using std::set;

class CharmapTranslator;
class DVIReader;
class FileFinder;
class Font;
class SpecialManager;
class XMLElementNode;

class DVIToSVGActions : public DVIActions, public SpecialActions
{
	template <typename T>
	class Property
	{
		public:
			Property (const T &val) : _val(val), _changed(false) {}
			Property (const  Property &p) : _val(p._val), _changed(false) {}
			bool operator == (const Property &p) const {return _val == p._val;}
			bool operator != (const Property &p) const {return _val != p._val;}
			void operator = (const Property &p)        {_val = p._val; _changed = false;}
			bool changed () const                      {return _changed;}
			void set (const T &val)                    {if (val != _val) {_val=val; _changed=true;}}
			const T& get () const                      {return _val;}
			void changed (bool c)                      {_changed=c;}

		private:
			T _val;
			bool _changed;
	};

	struct Nodes
	{
		Nodes (XMLElementNode *r);
		XMLElementNode *root, *page, *font, *text;
	};
	typedef map<const Font*, CharmapTranslator*> CharmapTranslatorMap;
	typedef map<const Font*, set<int> > UsedCharsMap;

	public:
		DVIToSVGActions (const DVIReader &reader, XMLElementNode *svgelem);
		~DVIToSVGActions ();
		void setChar (double x, double y, unsigned c, const Font *f);
		void setRule (double x, double y, double height, double width);
		void setColor (const vector<float> &color) {_color.set(color);}
		void appendInPage (XMLElementNode *node);
		void moveToX (double x) {_xmoved = true;}
		void moveToY (double y) {_ymoved = true;}
		void defineFont (int num, const Font *font);
		void setFont (int num, const Font *font);
		void special (const string &s);
		void preamble (const string &cmt);
		void postamble ();
		void beginPage (Int32 *c);
		void endPage ();
		const UsedCharsMap& getUsedChars () const {return _usedCharsMap;}
		const SpecialManager* setProcessSpecials (const char *ignorelist);
		void setTransformation (const TransformationMatrix &tm);
		CharmapTranslator* getCharmapTranslator (const Font *font) const;
		int getX() const {return _dviReader.getXPos();}
		int getY() const {return _dviReader.getYPos();}

	private:
		const DVIReader &_dviReader;
		SpecialManager *_specialManager;
		bool _xmoved, _ymoved;
		Property<Color> _color;
		int _pageCount;
		int _currentFont;
		Nodes _nodes;
//		XMLElementNode *svgElement, *pageElement, *styleElement, *charElement;
		CharmapTranslatorMap _charmapTranslatorMap;
		UsedCharsMap _usedCharsMap;
		TransformationMatrix *_transMatrix;
};


#endif
