/*************************************************************************
** PsSpecialHandler.cpp                                                 **
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

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include "FileFinder.h"
#include "Message.h"
#include "PsSpecialHandler.h"
#include "SpecialActions.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


static inline double str2double (const string &str) {
	double ret;
	istringstream iss(str);
	iss >> ret;
	return ret;
}


PsSpecialHandler::PsSpecialHandler () : _psi(this), _actions(0), _initialized(false)
{
}


/** Initializes the PostScript handler. It's called by the first use of process(). The
 *  deferred initialization speeds up the conversion of DVI files that doesn't contain
 *  PS specials. */
void PsSpecialHandler::initialize () {
	if (!_initialized) {
		// initial values of graphics state
		_linewidth = 1;
		_linecap = _linejoin = 0;
		_miterlimit = 4;
		_xmlnode = 0;

		// execute dvips prologue/header files
		const char *headers[] = {"tex.pro", "texps.pro", "special.pro", /*"color.pro",*/ 0};
		for (const char **p=headers; *p; ++p) {
			if (const char *path = FileFinder::lookup(*p, false)) {
				ifstream ifs(path);
				_psi.execute(ifs);
			}
			else
				Message::wstream(true) << "PostScript header file " << *p << " not found\n";
		}
		// push dictionary "TeXDict" with dvips definitions on dictionary stack
		// and initialize basic dvips PostScript variables
		_psi.execute(" TeXDict begin 0 0 1000 72.27 72.27 () @start ");
		_psi.execute(" 0 0 moveto ");
		_initialized = true;
	}
}


const char* PsSpecialHandler::info () const {
	static string str = "dvips PostScript specials (using " + Ghostscript().revision() + ")";
	return str.c_str();
}


void PsSpecialHandler::updatePos () {
	if (_actions) {
		ostringstream oss;
		const double x = _actions->getX(), y = _actions->getY();
		oss << ' ' << x << ' ' << y << " moveto ";
		_psi.execute(oss.str());
		_currentpoint = DPair(x, y);
	}
}


bool PsSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	if (!_initialized)
		initialize();
	_actions = actions;

	if (*prefix == '"') {
		// read and execute literal PostScript code (isolated by a wrapping save/restore pair)
		updatePos();
		_psi.execute(" @beginspecial @setspecial ");
		_psi.execute(is);
		_psi.execute(" @endspecial ");
	}
	else if (*prefix == '!') {
		// execute literal PostScript header
		_psi.execute(" @defspecial ");
		_psi.execute(is);
		_psi.execute(" @fedspecial ");
	}
	else if (strcmp(prefix, "header=") == 0) {
		// read and execute PS header files
		string fname;
		is >> fname;
		if (const char *path = FileFinder::lookup(fname, false)) {
			ifstream ifs(path);
			_psi.execute(ifs);
		}
		else
			Message::estream(true) << "PS header file '" << fname << "' not found";
	}
	else if (strcmp(prefix, "psfile=") == 0 || strcmp(prefix, "PSfile=") == 0) {
		if (_actions) {
			StreamInputReader in(is);
			string fname = in.getString(in.peek() == '"' ? '"' : 0);
			map<string,string> attr;
			in.parseAttributes(attr);
			psfile(fname, attr);
		}
	}
	else if (strcmp(prefix, "ps::") == 0) {
		if (is.peek() != '[') {
			// execute literal PostScript without initial positioning and without any wrapping code
			_psi.execute(is);
			_psi.execute(" setpos ");  // forces a call of PSActions::setpos to get the current PS position
			_actions->setX(_currentpoint.x());
			_actions->setY(_currentpoint.y());
		}
		else {
			is.get();
			string label;
			int c;
			while (!is.eof() && (c = is.get()) != ']')
				label += c;
			if (label != "begin" && label != "end") {
				Message::wstream(true) << "invalid PostScript special ps::[" << label << "]\n";
				return false;
			}
			_psi.execute(is);
		}
	}
	else { // ps: ...
		updatePos();
		StreamInputReader in(is);
		if (in.check(" plotfile ")) { // ps: plotfile fname
			string fname = in.getString();
			ifstream ifs(fname.c_str());
			if (ifs)
				_psi.execute(ifs);
			else
				Message::wstream(true) << "file '" << fname << "' not found in ps: plotfile\n";
		}
		else {
			// execute literal PostScript (without any wrapping code)
			_psi.execute(is);
			_psi.execute(" setpos ");  // forces a call of PSActions::setpos to get the current PS position
			_actions->setX(_currentpoint.x());
			_actions->setY(_currentpoint.y());
		}
	}
	return true;
}


