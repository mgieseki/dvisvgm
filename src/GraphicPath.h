/*************************************************************************
** GraphicPath.h                                                        **
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

#ifndef GRAPHICPATH_H
#define GRAPHICPATH_H

#include <list>
#include <ostream>
#include <vector>
#include "BoundingBox.h"
#include "Pair.h"
#include "Matrix.h"


template <typename T>
class GraphicPath
{
	typedef Pair<T> Point;
	
	struct Command {
		enum Type {MOVETO, LINETO, CONICTO, CUBICTO, CLOSEPATH};
		
		Command (Type t) : type(t) {}
		
		Command (Type t, const Point &p) : type(t) {
			params[0] = p;
		}

		Command (Type t, const Point &p1, const Point &p2) : type(t) {
			params[0] = p1;
			params[1] = p2;
		}

		Command (Type t, const Point &p1, const Point &p2, const Point &p3) : type(t) {
			params[0] = p1;
			params[1] = p2;
			params[2] = p3;
		}

		int numParams () const {
			switch (type) {
				case CLOSEPATH : return 0;
				case MOVETO    :
				case LINETO    : return 1;
				case CONICTO   : return 2;
				case CUBICTO   : return 3;
			}
			return 0;
		}

		void transform (const Matrix &matrix) {
			for (int i=0; i < numParams(); i++)
				params[i] = matrix * params[i];
		}
		
		Type type;
		Point params[3];
	};

	struct Actions
	{
		virtual void moveto (const Point &p) {}
		virtual void lineto (const Point &p) {}
		virtual void hlineto (const T &y) {}
		virtual void vlineto (const T &x) {}
		virtual void sconicto (const Point &p) {}
		virtual void conicto (const Point &p1, const Point &p2) {}
		virtual void scubicto (const Point &p1, const Point &p2) {}
		virtual void cubicto (const Point &p1, const Point &p2, const Point &p3) {}
		virtual void closepath () {}
	};

	typedef typename std::vector<Command>::iterator Iterator;
	typedef typename std::vector<Command>::const_iterator ConstIterator;
	typedef typename std::vector<Command>::const_reverse_iterator ConstRevIterator;

   public:
		void newpath () {
			_commands.clear();	
		}

		/// Returns true if path is empty (there is nothing to draw)
		bool empty () const {
			return _commands.empty();
		}

		void moveto (const T &x, const T &y) {
			moveto(Point(x, y));
		}
		
		void moveto (const Point &p) {
			// avoid sequences of several MOVETOs; always use latest
			if (_commands.empty() || _commands.back().type != Command::MOVETO)
				_commands.push_back(Command(Command::MOVETO, p));
			else
				_commands.back().params[0] = p;
		}

		void lineto (const T &x, const T &y) {
			lineto(Point(x, y));
		}
		
		void lineto (const Point &p) {
			_commands.push_back(Command(Command::LINETO, p));
		}

		void conicto (const T &x1, const T &y1, const T &x2, const T &y2) {
			conicto(Point(x1, y1), Point(x2, y2));
		}

		void conicto (const Point &p1, const Point &p2) {
			_commands.push_back(Command(Command::CONICTO, p1, p2));
		}

		void cubicto (const T &x1, const T &y1, const T &x2, const T &y2, const T &x3, const T &y3) {
			cubicto(Point(x1, y1), Point(x2, y2), Point(x3, y3));
		}

		void cubicto (const Point &p1, const Point &p2, const Point &p3) {
			_commands.push_back(Command(Command::CUBICTO, p1, p2, p3));
		}
		
		void closepath () {
			_commands.push_back(Command(Command::CLOSEPATH));
		}
		
		void writeSVG (std::ostream &os, double sx=1.0, double sy=1.0) const {
			struct WriteActions : Actions {
				WriteActions (std::ostream &os) : _os(os) {}
				void moveto (const Point &p)   {write('M', p);}
				void lineto (const Point &p)   {write('L', p);}
				void hlineto (const T &y)      {_os << 'H' << y;}
				void vlineto (const T &x)      {_os << 'V' << x;}
				void sconicto (const Point &p) {write('T', p); }
				void conicto (const Point &p1, const Point &p2)  {write('Q', p1); write(' ', p2);}
				void scubicto (const Point &p1, const Point &p2) {write('S', p1); write(' ', p2);}
				void cubicto (const Point &p1, const Point &p2, const Point &p3) {write('C', p1); write(' ', p2); write(' ', p3);}
				void closepath () {_os << 'Z';}
				void write (char prefix, const Point &p) {_os << prefix << p.x() << ' ' << p.y();}
				std::ostream &_os;
			} actions(os);
			iterate(actions, true);
		}

		void computeBBox (BoundingBox &bbox) const {
			struct BBoxActions : Actions {
				BBoxActions (BoundingBox &bb) : bbox(bb) {}
				void moveto (const Point &p) {bbox.embed(p);}
				void lineto (const Point &p) {bbox.embed(p);}
				void conicto (const Point &p1, const Point &p2) {bbox.embed(p1); bbox.embed(p2);}
				void cubicto (const Point &p1, const Point &p2, const Point &p3) {bbox.embed(p1); bbox.embed(p2); bbox.embed(p3);}
				BoundingBox &bbox;
			} actions(bbox);
			iterate(actions, false);
		}

		void transform (const Matrix &matrix) {
			FORALL(_commands, Iterator, it)
				it->transform(matrix);
		}

		void iterate (Actions &actions, bool optimize) const;

   private:
		std::vector<Command> _commands;
};


template <typename T>
void GraphicPath<T>::iterate (Actions &actions, bool optimize) const {
	ConstIterator prev;  // pointer to preceding command
	Point fp; // first point of current path
	Point cp; // current point
	Point pstore[2];
	FORALL(_commands, ConstIterator, it) {
		const Point *params = it->params;
		switch (it->type) {
			case Command::MOVETO:
				actions.moveto(params[0]);
				fp = params[0];
				break;
			case Command::LINETO:
				if (optimize) {
					if (cp.x() == params[0].x())
						actions.vlineto(params[0].y());
					else if (cp.y() == params[0].y())
						actions.hlineto(params[0].x());
					else
						actions.lineto(params[0]);
				}
				else
					actions.lineto(params[0]);
				break;
			case Command::CONICTO:
				if (optimize && prev->type == Command::CONICTO && params[0] == pstore[1]*T(2)-pstore[0])
					actions.sconicto(params[1]);
				else
					actions.conicto(params[0], params[1]);
				pstore[0] = params[0]; // store control point and
				pstore[1] = params[1]; // curve endpoint
				break;
			case Command::CUBICTO:
				// is first control point reflection of preceding second control point?
				if (optimize && prev->type == Command::CUBICTO && params[0] == pstore[1]*T(2)-pstore[0])
					actions.scubicto(params[1], params[2]);
				else
					actions.cubicto(params[0], params[1], params[2]);
				pstore[0] = params[1]; // store second control point and
				pstore[1] = params[2]; // curve endpoint
				break;
			case Command::CLOSEPATH:
				actions.closepath();
				cp = fp;
		}
		// update current point
		const int np = it->numParams();
		if (np > 0)
			cp = it->params[np-1];
		prev = it;
	}
}

#endif
