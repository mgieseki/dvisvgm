/*************************************************************************
** SpecialActions.h                                                     **
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

#ifndef SPECIALACTIONS_H
#define SPECIALACTIONS_H

#include <string>
#include "BoundingBox.h"
#include "Color.h"
#include "Matrix.h"

struct XMLNode;

struct SpecialActions
{
	virtual ~SpecialActions () {}
	virtual double getX() const =0;
	virtual double getY() const =0;
	virtual void setX(double x) =0;
	virtual void setY(double y) =0;
	virtual void finishLine () =0;
	virtual void setColor (const Color &color) =0;
	virtual Color getColor () const =0;
	virtual void setMatrix (const Matrix &m) =0;
	virtual const Matrix& getMatrix () const =0;
	virtual void setBgColor (const Color &color) =0;
	virtual void appendToPage (XMLNode *node) =0;
	virtual void appendToDefs (XMLNode *node) =0;
	virtual BoundingBox& bbox () =0;
	virtual BoundingBox& bbox (const std::string &name, bool reset=false) =0;
	virtual void embed (const BoundingBox &bbox) =0;
	virtual void embed (const DPair &p, double r=0) =0;
	virtual void progress (const char *id) {}
};


class SpecialEmptyActions : public SpecialActions
{
	public:
		double getX() const {return 0;}
		double getY() const {return 0;}
		void setX(double x) {}
		void setY(double y) {}
   	void finishLine ()  {}
		void setColor (const Color &color) {}
		void setBgColor (const Color &color) {}
		Color getColor () const {return 0;}
		void setMatrix (const Matrix &m) {}
		const Matrix& getMatrix () const {return _matrix;}
		void appendToPage (XMLNode *node) {}
		void appendToDefs (XMLNode *node) {}
		BoundingBox& bbox () {return _bbox;}
      BoundingBox& bbox (const std::string &name, bool reset=false) {return _bbox;}
      void embed (const BoundingBox &bbox) {}
      void embed (const DPair &p, double r=0) {}

	private:
		BoundingBox _bbox;
		Matrix _matrix;
};


#endif
