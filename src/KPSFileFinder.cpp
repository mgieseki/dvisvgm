/***********************************************************************
** KPSFileFinder.cpp                                                  **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2006 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id: KPSFileFinder.cpp,v 1.5 2006/01/05 16:05:06 mgieseki Exp $

#include <string>
#include "KPSFileFinder.h"

#ifdef MIKTEX
extern "C" {
	__stdcall int miktex_initialize ();
	__stdcall int miktex_uninitialize ();
}
#endif

// unfortunately the original kpathsea headers are not C++-ready so 
// we have to wrap them with some ugly code
namespace kps {
#ifdef MIKTEX	
	#include "miktex-kps.h"
#else
	extern "C" {
		#include <kpathsea/kpathsea.h>
	}
#endif
}


KPSFileFinder::KPSFileFinder (const char *progname) {
#ifdef MIKTEX
	miktex_initialize();
	kpse_set_program_name(progname, NULL); // empty macro can be omitted
#else
	kps::kpse_set_program_name(progname, NULL);
#endif
}


KPSFileFinder::~KPSFileFinder () {
#if MIKTEX
	miktex_uninitialize();
#endif
}


const char* KPSFileFinder::lookup (const std::string &fname) const {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;
	std::string ext = fname.substr(pos+1);
	kps::kpse_file_format_type formatType;
	if (ext == "tfm")
		formatType = kps::kpse_tfm_format;
	else if (ext == "pfb")
		formatType = kps::kpse_type1_format;
	else if (ext == "mf")
#ifdef MIKTEX
		formatType = kps::kpse_miscfonts_format;  // this is a bug, I think
#else
		formatType = kps::kpse_mf_format;
#endif
	else if (ext == "ttf")
		formatType = kps::kpse_truetype_format;
	else 
		return 0;
	return kps::kpse_find_file(fname.c_str(), formatType, 0);
}
