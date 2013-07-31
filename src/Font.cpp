/*************************************************************************
** Font.cpp                                                             **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "FileFinder.h"
#include "FileSystem.h"
#include "Font.h"
#include "FontEncoding.h"
#include "FontEngine.h"
#include "FontMap.h"
#include "GFGlyphTracer.h"
#include "Glyph.h"
#include "Message.h"
#include "MetafontWrapper.h"
#include "TFM.h"
#include "VFReader.h"
#include "SignalHandler.h"
#include "Subfont.h"
#include "SVGTree.h"
#include "macros.h"

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


/** Returns the encoding object of this font which is asigned in a map file.
 *  If there's no encoding assigned, the function returns 0. */
FontEncoding* Font::encoding () const {
	string fontname = name();
   size_t pos = fontname.rfind('.');
	if (pos != string::npos)
		fontname = fontname.substr(0, pos); // strip extension
	if (const FontMap::Entry *entry = FontMap::instance().lookup(fontname))
		return FontEncoding::encoding(entry->encname);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////


TFMFont::TFMFont (string name, UInt32 cs, double ds, double ss)
	: _metrics(0), _fontname(name), _checksum(cs), _dsize(ds), _ssize(ss)
{
}


TFMFont::~TFMFont () {
	delete _metrics;
}


/** Returns a font metrics object for the current font.
 *  @throw FontException if TFM file can't be found */
const FontMetrics* TFMFont::getMetrics () const {
	if (!_metrics) {
		try {
			_metrics = FontMetrics::read(_fontname.c_str());
			if (!_metrics) {
				_metrics = new NullFontMetric;
				Message::wstream(true) << "can't find "+_fontname+".tfm\n";
			}
		}
		catch (FontMetricException &e) {
			_metrics = new NullFontMetric;
			Message::wstream(true) << e.what() << " in " << _fontname << ".tfm\n";
		}
	}
	return _metrics;
}


double TFMFont::charWidth (int c) const {
	double w = getMetrics() ? getMetrics()->getCharWidth(c) : 0;
	if (style())
		w *= style()->extend;
	return w;
}


double TFMFont::italicCorr (int c) const {
	double w = getMetrics() ? getMetrics()->getItalicCorr(c) : 0;
	if (style())
		w *= style()->extend;
	return w;
}


double TFMFont::charDepth (int c) const  {return getMetrics() ? getMetrics()->getCharDepth(c) : 0;}
double TFMFont::charHeight (int c) const {return getMetrics() ? getMetrics()->getCharHeight(c) : 0;}


bool TFMFont::verifyChecksums () const   {
	if (_checksum != 0 && getMetrics() && getMetrics()->getChecksum() != 0)
		return _checksum == getMetrics()->getChecksum();
	return true;
}

//////////////////////////////////////////////////////////////////////////////

// static class variables
bool PhysicalFont::KEEP_TEMP_FILES = false;
const char *PhysicalFont::CACHE_PATH = 0;
double PhysicalFont::METAFONT_MAG = 4;
FontCache PhysicalFont::_cache;


Font* PhysicalFont::create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type) {
	return new PhysicalFontImpl(name, 0, checksum, dsize, ssize, type);
}


Font* PhysicalFont::create (string name, int fontindex, UInt32 checksum, double dsize, double ssize) {
	return new PhysicalFontImpl(name, fontindex, checksum, dsize, ssize, PhysicalFont::TTC);
}


const char* PhysicalFont::path () const {
	const char *ext=0;
	switch (type()) {
		case OTF: ext = "otf"; break;
		case PFB: ext = "pfb"; break;
		case TTC: ext = "ttc"; break;
		case TTF: ext = "ttf"; break;
		case MF : ext = "mf";  break;
	}
	return FileFinder::lookup(name()+"."+ext);
}


bool PhysicalFont::isCIDFont () const {
	if (type() == MF)
		return false;
	FontEngine::instance().setFont(*this);
	return FontEngine::instance().isCIDFont();
}


