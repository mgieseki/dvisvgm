/*************************************************************************
** Font.cpp                                                             **
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "FileSystem.h"
#include "Font.h"
#include "FontEngine.h"
#include "FileFinder.h"
#include "GFGlyphTracer.h"
#include "Glyph.h"
#include "Message.h"
#include "MetafontWrapper.h"
#include "TFM.h"
#include "VFReader.h"
#include "macros.h"
#include "FileSystem.h"
#include "SignalHandler.h"
#include "SVGTree.h"

using namespace std;


/** Returns true if 'unicode' is a valid unicode value in XML documents.
 *  XML version 1.0 doesn't allow various unicode character references
 *  (&#1; for example).  */
static bool valid_unicode (UInt32 unicode) {
	UInt32 ranges[] = {
		0x0000, 0x0020,
		0x007f, 0x0084,
		0x0086, 0x009f,
		0xfdd0, 0xfddf
	};
	for (int i=0; i < 4; i++)
		if (unicode >= ranges[2*i] && unicode <= ranges[2*i+1])
			return false;
	return true;
}


UInt32 Font::unicode (UInt32 c) const {
	// @@ this should be optimized :-)
	return valid_unicode(c) ? c : 0x3400+c;
}


///////////////////////////////////////////////////////////////////////////////////////


TFMFont::TFMFont (string name, UInt32 cs, double ds, double ss)
	: tfm(0), fontname(name), checksum(cs), dsize(ds), ssize(ss)
{
}


TFMFont::~TFMFont () {
	delete tfm;
}


/** Returns a font metrics object for the current font.
 *  @throw FontException if TFM file can't be found */
const TFM* TFMFont::getTFM () const {
	if (!tfm) {
		tfm = TFM::createFromFile(fontname.c_str());
		if (!tfm)
			throw FontException("can't find "+fontname+".tfm");
	}
	return tfm;
}


double TFMFont::charWidth (int c) const  {return getTFM()->getCharWidth(c);}
double TFMFont::charDepth (int c) const  {return getTFM()->getCharDepth(c);}
double TFMFont::charHeight (int c) const {return getTFM()->getCharHeight(c);}
double TFMFont::italicCorr (int c) const {return getTFM()->getItalicCorr(c);}

//////////////////////////////////////////////////////////////////////////////

// static class variables
bool PhysicalFont::KEEP_TEMP_FILES = false;
const char *PhysicalFont::CACHE_PATH = 0;
double PhysicalFont::METAFONT_MAG = 4;
FontCache PhysicalFont::_cache;



Font* PhysicalFont::create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type) {
	return new PhysicalFontImpl(name, checksum, dsize, ssize, type);
}


const char* PhysicalFont::path () const {
	string ext;
	switch (type()) {
		case PFB: ext = "pfb"; break;
		case TTF: ext = "ttf"; break;
		case MF : ext = "mf";  break;
	}
	return FileFinder::lookup(name()+"."+ext);
}


/** Returns the number of units per EM. The EM square is the virtual area a glyph is designed on.
 *  All coordinates used to specify portions of the glyph are relative to the origin (0,0) at the
 *  lower left corner of this square, while the upper right corner is located at (m,m), where m
 *  is an integer value defined with the font, and returned by this function. */
int PhysicalFont::unitsPerEm() const {
	if (type() == MF)
		return 1000;
	FontEngine::instance().setFont(*this);
	return FontEngine::instance().getUnitsPerEM();
}


int PhysicalFont::hAdvance () const {
	if (type() == MF)
		return 0;
	FontEngine::instance().setFont(*this);
	return FontEngine::instance().getHAdvance();
}


double PhysicalFont::hAdvance (int c) const {
	if (type() == MF)
		return unitsPerEm()*charWidth(c)/designSize();
	FontEngine::instance().setFont(*this);
	if (FontEncoding *enc = encoding())
		return FontEngine::instance().getHAdvance(enc->getEntry(c));
	return FontEngine::instance().getHAdvance(c);
}


