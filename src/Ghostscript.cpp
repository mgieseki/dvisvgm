/*************************************************************************
** Ghostscript.cpp                                                      **
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
#include <sstream>
#include "Ghostscript.h"

using namespace std;

#ifdef __WIN32__
#define GS_DL_NAME "gsdll32.dll"
#else
#define GS_DL_NAME "libgs.so"
#endif


/** Loads the Ghostscript library but does not create an instance. This
 *  constructor should only be used to call available() and revision(). */
Ghostscript::Ghostscript () : DLLoader(GS_DL_NAME), _inst(0)
{
}


/** Tries to load the shared library and to initialize Ghostscript. 
 * @param[in] argc number of parameters in array argv
 * @param[in] argv parameters passed to Ghostscript 
 * @param[in] caller this parameter is passed to all callback functions */
Ghostscript::Ghostscript (int argc, const char **argv, void *caller) 
	: DLLoader(GS_DL_NAME)
{
	int status = new_instance(&_inst, caller);
	if (status < 0) 
		_inst = 0;
	else {
		init_with_args(argc, (char**)argv);
	}
}


/** Exits Ghostscript and unloads the dynamic library. */
Ghostscript::~Ghostscript () {
	if (_inst) {
		exit();
		delete_instance();
	}
}


/** Returns true if Ghostscript library was found and can be loaded. */
bool Ghostscript::available () {
	return loaded();
}


/** Retrieves version information about Ghostscript. 
 * @param[out] r takes the revision information (see GS API documentation for further details) 
 * @return true on success  */
bool Ghostscript::revision (gsapi_revision_t *r) {
	if (PFN_gsapi_revision fn = (PFN_gsapi_revision)loadFunction("gsapi_revision"))
		return (fn(r, sizeof(gsapi_revision_t)) == 0);
	return false;
}


string Ghostscript::revision () {
	gsapi_revision_t r;
	if (revision(&r)) {
		ostringstream oss;
		oss << r.product << ' ' << (r.revision/100) << '.' << (r.revision%100);
		return oss.str();
	}
	return "";
}


/** Creates a new instance of Ghostscript. This method is called by the constructor and
 *  should not be used elsewhere. 
 *  @param[out] psinst handle of newly created instance (or 0 on error)
 *  @param[in] caller pointer forwarded to callback functions */
int Ghostscript::new_instance (void **psinst, void *caller) {
	if (PFN_gsapi_new_instance fn = (PFN_gsapi_new_instance)loadFunction("gsapi_new_instance"))
		return fn(psinst, caller);
   *psinst = 0;
	return 0;
}


/** Destroys the current instance of Ghostscript. This method is called by the destructor
 *  and should not be used elsewhere. */
void Ghostscript::delete_instance () {
	if (PFN_gsapi_delete_instance fn = (PFN_gsapi_delete_instance)loadFunction("gsapi_delete_instance"))
		fn(_inst);
}


/** Exits the interpreter. Must be called before destroying the GS instance. */
int Ghostscript::exit () {
	if (PFN_gsapi_exit fn = (PFN_gsapi_exit)loadFunction("gsapi_exit"))
		return fn(_inst);
	return 0;
}


/** Sets the I/O callback functions. 
 * @param[in] in pointer to stdin handler
 * @param[in] out pointer to stdout handler
 * @param[in] err pointer to stderr handler */
int Ghostscript::set_stdio (Stdin in, Stdout out, Stderr err) {
	if (PFN_gsapi_set_stdio fn = (PFN_gsapi_set_stdio)loadFunction("gsapi_set_stdio"))
		return fn(_inst, in, out, err);
	return 0;
}


/** Initializes Ghostscript with a set of optional parameters. This
 *  method is called by the constructor and should not be used elsewhere.
 *  @param[in] argc number of paramters
 *  @param[in] argv parameters passed to Ghostscript */
int Ghostscript::init_with_args (int argc, char **argv) {
	if (PFN_gsapi_init_with_args fn = (PFN_gsapi_init_with_args)loadFunction("gsapi_init_with_args"))
		return fn(_inst, argc, argv);
	return 0;
}


/** Tells Ghostscript that several calls of run_string_continue will follow. */
int Ghostscript::run_string_begin (int user_errors, int *pexit_code) {
	if (PFN_gsapi_run_string_begin fn = (PFN_gsapi_run_string_begin)loadFunction("gsapi_run_string_begin"))
		return fn(_inst, user_errors, pexit_code);
	*pexit_code = 0;
	return 0;
}


/** Executes a chunk of PostScript commands given by a buffer of characters. The size of
 *  this buffer must not exceed 64KB. Longer programs can be split into arbitrary smaller chunks
 *  and passed to Ghostscript by successive calls of run_string_continue. 
 *  @param[in] str buffer containing the PostScript code
 *  @param[in] length number of characters in buffer 
 *  @param[in] user_errors if non-negative, the default PS error values will be generated, otherwise this value is returned 
 *  @param[out] pexit_code takes the PS error code */
int Ghostscript::run_string_continue (const char *str, unsigned length, int user_errors, int *pexit_code) {
	if (PFN_gsapi_run_string_continue fn = (PFN_gsapi_run_string_continue)loadFunction("gsapi_run_string_continue"))
		return fn(_inst, str, length, user_errors, pexit_code);
	*pexit_code = 0;
	return 0;
}


/** Terminates the successive code feeding. Must be called after the last call of run_string_continue. */
int Ghostscript::run_string_end (int user_errors, int *pexit_code) {
	if (PFN_gsapi_run_string_end fn = (PFN_gsapi_run_string_end)loadFunction("gsapi_run_string_end"))
		return fn(_inst, user_errors, pexit_code);
	*pexit_code = 0;
	return 0;
}

