/***********************************************************************
** KPSFileFinder.cpp                                                  **
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "KPSFileFinder.h"
#include "Message.h"
#include "macros.h"

// static members of KPSFileFinder
bool KPSFileFinder::mktexEnabled = true;
bool KPSFileFinder::initialized = false;
const char *KPSFileFinder::usermap = 0;
FontMap KPSFileFinder::fontmap;

// prototypes of static functions 
static const char* find_file (const std::string &fname);
static const char* find_mapped_file (std::string fname, const FontMap &fontmap);
static const char* mktex (const std::string &fname);
static void init_fontmap (FontMap &fontmap);

#ifdef MIKTEX	
	#include "MessageException.h"
	#import <MiKTeX207-session.tlb>
	using namespace MiKTeXSession2_7;	
	
	static ISession2Ptr miktex_session;
#else
	// unfortunately, the kpathsea headers are not C++-ready,
	// so we have to wrap it with some ugly code
	namespace KPS {
		extern "C" {
			#include <kpathsea/kpathsea.h>
		}
	}
	using namespace KPS;
#endif


void KPSFileFinder::initialize (const char *progname, bool enable_mktexmf) {
	if (!initialized) {
		mktexEnabled = enable_mktexmf;
#ifdef MIKTEX				
		if (FAILED(CoInitialize(0)))
			throw MessageException("COM library could not be initialized\n");			

		HRESULT hres = miktex_session.CreateInstance(L"MiKTeX.Session");
		if (FAILED(hres))
			throw MessageException("MiKTeX.Session could not be initialized");
#else
		kpse_set_program_name(progname, NULL);
		// enable tfm and mf generation (actually invoked by calls of kpse_make_tex)
		kpse_set_program_enabled(kpse_tfm_format, 1, kpse_src_env);
		kpse_set_program_enabled(kpse_mf_format, 1, kpse_src_env);
		kpse_make_tex_discard_errors = false; // don't suppress messages of mktexFOO tools		
#endif
		init_fontmap(fontmap);
		initialized = true;
	}
}

void KPSFileFinder::finalize () {
#ifdef MIKTEX
	miktex_session = 0;  // avoid automatic calling of Release() after CoUninitialize()
	CoUninitialize();
	initialized = false;
#endif
}



/** Determines filetype by the filename extension and calls kpse_find_file
 *  to actually look up the file. 
 *  @param[in] fname name of file to look up
 *  @return file path on success, 0 otherwise */
static const char* find_file (const std::string &fname) {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;  // no extension => no search
	const std::string ext  = fname.substr(pos+1);  // file extension
#ifdef MIKTEX
	_bstr_t path;
	static string ret;
	try {
		if (miktex_session->FindFile(fname.c_str(), path.GetAddress())) {
			ret = path;
			return ret.c_str();
		}
	}
	catch (_com_error e) {
		throw MessageException((const char*)e.Description());
	}
	return 0;		

#else
		
	static std::map<std::string, kpse_file_format_type> types;
	if (types.empty()) {
		types["tfm"] = kpse_tfm_format;
		types["pfb"] = kpse_type1_format;
		types["vf"]  = kpse_vf_format;
		types["mf"]  = kpse_mf_format;
		types["ttf"] = kpse_truetype_format;
		types["map"] = kpse_fontmap_format;
		types["sty"] = kpse_tex_format;
		types["enc"] = kpse_enc_format;
	}
	std::map<std::string, kpse_file_format_type>::iterator it = types.find(ext.c_str());
	if (it == types.end())
		return 0;
	if (char *path = kpse_find_file(fname.c_str(), it->second, 0)) {
		static std::string buf;
		buf = path;
		std::free(path);
		return buf.c_str();
	}
	return 0;
#endif
}


/** Checks whether the given file is mapped to a different name and if the 
 *  file can be found under this name. 
 *  @param[in] fname name of file to look up
 *  @param[in] fontmap font mappings
 *  @return file path on success, 0 otherwise */
static const char* find_mapped_file (std::string fname, const FontMap &fontmap) {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos) 
		return 0;
	const std::string ext  = fname.substr(pos+1);  // file extension
	const std::string base = fname.substr(0, pos);
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
 *  @param[in] fname name of file to build
 *  @return file path on success, 0 otherwise */
