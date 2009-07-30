/***********************************************************************
** Ghostscript.h                                                      **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#ifndef GHOSTSCRIPT_H
#define GHOSTSCRIPT_H

#include <string>
#include "DLLoader.h"

#if defined(__WIN32__) && !defined(_Windows)
	#define _Windows
#endif

#include "iapi.h"


/** Wrapper class of (a subset of) the Ghostscript API. */
class Ghostscript : public DLLoader
{
	public:
		typedef int (GSDLLCALLPTR Stdin) (void *caller, char *buf, int len);
		typedef int (GSDLLCALLPTR Stdout) (void *caller, const char *str, int len); 
		typedef int (GSDLLCALLPTR Stderr) (void *caller, const char *str, int len);  

	public:
		Ghostscript ();
		Ghostscript (int argc, const char **argv, void *caller=0);
		~Ghostscript ();
		bool available ();
		bool revision (gsapi_revision_t *r);
		std::string revision ();
		int set_stdio (Stdin in, Stdout out, Stderr err);  
		int run_string_begin (int user_errors, int *pexit_code);
		int run_string_continue (const char *str, unsigned int length, int user_errors, int *pexit_code);
		int run_string_end (int user_errors, int *pexit_code); 
		int exit (); 

	protected:
		Ghostscript (const Ghostscript &gs) {}
		int init_with_args (int argc, char **argv);
		int new_instance (void **psinst, void *caller);
		void delete_instance (); 

	private:
		void *_inst; ///< Ghostscript handle needed to call the gsapi_foo functions
};

#endif
