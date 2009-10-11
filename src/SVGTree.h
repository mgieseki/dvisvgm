/*************************************************************************
** SVGTree.h                                                            **
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

#ifndef SVGTREE_H
#define SVGTREE_H

#include "Color.h"
#include "Matrix.h"
#include "XMLDocument.h"
#include "XMLNode.h"

class BoundingBox;
class CharmapTranslator;
class Color;
class Font;
class FontManager;
class Matrix;

class SVGTree
{
	template <typename T>
	class Property {
		public:
			Property (const T &v) : _value(v), _changed(false) {}

			void set (const T &v) {
				if (v != _value) {
					_value = v;
					_changed = true;
				}
			}

			const T& get () const {return _value;}
			operator const T& ()  {return _value;}
			bool changed () const {return _changed;}
			void changed (bool c) {_changed = c;}

		private:
			T _value;
			bool _changed;
	};

   public:
      SVGTree ();
		void reset ();
		void write (ostream &os) const    {_doc.write(os);}
		void newPage (int pageno);
		void appendToDefs (XMLNode *node);
		void appendToPage (XMLNode *node) {_page->append(node);}
		void prependToPage (XMLNode *node){_page->prepend(node);}
		void appendToDoc (XMLNode *node)  {_doc.append(node);}
		void appendToRoot (XMLNode *node) {_root->append(node);}
		void appendChar (int c, double x, double y, const FontManager &fm, const CharmapTranslator &cmt);
		void setBBox (const BoundingBox &bbox);
		void setFont (int id, const Font *font);
		void setX (double x) {_xchanged = true;}
		void setY (double y) {_ychanged = true;}
		void setMatrix (const Matrix &m) {_matrix.set(m);}
		void setColor (const Color &c)    {_color.set(c);}
		void transformPage (const Matrix &m);
		const Color& getColor () const    {return _color.get();}
		const Matrix& getMatrix () const {return _matrix.get();}
		XMLElementNode* rootNode () const {return _root;}

	protected:
		void newTextNode (double x, double y);

   private:
		XMLDocument _doc;
		XMLElementNode *_root, *_page, *_text, *_span, *_defs;
		bool _xchanged, _ychanged;
		Property<const Font*> _font;
		Property<Color> _color;
		Property<Matrix> _matrix;
		int _fontnum;
};

#endif
