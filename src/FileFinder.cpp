/*************************************************************************
** FileFinder.cpp                                                       **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <fstream>
#include <map>
#include <string>
#include "FileFinder.h"
#include "Message.h"
#include "macros.h"

using namespace std;

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

// ---------------------------------------------------
// static member variables of FileFinder::Impl

FileFinder::Impl* FileFinder::Impl::_instance = 0;
const char *FileFinder::Impl::_progname = 0;
bool FileFinder::Impl::_mktex_enabled = false;
FontMap FileFinder::Impl::_fontmap;
const char *FileFinder::Impl::_usermapname = 0;

// ---------------------------------------------------


FileFinder::Impl::Impl ()
{
#ifdef MIKTEX
	if (FAILED(CoInitialize(0)))
		throw MessageException("COM library could not be initialized\n");

	HRESULT hres = miktex_session.CreateInstance(L"MiKTeX.Session");
	if (FAILED(hres))
		throw MessageException("MiKTeX.Session could not be initialized");
#else
	kpse_set_program_name(_progname, NULL);
	// enable tfm and mf generation (actually invoked by calls of kpse_make_tex)
	kpse_set_program_enabled(kpse_tfm_format, 1, kpse_src_env);
	kpse_set_program_enabled(kpse_mf_format, 1, kpse_src_env);
	kpse_make_tex_discard_errors = false; // don't suppress messages of mktexFOO tools
#endif
}


FileFinder::Impl::~Impl () {
	delete _instance;
#ifdef MIKTEX
	miktex_session = 0;  // avoid automatic calling of Release() after CoUninitialize()
	CoUninitialize();
#endif
}


FileFinder::Impl& FileFinder::Impl::instance () {
	if (!_instance) {
		_instance = new FileFinder::Impl;
		_instance->initFontMap();
	}
	return *_instance;
}


/** Determines filetype by the filename extension and calls kpse_find_file
 *  to actually look up the file.
 *  @param[in] fname name of file to look up
 *  @return file path on success, 0 otherwise */
const char* FileFinder::Impl::findFile (const std::string &fname) {
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
		types["pro"] = kpse_tex_ps_header_format;
	}
	std::map<std::string, kpse_file_format_type>::iterator it = types.find(ext.c_str());
	if (it == types.end())
		return 0;
	if (char *path = kpse_find_file(fname.c_str(), it->second, 0)) {
		// In the current version of libkpathsea, each call of kpse_find_file produces
		// a memory leak since the path buffer is not freed. I don't think we can do
		// anything against it here...
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
const char* FileFinder::Impl::findMappedFile (std::string fname) {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;
	const std::string ext  = fname.substr(pos+1);  // file extension
	const std::string base = fname.substr(0, pos);
	const char *mapped_name = _fontmap.lookup(base);
	if (mapped_name) {
		fname = std::string(mapped_name) + "." + ext;
		const char *path;
		if ((path = findFile(fname)) || (path = mktex(fname)))
			return path;
	}
	return 0;
}


/** Runs external mktexFOO tool to create missing tfm or mf file.
 *  @param[in] fname name of file to build
 *  @return file path on success, 0 otherwise */
const char* FileFinder::Impl::mktex (const std::string &fname) {
	size_t pos = fname.rfind('.');
	if (!_mktex_enabled || pos == std::string::npos)
		return 0;

	std::string ext  = fname.substr(pos+1);  // file extension
	if (ext != "tfm" && ext != "mf")
		return 0;

	std::string base = fname.substr(0, pos);
	const char *path = 0;
#ifdef MIKTEX
	// maketfm and makemf are located in miktex/bin which is in the search PATH
	string toolname = (ext == "tfm" ? "maketfm" : "makemf");
	system((toolname+".exe "+fname).c_str());
	path = findFile(fname);
#else
	kpse_file_format_type type = (ext == "tfm" ? kpse_tfm_format : kpse_mf_format);
	path = kpse_make_tex(type, fname.c_str());
#endif
	return path;
}


/** Initializes a font map by reading the map file(s).
 *  @param[in,out] fontmap font map to be initialized */
void FileFinder::Impl::initFontMap () {
	const char *usermapname = _usermapname;
	if (usermapname && *usermapname == '+') // read additional map entries?
		usermapname++;
	if (usermapname) {
		// try to read user font map file
		const char *mappath = 0;
		if (!_fontmap.read(usermapname)) {
			if ((mappath = findFile(usermapname)))
				_fontmap.read(mappath);
			else
				Message::wstream(true) << "map file '" << usermapname << "' not found\n";
		}
	}
	if (!usermapname || *_usermapname == '+') {
		const char *mapfiles[] = {"ps2pk.map", "dvipdfm.map", "psfonts.map", 0};
		const char *mf=0;
		for (const char **p=mapfiles; *p && !mf; p++)
			if ((mf = findFile(*p))!=0)
				_fontmap.read(mf);
		if (!mf)
			Message::wstream(true) << "none of the default map files could be found";
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
const char* FileFinder::Impl::lookup (const std::string &fname, bool extended) {
	const char *path;
	if ((path = findFile(fname)) || (extended  && ((path = findMappedFile(fname)) || (path = mktex(fname)))))
		return path;
	return 0;
}


/** Returns the path to the corresponding encoding file for a given font file.
 *  @param[in] fname name of the font file
 *  @return path to encoding file on success, 0 otherwise */
const char* FileFinder::Impl::lookupEncFile (std::string fname) {
	if (const char *encname = lookupEncName(fname)) {
		fname = std::string(encname) + ".enc";
		const char *path = findFile(fname);
		if (path)
			return path;
	}
	return 0;
}


const char* FileFinder::Impl::lookupEncName (std::string fname) {
	size_t pos = fname.rfind('.');
	if (pos != std::string::npos)
		fname = fname.substr(0, pos); // strip extension
	return _fontmap.encoding(fname);
}