void PsSpecialHandler::psfile (const string &fname, const map<string,string> &attr) {
	ifstream ifs(fname.c_str());
	if (!ifs)
		Message::wstream(true) << "file '" << fname << "' not found in special 'psfile'\n";
	else {
		const double BP = 72.27/72.0;   // factor to convert bp -> pt
		map<string,string>::const_iterator it;
		// coordinates of lower left and upper right vertex (result in TeX points)
		double llx = (it = attr.find("llx")) != attr.end() ? str2double(it->second)*BP : 0;
		double lly = (it = attr.find("lly")) != attr.end() ? str2double(it->second)*BP : 0;
		double urx = (it = attr.find("urx")) != attr.end() ? str2double(it->second)*BP : 0;
		double ury = (it = attr.find("ury")) != attr.end() ? str2double(it->second)*BP : 0;

		// actual width and height (result in 10th of TeX points)
		double rwi = (it = attr.find("rwi")) != attr.end() ? str2double(it->second)*BP : 0;
		double rhi = (it = attr.find("rhi")) != attr.end() ? str2double(it->second)*BP : 0;

		// user transformations (default values chosen according to dvips manual)
		double hoffset = (it = attr.find("hoffset")) != attr.end() ? str2double(it->second)*BP : 0;
		double voffset = (it = attr.find("voffset")) != attr.end() ? str2double(it->second)*BP : 0;
		double hsize   = (it = attr.find("hsize")) != attr.end() ? str2double(it->second)*BP : 612*BP;
		double vsize   = (it = attr.find("vsize")) != attr.end() ? str2double(it->second)*BP : 792*BP;
		double hscale  = (it = attr.find("hscale")) != attr.end() ? str2double(it->second) : 100;
		double vscale  = (it = attr.find("vscale")) != attr.end() ? str2double(it->second) : 100;
		double angle   = (it = attr.find("angle")) != attr.end() ? str2double(it->second) : 0;

		double x=_actions->getX(), y=_actions->getY();
		double w=(urx-llx), h=(ury-lly);  // width and height of (E)PS image in TeX points
		if (w <= 0)
			w = hsize;
		if (h <= 0)
			h = vsize;
		h *= -1;  	// must be negative to get the bounding box right

		double sx=1, sy=1;
		if (rwi != 0 || rhi != 0) {
			sx = rwi/10/w;
			sy = rhi/10/fabs(h);
			if (sx == 0)
				sx = sy;
			else if (sy == 0)
				sy = sx;
		}
		w *= sx;
		h *= sy;

		Matrix usertrans(1);  // transformations given by the user
		if (hscale != 100 || vscale != 100)
			usertrans.scale(hscale/100, vscale/100);
		if (angle != 0)
			usertrans.rotate(angle);
		if (hoffset != 0 || voffset != 0)
			usertrans.translate(hoffset, voffset);

		Matrix trans(1);  // final transformation to position the graphic correctly
		trans.translate(-llx, -lly).rmultiply(usertrans).scale(sx, -sy).translate(x, y);

		_xmlnode = new XMLElementNode("g");
		_xmlnode->addAttribute("transform", trans.getSVG());
		updatePos();
		_psi.execute(ifs);
		_actions->appendToPage(_xmlnode);
		_xmlnode = 0;

		// adapt bounding box
		BoundingBox bbox(x, y, x+w, y+h);
		bbox.transform(usertrans);
		_actions->bbox().embed(bbox);
	}
}

///////////////////////////////////////////////////////

void PsSpecialHandler::gsave (vector<double> &p) {
	_clipStack.dup();
}


void PsSpecialHandler::grestore (vector<double> &p) {
	if (!_clipStack.empty())
		_clipStack.pop();
}


void PsSpecialHandler::moveto (vector<double> &p) {
	_path.moveto(p[0], p[1]);
}


void PsSpecialHandler::lineto (vector<double> &p) {
	_path.lineto(p[0], p[1]);
}


void PsSpecialHandler::curveto (vector<double> &p) {
	_path.cubicto(p[0], p[1], p[2], p[3], p[4], p[5]);
}


void PsSpecialHandler::closepath (vector<double> &p) {
	_path.closepath();
}


/** Draws the current path recorded by previously executed path commands (moveto, lineto,...).
 *  @param[in] p not used */
