/*************************************************************************
** dvisvgm.cpp                                                          **
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

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include "gzstream.h"
#include "CommandLine.h"
#include "DVIToSVG.h"
#include "DVIToSVGActions.h"
#include "FileSystem.h"
#include "FontCache.h"
#include "InputReader.h"
#include "Message.h"
#include "FileFinder.h"
#include "PageSize.h"
#include "SpecialManager.h"
#include "StreamCounter.h"
#include "SVGFontTraceEmitter.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined (HAVE_SYS_TIMEB_H)
#include <sys/timeb.h>
#endif

using namespace std;


/** Simple private auto pointer class to simplify pointer handling in main function. */
template <typename T>
class Pointer
{
	public:
		Pointer (T *p=0, bool free=true) : _p(p), _free(free) {}
		Pointer (const Pointer &p) : _p(p._p), _free(p._free) {p._p = 0;}
		~Pointer ()       {if (_free) delete _p;}
		T& operator * ()  {return *_p;}
		void operator = (const Pointer &p) {_p=p._p; _free=p._free; p._p=0;}
		void release () {
			if (_free) delete _p;
			_p=0;
		}

	private:
		mutable T *_p;
		bool _free; ///< delete pointer in destructor?
};


static void show_help (const CommandLine &cmd) {
	cout << PACKAGE_STRING "\n\n";
	cmd.help();
   cout << "\nCopyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> \n\n";
}


static string remove_path (string fname) {
	fname = FileSystem::adaptPathSeperators(fname);
	size_t slashpos = fname.rfind('/');
	if (slashpos == string::npos)
		return fname;
	return fname.substr(slashpos+1);
}


static string ensure_suffix (string fname, const string &suffix) {
	size_t dotpos = remove_path(fname).rfind('.');
	if (dotpos == string::npos) {
		dotpos = fname.length();
		fname += "."+suffix;
	}
	return fname;
}


static string remove_suffix (string fname) {
	size_t dotpos = fname.rfind('.');
	if (dotpos == string::npos)
		return fname;
	return fname.substr(0, dotpos);
}


/** Returns timestamp (wall time) in seconds. */
static double get_time () {
#if defined (HAVE_SYS_TIME_H)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec/1000000.0;
#elif defined (HAVE_SYS_TIMEB_H)
	struct timeb tb;
	ftime(&tb);
	return tb.time + tb.millitm/1000.0;
#else
	time_t myclock = time((time_t*)NULL);
	return myclock;
#endif
}


static void set_trans (DVIToSVG &dvisvg, const CommandLine &args) {
	ostringstream oss;
	if (args.rotate_given())
		oss << 'R' << args.rotate_arg() << ",w/2,h/2";
	if (args.translate_given())
		oss << 'T' << args.translate_arg();
	if (args.scale_given())
		oss << 'S' << args.scale_arg();
	if (args.transform_given())
		oss << args.transform_arg();
	dvisvg.setTransformation(oss.str());
}


static bool set_cache_dir (const CommandLine &args) {
	if (args.cache_given() && !args.cache_arg().empty()) {
		if (args.cache_arg() == "none")
			SVGFontTraceEmitter::CACHE_PATH = 0;
		else if (FileSystem::exists(args.cache_arg().c_str()))
			SVGFontTraceEmitter::CACHE_PATH = args.cache_arg().c_str();
		else
			Message::wstream(true) << "cache directory '" << args.cache_arg() << "' does not exist (caching disabled)" << endl;
	}
	else {
		const char *userdir = FileSystem::userdir();
		if (userdir) {
			static string path = userdir;
			path += "/.dvisvgm";
			path = FileSystem::adaptPathSeperators(path);
			if (!FileSystem::exists(path.c_str()))
				FileSystem::mkdir(path.c_str());
			SVGFontTraceEmitter::CACHE_PATH = path.c_str();
		}
		if (args.cache_given() && args.cache_arg().empty()) {
			cout << "cache directory: " << (SVGFontTraceEmitter::CACHE_PATH ? SVGFontTraceEmitter::CACHE_PATH : "(none)") << endl;
			FontCache::fontinfo(SVGFontTraceEmitter::CACHE_PATH, cout);
			return false;
		}
	}
	return true;
}


static bool check_bbox (const string &bboxstr) {
	const char *formats[] = {"none", "min", "dvi", 0};
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
		BoundingBox bbox;
		bbox.set(bboxstr);
		return true;
	}
	catch (const MessageException &e) {
		Message::estream(true) << e.getMessage() << endl;
		return false;
	}
}


