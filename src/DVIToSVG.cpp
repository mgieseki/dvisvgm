/*************************************************************************
** DVIToSVG.cpp                                                         **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <config.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include "Calculator.h"
#include "DVIToSVG.h"
#include "DVIToSVGActions.h"
#include "Font.h"
#include "FontManager.h"
#include "FileFinder.h"
#include "GlyphTracerMessages.h"
#include "InputBuffer.h"
#include "InputReader.h"
#include "Matrix.h"
#include "Message.h"
#include "PageRanges.h"
#include "PageSize.h"
#include "SVGOutputBase.h"
//
///////////////////////////////////
// special handlers

#include "BgColorSpecialHandler.h"
#include "ColorSpecialHandler.h"
#include "DvisvgmSpecialHandler.h"
#include "EmSpecialHandler.h"
#include "PdfSpecialHandler.h"
#include "HtmlSpecialHandler.h"
#ifndef HAVE_LIBGS
	#include "NoPsSpecialHandler.h"
#endif
#ifndef DISABLE_GS
	#include "PsSpecialHandler.h"
#endif
#include "TpicSpecialHandler.h"

///////////////////////////////////


using namespace std;


class PSHeaderActions : public DVIActions
{
	public :
		PSHeaderActions (DVIToSVG &dvisvg) : _dvisvg(dvisvg) {}

		void special (const std::string &str, double dvi2bp) {
			// execute PS headers only
			if (!str.empty() && (str[0] == '!' || str.substr(0, 7) == "header="))
				_dvisvg.specialManager().process(str, dvi2bp, 0);
		}

		bool fontProcessingEnabled () const {
			return false;
		}

		void endPage (unsigned) {
			_dvisvg.specialManager().leavePSHeaderSection();
		}

		BoundingBox& bbox () {return _bbox;}

	private:
		DVIToSVG &_dvisvg;
		BoundingBox _bbox;
};


/** 'a': trace all glyphs even if some of them are already cached
 *  'm': trace missing glyphs, i.e. glyphs not yet cached
 *   0 : only trace actually required glyphs */
char DVIToSVG::TRACE_MODE = 0;


DVIToSVG::DVIToSVG (istream &is, SVGOutputBase &out) : DVIReader(is), _out(out)
{
	replaceActions(new DVIToSVGActions(*this, _svg));
}


DVIToSVG::~DVIToSVG () {
	delete replaceActions(0);
}


/** Starts the conversion process.
 *  @param[in] first number of first page to convert
 *  @param[in] last number of last page to convert
 *  @param[out] pageinfo (number of converted pages, number of total pages) */
void DVIToSVG::convert (unsigned first, unsigned last, pair<int,int> *pageinfo) {
	if (getTotalPages() == 0) {
		executePreamble();
		executePostamble();    // collect scaling information
	}
	if (first > last)
		swap(first, last);
	if (first > getTotalPages()) {
		ostringstream oss;
		oss << "file contains only " << getTotalPages() << " page";
		if (getTotalPages() > 1)
			oss << 's';
		throw DVIException(oss.str());
	}
	last = min(last, getTotalPages());

#ifndef DISABLE_GS
	// ensure loading of PostScript prologues given at the beginning of the first page
	// (prologue files are always referenced in first page)
	PSHeaderActions headerActions(*this);
	DVIActions *save = replaceActions(&headerActions);
	try {
		executePage(1);
		replaceActions(save);
	}
	catch (PSException &e) {
		replaceActions(save);
		throw;
	}
#endif

	for (unsigned i=first; i <= last; ++i) {
		executePage(i);
		embedFonts(_svg.rootNode());
		_svg.write(_out.getPageStream(getCurrentPageNumber(), getTotalPages()));
		string fname = _out.filename(i, getTotalPages());
		Message::mstream(false, Message::MC_PAGE_WRITTEN) << "\npage written to " << (fname.empty() ? "<stdout>" : fname) << '\n';
		_svg.reset();
		static_cast<DVIToSVGActions*>(getActions())->reset();
	}
	if (pageinfo) {
		pageinfo->first = last-first+1;
		pageinfo->second = getTotalPages();
	}
}


