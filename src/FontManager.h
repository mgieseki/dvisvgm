/***********************************************************************
** FontManager.h                                                      **
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

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <map>
#include <string>

using std::map;
using std::string;

struct Font
{
	virtual ~Font () {}
	virtual Font* clone () const =0;
	virtual bool isVirtual () const =0;
	virtual double charWidth (int c) const =0;
	virtual double charDepth (int c) const =0;
	virtual double charHeight (int c) const =0;
};


class FontBase : public Font
{
	public:
		double charWidth (int c) const;
		double charDepth (int c) const;
		double charHeight (int c) const;

	protected:
		TFM* getTFM () const {return tfm;}

	private:
		TFM *tfm;
};


class PhysicalFont : public FontBase
{
	enum Type {MF, PFB, TTF};
	public:
		PhysicalFont (string name, Type type);
		Font* clone () const     {return PhysicalFontRef(*this);}
		bool isVirtual () const  {return false;}
};


class PhysicalFontRef : public Font
{
	public:
		PhysicalFontRef (PhysicalFont &pf) : pfont(pf) {}
		bool isVirtual () const {return false;}

	private:
		PhysicalFont &pfont;
};

class VirtualFontRef;

class VirtualFont : public Font
{
	public:
		VirtualFont (string name);
		~VirtualFont ();
		Font* clone () const    {return new VirtualFontRef(*this);}
		bool isVirtual () const {return true;}
		void setChar (int c, DVIReader &dviReader) const;

	private:
		map<int, Font*> fontDefs;
		map<int, Byte*> charDefs;
};


class VirtualFontRef : public Font
{
	public:
		VirtualFontRef (VirtualFont &vf) : vfont(vf) {}
		bool isVirtual () const {return true;}
	private:
		VirtualFont &vfont;
};

////////////////////////////////////////////////////////

class TFM;

class FontManager
{
   public:
      FontManager ();
      ~FontManager ();
		int addFont (int n, string name);
		const Font* getFont (int n) const;
		int getIndex (int n) const;

   private:
		map<int,int>   num2index;
		map<name, int> name2index;
		vector<Font*>  fonts;
		vector<TFM*>   tfms;
};

#endif
