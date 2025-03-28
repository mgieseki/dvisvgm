/*************************************************************************
** StreamWriter.hpp                                                     **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2025 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef STREAMWRITER_HPP
#define STREAMWRITER_HPP

#include <ostream>
#include <vector>

class HashFunction;

class StreamWriter {
	public:
		explicit StreamWriter (std::ostream &os) : _os(os) {}
		void writeUnsigned (uint32_t val, int n) const;
		void writeSigned (int32_t val, int n) const;
		void writeBytes (const std::vector<uint8_t> &bytes) const;
		void writeBytes (const uint8_t *buf, size_t bufsize) const;
		void writeBytes (int byte, size_t count) const;
		void writeString (const std::string &str, bool finalZero=false) const;
		void writeUnsigned (uint32_t val, int n, HashFunction &hashfunc) const;
		void writeSigned (int32_t val, int n, HashFunction &hashfunc) const;
		void writeString (const std::string &str, HashFunction &hashfunc, bool finalZero=false) const;

	private:
		std::ostream &_os;
};

#endif
