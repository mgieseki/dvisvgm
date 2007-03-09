/***********************************************************************
** MetafontWrapper.cpp                                                **
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

#include <cstdlib>
#include <cctype>
#include <fstream>
#include <sstream>
#include "FileSystem.h"
#include "KPSFileFinder.h"
#include "Message.h"
#include "MetafontWrapper.h"

using namespace std;


MetafontWrapper::MetafontWrapper (const string &fname) 
	: fontname(fname)
{
}


/** This helper function tries to find the mf file for a given fontname. */
static const char* lookup (string fontname) {
	// try to find file with exact fontname
	string mfname = fontname+".mf";
	if (const char *path = KPSFileFinder::find(mfname))
		return path;
	
	// lookup fontname with trailing numbers stripped
	// (this will find the mf files of ec fonts)
	int pos = fontname.length()-1;
	while (pos >= 0 && isdigit(fontname[pos]))
		pos--;
	mfname = fontname.substr(0, pos+1)+".mf";
	if (const char *path = KPSFileFinder::find(mfname))
		return path;
	// not found either => give up
	return 0;
}	


/** Calls Metafont and evaluates the logfile. If a gf file was successfully
 *  generated the dpi value is stripped from the filename
 *  (e.g. cmr10.600gf => cmr10.gf). This makes life easier...
 *  @param mode Metafont mode, e.g. "ljfour"
 *  @param mag magnification factor
 *  @return return value of Metafont system call */
int MetafontWrapper::call (const string &mode, double mag) {
	if (!lookup(fontname))
		return 1;     // mf file not available => no need to call the "slow" Metafont
	
	FileSystem::remove(fontname+".gf");	
	ostringstream oss;
	oss << "mf \"\\mode=" << mode  << ";"
		    "mag:=" << mag << ";"
		    "batchmode;"
		    "input " << fontname << "\" >" << FileSystem::DEVNULL;
	Message::mstream() << "running Metafont for " << fontname << endl;
	int ret = system(oss.str().c_str());

	// try to read Metafont's logfile and get name of created GF file
	ifstream ifs((fontname+".log").c_str());
	if (ifs) {	
		char buf[128];
		while (ifs) {		
			ifs.getline(buf, 128);
			string line = buf;
			if (line.substr(0, 17) == "Output written on") {
				unsigned pos = line.find("gf ", 15);
				if (pos != string::npos) {
					string gfname = line.substr(18, pos-16);  // GF filename found
					FileSystem::rename(gfname, fontname+".gf");
				}
				break;
			}
		}
	}
	return ret;
}


/** Calls Metafont if output files (tfm and gf) don't already exist. */
int MetafontWrapper::make (const string &mode, double mag) {
//	ifstream tfm((fontname+".tfm").c_str());
	ifstream gf((fontname+".gf").c_str());
	if (gf) // @@ distinguish between gf and tfm
		return 0;
	return call(mode, mag);
}


bool MetafontWrapper::success () const {
	ifstream tfm((fontname+".tfm").c_str());
	ifstream gf((fontname+".gf").c_str());
	return tfm && gf;
}


/** Remove all files created by a Metafont call (tfm, gf, log). */
void MetafontWrapper::removeOutputFiles () {
	removeOutputFiles(fontname);
}


void MetafontWrapper::removeOutputFiles (const string &fontname) {
	FileSystem::remove(fontname+".gf");
	FileSystem::remove(fontname+".log");
	FileSystem::remove(fontname+".tfm");
}