void PsSpecialHandler::stroke (vector<double> &p) {
	if (!_path.empty() && _actions) {
		const double pt = 72.27/72.0;  // factor to convert bp -> pt
		ScalingMatrix scale(pt, pt);
		_path.transform(scale);

		// compute bounding box
		BoundingBox bbox;
		_path.computeBBox(bbox);
		bbox.expand(_linewidth/2);
		if (!_actions->getMatrix().isIdentity()) {
			_path.transform(_actions->getMatrix());
			if (!_xmlnode)
				bbox.transform(_actions->getMatrix());
		}

		ostringstream oss;
		_path.writeSVG(oss);
		XMLElementNode *path = new XMLElementNode("path");
		path->addAttribute("d", oss.str());
		path->addAttribute("stroke", _actions->getColor().rgbString());
		path->addAttribute("fill", "none");
		if (_linewidth != 1)
			path->addAttribute("stroke-width", XMLString(_linewidth));
		if (_miterlimit != 4)
			path->addAttribute("stroke-miterlimit", XMLString(_miterlimit));
		if (_linecap > 0)     // default value is "butt", no need to set it explicitely
			path->addAttribute("stroke-linecap", XMLString(_linecap == 1 ? "round" : "square"));
		if (_linejoin > 0)    // default value is "miter", no need to set it explicitely
			path->addAttribute("stroke-linejoin", XMLString(_linecap == 1 ? "round" : "bevel"));
		if (_clipStack.top()) {
			// assign clipping path and clip bounding box
			path->addAttribute("clip-path", XMLString("url(#clip")+XMLString(_clipStack.topID())+XMLString(")"));
			BoundingBox clipbox;
			_clipStack.top()->computeBBox(clipbox);
			bbox.intersect(clipbox);
		}
		if (_dashpattern.size() > 0) {
			ostringstream oss;
			for (size_t i=0; i < _dashpattern.size(); i++) {
				if (i > 0)
					oss << ',';
				oss << _dashpattern[i];
			}
			path->addAttribute("stroke-dasharray", oss.str());
			if (_dashoffset != 0)
				path->addAttribute("stroke-dashoffset", _dashoffset);
		}
		if (_xmlnode)
			_xmlnode->append(path);
		else {
			_actions->appendToPage(path);
			_actions->bbox().embed(bbox);
		}
		_path.newpath();
	}
}


/** Draws a closed path filled with the current color.
 *  @param[in] p not used
 *  @param[in] evenodd true: use even-odd fill algorithm, false: use nonzero fill algorithm */
void PsSpecialHandler::fill (vector<double> &p, bool evenodd) {
	if (!_path.empty() && _actions) {
		const double pt = 72.27/72.0;  // factor to convert bp -> pt
		ScalingMatrix scale(pt, pt);
		_path.transform(scale);

		// compute bounding box
		BoundingBox bbox;
		_path.computeBBox(bbox);
		if (!_actions->getMatrix().isIdentity()) {
			_path.transform(_actions->getMatrix());
			if (!_xmlnode)
				bbox.transform(_actions->getMatrix());
		}

		ostringstream oss;
		_path.writeSVG(oss);
		XMLElementNode *path = new XMLElementNode("path");
		path->addAttribute("d", oss.str());
		if (_actions->getColor() != Color::BLACK)
			path->addAttribute("fill", _actions->getColor().rgbString());
		if (_clipStack.top()) {
			// assign clipping path and clip bounding box
			path->addAttribute("clip-path", XMLString("url(#clip")+XMLString(_clipStack.topID())+XMLString(")"));
			BoundingBox clipbox;
			_clipStack.top()->computeBBox(clipbox);
			bbox.intersect(clipbox);
		}
		if (evenodd)  // SVG default fill rule is "nonzero" algorithm
			path->addAttribute("fill-rule", "evenodd");
		if (_xmlnode)
			_xmlnode->append(path);
		else {
			_actions->appendToPage(path);
			_actions->bbox().embed(bbox);
		}
		_path.newpath();
	}
}


/** Clears the current clipping path.
 *  @param[in] p not used */
void PsSpecialHandler::initclip (vector<double> &p) {
	_clipStack.push();  // push empty path
}


/** Assigns a new clipping path.
 *  @param[in] p not used
 *  @param[in] evenodd true: use even-odd fill algorithm, false: use nonzero fill algorithm */
void PsSpecialHandler::clip (vector<double> &p, bool evenodd) {
	// when this method is called, _path contains the clipping path
	if (!_path.empty() && _actions) {
		const double pt = 72.27/72.0;  // factor to convert bp -> pt
		ScalingMatrix scale(pt, pt);
		_path.transform(scale);

		if (!_actions->getMatrix().isIdentity())
			_path.transform(_actions->getMatrix());

		int oldID = _clipStack.topID();
		_clipStack.replace(_path);
		int newID = _clipStack.topID();

		ostringstream oss;
		_path.writeSVG(oss);
		XMLElementNode *path = new XMLElementNode("path");
		path->addAttribute("d", oss.str());
		if (evenodd)
			path->addAttribute("clip-rule", "evenodd");

		XMLElementNode *clip = new XMLElementNode("clipPath");
		clip->addAttribute("id", XMLString("clip")+XMLString(newID));
		if (oldID)
			clip->addAttribute("clip-path", XMLString("url(#clip")+XMLString(oldID)+XMLString(")"));

		clip->append(path);
		_actions->appendToDefs(clip);
	}
}


