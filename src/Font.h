/***********************************************************************
** Font.h                                                             **
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

#ifndef FONT_H
#define FONT_H

#include <map>
#include <string>
#include <vector>
#include "MessageException.h"
#include "types.h"

using std::map;
using std::string;
using std::vector;


class TFM;


/** Base interface for all font classes. */
struct Font
{
	virtual ~Font () {}
	virtual Font* clone (double ds, double sc) const =0;
	virtual string name () const =0;
	virtual double designSize () const =0;
	virtual double scaledSize () const =0;
	virtual double scaleFactor () const        {return scaledSize()/designSize();}
	virtual double charWidth (int c) const =0;
	virtual double charDepth (int c) const =0;
	virtual double charHeight (int c) const =0;
	virtual const TFM* getTFM () const =0;
};


/** Interface for all physical fonts. */
struct PhysicalFont : public Font
{
	enum Type {MF, PFB, TTF};
	static Font* create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type);
};


/** Interface for all virtual fonts. */
struct VirtualFont : public Font
{
	static Font* create (string name, UInt32 checksum, double dsize, double ssize);
	virtual int fontID (int n) const =0;
	virtual int firstFontNum () const =0;
	virtual UInt8* getDVI (int c) const =0;
};



class PhysicalFontProxy : public PhysicalFont
{
	friend class PhysicalFontImpl;
	public:
		Font* clone (double ds, double sc) const {return new PhysicalFontProxy(*this, ds, sc);}
		string name () const            {return pf->name();}
		double designSize () const      {return dsize;}
		double scaledSize () const      {return ssize;}
		double charWidth (int c) const  {return pf->charWidth(c);} 
		double charDepth (int c) const  {return pf->charDepth(c);} 
		double charHeight (int c) const {return pf->charHeight(c);} 
		const TFM* getTFM () const      {return pf->getTFM();}

	protected:
		PhysicalFontProxy (const PhysicalFont *font, double ds, double ss) : pf(font), dsize(ds), ssize(ss) {}
		PhysicalFontProxy (const PhysicalFontProxy &proxy, double ds, double ss) : pf(proxy.pf), dsize(ds), ssize(ss) {}

	private:
		const PhysicalFont *pf;
		double dsize;  ///< design size in TeX point units
		double ssize;  ///< scaled size
};


class PhysicalFontImpl : public PhysicalFont
{
	friend class PhysicalFont;
	public:
		~PhysicalFontImpl ();
		Font* clone (double ds, double ss) const {return new PhysicalFontProxy(this, ds, ss);}
		double designSize () const      {return dsize;}
		double scaledSize () const      {return ssize;}
		double charWidth (int c) const;
		double charDepth (int c) const;
		double charHeight (int c) const;
		const TFM* getTFM () const;
		string name () const            {return _name;}

	protected:
		PhysicalFontImpl (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type);

	private:
		mutable TFM *_tfm;
		string _name;    ///< fontname
		UInt32 checksum; ///< cheksum to be compared with TFM checksum
		double dsize;    ///< design size in TeX point units
		double ssize;    ///< scaled size
};


class VirtualFontProxy : public VirtualFont
{
	friend class VirtualFontImpl;
	public:
		Font* clone (double ds, double ss) const {return new VirtualFontProxy(*this, ds, ss);}
		string name () const              {return vf->name();}
		int fontID (int n) const          {return vf->fontID(n);}
		int firstFontNum () const         {return vf->firstFontNum();}
		UInt8* getDVI (int c) const       {return vf->getDVI(c);}
		double designSize () const        {return dsize;}
		double scaledSize () const        {return ssize;}
		double charWidth (int c) const    {return vf->charWidth(c);} 
		double charDepth (int c) const    {return vf->charDepth(c);} 
		double charHeight (int c) const   {return vf->charHeight(c);} 
		const TFM* getTFM () const        {return vf->getTFM();}

	protected:
		VirtualFontProxy (const VirtualFont *font, double ds, double ss) : vf(font), dsize(ds), ssize(ss) {}
		VirtualFontProxy (const VirtualFontProxy &proxy, double ds, double ss) : vf(proxy.vf), dsize(ds), ssize(ss) {}

	private:
		const VirtualFont *vf;
		double dsize;  ///< design size in TeX point units
		double ssize;  ///< scaled size in TeX point units
};


class VirtualFontImpl : public VirtualFont
{
	friend class VirtualFont;
	public:
		~VirtualFontImpl ();
		Font* clone (double ds, double ss) const {return new VirtualFontProxy(this, ds, ss);}
		double designSize () const      {return dsize;}
		double scaledSize () const      {return ssize;}
		double charWidth (int c) const;
		double charDepth (int c) const;
		double charHeight (int c) const;
		string name () const            {return _name;}
		int fontID (int n) const;
		int firstFontNum () const;
		UInt8* getDVI (int c) const;
		const TFM* getTFM () const;

	protected:
		VirtualFontImpl (string name, UInt32 checksum, double dsize, double ssize);

	private:
		map<int,int> num2id;
		mutable TFM *_tfm;
		string _name;    ///< fontname
		UInt32 checksum; ///< cheksum to be compared with TFM checksum
		double dsize;    ///< design size in TeX point units
		double ssize;    ///< scaled size
};


struct FontException : public MessageException
{
	FontException (string msg) : MessageException(msg) {}
};

#endif
