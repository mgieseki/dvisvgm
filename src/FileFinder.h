/*************************************************************************
** FileFinder.h                                                         **
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

#ifndef KPSFILEFINDER_H
#define KPSFILEFINDER_H

#include <string>
#include "FontMap.h"

class FileFinder
{
	class Impl
	{
		public:
			const char* lookup (const std::string &fname, bool extended);
			const char* lookupEncFile (std::string fontname);
			const char* lookupEncName (std::string fname);
			static Impl& instance ();
			static void setProgname (const char *progname) {_progname = progname;}
			static void enableMktex (bool enable)          {_mktex_enabled = enable;}
			static void setUserFontMap (const char *fname) {_usermapname = fname;}

		protected:
			Impl ();
			~Impl ();
			const char* findFile (const std::string &fname);
			const char* findMappedFile (std::string fname);			
			const char* mktex (const std::string &fname);
			void initFontMap ();

		private:
			static Impl *_instance;
			static const char *_progname;
			static bool _mktex_enabled;
		   static FontMap _fontmap;
			static const char *_usermapname;
	};

   public:
		static const char* lookup (const std::string &fname, bool extended=true) {
			return Impl::instance().lookup(fname, extended);
		}

		static const char* lookupEncFile (std::string fontname) {
			return Impl::instance().lookupEncFile(fontname);
		}

		static const char* lookupEncName (std::string fname) {
			return Impl::instance().lookupEncName(fname);
		}
			
		static const void init (const char *progname, bool enable_mktexmf) {
			Impl::setProgname(progname);
			Impl::enableMktex(enable_mktexmf);
		}

		static void setUserFontMap (const char *fname) {
			Impl::setUserFontMap(fname);
		}
};

#endif