static const char* mktex (const std::string &fname) {
	size_t pos = fname.rfind('.');
	if (!KPSFileFinder::mktexEnabled || pos == std::string::npos)
		return 0;

	std::string ext  = fname.substr(pos+1);  // file extension
	if (ext != "tfm" && ext != "mf")
		return 0;

	std::string base = fname.substr(0, pos);
	const char *path = 0;
#ifdef MIKTEX
	const char *toolname = (ext == "tfm" ? "maketfm.exe" : "makemf.exe");
	const char *toolpath = find_file(toolname);	
	if (toolpath) {
		system((string(toolpath) + " " + fname).c_str());
		path = find_file(fname);
	}	
#else
	kpse_file_format_type type = (ext == "tfm" ? kpse_tfm_format : kpse_mf_format);
	path = kpse_make_tex(type, fname.c_str());
#endif
	return path;
}


/** Initializes a font map by reading the map file(s). 
 *  @param[in,out] fontmap font map to be initialized */
static void init_fontmap (FontMap &fontmap) {
	if (KPSFileFinder::usermap) {
		// try to read user font map file
		const char *mappath = 0;
		std::ifstream ifs(KPSFileFinder::usermap);
		if (ifs)
			fontmap.read(ifs);
		else if ((mappath = find_file(KPSFileFinder::usermap))) {
			std::ifstream ifs(mappath);
			fontmap.read(ifs);
		}
		else
			Message::wstream(true) << "map file '" << KPSFileFinder::usermap << "' not found\n";
	}
	else {
#ifdef MIKTEX
		try {
			MiKTeXSetupInfo info = miktex_session->GetMiKTeXSetupInfo();	
			
			// read all dvipdfm mapfiles		
			for (unsigned i=0; i < info.numRoots; i++) {
				_bstr_t bdir = miktex_session->GetRootDirectory(i);
				string dir = (const char*)bdir;				
				// strip trailing slash
				size_t len = dir.length();
				if (len > 0 && (dir[len-1] == '/' || dir[len-1] == '\\'))
					dir = dir.substr(0, len-1);
				dir += "\\dvipdfm\\config";
				fontmap.readdir(dir);
			}
		}
		catch (_com_error e) {
			throw MessageException((const char*)e.Description());
		}
#else
		const char *fname = "dvipdfm.map";
		const char *mapfile = KPSFileFinder::lookup(fname, false);
		if (mapfile) {
			std::ifstream ifs(mapfile);
			fontmap.read(ifs);
		}
		else
			Message::wstream(true) << "default map file '" << fname << "' not found";
#endif
	}
}


/** Searches a file in the TeX directory tree. 
 *  If the file doesn't exist, maximal two further steps are applied
 *  (if "extended" is true):
 *  - checks whether the filename is mapped to a different name and returns
 *    the path to that name
 *  - in case of tfm or mf files: invokes the external mktextfm/mktexmf tool
 *    to create the missing file
 *  @param[in] fname name of file to look up
 *  @param[in] extended if true, use fontmap lookup and mktexFOO calls
 *  @return path to file on success, 0 otherwise */
const char* KPSFileFinder::lookup (const std::string &fname, bool extended) {
	const char *path;
	if ((path = find_file(fname)) || (extended  && (path = find_mapped_file(fname, fontmap)) || (path = mktex(fname)))) 
		return path;
	return 0;
}


/** Returns the path to the corresponding encoding file for a given font file. 
 *  @param[in] fname name of the font file
 *  @return path to encoding file on success, 0 otherwise */
const char* KPSFileFinder::lookupEncFile (std::string fname) {
	if (const char *encname = lookupEncName(fname)) {
		fname = std::string(encname) + ".enc";
		const char *path = find_file(fname);
		if (path)
			return path;
	}	
	return 0;
}


const char* KPSFileFinder::lookupEncName (std::string fname) {
	size_t pos = fname.rfind('.');
	if (pos != std::string::npos)
		fname = fname.substr(0, pos); // strip extension
	return fontmap.encoding(fname);
}