string PhysicalFont::glyphName (int c) const {
	if (type() == MF)
		return "";
	FontEngine::instance().setFont(*this);
	if (FontEncoding *enc = encoding())
		return enc->getEntry(c);
	return FontEngine::instance().getGlyphName(c);
}


int PhysicalFont::ascent () const {
	if (type() == MF)
		return 0;
	FontEngine::instance().setFont(*this);
	return FontEngine::instance().getAscender();
}


int PhysicalFont::descent () const {
	if (type() == MF)
		return 0;
	FontEngine::instance().setFont(*this);
	return FontEngine::instance().getDescender();
}


/** Extracts the glyph outlines of a given character.
 *  @param[in]  c character code of requested glyph
 *  @param[out] glyph path segments of the glyph outline
 *  @param[in]  cb optional callback object for tracer class
 *  @return true if outline could be computed */
bool PhysicalFont::getGlyph (int c, GraphicPath<Int32> &glyph, GFGlyphTracer::Callback *cb) const {
	if (type() == MF) {
		const Glyph *cached_glyph=0;
		if (CACHE_PATH) {
			_cache.write(CACHE_PATH);
			_cache.read(name().c_str(), CACHE_PATH);
			cached_glyph = _cache.getGlyph(c);
		}
		if (cached_glyph) {
			glyph = *cached_glyph;
			return true;
		}
		else {
			string gfname;
			if (createGF(gfname)) {
				try {
					GFGlyphTracer tracer(gfname, unitsPerEm()/getTFM()->getDesignSize(), cb);
					tracer.setGlyph(glyph);
					tracer.executeChar(c);
					glyph.closeOpenSubPaths();
					if (CACHE_PATH)
						_cache.setGlyph(c, glyph);
					return true;
				}
				catch (GFException &e) {
					// @@ print error message
				}
			}
			else {
				Message::wstream(true) << "failed creating " << name() << ".gf\n";
			}
		}
	}
	else { // PFB or TTF
		bool ok=true;
		FontEngine::instance().setFont(*this);
		if (FontEncoding *enc = encoding()) {
			if (const char *encname = enc->getEntry(c))
				ok = FontEngine::instance().traceOutline(encname, glyph, false);
		}
		else
			ok = FontEngine::instance().traceOutline((unsigned char)c, glyph, false);
		glyph.closeOpenSubPaths();
		return ok;
	}
	return false;
}


/** Creates a GF file for this font object.
 *  @param[out] gfname name of GF font file
 *  @return true on success */
bool PhysicalFont::createGF (string &gfname) const {
	SignalHandler::instance().check();
	gfname = name()+".gf";
	MetafontWrapper mf(name());
	bool ok = mf.make("ljfour", METAFONT_MAG); // call Metafont if necessary
	return ok && mf.success() && getTFM();
}


/** Traces all glyphs of the current font and stores them in the cache. If caching is disabled, nothing happens.
 *  @param[in] includeCached if true, glyphs already cached are traced again
 *  @param[in] cb optional callback methods called by the tracer
 *  @return number of glyphs traced */
int PhysicalFont::traceAllGlyphs (bool includeCached, GFGlyphTracer::Callback *cb) const {
	int count = 0;
	if (type() == MF && CACHE_PATH) {
		if (const TFM *tfm = getTFM()) {
			int fchar = tfm->firstChar();
			int lchar = tfm->lastChar();
			string gfname;
			Glyph glyph;
			if (createGF(gfname)) {
				_cache.read(name().c_str(), CACHE_PATH);
				GFGlyphTracer tracer(gfname, unitsPerEm()/getTFM()->getDesignSize(), cb);
				tracer.setGlyph(glyph);
				for (int i=fchar; i <= lchar; i++) {
					if (includeCached || !_cache.getGlyph(i)) {
						glyph.newpath();
						tracer.executeChar(i);
						glyph.closeOpenSubPaths();
						_cache.setGlyph(i, glyph);
						++count;
					}
				}
				_cache.write(CACHE_PATH);
			}
		}
	}
	return count;
}


