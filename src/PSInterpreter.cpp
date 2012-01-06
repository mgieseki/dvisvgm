/*************************************************************************
** PSInterpreter.cpp                                                    **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include "FileFinder.h"
#include "InputReader.h"
#include "Message.h"
#include "PSInterpreter.h"
#include "psdefs.psc"
#include "SignalHandler.h"

using namespace std;


const char *PSInterpreter::GSARGS[] = {
	"gs",                // dummy name
	"-q",                // be quiet, suppress gs banner
	"-dSAFER",           // disallow writing of files
	"-dNODISPLAY",       // we don't need a display device
	"-dNOPAUSE",         // keep going
	"-dWRITESYSTEMDICT", // leave systemdict writable as some operators must be replaced
	"-dNOPROMPT",
//	"-dNOBIND",
};


/** Constructs a new PSInterpreter object.
 *  @param[in] actions template methods to be executed after recognizing the corresponding PS operator. */
PSInterpreter::PSInterpreter (PSActions *actions)
	: _mode(PS_NONE), _actions(actions), _inError(false), _initialized(false)
{
}


void PSInterpreter::init () {
	if (!_initialized) {
		_gs.init(sizeof(GSARGS)/sizeof(char*), GSARGS, this);
		_gs.set_stdio(input, output, error);
		_initialized = true;
		// Before executing any random PS code redefine some operators and run
		// initializing PS code. This cannot be done in the constructor because we
		// need the completely initialized PSInterpreter object here.
		execute(PSDEFS);
	}
}


PSActions* PSInterpreter::setActions (PSActions *actions) {
	PSActions *old_actions = _actions;
	_actions = actions;
	return old_actions;
}


/** Executes a chunk of PostScript code.
 *  @param[in] str buffer containing the code
 *  @param[in] len number of characters in buffer
 *  @param[in] flush If true, a final 'flush' is sent which forces the
 *  	output buffer to be written immediately.*/
void PSInterpreter::execute (const char *str, size_t len, bool flush) {
	init();
	if (_mode != PS_QUIT) {
		int status;
		if (_mode == PS_NONE) {
			_gs.run_string_begin(0, &status);
			_mode = PS_RUNNING;
		}
		const char *p=str;
		// feed Ghostscript with code chunks that are not larger than 64KB
		// => see documentation of gsapi_run_string_foo()
		while (PS_RUNNING && len > 0) {
			SignalHandler::instance().check();
			size_t chunksize = min(len, (size_t)0xffff);
			_gs.run_string_continue(p, chunksize, 0, &status);
			p += chunksize;
			len -= chunksize;
			if (status == -101) { // e_Quit
				_gs.exit();
				_mode = PS_QUIT;
			}
			else if (status <= -100) {
				_gs.exit();
				_mode = PS_QUIT;
				throw PSException("fatal PostScript error");
			}
		}
		if (flush) {
			// force writing contents of output buffer
			_gs.run_string_continue("\nflush ", 7, 0, &status);
		}
	}
}


/** Executes a chunk of PostScript code read from a stream. The method returns on EOF.
 *  @param[in] is the input stream */
void PSInterpreter::execute (istream &is) {
	char buf[4096];
	while (is && !is.eof()) {
		is.read(buf, 4096);
		execute(buf, is.gcount(), false);
	}
	execute("\n", 1);
}


/** This callback function handles input from stdin to Ghostscript. Currently not needed.
 *  @param[in] inst pointer to calling instance of PSInterpreter
 *  @param[in] buf takes the read characters
 *  @param[in] len size of buffer buf
 *  @return number of read characters */
int GSDLLCALL PSInterpreter::input (void *inst, char *buf, int len) {
	return 0;
}


/** This callback function handles output from Ghostscript to stdout. It looks for
 *  emitted commands staring with "dvi." and executes them by calling method callActions.
 *  Ghostscript sends the text in chunks by several calls of this function.
 *  Unfortunately, the PostScript specification wants error messages also to be sent to stdout
 *  instead of stderr. Thus, we must collect and concatenate the chunks until an evaluable text
 *  snippet is completely received. Furthermore, error messages have to be recognized and to be
 *  filtered out.
 *  @param[in] inst pointer to calling instance of PSInterpreter
 *  @param[in] buf contains the characters to be output
 *  @param[in] len number of characters in buf
 *  @return number of processed characters (equals 'len') */
