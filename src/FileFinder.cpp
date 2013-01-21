/*************************************************************************
** FileFinder.cpp                                                       **
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
#include <fstream>
#include <map>
#include <string>
#include "FileFinder.h"
#include "FileSystem.h"
#include "FontMap.h"
#include "Message.h"

using namespace std;

#ifdef MIKTEX
	#include "MessageException.h"
	#include "MiKTeXCom.h"
	static MiKTeXCom *miktex=0;
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

static bool _initialized = false;
static bool _mktex_enabled = false;

// ---------------------------------------------------

static const char* find_file (const std::string &fname);
static const char* find_mapped_file (std::string fname);
static const char* mktex (const std::string &fname);


/** Initializes the file finder. This function must be called before any other
 *  FileFinder function.
 *  @param[in] argv0 argv[0] of main() function
 *  @param[in] progname name of application using the FileFinder
 *  @param[in] enable_mktexmf if true, tfm and mf file generation is activated */
void FileFinder::init (const char *argv0, const char *progname, bool enable_mktexmf) {
	if (_initialized)
		return;

	_mktex_enabled = enable_mktexmf;
#ifdef MIKTEX
	miktex = new MiKTeXCom;
#else
	kpse_set_program_name(argv0, progname);
	// enable tfm and mf generation (actually invoked by calls of kpse_make_tex)
	kpse_set_program_enabled(kpse_tfm_format, 1, kpse_src_env);
	kpse_set_program_enabled(kpse_mf_format, 1, kpse_src_env);
	kpse_make_tex_discard_errors = false; // don't suppress messages of mktexFOO tools
#ifdef TEXLIVEWIN32
	texlive_gs_init();
#endif
#endif
	_initialized = true;
}


/** Cleans up the FileFinder. This function must be called before leaving the
 *  applications main() function. */
void FileFinder::finish () {
#ifdef MIKTEX
	if (miktex) {
		delete miktex;
		miktex = 0;
	}
#endif
	_initialized = false;
}


/** Returns the version string of the underlying file searching library (kpathsea, MiKTeX) */
std::string FileFinder::version () {
#ifdef MIKTEX
	bool autoinit=false;
	if (!_initialized) {
		init("", "", false);
		autoinit = true;
	}
	try {
		string ret = miktex->getVersion();
		if (autoinit)
			finish();
		return ret;
	}
	catch (MessageException &e) {
		if (autoinit)
			finish();
		throw;
	}
#else
	if (const char *v = strrchr(KPSEVERSION, ' '))
		return v+1;
#endif
	return "unknown";
}


/** Determines filetype by the filename extension and calls kpse_find_file
 *  to actually look up the file.
 *  @param[in] fname name of file to look up
 *  @return file path on success, 0 otherwise */
static const char* find_file (const std::string &fname) {
	if (!_initialized)
		return 0;

	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;  // no extension => no search
	const std::string ext  = fname.substr(pos+1);  // file extension
#ifdef MIKTEX
	return miktex->findFile(fname.c_str());
#else
	static std::string buf;

#ifdef TEXLIVEWIN32
	if (ext == "exe") {
		// lookup exe files in directory where dvisvgm is located
		if (const char *path = kpse_var_value("SELFAUTOLOC")) {
			buf = std::string(path) + "/" + fname;
			return FileSystem::exists(buf.c_str()) ? buf.c_str() : 0;
		}
		return 0;
	}
#endif

	static std::map<std::string, kpse_file_format_type> types;
	if (types.empty()) {
		types["tfm"] = kpse_tfm_format;
		types["pfb"] = kpse_type1_format;
		types["vf"]  = kpse_vf_format;
		types["mf"]  = kpse_mf_format;
		types["ttc"] = kpse_truetype_format;
		types["ttf"] = kpse_truetype_format;
		types["map"] = kpse_fontmap_format;
		types["sty"] = kpse_tex_format;
		types["enc"] = kpse_enc_format;
		types["pro"] = kpse_tex_ps_header_format;
      types["sfd"] = kpse_sfd_format;
	}
	std::map<std::string, kpse_file_format_type>::iterator it = types.find(ext.c_str());
	if (it == types.end())
		return 0;

	if (char *path = kpse_find_file(fname.c_str(), it->second, 0)) {
		// In the current version of libkpathsea, each call of kpse_find_file produces
		// a memory leak since the path buffer is not freed. I don't think we can do
		// anything against it here...
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
static const char* find_mapped_file (std::string fname) {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;
	const std::string ext  = fname.substr(pos+1);  // file extension
	const std::string base = fname.substr(0, pos);
	if (const FontMap::Entry *entry = FontMap::instance().lookup(base)) {
		const char *path=0;
		if (entry->fontname.find('.') != std::string::npos)  // does the mapped filename has an extension?
			path = find_file(entry->fontname);           // look for that file
		else {                             // otherwise, use extension of unmapped file
			fname = entry->fontname + "." + ext;
			(path = find_file(fname)) || (path = mktex(fname));
		}
		return path;
	}
	return 0;
}


/** Runs external mktexFOO tool to create missing tfm or mf file.
 *  @param[in] fname name of file to build
 *  @return file path on success, 0 otherwise */
static const char* mktex (const std::string &fname) {
	if (!_initialized)
		return 0;

	size_t pos = fname.rfind('.');
	if (!_mktex_enabled || pos == std::string::npos)
		return 0;

	std::string ext  = fname.substr(pos+1);  // file extension
	if (ext != "tfm" && ext != "mf")
		return 0;

	const char *path = 0;
#ifdef MIKTEX
	// maketfm and makemf are located in miktex/bin which is in the search PATH
	string toolname = (ext == "tfm" ? "miktex-maketfm" : "miktex-makemf");
	system((toolname+".exe "+fname).c_str());
	path = find_file(fname);
#else
	kpse_file_format_type type = (ext == "tfm" ? kpse_tfm_format : kpse_mf_format);
	path = kpse_make_tex(type, fname.c_str());
#endif
	return path;
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
const char* FileFinder::lookup (const std::string &fname, bool extended) {
	const char *path;
	if ((path = find_file(fname)) || (extended  && ((path = find_mapped_file(fname)) || (path = mktex(fname)))))
		return path;
	return 0;
}