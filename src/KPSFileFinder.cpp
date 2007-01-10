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
#include <string>
#include "KPSFileFinder.h"


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

KPSFileFinder::KPSFileFinder (const char *progname) {
#ifndef MIKTEX
	kpse_set_program_name(progname, NULL);
#endif
}


KPSFileFinder::~KPSFileFinder () {
}


const char* KPSFileFinder::lookup (const std::string &fname) const {
	size_t pos = fname.rfind('.');
	if (pos == std::string::npos)
		return 0;

	std::string ext = fname.substr(pos+1);
	kpse_file_format_type formatType;
	if (ext == "tfm")
		formatType = kpse_tfm_format;
	else if (ext == "pfb")
		formatType = kpse_type1_format;
	else if (ext == "mf")
#ifdef MIKTEX
		formatType = kpse_miscfonts_format;  // this is a bug, I think
#else
		formatType = kpse_mf_format;
#endif
	else if (ext == "ttf")
		formatType = kpse_truetype_format;
	else 
		return 0;
	return kpse_find_file(fname.c_str(), formatType, 0);
}