int GSDLLCALL PSInterpreter::output (void *inst, const char *buf, int len) {
	PSInterpreter *self = static_cast<PSInterpreter*>(inst);
	if (self && self->_actions) {
		const size_t MAXLEN = 512;    // maximal line length (longer lines are of no interest)
		const char *end = buf+len-1;  // last position of buf
		for (const char *first=buf, *last=buf; first <= end; last++, first=last) {
			// move first and last to begin and end of the next line, respectively
			while (last <= end && *last != '\n')
				last++;
			size_t linelength = last-first+1;
			if (linelength > MAXLEN)  // skip long lines since they don't contain any relevant information
				continue;

			vector<char> &linebuf = self->_linebuf;  // just a shorter name...
			if ((*last == '\n' || !self->active())) {
				if (linelength + linebuf.size() > 5) {  // prefix "dvi." plus final newline
					SplittedCharInputBuffer ib(linebuf.empty() ? 0 : &linebuf[0], linebuf.size(), first, linelength);
					BufferInputReader in(ib);
					in.skipSpace();
					if (self->_inError) {
						if (in.check("dvi.enderror")) {
							// @@
							self->_errorMessage.clear();
							self->_inError = false;
						}
						else
							self->_errorMessage += string(first, linelength);
					}
					if (in.check("dvi.")) {
						if (in.check("beginerror"))  // all following output belongs to an error message
							self->_inError = true;
						else
							self->callActions(in);
					}
				}
				linebuf.clear();
			}
			else { // no line end found =>
				// save remaining characters and prepend them to the next incoming chunk of characters
				linelength--;         // last == end+1 => linelength is 1 too large
				if (linebuf.size() + linelength > MAXLEN)
					linebuf.clear();   // don't care for long lines
				else {
					size_t currsize = linebuf.size();
					linebuf.resize(currsize+linelength);
					memcpy(&linebuf[currsize], first, linelength);
				}
			}
		}
	}
	return len;
}


/** Converts a vector of strings to a vector of doubles.
 * @param[in] str the strings to be converted
 * @param[out] d the resulting doubles */
static void str2double (const vector<string> &str, vector<double> &d) {
	for (size_t i=0; i < str.size(); i++) {
		istringstream iss(str[i]);
		iss >> d[i];
	}
}


/** Evaluates a command emitted by Ghostscript and invokes the corresponding
 *  method of interface class PSActions.
 *  @param[in] in reader pointing to the next command */
void PSInterpreter::callActions (InputReader &in) {
	// array of currently supported operators (must be ascendingly sorted)
	static const struct Operator {
		const char *name; // name of operator
		int pcount;       // number of parameters (< 0 : variable number of parameters)
		void (PSActions::*op)(vector<double> &p);  // operation handler
	} operators [] = {
		{"applyscalevals",  3, &PSActions::applyscalevals},
		{"clip",            0, &PSActions::clip},
		{"closepath",       0, &PSActions::closepath},
		{"curveto",         6, &PSActions::curveto},
		{"eoclip",          0, &PSActions::eoclip},
		{"eofill",          0, &PSActions::eofill},
		{"fill",            0, &PSActions::fill},
		{"grestore",        0, &PSActions::grestore},
		{"grestoreall",     0, &PSActions::grestoreall},
		{"gsave",           0, &PSActions::gsave},
		{"initclip",        0, &PSActions::initclip},
		{"lineto",          2, &PSActions::lineto},
		{"moveto",          2, &PSActions::moveto},
		{"newpath",         0, &PSActions::newpath},
		{"querypos",        2, &PSActions::querypos},
		{"restore",         1, &PSActions::restore},
		{"rotate",          1, &PSActions::rotate},
		{"save",            1, &PSActions::save},
		{"scale",           2, &PSActions::scale},
		{"setcmykcolor",    4, &PSActions::setcmykcolor},
		{"setdash",        -1, &PSActions::setdash},
		{"setgray",         1, &PSActions::setgray},
		{"sethsbcolor",     3, &PSActions::sethsbcolor},
		{"setlinecap",      1, &PSActions::setlinecap},
		{"setlinejoin",     1, &PSActions::setlinejoin},
		{"setlinewidth",    1, &PSActions::setlinewidth},
		{"setmatrix",       6, &PSActions::setmatrix},
		{"setmiterlimit",   1, &PSActions::setmiterlimit},
		{"setopacityalpha", 1, &PSActions::setopacityalpha},
		{"setrgbcolor",     3, &PSActions::setrgbcolor},
		{"stroke",          0, &PSActions::stroke},
		{"translate",       2, &PSActions::translate},
	};
	if (_actions) {
		in.skipSpace();
		// binary search
		int first=0, last=sizeof(operators)/sizeof(Operator)-1;
		while (first <= last) {
			int mid = first+(last-first)/2;
			int cmp = in.compare(operators[mid].name);
			if (cmp > 0)
				last = mid-1;
			else if (cmp < 0)
				first = mid+1;
			else {
				// collect parameters and call handler
				vector<string> params;
				int pcount = operators[mid].pcount;
				if (pcount < 0) {       // variable number of parameters?
					in.skipSpace();
					while (!in.eof()) {  // read all available parameters
						params.push_back(in.getString());
						in.skipSpace();
					}
				}
				else {                  // fixed number of parameters
					for (int i=0; i < pcount; i++) {
						in.skipSpace();
						params.push_back(in.getString());
					}
				}
				vector<double> v(params.size());
				str2double(params, v);
				(_actions->*operators[mid].op)(v);
				_actions->executed();
			}
		}
	}
}


/** This callback function handles output from Ghostscript to stderr.
 *  @param[in] inst pointer to calling instance of PSInterpreter
 *  @param[in] buf contains the characters to be output
 *  @param[in] len number of chars in buf
 *  @return number of processed characters */
int GSDLLCALL PSInterpreter::error (void *inst, const char *buf, int len) {
	ostringstream oss;
	oss << "PostScript error:\n";
	oss.write(buf, len);
	return len;
}

