/*************************************************************************
** GlyphTracerMessages.h                                                **
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

#ifndef GLYPHTRACERMESSAGES_H
#define GLYPHTRACERMESSAGES_H

#include <sstream>
#include "GFGlyphTracer.h"
#include "Message.h"
#include "types.h"

class GlyphTracerMessages : public GFGlyphTracer::Callback
{
	public:
		GlyphTracerMessages (bool sfmsg=true, bool autonl=true) : _sfmsg(sfmsg), _autonl(autonl) {}

		~GlyphTracerMessages () {
			if (_autonl)
				Message::mstream() << '\n';
		}

		void endChar (UInt8 c) {
			std::ostringstream oss;
			oss << '[';
			if (isprint(c))
				oss << c;
			else
				oss << '#' << unsigned(c);
			oss << ']';
			Message::mstream(false, Message::MC_TRACING) << oss.str();
		}

		void setFont (const std::string &fname) {
			if (_sfmsg && fname != _fname) {
				if (!_fname.empty())
					Message::mstream() << '\n';
				Message::mstream(false, Message::MC_STATE) << "tracing glyphs of " << fname.substr(0, fname.length()-3) << '\n';
				_fname = fname;
			}
		}

	private:
		std::string _fname;
		bool _sfmsg, _autonl;
};


#endif
