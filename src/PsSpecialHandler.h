/*************************************************************************
** PsSpecialHandler.h                                                   **
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

#ifndef PSSPECIALHANDLER_H
#define PSSPECIALHANDLER_H

#include <stack>
#include <vector>
#include "GraphicPath.h"
#include "PSInterpreter.h"
#include "SpecialHandler.h"

class XMLElementNode;

class PsSpecialHandler : public SpecialHandler, protected PSActions
{
	typedef GraphicPath<double> Path;

	class ClippingStack
	{
		public:
			void push ();
			void push (const Path &path, int saveID=-1);
			void replace (const Path &path);
			void dup (int saveID=-1);
			void pop (int saveID=-1, bool grestore=false);
			void clear ();
			bool empty () {return _stack.empty();}
			Path* top ();
			int topID ()  {return _stack.empty() ? 0 : _stack.top().pathID;}

		private:
			struct Entry {
				int pathID;  ///< index referencing a path of the pool
				int saveID;  ///< if >=0, path was pushed by 'save', and saveID holds the ID of the
				Entry (int pid, int sid) : pathID(pid), saveID(sid) {}
			};
			std::vector<Path> _paths;  ///< pool of all clipping paths
			std::stack<Entry> _stack;
	};

	public:
		PsSpecialHandler ();
		~PsSpecialHandler ();
		const char* name () const   {return "ps";}
		const char* info () const   {return "dvips PostScript specials";}
		const char** prefixes () const;
		bool process (const char *prefix, std::istream &is, SpecialActions *actions);

	protected:
		void initialize (SpecialActions *actions);
		void moveToDVIPos ();
		void psfile (const std::string &fname, const std::map<std::string,std::string> &attr);

      /// scale given value by current PS scale factors
      double scale (double v) const {return v*(_sx*_cos*_cos + _sy*(1-_cos*_cos));}

      void applyscalevals (std::vector<double> &p) {_sx = p[0]; _sy = p[1]; _cos = p[2];}
		void clip (std::vector<double> &p)           {clip(p, false);}
		void clip (std::vector<double> &p, bool evenodd);
		void closepath (std::vector<double> &p);
		void curveto (std::vector<double> &p);
		void eoclip (std::vector<double> &p)         {clip(p, true);}
		void eofill (std::vector<double> &p)         {fill(p, true);}
		void fill (std::vector<double> &p, bool evenodd);
		void fill (std::vector<double> &p)           {fill(p, false);}
		void grestore (std::vector<double> &p);
		void grestoreall (std::vector<double> &p);
		void gsave (std::vector<double> &p);
		void initclip (std::vector<double> &p);
		void lineto (std::vector<double> &p);
		void moveto (std::vector<double> &p);
		void newpath (std::vector<double> &p);
		void querypos (std::vector<double> &p)       {_currentpoint = DPair(p[0], p[1]);}
		void restore (std::vector<double> &p);
		void rotate (std::vector<double> &p);
		void save (std::vector<double> &p);
		void scale (std::vector<double> &p);
		void setcmykcolor (std::vector<double> &cmyk);
		void setdash (std::vector<double> &p);
		void setgray (std::vector<double> &p);
		void sethsbcolor (std::vector<double> &hsb);
		void setlinecap (std::vector<double> &p)     {_linecap = UInt8(p[0]);}
		void setlinejoin (std::vector<double> &p)    {_linejoin = UInt8(p[0]);}
		void setlinewidth (std::vector<double> &p)   {_linewidth = p[0] ? scale(p[0])*1.00375 : 0.5;}
		void setmatrix (std::vector<double> &p);
		void setmiterlimit (std::vector<double> &p)  {_miterlimit = p[0]*1.00375;}
		void setopacityalpha (std::vector<double> &p){_opacityalpha = p[0];}
		void setrgbcolor (std::vector<double> &rgb);
		void stroke (std::vector<double> &p);
		void translate (std::vector<double> &p);
		void executed ();

   private:
		PSInterpreter _psi;
		SpecialActions *_actions;
		bool _initialized;
		XMLElementNode *_xmlnode;   ///< if != 0, created SVG elements are appended to this node
		Path _path;
		DPair _currentpoint;        ///< current PS position in bp units
      double _sx, _sy;            ///< horizontal and vertical scale factors retrieved by operator "applyscalevals"
      double _cos;                ///< cosine of angle between (1,0) and transform(1,0)
		double _linewidth;          ///< current linewidth
		double _miterlimit;         ///< current miter limit
		double _opacityalpha;       ///< opacity level (0=fully transparent, ..., 1=opaque)
		UInt8 _linecap  : 2;        ///< current line cap (0=butt, 1=round, 2=projecting square)
		UInt8 _linejoin : 2;        ///< current line join (0=miter, 1=round, 2=bevel)
		double _dashoffset;         ///< current dash offset
		std::vector<double> _dashpattern;
		ClippingStack _clipStack;
};

#endif
