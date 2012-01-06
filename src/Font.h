/*************************************************************************
** Font.h                                                               **
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

#ifndef FONT_H
#define FONT_H

#include <map>
#include <string>
#include <vector>
#include "FontCache.h"
#include "FontEncoding.h"
#include "GFGlyphTracer.h"
#include "Glyph.h"
#include "GraphicPath.h"
#include "MessageException.h"
#include "VFActions.h"
#include "VFReader.h"
#include "types.h"

class TFM;


/** Abstract base for all font classes. */
struct Font
{
	virtual ~Font () {}
	virtual Font* clone (double ds, double sc) const =0;
	virtual const Font* uniqueFont () const =0;
	virtual std::string name () const =0;
	virtual double designSize () const =0;
	virtual double scaledSize () const =0;
	virtual double scaleFactor () const        {return scaledSize()/designSize();}
	virtual double charWidth (int c) const =0;
	virtual double charDepth (int c) const =0;
	virtual double charHeight (int c) const =0;
	virtual double italicCorr (int c) const =0;
	virtual const TFM* getTFM () const =0;
	virtual const char* path () const =0;
	virtual FontEncoding* encoding () const    {return FontEncoding::encoding(name());}
	virtual bool getGlyph (int c, Glyph &glyph, GFGlyphTracer::Callback *cb=0) const =0;
   virtual UInt32 unicode (UInt32 c) const;
   virtual void tidy () const {}
};


/** Empty font without any glyphs. Instances of this class are used
 *  if no physical or virtual font file can be found.
 *  The metric values returned by the member functions are based on cmr10. */
struct EmptyFont : public Font
{
	public:
		EmptyFont (std::string name) : fontname(name) {}
		Font* clone (double ds, double sc) const  {return new EmptyFont(*this);}
		const Font* uniqueFont () const           {return this;}
		std::string name () const                 {return fontname;}
		double designSize () const                {return 10;}    // cmr10 design size in pt
		double scaledSize () const                {return 10;}    // cmr10 scaled size in pt
		double charWidth (int c) const            {return 9.164;} // width of cmr10's 'M' in pt
		double charHeight (int c) const           {return 6.833;} // height of cmr10's 'M' in pt
		double charDepth (int c) const            {return 0;}
		double italicCorr (int c) const           {return 0;}
		const TFM* getTFM () const                {return 0;}
		const char* path () const                 {return 0;}
		bool getGlyph (int c, Glyph &glyph, GFGlyphTracer::Callback *cb=0) const {return false;}

	private:
		std::string fontname;
};


/** Interface for all physical fonts. */
class PhysicalFont : public virtual Font
{
   public:
      enum Type {MF, PFB, TTF};
      static Font* create (std::string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type);
      virtual Type type () const =0;
      virtual bool getGlyph (int c, Glyph &glyph, GFGlyphTracer::Callback *cb=0) const;
      virtual bool getGlyphBox (int c, BoundingBox &bbox, GFGlyphTracer::Callback *cb=0) const;
      virtual int hAdvance () const;
      virtual double hAdvance (int c) const;
      std::string glyphName (int c) const;
      virtual int unitsPerEm () const;
      virtual int ascent () const;
      virtual int descent () const;
      virtual int traceAllGlyphs (bool includeCached, GFGlyphTracer::Callback *cb=0) const;
      const char* path () const;

   protected:
      bool createGF (std::string &gfname) const;

   public:
      static bool KEEP_TEMP_FILES;
      static const char *CACHE_PATH; ///< path to cache directory (0 if caching is disabled)
      static double METAFONT_MAG;    ///< magnification factor for Metafont calls

   protected:
      static FontCache _cache;
};


/** Interface for all virtual fonts. */
class VirtualFont : public virtual Font
{
	friend class FontManager;
	public:
		typedef std::vector<UInt8> DVIVector;

	public:
		static Font* create (std::string name, UInt32 checksum, double dsize, double ssize);
		virtual const DVIVector* getDVI (int c) const =0;
		bool getGlyph (int c, Glyph &glyph, GFGlyphTracer::Callback *cb=0) const {return false;}

	protected:
		virtual void assignChar (UInt32 c, DVIVector *dvi) =0;
};


class TFMFont : public virtual Font
{
	public:
		TFMFont (std::string name, UInt32 checksum, double dsize, double ssize);
		~TFMFont ();
		const TFM* getTFM () const;
		std::string name () const   {return fontname;}
		double designSize () const  {return dsize;}
		double scaledSize () const  {return ssize;}
		double charWidth (int c) const;
		double charDepth (int c) const;
		double charHeight (int c) const;
		double italicCorr (int c) const;