/** Computes the exact bounding box of a glyph.
 *  @param[in]  c character code of the glyph
 *  @param[out] bbox the computed bounding box
 *  @param[in]  optional calback object forwarded to the tracer
 *  @return true if the box could be computed successfully */
bool PhysicalFont::getGlyphBox(int c, BoundingBox& bbox, GFGlyphTracer::Callback* cb) const {
	Glyph glyph;
	if (getGlyph(c, glyph, cb)) {
		glyph.computeBBox(bbox);
		double s = scaledSize()/unitsPerEm();
		bbox.scale(s, s);
		return true;
	}
	return false;
}


Font* VirtualFont::create (string name, UInt32 checksum, double dsize, double ssize) {
	return new VirtualFontImpl(name, checksum, dsize, ssize);
}


//////////////////////////////////////////////////////////////////////////////


PhysicalFontImpl::PhysicalFontImpl (string name, UInt32 cs, double ds, double ss, PhysicalFont::Type type)
	: TFMFont(name, cs, ds, ss), _filetype(type), _charmap(0)
{
}


PhysicalFontImpl::~PhysicalFontImpl () {
	if (CACHE_PATH)
		_cache.write(CACHE_PATH);
	if (!KEEP_TEMP_FILES)
		tidy();
	delete _charmap;
}


UInt32 PhysicalFontImpl::unicode (UInt32 c) const {
	if (type() == MF)
		return Font::unicode(c);

	if (_charmap == 0) {
		FontEngine &fe = FontEngine::instance();
		if (fe.setFont(*this)) {
			_charmap = new map<UInt32,UInt32>;
			fe.buildTranslationMap(*_charmap);
		}
	}
	typedef map<UInt32,UInt32>::const_iterator ConstIterator;
	ConstIterator it = _charmap->find(c);
	if (it != _charmap->end())
		return it->second;

	// No unicode equivalent found in font file.
	// Now we should look for a smart alternative but at the moment
	// it's sufficient to simply choose a valid unused unicode value...
	map<UInt32,UInt32> reverse_map;
	FORALL(*_charmap, ConstIterator, it)
		reverse_map[it->second] = it->first;
	// can we use charcode itself as unicode replacement?
	if (valid_unicode(c) && (reverse_map.empty() || reverse_map.find(c) != reverse_map.end()))
		return c;
	return 0x3400+c;
}


void PhysicalFontImpl::tidy () const {
   if (type() == MF) {
		const char *ext[] = {"gf", "tfm", "log", 0};
		for (const char **p=ext; *p; ++p) {
			if (FileSystem::exists((name()+"."+(*p)).c_str()))
				FileSystem::remove(name()+"."+(*p));
		}
   }
}


//////////////////////////////////////////////////////////////////////////////

VirtualFontImpl::VirtualFontImpl (string name, UInt32 cs, double ds, double ss)
	: TFMFont(name, cs, ds, ss)
{
}


VirtualFontImpl::~VirtualFontImpl () {
	// delete dvi vectors received by VFReaderAction
	for (map<UInt32, DVIVector*>::iterator i=charDefs.begin(); i != charDefs.end(); ++i)
		delete i->second;
}


const char* VirtualFontImpl::path () const {
	return FileFinder::lookup(name()+".vf");
}


void VirtualFontImpl::assignChar (UInt32 c, DVIVector *dvi) {
	if (dvi) {
		if (charDefs.find(c) == charDefs.end())
			charDefs[c] = dvi;
		else
			delete dvi;
	}
}


const vector<UInt8>* VirtualFontImpl::getDVI (int c) const {
	map<UInt32,DVIVector*>::const_iterator it = charDefs.find(c);
	return (it == charDefs.end() ? 0 : it->second);
}

