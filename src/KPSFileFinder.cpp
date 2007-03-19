/***********************************************************************
** KPSFileFinder.cpp                                                  **
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

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "KPSFileFinder.h"
#include "macros.h"


// static members
const char *KPSFileFinder::progname = 0;
bool KPSFileFinder::initialized = false;
FontMap KPSFileFinder::fontmap;

// prototypes of static functions 
static const char* find_file (const std::string &fname);
static const char* find_mapped_file (std::string fname, const FontMap &fontmap);
static const char* mktex (const std::string &fname);

#ifdef MIKTEX	
	#include <kpathsea/kpathsea.h>
#else
	// unfortunately, kpathsea is not C++-ready, so we have to wrap it with some ugly code
	namespace KPS {
		extern "C" {
			#include <kpathsea/kpathsea.h>
		}
	}
	using namespace KPS;
#endif


/** Determines filetype by the filename extension and calls kpse_find_file
 *  to actually look up the file. 
 *  @param fname name of file to look up
 *  @return file path on success, 0 otherwise */
static const char* find_file (const std::string &fname) {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;  // no extension => no search

	const std::string ext  = fname.substr(pos+1);  // file extension
	static std::map<std::string, kpse_file_format_type> types;
	if (types.empty()) {
		types["tfm"] = kpse_tfm_format;
		types["pfb"] = kpse_type1_format;
		types["vf"]  = kpse_vf_format;
#ifdef MIKTEX
		types["mf"] = kpse_miscfonts_format;  // this is a bug, I think
#else
		types["mf"] = kpse_mf_format;
#endif
		types["ttf"] = kpse_truetype_format;
		types["exe"] = kpse_program_binary_format;
		types["map"] = kpse_fontmap_format;
	}
	std::map<std::string, kpse_file_format_type>::iterator it = types.find(ext.c_str());
	if (it == types.end())
		return 0;
	return kpse_find_file(fname.c_str(), it->second, 0);
}


/** Checks whether the given file is mapped to a different name and if the 
 *  file can be found under this name. 
 *  @param fname name of file to look up
 *  @param fontmap font map to use 
 *  @return file path on success, 0 otherwise */
static const char* find_mapped_file (std::string fname, const FontMap &fontmap) {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos) 
		return 0;
	const std::string ext  = fname.substr(pos+1);  // file extension
	const std::string base = fname.substr(0, pos-1);
	const char *mapped_name = fontmap.lookup(base);
	if (mapped_name) {
		fname = std::string(mapped_name) + "." + ext;
		const char *path;
		if ((path = find_file(fname)) || (path = mktex(fname)))
			return path;
	}
	return 0;
}


/** Runs external mktexFOO tool to create missing tfm or mf file.
 *  @param fname name of file to build
 *  @return file path on success, 0 otherwise */
static const char* mktex (const std::string &fname) {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;

	std::string ext  = fname.substr(pos+1);  // file extension
	if (ext != "tfm" && ext != "mf")
		return 0;

	std::string base = fname.substr(0, pos-1);
	const char *path = 0;
#ifdef MIKTEX
	const char *toolname = (ext == "tfm" ? MIKTEX_MAKETFM_EXE : MIKTEX_MAKEMF_EXE);
	const char *toolpath = Find(toolname, FileType::EXE, fname);
	if (toolpath) {
   	Process::Run(toolname, fname.c_str());
		path = find_file(fname);
	}

/*   PathName pathMakePk;
   MIKTEXCHAR szArguments[1024];
   SessionWrapper(true)->MakeMakePkCommandLine (name, dpi, bdpi, mfmode, pathMakePk, szArguments, 1024);
   Process::Run(pathMakePk.Get(), szArguments); */
#else
	kpse_file_format_type type = (ext == "tfm" ? kpse_tfm_format : kpse_mf_format);
	path = kpse_make_tex(type, fname.c_str());
#endif
	return path;
}


/** Searches a file in the TeX tree. 
 *  If the file doesn't exist, maximal two further steps are applied:
 *  - checks whether the filename is mapped to a different name and returns
 *    the path to that name
 *  - in case of tfm or mf files: invokes the external mktextfm/mktexmf tool
 *    to create the missing file
 *  @param fname name of file to look up 
 *  @return path to file on success, 0 otherwise */
const char* KPSFileFinder::lookup (const std::string &fname) {
	if (!initialized) {
#ifndef MIKTEX
		kpse_set_program_name(progname, NULL);
		// enable tfm and mf generation (actually invoked by calls of kpse_make_tex)
		kpse_set_program_enabled(kpse_tfm_format, 1, kpse_src_env);
		kpse_set_program_enabled(kpse_mf_format, 1, kpse_src_env);
//		kpse_make_tex_discard_errors = true; 
//		const char *path = kpse_make_tex(kpse_tfm_format, "ecrm10");
#endif
#if 1
		const char *mapfile = find_file("dvipdfm.map");
		std::ifstream ifs(mapfile);
		fontmap.read(ifs);
#endif
		initialized = true;
	}
	const char *path = find_file(fname);
	if ((path = find_file(fname)) || (path = find_mapped_file(fname, fontmap)) || (path = mktex(fname))) 
		return path;
	return 0;
}

