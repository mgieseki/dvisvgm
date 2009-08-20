/*************************************************************************
** CmdLineParserBase.cpp                                                **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <iostream>
#include "CmdLineParserBase.h"
#include "InputBuffer.h"
#include "InputReader.h"
#include "Message.h"

using namespace std;

void CmdLineParserBase::parse () {
	for (int i=1; i < _argc; i++) {
		CharInputBuffer ib(_argv[i], strlen(_argv[i]));
		BufferInputReader ir(ib);
		const Option *opt;
		if (ir.peek() == '-') {
			ir.get();
			if (ir.peek() == '-') {
				// scan long option
				ir.get();
				string longname;
				while (isalnum(ir.peek()) || ir.peek() == '-')
					longname += ir.get();
				if (const Option *opt = option(longname))
					opt->handler(ir, *opt, true);
				else
					Message::estream(false) << "unknown option --" << longname;
			}
			else {
				// scan short option
				char shortname = ir.get();
				if (const Option *opt = option(shortname))
					opt->handler(ir, *opt, false);
				else
					Message::estream(false) << "unknown option -" << shortname;
			}
		}
		else {
			// scan filename
		}
	}
}


void CmdLineParserBase::out (const char *str) const {
	cout << str;
}


void CmdLineParserBase::error (const Option &opt, bool longopt, const char *msg) const {
	Message::estream(false) << "commandline option ";
	if (longopt)
		Message::estream(false) << "--" << opt.longname;
	else
		Message::estream(false) << "-" << opt.shortname;
	Message::estream(false) << ": " << msg << endl;
}


const CmdLineParserBase::Option* CmdLineParserBase::option (char shortname) const {
	const Option *opts = options();
	for (int i=0; i < numOptions(); i++)
		if (opts[i].shortname == shortname) 
			return &opts[i];
	return 0;
}


const CmdLineParserBase::Option* CmdLineParserBase::option (const string &longname) const {
	const Option *opts = options();
	for (int i=0; i < numOptions(); i++)
		if (opts[i].longname == longname)
			return &opts[i];
	return 0;
}


bool CmdLineParserBase::checkArgPrefix (InputReader &ir, const Option &opt, bool longopt) const {
	if (longopt && ir.peek() == '=') 
		ir.get();
	else {
		error(opt, longopt, "'=' expected");
		return false;
	}
	return true;
}


bool CmdLineParserBase::checkNoArg (InputReader &ir, const Option &opt, bool longopt) const {
	if (ir.eof())
		return true;
	error(opt, longopt, "no argument expected");
	return false;
}


bool CmdLineParserBase::getIntArg (InputReader &ir, const Option &opt, bool longopt, int &arg) const {
	if (checkArgPrefix(ir, opt, longopt)) {
		if (ir.parseInt(arg) && ir.eof())
			return true;
		error(opt, longopt, "integer value expected");
	}
	return false;
}


bool CmdLineParserBase::getUIntArg (InputReader &ir, const Option &opt, bool longopt, unsigned &arg) const {
	if (checkArgPrefix(ir, opt, longopt)) {
		if (ir.parseUInt(arg) && ir.eof())
			return true;
		error(opt, longopt, "unsigned integer value expected");
	}
	return false;
}


bool CmdLineParserBase::getDoubleArg (InputReader &ir, const Option &opt, bool longopt, double &arg) const {
	if (checkArgPrefix(ir, opt, longopt)) {
		if (ir.parseDouble(arg) != 0 && ir.eof())
			return true;
		error(opt, longopt, "floating point value expected");
	}
	return false;
}


bool CmdLineParserBase::getStringArg (InputReader &ir, const Option &opt, bool longopt, string &arg) const {
	if (checkArgPrefix(ir, opt, longopt)) {
		arg.clear();
		while (!ir.eof())
			arg += ir.get();
		if (!arg.empty()) 
			return true;
		error(opt, longopt, "string argument expected");
	}
	return false;
}


