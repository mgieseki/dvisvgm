/***********************************************************************
** dvisvgm.cpp                                                        **
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
#include "KPSFileFinder.h"
#include "StreamCounter.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#define EMAIL " <" PACKAGE_BUGREPORT ">"
#else
#define EMAIL ""
#endif

#ifdef MIKTEX
#include <miktex/app.h>
#include <miktex/core.h>
#endif

using namespace std;


static void show_help () {
   cmdline_parser_print_help();
   cout << "\nCopyright (C) 2005-2007 Martin Gieseking" EMAIL "\n\n";
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


static int dvisvgm (int argc, char *argv[]) {
	struct gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args))
		return 1;
	if (args.inputs_num < 1 || args.help_given) {
		show_help();
		return 0;
	}
	if (args.stdout_given && args.zip_given) {
		Message::estream(true) << "writing SVGZ files to stdout is not supported\n";
		return 1;
	}
	KPSFileFinder::progname = argv[0];
	KPSFileFinder::mktexEnabled = !args.no_mktexmf_flag;

	double start_time = get_time();
	
	string dvifile = ensure_suffix(args.inputs[0], "dvi");
	string svgfile = args.output_given ? args.output_arg : remove_suffix(remove_path(dvifile));
	svgfile = ensure_suffix(svgfile, args.zip_given ? "svgz" : "svg");	
	
	ifstream ifs(dvifile.c_str(), ios_base::binary|ios_base::in);
	ostream *out = 0;		
   if (args.stdout_given) 
		out = &cout;
	else if (args.zip_given) 
		out = new ogzstream(svgfile.c_str(), args.zip_arg);
	else 
		out = new ofstream(svgfile.c_str(), ios_base::binary);
	
	if (ifs && *out) {
		StreamCounter<char> sc(*out);
		Message::level = args.verbosity_arg;
		DVIToSVG dvisvg(ifs, *out);
		dvisvg.setMetafontMag(args.mag_arg);
		dvisvg.setProcessSpecials(args.specials_flag);
		set_trans(dvisvg, args);
		for (char *c=args.bbox_format_arg; *c; c++)
			*c = tolower(*c);
		dvisvg.setPageSize(args.bbox_format_arg);
		
		try {
			if (int pages = dvisvg.convert(args.page_arg, args.page_arg)) {
				if (!args.stdout_given) {
					sc.invalidate();  // output buffer is no longer valid
					delete out;       // close file stream and force writing
				}
				out = 0;
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
   else if (!ifs)
      Message::estream(true) << "can't open file '" << dvifile << "' for reading\n";
	else if (!*out)
      Message::estream(true) << "can't open file '" << svgfile << "' for writing\n";
	
	if (!args.stdout_given)
		delete out;
	return 0;
}


int main (int argc, char *argv[]) {
#ifdef MIKTEX
	try {
		MiKTeX::App::Application app;
		app.Init(argv[0]);
		KPSFileFinder::app = &app;
		int ret = dvisvgm(argc, argv);
		app.Finalize();
		return ret;
	}
	catch (const MiKTeX::Core::MiKTeXException &e) {
		MiKTeX::Core::Utils::PrintException(e);
	}
	catch (const exception &e) {
		MiKTeX::Core::Utils::PrintException(e);
	}
	return 1;
#else
	return dvisvgm(argc, argv);
#endif
}
