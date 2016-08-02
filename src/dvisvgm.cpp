/*************************************************************************
** dvisvgm.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <clipper.hpp>
#include <fstream>
#include <iostream>
#include <potracelib.h>
#include <sstream>
#include <xxhash.h>
#include <zlib.h>
#include "CommandLine.hpp"
#include "DVIToSVG.hpp"
#include "DVIToSVGActions.hpp"
#include "EPSToSVG.hpp"
#include "FileFinder.hpp"
#include "FileSystem.hpp"
#include "Font.hpp"
#include "FontEngine.hpp"
#include "Ghostscript.hpp"
#include "HtmlSpecialHandler.hpp"
#include "Message.hpp"
#include "PageSize.hpp"
#include "PSInterpreter.hpp"
#include "PsSpecialHandler.hpp"
#include "SignalHandler.hpp"
#include "SVGOutput.hpp"
#include "System.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

static string remove_path (string fname) {
	fname = FileSystem::adaptPathSeperators(fname);
	size_t slashpos = fname.rfind('/');
	if (slashpos == string::npos)
		return fname;
	return fname.substr(slashpos+1);
}


static string ensure_suffix (string fname, bool eps) {
	size_t dotpos = remove_path(fname).rfind('.');
	if (dotpos == string::npos)
		fname += (eps ? ".eps" : ".dvi");
	return fname;
}


static string get_transformation_string (const CommandLine &args) {
	ostringstream oss;
	if (args.rotateOpt.given())
		oss << 'R' << args.rotateOpt.value() << ",w/2,h/2";
	if (args.translateOpt.given())
		oss << 'T' << args.translateOpt.value();
	if (args.scaleOpt.given())
		oss << 'S' << args.scaleOpt.value();
	if (args.transformOpt.given())
		oss << args.transformOpt.value();
	return oss.str();
}


static void set_libgs (CommandLine &args) {
#if !defined(DISABLE_GS) && !defined(HAVE_LIBGS)
	if (args.libgsOpt.given())
		Ghostscript::LIBGS_NAME = args.libgsOpt.value();
	else if (getenv("LIBGS"))
		Ghostscript::LIBGS_NAME = getenv("LIBGS");
#endif
}


static bool set_cache_dir (const CommandLine &args) {
	if (args.cacheOpt.given() && !args.cacheOpt.value().empty()) {
		if (args.cacheOpt.value() == "none")
			PhysicalFont::CACHE_PATH = 0;
		else if (FileSystem::exists(args.cacheOpt.value()))
			PhysicalFont::CACHE_PATH = args.cacheOpt.value().c_str();
		else
			Message::wstream(true) << "cache directory '" << args.cacheOpt.value() << "' does not exist (caching disabled)\n";
	}
	else if (const char *userdir = FileSystem::userdir()) {
		static string cachepath = userdir + string("/.dvisvgm/cache");
		if (!FileSystem::exists(cachepath))
			FileSystem::mkdir(cachepath);
		PhysicalFont::CACHE_PATH = cachepath.c_str();
	}
	if (args.cacheOpt.given() && args.cacheOpt.value().empty()) {
		cout << "cache directory: " << (PhysicalFont::CACHE_PATH ? PhysicalFont::CACHE_PATH : "(none)") << '\n';
		try {
			FontCache::fontinfo(PhysicalFont::CACHE_PATH, cout, true);
		}
		catch (StreamReaderException &e) {
			Message::wstream(true) << "failed reading cache data";
		}
		return false;
	}
	return true;
}


static bool check_bbox (const string &bboxstr) {
	const char *formats[] = {"none", "min", "preview", "papersize", "dvi", 0};
	for (const char **p=formats; *p; ++p)
		if (bboxstr == *p)
			return true;
	if (isalpha(bboxstr[0])) {
		try {
			PageSize size(bboxstr);
			return true;
		}
		catch (const PageSizeException &e) {
			Message::estream(true) << "invalid bounding box format '" << bboxstr << "'\n";
			return false;
		}
	}
	try {
		// check if given bbox argument is valid, i.e. doesn't throw an exception
		BoundingBox bbox;
		bbox.set(bboxstr);
		return true;
	}
	catch (const MessageException &e) {
		Message::estream(true) << e.what() << '\n';
		return false;
	}
}


static void print_version (bool extended) {
	ostringstream oss;
	oss << PACKAGE_STRING;
	if (extended) {
		if (strlen(TARGET_SYSTEM) > 0)
			oss << " (" TARGET_SYSTEM ")";
		int len = oss.str().length();
		oss << "\n" << string(len, '-') << "\n"
			"clipper:     " << CLIPPER_VERSION "\n"
			"freetype:    " << FontEngine::version() << "\n";

		Ghostscript gs;
		string gsver = gs.revision(true);
		if (!gsver.empty())
			oss << "Ghostscript: " << gsver + "\n";
		const unsigned xxh_ver = XXH_versionNumber();
		oss <<
#ifdef MIKTEX
			"MiKTeX:      " << FileFinder::version() << "\n"
#else
			"kpathsea:    " << FileFinder::version() << "\n"
#endif
			"potrace:     " << (strchr(potrace_version(), ' ') ? strchr(potrace_version(), ' ')+1 : "unknown") << "\n"
			"xxhash:      " << xxh_ver/10000 << '.' << (xxh_ver/100)%100 << '.' << xxh_ver%100 << "\n"
			"zlib:        " << zlibVersion();
	}
	cout << oss.str() << endl;
}


static void init_fontmap (const CommandLine &args) {
	const char *mapseq = args.fontmapOpt.given() ? args.fontmapOpt.value().c_str() : 0;
	bool additional = mapseq && strchr("+-=", *mapseq);
	if (!mapseq || additional) {
		const char *mapfiles[] = {"ps2pk.map", "dvipdfm.map", "psfonts.map", 0};
		bool found = false;
		for (const char **p=mapfiles; *p && !found; p++)
			found = FontMap::instance().read(*p);
		if (!found)
			Message::wstream(true) << "none of the default map files could be found";
	}
	if (mapseq)
		FontMap::instance().read(mapseq);
}


int main (int argc, char *argv[]) {
	CommandLine args;
	try {
		args.parse(argc, argv);
	}
	catch (CL::CommandLineException &e) {
		Message::estream() << e.what() << '\n';
		return 1;
	}

	if (argc == 1 || args.helpOpt.given()) {
		args.help(cout, args.helpOpt.value());
		return 0;
	}

	Message::COLORIZE = args.colorOpt.given();

	try {
		FileFinder::init(argv[0], "dvisvgm", !args.noMktexmfOpt.given());
	}
	catch (MessageException &e) {
		Message::estream(true) << e.what() << '\n';
		return 0;
	}

	set_libgs(args);
	if (args.versionOpt.given()) {
		print_version(args.versionOpt.value());
		return 0;
	}
	if (args.listSpecialsOpt.given()) {
		DVIToSVG::setProcessSpecials();
		SpecialManager::instance().writeHandlerInfo(cout);
		return 0;
	}

	if (!set_cache_dir(args))
		return 0;

	if (argc > 1 && args.filenames().size() < 1) {
		Message::estream(true) << "no input file given\n";
		return 1;
	}

	if (args.stdoutOpt.given() && args.zipOpt.given()) {
		Message::estream(true) << "writing SVGZ files to stdout is not supported\n";
		return 1;
	}

	if (!check_bbox(args.bboxOpt.value()))
		return 1;

	if (args.progressOpt.given()) {
		DVIToSVG::COMPUTE_PROGRESS = args.progressOpt.given();
		SpecialActions::PROGRESSBAR_DELAY = args.progressOpt.value();
	}
	Color::SUPPRESS_COLOR_NAMES = !args.colornamesOpt.given();
	SVGTree::CREATE_CSS = !args.noStylesOpt.given();
	SVGTree::USE_FONTS = !args.noFontsOpt.given();
	SVGTree::CREATE_USE_ELEMENTS = args.noFontsOpt.value() < 1;
	SVGTree::ZOOM_FACTOR = args.zoomOpt.value();
	SVGTree::RELATIVE_PATH_CMDS = args.relativeOpt.given();
	SVGTree::MERGE_CHARS = !args.noMergeOpt.given();
	SVGTree::ADD_COMMENTS = args.commentsOpt.given();
	DVIToSVG::TRACE_MODE = args.traceAllOpt.given() ? (args.traceAllOpt.value() ? 'a' : 'm') : 0;
	Message::LEVEL = args.verbosityOpt.value();
	PhysicalFont::EXACT_BBOX = args.exactOpt.given();
	PhysicalFont::KEEP_TEMP_FILES = args.keepOpt.given();
	PhysicalFont::METAFONT_MAG = max(1.0, args.magOpt.value());
	XMLString::DECIMAL_PLACES = max(0, min(6, args.precisionOpt.value()));
	if (!HtmlSpecialHandler::setLinkMarker(args.linkmarkOpt.value()))
		Message::wstream(true) << "invalid argument '"+args.linkmarkOpt.value()+"' supplied for option --linkmark\n";
	double start_time = System::time();
	bool eps_given=false;
#ifndef DISABLE_GS
	eps_given = args.epsOpt.given();
	PsSpecialHandler::COMPUTE_CLIPPATHS_INTERSECTIONS = args.clipjoinOpt.given();
	PsSpecialHandler::SHADING_SEGMENT_OVERLAP = args.gradOverlapOpt.given();
	PsSpecialHandler::SHADING_SEGMENT_SIZE = max(1, args.gradSegmentsOpt.value());
	PsSpecialHandler::SHADING_SIMPLIFY_DELTA = args.gradSimplifyOpt.value();
#endif
	string inputfile = ensure_suffix(args.filenames()[0], eps_given);
	ifstream ifs(inputfile.c_str(), ios::binary|ios::in);
	if (!ifs) {
		Message::estream(true) << "can't open file '" << inputfile << "' for reading\n";
		return 0;
	}
	try {
		SVGOutput out(args.stdoutOpt.given() ? 0 : inputfile.c_str(), args.outputOpt.value(), args.zipOpt.given() ? args.zipOpt.value() : 0);
		SignalHandler::instance().start();
#ifndef DISABLE_GS
		if (args.epsOpt.given()) {
			EPSToSVG eps2svg(inputfile, out);
			eps2svg.convert();
			Message::mstream().indent(0);
			Message::mstream(false, Message::MC_PAGE_NUMBER)
				<< "file converted in " << (System::time()-start_time) << " seconds\n";
		}
		else
#endif
		{
			init_fontmap(args);
			DVIToSVG dvi2svg(ifs, out);
			const char *ignore_specials = args.noSpecialsOpt.given() ? (args.noSpecialsOpt.value().empty() ? "*" : args.noSpecialsOpt.value().c_str()) : 0;
			dvi2svg.setProcessSpecials(ignore_specials, true);
			dvi2svg.setPageTransformation(get_transformation_string(args));
			dvi2svg.setPageSize(args.bboxOpt.value());

			pair<int,int> pageinfo;
			dvi2svg.convert(args.pageOpt.value(), &pageinfo);
			Message::mstream().indent(0);
			Message::mstream(false, Message::MC_PAGE_NUMBER) << "\n" << pageinfo.first << " of " << pageinfo.second << " page";
			if (pageinfo.second > 1)
				Message::mstream(false, Message::MC_PAGE_NUMBER) << 's';
			Message::mstream(false, Message::MC_PAGE_NUMBER) << " converted in " << (System::time()-start_time) << " seconds\n";
		}
	}
	catch (DVIException &e) {
		Message::estream() << "\nDVI error: " << e.what() << '\n';
	}
	catch (PSException &e) {
		Message::estream() << "\nPostScript error: " << e.what() << '\n';
	}
	catch (SignalException &e) {
		Message::wstream().clearline();
		Message::wstream(true) << "execution interrupted by user\n";
	}
	catch (MessageException &e) {
		Message::estream(true) << e.what() << '\n';
	}
	FileFinder::finish();
	return 0;
}

