/*************************************************************************
** MapLine.cpp                                                          **
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

#include <cstring>
#include <sstream>
#include "InputBuffer.h"
#include "InputReader.h"
#include "MapLine.h"

using namespace std;


/** Constructs a MapLine object by parsing a single mapline from the given stream. */
MapLine::MapLine (istream &is) : _sfindex(0), _slant(0), _extend(0), _embed(true) {
   char buf[256];
   is.getline(buf, 256);
   parse(buf);
}


// Some of the following functions have been derived from the dvipdfmx source file fontmap.c:
// http://cvs.ktug.or.kr/viewcvs/dvipdfmx/src/fontmap.c?revision=1.43&view=markup


/** Returns true if the given string is in dvips mapline format, and false if it's in dvipdfm format.
    @param[in] line string to check */
bool MapLine::isDVIPSFormat (const char *line) const {
   if (strchr(line, '"') || strchr(line, '<'))  // these chars are only present in dvips maps
      return true;
   char prevchar = ' ';
   int entry_count=0;
   for (const char *p=line; *p; ++p) {
      if (isspace(prevchar)) {
         if (*p == '-') // options starting with '-' are only present in dvipdfm map files
            return false;
         if (!isspace(*p))
            entry_count++;
      }
      prevchar = *p;
   }
   // tfm_name and ps_name only => dvips map
   return entry_count == 2;
}


/** Separates main font name and subfont name from a given combined name.
 *  Example: "basename@sfname@10" => {"basename10", "sfname"}
 *  @param[in,out] fontname complete fontname; after separation: main fontname only
 *  @param[out] subfontname name of subfont
 *  @return true on success */
static bool split_fontname (string &fontname, string &subfontname) {
	size_t pos1;    // index of first '@'
	if ((pos1 = fontname.find('@')) != string::npos && pos1 > 0) {
		size_t pos2; // index of second '@'
		if ((pos2 = fontname.find('@', pos1+1)) != string::npos && pos2 > pos1+1) {
			subfontname = fontname.substr(pos1+1, pos2-pos1-1);
			fontname = fontname.substr(0, pos1) + fontname.substr(pos2+1);
			return true;
		}
	}
	return false;
}


/** Parses a single mapline and stores the scanned data in member variables.
 *  The line may either be given in dvips or dvipdfmx mapfile format.
 *  @param[in] line the mapline */
void MapLine::parse (const char *line) {
   CharInputBuffer ib(line, strlen(line));
   BufferInputReader ir(ib);
   _texname = ir.getString();
	split_fontname(_texname, _sfname);
   if (isDVIPSFormat(line))
      parseDVIPSLine(ir);
   else
      parseDVIPDFMLine(ir);
}


/** Parses a single line in dvips mapfile format.
 *  @param[in] ir the input stream must be assigned to this reader */
void MapLine::parseDVIPSLine (InputReader &ir) {
   ir.skipSpace();
   if (ir.peek() != '<' && ir.peek() != '"')
      _psname = ir.getString();
   ir.skipSpace();
   while (ir.peek() == '<' || ir.peek() == '"') {
      if (ir.peek() == '<') {
         ir.get();
         if (ir.peek() == '[')
            ir.get();
         string name = ir.getString();
         if (name.length() > 4 && name.substr(name.length()-4) == ".enc")
            _encname = name.substr(0, name.length()-4);
         else
            _fontname = name;
      }
      else {  // ir.peek() == '"' => list of PS font operators
         string options = ir.getQuotedString('"');
         StringInputBuffer sib(options);
         BufferInputReader sir(sib);
         while (!sir.eof()) {
            double number;
            if (sir.parseDouble(number)) {
               // operator with preceding numeric parameter (value opstr)
               string opstr = sir.getString();
               if (opstr == "SlantFont")
                  _slant = number;
               else if (opstr == "ExtendFont")
                  _extend = number;
            }
            else {
               // operator without parameter => skip for now
               sir.getString();
            }
         }
      }
		ir.skipSpace();
   }
}


static void throw_number_expected (char opt, bool integer_only=false) {
   ostringstream oss;
   oss << "option -" << opt << ": " << (integer_only ? "integer" : "floating point") << " value expected";
   throw MapLineException(oss.str());
}


/** Parses a single line in dvipdfmx mapfile format.
 *  @param[in] ir the input stream must be assigned to this reader */
void MapLine::parseDVIPDFMLine (InputReader &ir) {
   ir.skipSpace();
   if (ir.peek() != '-') {
      _encname = ir.getString();
		if (_encname == "default" || _encname == "none")
			_encname.clear();
	}
   ir.skipSpace();
      if (ir.peek() != '-')
      _fontname = ir.getString();
   if (!_fontname.empty()) {
		parseFilenameOptions(_fontname);
   }
   ir.skipSpace();
   while (ir.peek() == '-') {
      ir.get();
      char option = ir.get();
      if (!isprint(option))
         throw MapLineException("option character expected");
      ir.skipSpace();
      switch (option) {
         case 's': // slant
            if (!ir.parseDouble(_slant))
               throw_number_expected('s');
            break;
         case 'e': // extend
            if (!ir.parseDouble(_extend))
               throw_number_expected('e');
            break;
         case 'b': // bold
            double bold;
            if (!ir.parseDouble(bold))
               throw_number_expected('b');
            break;
         case 'r': //remap
            break;
         case 'i': // ttc index
				int index;
            if (!ir.parseInt(index, false))
               throw_number_expected('i', true);
            break;
         case 'p':
            int dummy;
            if (!ir.parseInt(dummy, false))
               throw_number_expected('p', true);
            break;
         case 'u': // to unicode
            ir.getString();
            break;
         case 'v': // stemV
            int stemv;
            if (!ir.parseInt(stemv, true))
               throw_number_expected('v', true);
            break;
         case 'm': // map single chars
				ir.skipUntil("-");
            break;
         case 'w': // writing mode
            int wmode;
            if (!ir.parseInt(wmode, false))
               throw_number_expected('w', true);
            break;
         default:
            ostringstream oss;
            oss << "invalid option: -" << option;
            throw MapLineException(oss.str());
      }
      ir.skipSpace();
   }
}


/** [:INDEX:][!]FONTNAME[/CSI][,VARIANT] */
void MapLine::parseFilenameOptions (string fname) {
   _fontname = fname;
	StringInputBuffer ib(fname);
	BufferInputReader ir(ib);
	if (ir.peek() == ':' && isdigit(ir.peek(1))) {  // index given?
		ir.get();
		_sfindex = ir.getInt();  // subfont index
      if (ir.peek() == ':')
         ir.get();
      else
         _sfindex = 0;
	}
	if (ir.peek() == '!') { // no embedding
		ir.get();
      _embed = false;
   }

	bool csi_given=false, style_given=false;
	int pos;
	if ((pos = ir.find('/')) >= 0) {  // csi delimiter
		csi_given = true;
		_fontname = ir.getString(pos);
	}
	else if ((pos = ir.find(',')) >= 0) {
		style_given = true;
		_fontname = ir.getString(pos);
	}
	else
		_fontname = ir.getString();

	if (csi_given) {
		if ((pos = ir.find(',')) >= 0) {
			style_given = true;
			ir.getString(pos);  // charcoll
		}
		else if (ir.eof())
			throw MapLineException("CSI specifier expected");
		else
			ir.getString();  // charcoll
	}
	if (style_given) {
      ir.get();  // skip ','
		if (ir.check("BoldItalic")) {
		}
		else if (ir.check("Bold")) {
		}
		else if (ir.check("Italic")) {
		}
		if (!ir.eof())
			throw MapLineException("invalid style given");
	}
}
