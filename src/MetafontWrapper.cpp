/*************************************************************************
** MetafontWrapper.cpp                                                  **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2011 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <cctype>
#include <fstream>
#include <sstream>
#include "FileSystem.h"
#include "FileFinder.h"
#include "Message.h"
#include "MetafontWrapper.h"

#ifdef __WIN32__
#include <windows.h>
#endif

using namespace std;


static int execute (const char *cmd, const char *params) {
#ifdef __WIN32__
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = true;

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	HANDLE devnull = CreateFile("nul", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdOutput = devnull;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	string cmdline = string("\"")+cmd+"\" "+params;
	CreateProcess(NULL, (LPSTR)cmdline.c_str(), NULL, NULL, true, 0, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD exitcode = (DWORD)-1;
	GetExitCodeProcess(pi.hProcess, &exitcode);
	CloseHandle(devnull);
	return exitcode;
#else
	ostringstream oss;
	oss << cmd << ' ' << params << " >" << FileSystem::DEVNULL;
	return system(oss.str().c_str());
#endif
}


MetafontWrapper::MetafontWrapper (const string &fname)
	: _fontname(fname)
{
}


/** Calls Metafont and evaluates the logfile. If a gf file was successfully
 *  generated the dpi value is stripped from the filename
 *  (e.g. cmr10.600gf => cmr10.gf). This makes life easier...
 *  @param[in] mode Metafont mode, e.g. "ljfour"
 *  @param[in] mag magnification factor
 *  @return return value of Metafont system call */
int MetafontWrapper::call (const string &mode, double mag) {
	if (!FileFinder::lookup(_fontname+".mf"))
		return 1;     // mf file not available => no need to call the "slow" Metafont
	FileSystem::remove(_fontname+".gf");

#ifdef __WIN32__
	const char *cmd = FileFinder::lookup("mf.exe", false);
#else
	const char *cmd = "mf";
#endif
	ostringstream oss;
	oss << "\"\\mode=" << mode  << ";"
		   "mag:=" << mag << ";"
		   "batchmode;"
		   "input " << _fontname << "\"";
	Message::mstream(false, Terminal::CYAN) << "\nrunning Metafont for " << _fontname << '\n';
	int ret = execute(cmd, oss.str().c_str());

	// try to read Metafont's logfile and get name of created GF file
	ifstream ifs((_fontname+".log").c_str());
	if (ifs) {
		char buf[128];
		while (ifs) {
			ifs.getline(buf, 128);
			string line = buf;
			if (line.substr(0, 17) == "Output written on") {
				size_t pos = line.find("gf ", 18+_fontname.length());
				if (pos != string::npos) {
					string gfname = line.substr(18, pos-16);  // GF filename found
					FileSystem::rename(gfname, _fontname+".gf");
				}
				break;
			}
		}
	}
	return ret;
}


/** Calls Metafont if output files (tfm and gf) don't already exist. */
int MetafontWrapper::make (const string &mode, double mag) {
	ifstream tfm((_fontname+".tfm").c_str());
	ifstream gf((_fontname+".gf").c_str());
	if (gf && tfm) // @@ distinguish between gf and tfm
		return 0;
	return call(mode, mag);
}


bool MetafontWrapper::success () const {
	ifstream tfm((_fontname+".tfm").c_str());
	ifstream gf((_fontname+".gf").c_str());
	return tfm && gf;
}


/** Remove all files created by a Metafont call (tfm, gf, log).
 *  @param[in] keepGF if true, GF files won't be removed */
void MetafontWrapper::removeOutputFiles (bool keepGF) {
	removeOutputFiles(_fontname, keepGF);
}


/** Remove all files created by a Metafont call for a given font (tfm, gf, log).
 *  @param[in] fontname name of font whose temporary files should be removed
 *  @param[in] keepGF if true, GF files will be kept */
void MetafontWrapper::removeOutputFiles (const string &fontname, bool keepGF) {
	const char *ext[] = {"gf", "tfm", "log", 0};
	for (const char **p = keepGF ? ext+2 : ext; *p; ++p)
		FileSystem::remove(fontname + "." + *p);
}
