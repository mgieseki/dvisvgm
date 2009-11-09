/*************************************************************************
** DVIToSVG.cpp                                                         **
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

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include "Calculator.h"
#include "CharmapTranslator.h"
#include "DVIToSVG.h"
#include "DVIToSVGActions.h"
#include "Font.h"
#include "FontManager.h"
#include "FileFinder.h"
#include "InputReader.h"
#include "Matrix.h"
#include "Message.h"
#include "PageSize.h"
#include "SVGFontEmitter.h"
#include "SVGFontTraceEmitter.h"
#include "SVGTree.h"
#include "TFM.h"
#include "XMLDocument.h"
#include "XMLDocTypeNode.h"
#include "XMLNode.h"
#include "XMLString.h"
//
///////////////////////////////////
// special handlers

#include "BgColorSpecialHandler.h"
#include "ColorSpecialHandler.h"
#include "DvisvgmSpecialHandler.h"
#include "EmSpecialHandler.h"
//#include "HtmlSpecialHandler.h"
#if !DISABLE_GS
	#include "PsSpecialHandler.h"
#endif
#include "TpicSpecialHandler.h"
///////////////////////////////////


#ifdef HAVE_CONFIG_H
#include "config.h"
#define VERSION_STR VERSION " (" TARGET_SYSTEM ")"
#else
#define VERSION_STR ""
#endif

using namespace std;

// static class variables
bool DVIToSVG::CREATE_STYLE=true;
bool DVIToSVG::USE_FONTS=true;


/** Returns time stamp of current date/time. */
static string datetime () {
	time_t t;
	time(&t);
	struct tm *tm = localtime(&t);
	char *timestr = asctime(tm);
	timestr[24] = 0;  // remove newline
	return timestr;
}


class PSHeaderActions : public DVIActions
{
	public :
		PSHeaderActions (DVIToSVG &dvisvg) : _dvisvg(dvisvg) {}

		void special (const std::string &s) {
			if (s.substr(0, 7) == "header=")
				_dvisvg.specialManager().process(s, 0);
		}

		BoundingBox& bbox () {return _bbox;}

	private:
		DVIToSVG &_dvisvg;
		BoundingBox _bbox;
};


DVIToSVG::DVIToSVG (istream &is, ostream &os)
	: DVIReader(is), _out(os)
{
	replaceActions(new DVIToSVGActions(*this, _svg));
}


DVIToSVG::~DVIToSVG () {
	delete replaceActions(0);
}


/** Starts the conversion process.
 *  @return total number of pages in DVI file */
int DVIToSVG::convert (unsigned firstPage, unsigned lastPage) {
	executePostamble();    // collect scaling and font information
	if (firstPage > getTotalPages()) {
		ostringstream oss;
		oss << "file contains only " << getTotalPages() << " page(s)";
		throw DVIException(oss.str());
	}
	if (firstPage < 0)
		firstPage = 1;

	if (firstPage > 1) {
		// ensure loading of PostScript prologues given at the beginning of the first page
		// (prologue files are always referenced in first page)
		PSHeaderActions headerActions(*this);
		DVIActions *save = replaceActions(&headerActions);
		executePage(1);
		replaceActions(save);
	}

	if (CREATE_STYLE && USE_FONTS) {
		const vector<Font*> &fonts = getFontManager().getFonts();
		if (!fonts.empty()) {
			XMLElementNode *styleNode = new XMLElementNode("style");
			styleNode->addAttribute("type", "text/css");
			_svg.appendToRoot(styleNode);
			ostringstream style;
			FORALL(fonts, vector<Font*>::const_iterator, i) {
				if (!dynamic_cast<VirtualFont*>(*i)) {  // skip virtual fonts
					style << "text.f"        << getFontManager().fontID(*i) << ' '
							<< "{font-family:" << (*i)->name()
							<< ";font-size:"   << (*i)->scaledSize() << "}\n";
				}
			}
			XMLCDataNode *cdata = new XMLCDataNode(style.str());
			styleNode->append(cdata);
		}
	}

	if (executePage(firstPage)) {  // @@
		Message::mstream() << endl;
		embedFonts(_svg.rootNode());
		_svg.write(_out);
		_svg.reset();

	}
	return getTotalPages();
}


/** This template method is called by parent class DVIReader before
 *  executing the BOP actions.
 *  @param[in] c contains information about the page (page number etc.) */
void DVIToSVG::beginPage (Int32 *c) {
	if (dynamic_cast<DVIToSVGActions*>(getActions())) {
		Message::mstream() << '[' << c[0];
		_svg.appendToDoc(new XMLCommentNode(" This file was generated by dvisvgm "VERSION_STR" "));
		_svg.appendToDoc(new XMLCommentNode(" " + datetime() + " "));
	}
}


