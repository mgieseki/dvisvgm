/***********************************************************************
** dvisvgm.cpp                                                        **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/timeb.h>
#include <time.h>
#include "cmdline.h"
#include "gzstream.h"
#include "DVIToSVG.h"
#include "FileSystem.h"
#include "Message.h"
#include "FileFinder.h"
#include "SpecialManager.h"
#include "StreamCounter.h"
#include "SVGFontTraceEmitter.h"
#include "debug.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#define EMAIL " <" PACKAGE_BUGREPORT ">"
#else
#define EMAIL "Martin Gieseking <martin.gieseking@uos.de>"
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


static void show_help () {
   cmdline_parser_print_help();
   cout << "\nCopyright (C) 2005-2009 Martin Gieseking" EMAIL "\n\n";
}


static char* tolower (char *str) {
	for (char *p=str; *p; ++p)
		*p = tolower(*p);
	return str;
}


static string remove_path (string fname) {
	fname = FileSystem::adaptPathSeperators(fname);
	unsigned slashpos = fname.rfind('/');
	if (slashpos == string::npos)
		return fname;
	return fname.substr(slashpos+1);
}


static string ensure_suffix (string fname, string suffix) {
	unsigned dotpos = remove_path(fname).rfind('.');
	if (dotpos == string::npos) {
		dotpos = fname.length();
		fname += "."+suffix;		
	}
	return fname;
}


static string remove_suffix (string fname) {
	unsigned dotpos = fname.rfind('.');
	if (dotpos == string::npos) 
		return fname;
	return fname.substr(0, dotpos);
}


/** Returns timestamp (wall time) in seconds. */
static double get_time () {
	struct timeb tb;
	ftime(&tb);
	return tb.time + tb.millitm/1000.0;
}


static void set_trans (DVIToSVG &dvisvg, const gengetopt_args_info &args) {
	ostringstream oss;
	if (args.rotate_given)
		oss << 'R' << args.rotate_arg << ",w/2,h/2";
	if (args.translate_given)
		oss << 'T' << args.translate_arg;
	if (args.scale_given)
		oss << 'S' << args.scale_arg;
	if (args.transform_given)
		oss << args.transform_arg;
	dvisvg.setTransformation(oss.str());
}


static bool set_cache_dir (const gengetopt_args_info &args) {
	if (args.cache_given && strcmp(args.cache_arg, "?") != 0) {
		if (strcmp(args.cache_arg, "none") == 0) 
			SVGFontTraceEmitter::CACHE_PATH = 0;
		else if (FileSystem::exists(args.cache_arg))
			SVGFontTraceEmitter::CACHE_PATH = args.cache_arg;
		else
			Message::wstream(true) << "cache directory " << args.cache_arg << " does not exist (caching disabled)" << endl;
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
		if (args.cache_given && strcmp(args.cache_arg, "?") == 0) {
			cout << "cache directory: " << (SVGFontTraceEmitter::CACHE_PATH ? SVGFontTraceEmitter::CACHE_PATH : "(none)") << endl;
			return false;
		}
	}
	return true;
}


int main (int argc, char *argv[]) {
	struct gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args))
		return 1;

	if (args.version_given) {
		cout << PACKAGE_NAME " " PACKAGE_VERSION << endl;
		return 0;
	}
	if (args.list_specials_given) {
		DVIToSVG dvisvg(cin, cout);
		if (const SpecialManager *sm = dvisvg.setProcessSpecials())
			sm->writeHandlerInfo(cout);
		return 0;
	}

	if (!set_cache_dir(args))
		return 0;

	if (args.inputs_num < 1 || args.help_given) {
		show_help();
		return 0;
	}

	if (args.stdout_given && args.zip_given) {
		Message::estream(true) << "writing SVGZ files to stdout is not supported\n";
		return 1;
	}
	if (args.map_file_given)
		FileFinder::setUserFontMap(args.map_file_arg);

	DVIToSVG::CREATE_STYLE = !args.no_styles_given;
	DVIToSVG::USE_FONTS = !args.no_fonts_given;
	SVGFontTraceEmitter::TRACE_ALL = args.trace_all_given;
	SVGFontTraceEmitter::METAFONT_MAG = args.mag_arg;

	double start_time = get_time();
	
	string dvifile = ensure_suffix(args.inputs[0], "dvi");
	string svgfile = args.output_given ? args.output_arg : remove_suffix(remove_path(dvifile));
	svgfile = ensure_suffix(svgfile, args.zip_given ? "svgz" : "svg");	
	
	ifstream ifs(dvifile.c_str(), ios_base::binary|ios_base::in);
   if (!ifs)
      Message::estream(true) << "can't open file '" << dvifile << "' for reading\n";
	else {
		Pointer<ostream> out;
		if (args.stdout_given) 
			out = Pointer<ostream>(&cout, false);
		else if (args.zip_given) 
			out = Pointer<ostream>(new ogzstream(svgfile.c_str(), args.zip_arg));
		else 
			out = Pointer<ostream>(new ofstream(svgfile.c_str(), ios_base::binary));
		
		if (!*out)
      	Message::estream(true) << "can't open file '" << svgfile << "' for writing\n";
		else {
			StreamCounter<char> sc(*out);
			Message::level = args.verbosity_arg;
			DVIToSVG dvisvg(ifs, *out);
			const char *ignore_specials = args.no_specials_given ? (args.no_specials_arg ? args.no_specials_arg : "*") : 0;
			dvisvg.setProcessSpecials(ignore_specials);
			set_trans(dvisvg, args);
			tolower(args.bbox_format_arg);
			dvisvg.setPageSize(args.bbox_format_arg);
			
			try {
				FileFinder::init(argv[0], !args.no_mktexmf_given);
				if (int pages = dvisvg.convert(args.page_arg, args.page_arg)) {
					if (!args.stdout_given)
						sc.invalidate();  // output buffer is no longer valid
					// valgrind issues an invalid conditional jump/move in the deflate function of libz here.
					// According to libz FAQ #36 this is not a bug but intended behavior.
					out.release();       // force writing
					const char *pstr = pages == 1 ? "" : "s";
					UInt64 nbytes = args.stdout_given ? sc.count() : FileSystem::filesize(svgfile);
					Message::mstream() << pages << " page" << pstr; 
					Message::mstream() << " (" << nbytes << " bytes";
					if (args.zip_given)
						Message::mstream() << " = " << floor(double(nbytes)/sc.count()*100.0+0.5) << "%";
					Message::mstream() << ") written to " 
						<< (args.stdout_given ? "<stdout>" : svgfile)
						<< " in " << (get_time()-start_time) << " seconds\n";
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