int main (int argc, char *argv[]) {
	CommandLine args;
	args.parse(argc, argv);
	if (args.error())
		return 1;

	if (args.version_given()) {
		cout << PACKAGE_STRING "\n";
		return 0;
	}
	if (args.list_specials_given()) {
		DVIToSVG dvisvg(cin, cout);
		if (const SpecialManager *sm = dvisvg.setProcessSpecials())
			sm->writeHandlerInfo(cout);
		return 0;
	}

	if (!set_cache_dir(args))
		return 0;

	if (argc == 1 || args.help_given()) {
		show_help(args);
		return 0;
	}

	if (argc > 1 && args.numFiles() < 1) {
		Message::estream(true) << "no input file given" << endl;
		return 1;
	}

	if (args.progress_given())
		DVIToSVGActions::PROGRESSBAR = args.progress_arg()+1;

	if (args.stdout_given() && args.zip_given()) {
		Message::estream(true) << "writing SVGZ files to stdout is not supported\n";
		return 1;
	}
	if (args.map_file_given())
		FileFinder::setUserFontMap(args.map_file_arg().c_str());

	if (!check_bbox(args.bbox_arg()))
		return 1;

	DVIToSVG::CREATE_STYLE = !args.no_styles_given();
	DVIToSVG::USE_FONTS = !args.no_fonts_given();
	SVGFontTraceEmitter::TRACE_ALL = args.trace_all_given();
	SVGFontTraceEmitter::METAFONT_MAG = args.mag_arg();

	double start_time = get_time();

	string dvifile = ensure_suffix(args.file(0), "dvi");
	string svgfile = args.output_given() ? args.output_arg() : remove_suffix(remove_path(dvifile));
	svgfile = ensure_suffix(svgfile, args.zip_given() ? "svgz" : "svg");

	ifstream ifs(dvifile.c_str(), ios_base::binary|ios_base::in);
   if (!ifs)
      Message::estream(true) << "can't open file '" << dvifile << "' for reading\n";
	else {
		Pointer<ostream> out;
		if (args.stdout_given())
			out = Pointer<ostream>(&cout, false);
		else if (args.zip_given())
			out = Pointer<ostream>(new ogzstream(svgfile.c_str(), args.zip_arg()));
		else
			out = Pointer<ostream>(new ofstream(svgfile.c_str(), ios_base::binary));

		if (!*out)
      	Message::estream(true) << "can't open file '" << svgfile << "' for writing\n";
		else {
			StreamCounter<char> sc(*out);
			Message::level = args.verbosity_arg();
			DVIToSVG dvisvg(ifs, *out);
			const char *ignore_specials = args.no_specials_given() ? (args.no_specials_arg().empty() ? "*" : args.no_specials_arg().c_str()) : 0;
			dvisvg.setProcessSpecials(ignore_specials);
			set_trans(dvisvg, args);
			dvisvg.setPageSize(args.bbox_arg());

			try {
				FileFinder::init(argv[0], !args.no_mktexmf_given());
				if (int pages = dvisvg.convert(args.page_arg(), args.page_arg())) {
					if (!args.stdout_given())
						sc.invalidate();  // output buffer is no longer valid
					// valgrind issues an invalid conditional jump/move in the deflate function of libz here.
					// According to libz FAQ #36 this is not a bug but intended behavior.
					out.release();       // force writing
					const char *pstr = pages == 1 ? "" : "s";
					UInt64 nbytes = args.stdout_given() ? sc.count() : FileSystem::filesize(svgfile);
					Message::mstream() << "1 ";
					if (pages > 1)
						Message::mstream() << "of " << pages << " ";
					Message::mstream() << "page" << pstr << " written to "
						                << (args.stdout_given() ? "<stdout>" : svgfile)
					                   << " (" << nbytes << " bytes";
					if (args.zip_given())
						Message::mstream() << " = " << floor(double(nbytes)/sc.count()*100.0+0.5) << "%";
					Message::mstream() << ") in " << (get_time()-start_time) << " seconds\n";
				}
			}
			catch (DVIException &e) {
				Message::estream() << "DVI error: " << e.getMessage() << endl;
			}
			catch (MessageException &e) {
				Message::estream(true) << e.getMessage() << endl;
			}
		}
	}
	return 0;
}