/** Clears current path */
void PsSpecialHandler::newpath (vector<double> &p) {
	_path.newpath();
}


void PsSpecialHandler::setmatrix (vector<double> &p) {
	if (_actions) {
		// Ensure vector p has 6 elements. If necessary, add missing ones
		// using corresponding values of the identity matrix.
		if (p.size() < 6) {
			p.resize(6);
			for (int i=p.size(); i < 6; i++)
				p[i] = (i%3 ? 0 : 1);
		}
		// PS matrix [a b c d e f] equals ((a,b,0),(c,d,0),(e,f,1)).
		// Since PS uses left multiplications, we must transpose and reorder
		// the matrix to ((a,c,e),(b,d,f),(0,0,1)). This is done by the
		// following swaps.
		swap(p[1], p[2]);  // => (a, c, b, d, e, f)
		swap(p[2], p[4]);  // => (a, c, e, d, b, f)
		swap(p[3], p[4]);  // => (a, c, e, b, d, f)
		Matrix m(p);
		_actions->setMatrix(m);
	}
}


// In contrast to SVG, PostScript transformations are applied in
// reverse order (M' = T*M). Thus, the transformation matrices must be
// left-multiplied in the following methods scale(), translate() and rotate().


void PsSpecialHandler::scale (vector<double> &p) {
	if (_actions) {
		Matrix m = _actions->getMatrix();
		ScalingMatrix s(p[0], p[1]);
		m.lmultiply(s);
		_actions->setMatrix(m);
	}
}


void PsSpecialHandler::translate (vector<double> &p) {
	if (_actions) {
		Matrix m = _actions->getMatrix();
		TranslationMatrix t(p[0], p[1]);
		m.lmultiply(t);
		_actions->setMatrix(m);
	}
}


void PsSpecialHandler::rotate (vector<double> &p) {
	if (_actions) {
		Matrix m = _actions->getMatrix();
		RotationMatrix r(p[0]);
		m.lmultiply(r);
		_actions->setMatrix(m);
	}
}

void PsSpecialHandler::setgray (vector<double> &p) {
	if (_actions) {
		Color c;
		c.setGray((float)p[0]);
		_actions->setColor(c);
	}
}


void PsSpecialHandler::setrgbcolor (vector<double> &p) {
	if (_actions)
		_actions->setColor(Color((float)p[0], (float)p[1], (float)p[2]));
}


void PsSpecialHandler::setcmykcolor (vector<double> &p) {
	if (_actions) {
		Color c;
		c.setCMYK(p[0], p[1], p[2], p[3]);
		_actions->setColor(c);
	}
}


void PsSpecialHandler::sethsbcolor (vector<double> &p) {
	if (_actions) {
		Color c;
		c.setHSB(p[0], p[1], p[2]);
		_actions->setColor(c);
	}
}


/** Sets the dash parameters used for stroking.
 *  @param[in] p dash pattern array m1,...,mn plus trailing dash offset */
void PsSpecialHandler::setdash (vector<double> &p) {
	_dashpattern.clear();
	for (size_t i=0; i < p.size()-1; i++)
		_dashpattern.push_back(p[i]*1.00375);
	_dashoffset = p.back()*1.00375;
}


////////////////////////////////////////////

void PsSpecialHandler::ClippingStack::push () {
	if (!_stack.empty())
		_stack.push(0);
}


void PsSpecialHandler::ClippingStack::push (const Path &path) {
	if (!path.empty()) {
		_paths.push_back(path);
		_stack.push(_paths.size());
	}
}


void PsSpecialHandler::ClippingStack::replace (const Path &path) {
	if (path.empty())
		push(path);
	else {
		_paths.push_back(path);
		if (!_stack.empty())
			_stack.pop();
		_stack.push(_paths.size());
	}
}


void PsSpecialHandler::ClippingStack::dup () {
	if (!_stack.empty())
		_stack.push(_stack.top());
}


const char** PsSpecialHandler::prefixes () const {
	static const char *pfx[] = {"header=", "psfile=", "PSfile=", "ps:", "ps::", "!", "\"", 0};
	return pfx;
}

