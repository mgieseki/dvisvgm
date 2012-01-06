/*************************************************************************
** DLLoader.cpp                                                         **
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

#include "DLLoader.h"


DLLoader::DLLoader (const char *dlname) {
#ifdef __WIN32__
	_handle = LoadLibrary(dlname);
#else
	_handle = dlopen(dlname, RTLD_LAZY);
#endif
}


DLLoader::~DLLoader () {
	if (_handle) {
#ifdef __WIN32__
		FreeLibrary(_handle);
#else
		dlclose(_handle);
#endif
	}
}


void* DLLoader::loadFunction (const char *name) {
	if (_handle) {
#ifdef __WIN32__
		return (void*)GetProcAddress(_handle, name);
#else
		return dlsym(_handle, name);
#endif
	}
	return 0;
}
