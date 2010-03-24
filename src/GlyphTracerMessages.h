/*************************************************************************
** GlyphTracerMessages.h                                                **
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

#ifndef GLYPHTRACERMESSAGES_H
#define GLYPHTRACERMESSAGES_H

#include "GFGlyphTracer.h"
#include "Message.h"
#include "types.h"

class GlyphTracerMessages : public GFGlyphTracer::Callback
{
	public:

		GlyphTracerMessages () : _output(false) {}
		~GlyphTracerMessages ()                 {if (_output) Message::mstream() << std::endl;}
		void beginChar (UInt8 c) {Message::mstream() << '[';}
		void emptyChar (UInt8 c) {Message::mstream() << "(empty)]";}

		void setFont (const std::string &fname) {
			if (fname != _fname) {
				if (!_fname.empty())
					Message::mstream() << std::endl;
				Message::mstream() << "tracing glyphs of " << fname.substr(0, fname.length()-3) << std::endl;
				_fname = fname;
				_output = true;
			}
		}

      void endChar (UInt8 c) {
			if (isprint(c))
				Message::mstream() << c;
			else
				Message::mstream() << '#' << unsigned(c);
			Message::mstream() << ']';
		}

	private:
		std::string _fname;
		bool _output;
};


#endif