/** This template method is called by parent class DVIReader before
 *  executing the EOP actions. */
void DVIToSVG::endPage () {
	if (!dynamic_cast<DVIToSVGActions*>(getActions()))
		return;

	_specialManager.notifyEndPage();
	Message::mstream() << ']';
	// set bounding box and apply page transformations
	BoundingBox &bbox = getActions()->bbox();
	if (!_transCmds.empty()) {
		Calculator calc;
		calc.setVariable("ux", bbox.minX());
		calc.setVariable("uy", bbox.minY());
		calc.setVariable("w", bbox.width());
		calc.setVariable("h", bbox.height());
		calc.setVariable("pt", 1);
		calc.setVariable("in", 72.27);
		calc.setVariable("cm", 72.27/2.54);
		calc.setVariable("mm", 72.27/25.4);
		Matrix matrix(_transCmds, calc);
		static_cast<DVIToSVGActions*>(getActions())->setPageMatrix(matrix);
		if (_bboxString == "min")
			bbox.transform(matrix);
	}
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
				const double border = -72.27;
				bbox = BoundingBox(border, border, size.widthInPT()+border, size.heightInPT()+border);
			}
		}
		else {
			try {
				bbox = BoundingBox(_bboxString);
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
	if (_bboxString != "none" && bbox.width() > 0) {
		_svg.setBBox(bbox);

		Message::mstream() << "\npage size: " << bbox.width() << "pt"
			" x " << bbox.height() << "pt"
			" (" << bbox.width()/72.27*25.4 << "mm"
			" x " << bbox.height()/72.27*25.4 << "mm)\n";
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

	FORALL(usedChars, UsedCharsMap::const_iterator, it) {
		const Font *font = it->first;
		if (const PhysicalFont *ph_font = dynamic_cast<const PhysicalFont*>(font)) {
			CharmapTranslator *cmt = svgActions->getCharmapTranslator(font);
			// If the same character is used in various sizes we don't want to embed the complete (lengthy) path
			// description multiple times because they would only differ by a scale factor. Thus it's better to
			// reference the already embedded path together with a transformation attribute and let the SVG renderer
			// scale the glyph properly. This is only necessary if we don't want to use font but path elements.
			if (font != font->uniqueFont()) {
				FORALL(it->second, set<int>::const_iterator, cit) {
					ostringstream oss;
					XMLElementNode *use = new XMLElementNode("use");
					oss << 'g' << getFontManager().fontID(font) << *cit;
					use->addAttribute("id", oss.str());
					oss.str("");
					oss << "#g" << getFontManager().fontID(font->uniqueFont()) << *cit;
					use->addAttribute("xlink:href", oss.str());
					oss.str("");
					oss << "scale(" << (font->scaledSize()/font->uniqueFont()->scaledSize()) << ')';
					use->addAttribute("transform", oss.str());
					_svg.appendToDefs(use);
				}
			}
			else {
				if (ph_font->type() == PhysicalFont::MF) {
					SVGFontTraceEmitter emitter(font, getFontManager(), *cmt, _svg, USE_FONTS);
					emitter.emitFont(it->second, font->name().c_str());
				}
				else if (font->path()) { // path to pfb/ttf file
					SVGFontEmitter emitter(font, getFontManager(), *cmt, _svg, USE_FONTS);
					emitter.emitFont(it->second, font->name().c_str());
				}
				else
					Message::wstream(true) << "can't embed font '" << font->name() << "'\n";
			}
		}
		else
			Message::wstream(true) << "can't embed font '" << font->name() << "'\n";
	}
}


/** Enables or disables processing of specials. If ignorelist == 0, all
 *  supported special handlers are loaded. To disable selected sets of specials,
 *  the corresponding prefixes can be given separated by non alpha-numeric characters,
 *  e.g. "color, ps, em" or "color: ps em" etc.
 *  A single "*" in the ignore list disables all specials.
 *  @param[in] ignorelist list of special prefixes to ignore
 *  @return the SpecialManager that handles special statements */
const SpecialManager* DVIToSVG::setProcessSpecials (const char *ignorelist) {
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
//			new HtmlSpecialHandler,     // handles hyperref specials
			new TpicSpecialHandler,     // handles tpic specials
			0
		};
		SpecialHandler **p = handlers;
#if !DISABLE_GS
		if (Ghostscript().available())
			*p = new PsSpecialHandler;
		else
#endif
			p++;
		_specialManager.unregisterHandlers();
		_specialManager.registerHandlers(p, ignorelist);
	}
	return &_specialManager;
}

