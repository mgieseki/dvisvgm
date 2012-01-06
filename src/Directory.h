/*************************************************************************
** Directory.h                                                          **
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

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#ifdef __WIN32__
	#include <windows.h>
#else
	#include <dirent.h>
#endif

class Directory
{
   public:
		Directory ();
      Directory (std::string path);
		~Directory ();
		bool open (std::string path);
		void close ();
		const char* read (char type='a');

   private:
		std::string _dirname;
#ifdef __WIN32__
		bool firstread;
		HANDLE handle;
		WIN32_FIND_DATA fileData;
#else
		DIR *dir;
		struct dirent *dirent;
#endif
};

#endif