/** Convert DVI pages specified by a page range string.
 *  @param[in] rangestr string describing the pages to convert
 *  @param[out] pageinfo (number of converted pages, number of total pages) */
void DVIToSVG::convert (const string &rangestr, pair<int,int> *pageinfo) {
	if (getTotalPages() == 0) {
		executePreamble();
		executePostamble();    // collect scaling information
	}
	PageRanges ranges;
	if (!ranges.parse(rangestr, getTotalPages()))
		throw MessageException("invalid page range format");
	FORALL(ranges, PageRanges::ConstIterator, it)
		convert(it->first, it->second);
	if (pageinfo) {
		pageinfo->first = ranges.pages();
		pageinfo->second = getTotalPages();
	}
}


/** This template method is called by parent class DVIReader before
 *  executing the BOP actions.
 *  @param[in] pageno physical page number (1 = first page)
 *  @param[in] c contains information about the page (page number etc.) */
void DVIToSVG::beginPage (unsigned pageno, Int32 *c) {
	if (dynamic_cast<DVIToSVGActions*>(getActions())) {
		Message::mstream().indent(0);
		Message::mstream(false, Message::MC_PAGE_NUMBER) << "processing page " << pageno;
		if (pageno != (unsigned)c[0])  // Does page number shown on page differ from physical page number?
			Message::mstream(false) << " [" << c[0] << ']';
		Message::mstream().indent(1);
		_svg.appendToDoc(new XMLCommentNode(" This file was generated by dvisvgm " VERSION " "));
	}
}


/** This template method is called by parent class DVIReader before
 *  executing the EOP actions. */
void DVIToSVG::endPage (unsigned pageno) {
	if (!dynamic_cast<DVIToSVGActions*>(getActions()))
		return;

	_specialManager.notifyEndPage(pageno);
	// set bounding box and apply page transformations
	BoundingBox &bbox = getActions()->bbox();
	bbox.unlock();
	Matrix matrix;
	getPageTransformation(matrix);
	static_cast<DVIToSVGActions*>(getActions())->setPageMatrix(matrix);
	if (_bboxString == "min")
		bbox.transform(matrix);
	if (string("dvi none min").find(_bboxString) == string::npos) {
		istringstream iss(_bboxString);
		StreamInputReader ir(iss);
		ir.skipSpace();
		if (isalpha(ir.peek())) {
			// set explicitly given page format
			PageSize size(_bboxString);
			if (size.valid()) {
				// convention: DVI position (0,0) equals (1in, 1in) relative
				// to the upper left vertex of the page (see DVI specification)
				const double border = -72;
				bbox = BoundingBox(border, border, size.widthInBP()+border, size.heightInBP()+border);
			}
		}
		else { // set/modify bounding box by explicitly given values
			try {
				bbox.set(_bboxString);
			}
			catch (const MessageException &e) {
			}
		}
	}
	else if (_bboxString == "dvi") {
		// center page content
		double dx = (getPageWidth()-bbox.width())/2;
		double dy = (getPageHeight()-bbox.height())/2;
		bbox += BoundingBox(-dx, -dy, dx, dy);
	}
	if (_bboxString != "none") {
		if (bbox.width() == 0)
			Message::wstream(false) << "\npage is empty\n";
		else {
			_svg.setBBox(bbox);

			const double bp2pt = 72.27/72;
			const double bp2mm = 25.4/72;
			Message::mstream(false) << '\n';
			Message::mstream(false, Message::MC_PAGE_SIZE) << "page size: " << XMLString(bbox.width()*bp2pt) << "pt"
				" x " << XMLString(bbox.height()*bp2pt) << "pt"
				" (" << XMLString(bbox.width()*bp2mm) << "mm"
				" x " << XMLString(bbox.height()*bp2mm) << "mm)";
			Message::mstream(false) << '\n';
		}
	}
}


