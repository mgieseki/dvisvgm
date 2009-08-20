/*************************************************************************
** CmdLineParserBase.h                                                  **
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

#ifndef CMDLINEPARSERBASE_H
#define CMDLINEPARSERBASE_H

#include <string>
#include <vector>

class InputReader;

class CmdLineParserBase
{	
	protected:
		struct Option;

		struct OptionHandler {
			virtual void operator () (InputReader &ir, const Option &opt, bool longopt) const=0;
		};

		template <typename T>
		class OptionHandlerImpl : public OptionHandler {
			protected:
				typedef void (T::*LocalHandler)(InputReader &ir, const Option &opt, bool longopt);

			public:
				OptionHandlerImpl (T &obj, LocalHandler handler) : _obj(obj), _handler(handler) {}
				void operator () (InputReader &ir, const Option &opt, bool longopt) const {(_obj.*_handler)(ir, opt, longopt);}

			private:
				T& _obj;
				void (T::*_handler)(InputReader &ir, const Option &opt, bool longopt);
		};

		struct Option {
			char shortname;
			const char *longname;
			const OptionHandler& handler;
		};

	public:
		virtual void help () const  {}
		virtual int numOptions () const  {return 0;}
		virtual int numFiles () const    {return _files.size();}
		virtual const char* file (int n) {n >= 0 && n < _files.size() ? _files[n].c_str() : 0;}
		virtual void status () const;

	protected:
		CmdLineParserBase (int argc, char **argv) : _argc(argc), _argv(argv) {}
		void parse ();
		void out (const char *str) const;
		void error (const Option &opt, bool longopt, const char *msg) const;
		bool checkArgPrefix (InputReader &ir, const Option &opt, bool longopt) const;
		bool checkNoArg (InputReader &ir, const Option &opt, bool longopt) const;
		bool getIntArg (InputReader &ir, const Option &opt, bool longopt, int &arg) const;
		bool getUIntArg (InputReader &ir, const Option &opt, bool longopt, unsigned &arg) const;
		bool getDoubleArg (InputReader &ir, const Option &opt, bool longopt, double &arg) const;
		bool getStringArg (InputReader &ir, const Option &opt, bool longopt, std::string &arg) const;
		const Option* option (char shortname) const;
		const Option* option (const std::string &longname) const;
		virtual const Option* options () const {return 0;}

	private:
		int _argc;
		char **_argv;
		std::vector<std::string> _files;  ///< filename parameters
};

#endif