Character PhysicalFont::decodeChar (UInt32 c) const {
	if (FontEncoding *enc = encoding())
		return enc->decode(c);
	return Character(Character::CHRCODE, c);
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
	if (const FontMap::Entry *map_entry = FontMap::instance().lookup(name()))
		if (Subfont *sf = map_entry->subfont)
			c = sf->decode(c);
	return FontEngine::instance().getHAdvance(decodeChar(c));
}


string PhysicalFont::glyphName (int c) const {
	if (type() == MF)
		return "";
	FontEngine::instance().setFont(*this);
	if (const FontMap::Entry *map_entry = FontMap::instance().lookup(name()))
		if (Subfont *sf = map_entry->subfont)
			c = sf->decode(c);
	return FontEngine::instance().getGlyphName(decodeChar(c));
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
					double ds = getMetrics() ? getMetrics()->getDesignSize() : 1;
					GFGlyphTracer tracer(gfname, unitsPerEm()/ds, cb);
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
	else { // vector fonts (OTF, PFB, TTF, TTC)
		bool ok=true;
		FontEngine::instance().setFont(*this);
		if (const FontMap::Entry *map_entry = FontMap::instance().lookup(name()))
			if (Subfont *sf = map_entry->subfont)
				c = sf->decode(c);
		ok = FontEngine::instance().traceOutline(decodeChar(c), glyph, false);
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
	return ok && mf.success() && getMetrics();
}


/** Traces all glyphs of the current font and stores them in the cache. If caching is disabled, nothing happens.
 *  @param[in] includeCached if true, glyphs already cached are traced again
 *  @param[in] cb optional callback methods called by the tracer
 *  @return number of glyphs traced */
int PhysicalFont::traceAllGlyphs (bool includeCached, GFGlyphTracer::Callback *cb) const {
	int count = 0;
	if (type() == MF && CACHE_PATH) {
		if (const FontMetrics *metrics = getMetrics()) {
			int fchar = metrics->firstChar();
			int lchar = metrics->lastChar();
			string gfname;
			Glyph glyph;
			if (createGF(gfname)) {
				_cache.read(name().c_str(), CACHE_PATH);
				double ds = getMetrics() ? getMetrics()->getDesignSize() : 1;
				GFGlyphTracer tracer(gfname, unitsPerEm()/ds, cb);
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


PhysicalFontImpl::PhysicalFontImpl (string name, int fontindex, UInt32 cs, double ds, double ss, PhysicalFont::Type type)
	: TFMFont(name, cs, ds, ss), _filetype(type), _fontIndex(fontindex), _style(0), _charmap(0)
{
}


PhysicalFontImpl::~PhysicalFontImpl () {
	if (CACHE_PATH)
		_cache.write(CACHE_PATH);
	if (!KEEP_TEMP_FILES)
		tidy();
	delete _style;
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


void PhysicalFontImpl::setStyle (double bold, double extend, double slant) {
	if (_style) {
		_style->bold = bold;
		_style->extend = extend;
		_style->slant = slant;
	}
	else if (bold != 0 || extend != 1 || slant != 0)
		_style = new Style(bold, extend, slant);
}


//////////////////////////////////////////////////////////////////////////////

VirtualFontImpl::VirtualFontImpl (string name, UInt32 cs, double ds, double ss)
	: TFMFont(name, cs, ds, ss)
{
}


VirtualFontImpl::~VirtualFontImpl () {
	// delete dvi vectors received by VFReaderAction
	for (map<UInt32, DVIVector*>::iterator i=_charDefs.begin(); i != _charDefs.end(); ++i)
		delete i->second;
}


const char* VirtualFontImpl::path () const {
	return FileFinder::lookup(name()+".vf");
}


void VirtualFontImpl::assignChar (UInt32 c, DVIVector *dvi) {
	if (dvi) {
		if (_charDefs.find(c) == _charDefs.end())
			_charDefs[c] = dvi;
		else
			delete dvi;
	}
}


const vector<UInt8>* VirtualFontImpl::getDVI (int c) const {
	map<UInt32,DVIVector*>::const_iterator it = _charDefs.find(c);
	return (it == _charDefs.end() ? 0 : it->second);
}