void DVIToSVG::getPageTransformation(Matrix &matrix) const {
	if (_transCmds.empty())
		matrix.set(1);  // unity matrix
	else {
		Calculator calc;
		if (getActions()) {
			const double bp2pt = 72.27/72;
			BoundingBox &bbox = getActions()->bbox();
			calc.setVariable("ux", bbox.minX()*bp2pt);
			calc.setVariable("uy", bbox.minY()*bp2pt);
			calc.setVariable("w",  bbox.width()*bp2pt);
			calc.setVariable("h",  bbox.height()*bp2pt);
		}
		calc.setVariable("pt", 1);
		calc.setVariable("in", 72.27);
		calc.setVariable("cm", 72.27/2.54);
		calc.setVariable("mm", 72.27/25.4);
		matrix.set(_transCmds, calc);
	}
}


static void collect_chars (map<const Font*, set<int> > &fm) {
	typedef const map<const Font*, set<int> > UsedCharsMap;
	FORALL(fm, UsedCharsMap::const_iterator, it) {
		if (it->first->uniqueFont() != it->first) {
			FORALL(it->second, set<int>::const_iterator, cit)
				fm[it->first->uniqueFont()].insert(*cit);
		}
	}
}


/** Adds the font information to the SVG tree.
 *  @param[in] svgElement the font nodes are added to this node */
void DVIToSVG::embedFonts (XMLElementNode *svgElement) {
	if (!svgElement)
		return;
	if (!getActions())  // no dvi actions => no chars written => no fonts to embed
		return;

	typedef map<const Font*, set<int> > UsedCharsMap;
	const DVIToSVGActions *svgActions = static_cast<DVIToSVGActions*>(getActions());
	UsedCharsMap &usedChars = svgActions->getUsedChars();

	collect_chars(usedChars);

	GlyphTracerMessages messages;
	FORALL(usedChars, UsedCharsMap::const_iterator, it) {
		const Font *font = it->first;
		if (const PhysicalFont *ph_font = dynamic_cast<const PhysicalFont*>(font)) {
			if (TRACE_MODE)
				ph_font->traceAllGlyphs(TRACE_MODE == 'a', &messages);

			if (font->path())  // does font file exist?
				_svg.append(*ph_font, it->second, &messages);
			else
				Message::wstream(true) << "can't embed font '" << font->name() << "'\n";
		}
		else
			Message::wstream(true) << "can't embed font '" << font->name() << "'\n";
	}
	_svg.appendFontStyles(svgActions->getUsedFonts());
}


/** Enables or disables processing of specials. If ignorelist == 0, all
 *  supported special handlers are loaded. To disable selected sets of specials,
 *  the corresponding prefixes can be given separated by non alpha-numeric characters,
 *  e.g. "color, ps, em" or "color: ps em" etc.
 *  A single "*" in the ignore list disables all specials.
 *  @param[in] ignorelist list of special prefixes to ignore
 *  @param[in] pswarning if true, shows warning about disabled PS support
 *  @return the SpecialManager that handles special statements */
const SpecialManager* DVIToSVG::setProcessSpecials (const char *ignorelist, bool pswarning) {
	if (ignorelist && strcmp(ignorelist, "*") == 0) { // ignore all specials?
		_specialManager.unregisterHandlers();
	}
	else {
		// add special handlers
		SpecialHandler *handlers[] = {
			0,                          // placeholder for PsSpecialHandler
			new BgColorSpecialHandler,  // handles background color special
			new ColorSpecialHandler,    // handles color specials
			new DvisvgmSpecialHandler,  // handles raw SVG embeddings
			new EmSpecialHandler,       // handles emTeX specials
			new HtmlSpecialHandler,     // handles hyperref specials
			new PdfSpecialHandler,      // handles pdf specials
			new TpicSpecialHandler,     // handles tpic specials
			0
		};
		SpecialHandler **p = handlers;
#ifndef DISABLE_GS
		if (Ghostscript().available())
			*p = new PsSpecialHandler;
		else
#endif
		{
#ifndef HAVE_LIBGS
			*p = new NoPsSpecialHandler; // dummy PS special handler that only prints warning messages
			if (pswarning) {
#ifdef DISABLE_GS
				Message::wstream() << "processing of PostScript specials has been disabled permanently\n";
#else
				Message::wstream() << "processing of PostScript specials is disabled (Ghostscript not found)\n";
#endif
			}
#endif
		}
		_specialManager.unregisterHandlers();
		_specialManager.registerHandlers(p, ignorelist);
	}
	return &_specialManager;
}