	private:
		mutable TFM *tfm;
		std::string fontname;
		UInt32 checksum; ///< cheksum to be compared with TFM checksum
		double dsize;    ///< design size in TeX point units
		double ssize;    ///< scaled size
};


class PhysicalFontProxy : public PhysicalFont
{
	friend class PhysicalFontImpl;
	public:
		Font* clone (double ds, double sc) const  {return new PhysicalFontProxy(*this, ds, sc);}
		const Font* uniqueFont () const           {return pf;}
		std::string name () const                 {return pf->name();}
		double designSize () const                {return dsize;}
		double scaledSize () const                {return ssize;}
		double charWidth (int c) const            {return pf->charWidth(c);}
		double charDepth (int c) const            {return pf->charDepth(c);}
		double charHeight (int c) const           {return pf->charHeight(c);}
		double italicCorr (int c) const           {return pf->italicCorr(c);}
		const TFM* getTFM () const                {return pf->getTFM();}
		Type type () const                        {return pf->type();}
      UInt32 unicode (UInt32 c) const           {return pf->unicode(c);}

	protected:
		PhysicalFontProxy (const PhysicalFont *font, double ds, double ss) : pf(font), dsize(ds), ssize(ss) {}
		PhysicalFontProxy (const PhysicalFontProxy &proxy, double ds, double ss) : pf(proxy.pf), dsize(ds), ssize(ss) {}

	private:
		const PhysicalFont *pf;
		double dsize;  ///< design size in TeX point units
		double ssize;  ///< scaled size
};


class PhysicalFontImpl : public PhysicalFont, public TFMFont
{
	friend class PhysicalFont;
	public:
      ~PhysicalFontImpl();
		Font* clone (double ds, double ss) const {return new PhysicalFontProxy(this, ds, ss);}
		const Font* uniqueFont () const          {return this;}
      Type type () const                       {return _filetype;}
      UInt32 unicode (UInt32 c) const;
      void tidy () const;

	protected:
		PhysicalFontImpl (std::string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type);

	private:
		Type _filetype;
      mutable std::map<UInt32,UInt32> *_charmap;
};


class VirtualFontProxy : public VirtualFont
{
	friend class VirtualFontImpl;
	public:
		Font* clone (double ds, double ss) const {return new VirtualFontProxy(*this, ds, ss);}
		const Font* uniqueFont () const       {return vf;}
		std::string name () const             {return vf->name();}
		const DVIVector* getDVI (int c) const {return vf->getDVI(c);}
		double designSize () const            {return dsize;}
		double scaledSize () const            {return ssize;}
		double charWidth (int c) const        {return vf->charWidth(c);}
		double charDepth (int c) const        {return vf->charDepth(c);}
		double charHeight (int c) const       {return vf->charHeight(c);}
		double italicCorr (int c) const       {return vf->italicCorr(c);}
		const TFM* getTFM () const            {return vf->getTFM();}
		const char* path () const             {return vf->path();}

	protected:
		VirtualFontProxy (const VirtualFont *font, double ds, double ss) : vf(font), dsize(ds), ssize(ss) {}
		VirtualFontProxy (const VirtualFontProxy &proxy, double ds, double ss) : vf(proxy.vf), dsize(ds), ssize(ss) {}
		void assignChar (UInt32 c, DVIVector *dvi) {delete dvi;}

	private:
		const VirtualFont *vf;
		double dsize;  ///< design size in TeX point units
		double ssize;  ///< scaled size in TeX point units
};


class VirtualFontImpl : public VirtualFont, public TFMFont
{
	friend class VirtualFont;
	public:
		~VirtualFontImpl ();
		Font* clone (double ds, double ss) const {return new VirtualFontProxy(this, ds, ss);}
		const Font* uniqueFont () const   {return this;}
		const DVIVector* getDVI (int c) const;
		const char* path () const;

	protected:
		VirtualFontImpl (std::string name, UInt32 checksum, double dsize, double ssize);
		void assignChar (UInt32 c, DVIVector *dvi);

	private:
		std::map<UInt32, DVIVector*> charDefs; ///< dvi subroutines defining the characters
};


struct FontException : public MessageException
{
	FontException (std::string msg) : MessageException(msg) {}
};

#endif
