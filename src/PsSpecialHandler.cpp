/*************************************************************************
** PsSpecialHandler.cpp                                                 **
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

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include "FileFinder.h"
#include "Ghostscript.h"
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


PsSpecialHandler::~PsSpecialHandler () {
	// ensure no further PS actions are performed
	_psi.setActions(0);
}


/** Initializes the PostScript handler. It's called by the first use of process(). The
 *  deferred initialization speeds up the conversion of DVI files that doesn't contain
 *  PS specials. */
void PsSpecialHandler::initialize (SpecialActions *actions) {
	if (!_initialized) {
		// initial values of graphics state
		_linewidth = 1;
		_linecap = _linejoin = 0;
		_miterlimit = 4;
		_xmlnode = 0;
		_opacityalpha = 1;  // fully opaque
		_sx = _sy = _cos = 1.0;

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
		ostringstream oss;
		oss << " TeXDict begin 0 0 1000 72 72 () @start "
		       " 0 0 moveto ";
  		if (actions) {
			float r, g, b;
			actions->getColor().getRGB(r, g, b);
			oss << r << ' ' << g << ' ' << b << " setrgbcolor ";
		}
		_psi.execute(oss.str());
		_initialized = true;
	}
}


/** Move PS graphic position to current DVI location. */
void PsSpecialHandler::moveToDVIPos () {
	if (_actions) {
		const double bp=72.0/72.27; // pt -> bp
		const double x = _actions->getX()*bp;
		const double y = _actions->getY()*bp;
		ostringstream oss;
      oss << '\n' << x << ' ' << y << " moveto ";
      _psi.execute(oss.str());
      _currentpoint = DPair(x, y);
   }
}


/** Executes a PS snippet and moves the DVI cursor to the current DVI position afterwards.
 *  It's just a shorthand function as this action sequence is required several times.
 *  @param[in] psi PS interpreter instance
 *  @param[in] is  stream to read the PS code from
 *  @param[in] pos current PS graphic position
 *  @param[in] actions special actions */
static void exec_and_syncpos (PSInterpreter &psi, istream &is, const DPair &pos, SpecialActions *actions) {
	psi.execute(is);
	psi.execute("\nquerypos ");   // retrieve current PS position (stored in 'pos')
	const double pt = 72.27/72.0; // bp -> pt
	actions->setX(pos.x()*pt);
	actions->setY(pos.y()*pt);
}


bool PsSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	if (!_initialized)
		initialize(actions);
	_actions = actions;

	if (*prefix == '"') {
		// read and execute literal PostScript code (isolated by a wrapping save/restore pair)
		moveToDVIPos();
		_psi.execute("\n@beginspecial @setspecial ");
		_psi.execute(is);
		_psi.execute("\n@endspecial ");
	}
	else if (*prefix == '!') {
		// execute literal PostScript header
		_psi.execute("\nTeXDict begin @defspecial ");
		_psi.execute(is);
		_psi.execute("\n@fedspecial end ");
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
		_actions->finishLine();  // reset DVI position on next DVI command
		if (is.peek() == '[') {
			// collect characters inside the brackets
			string code;
			for (int i=0; i < 9 && is.peek() != ']' && !is.eof(); ++i)
				code += is.get();
			if (is.peek() == ']')
				code += is.get();

			if (code == "[begin]" || code == "[nobreak]") {
				moveToDVIPos();
				exec_and_syncpos(_psi, is, _currentpoint, _actions);
			}
			else {
				// no move to DVI position here
				if (code != "[end]") // PS array?
					_psi.execute(code);
				exec_and_syncpos(_psi, is, _currentpoint, _actions);
			}
		}
		else { // ps::<code> behaves like ps::[end]<code>
			// no move to DVI position here
			exec_and_syncpos(_psi, is, _currentpoint, _actions);
		}
	}
	else { // ps: ...
		_actions->finishLine();
		moveToDVIPos();
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
			// ps:<code> is almost identical to ps::[begin]<code> but does
			// a final repositioning to the current DVI location
			exec_and_syncpos(_psi, is, _currentpoint, _actions);
			moveToDVIPos();
		}
	}
	return true;
}


/** Handles psfile special.
 *  @param[in] fname EPS file to be included
 *  @param[in] attr attributes given with \special psfile */
void PsSpecialHandler::psfile (const string &fname, const map<string,string> &attr) {
	ifstream ifs(fname.c_str());
	if (!ifs)
		Message::wstream(true) << "file '" << fname << "' not found in special 'psfile'\n";
	else {
		map<string,string>::const_iterator it;
		const double pt = 72.27/72.0;  // bp -> pt

		// bounding box of EPS figure
		double llx = (it = attr.find("llx")) != attr.end() ? str2double(it->second)*pt : 0;
		double lly = (it = attr.find("lly")) != attr.end() ? str2double(it->second)*pt : 0;
		double urx = (it = attr.find("urx")) != attr.end() ? str2double(it->second)*pt : 0;
		double ury = (it = attr.find("ury")) != attr.end() ? str2double(it->second)*pt : 0;

		// desired width/height of resulting figure
		double rwi = (it = attr.find("rwi")) != attr.end() ? str2double(it->second)/10.0*pt : -1;
		double rhi = (it = attr.find("rhi")) != attr.end() ? str2double(it->second)/10.0*pt : -1;
		if (rwi == 0 || rhi == 0 || urx-llx == 0 || ury-lly == 0)
			return;

		// user transformations (default values chosen according to dvips manual)
		double hoffset = (it = attr.find("hoffset")) != attr.end() ? str2double(it->second)*pt : 0;
		double voffset = (it = attr.find("voffset")) != attr.end() ? str2double(it->second)*pt : 0;
//		double hsize   = (it = attr.find("hsize")) != attr.end() ? str2double(it->second) : 612;
//		double vsize   = (it = attr.find("vsize")) != attr.end() ? str2double(it->second) : 792;
		double hscale  = (it = attr.find("hscale")) != attr.end() ? str2double(it->second) : 100;
		double vscale  = (it = attr.find("vscale")) != attr.end() ? str2double(it->second) : 100;
		double angle   = (it = attr.find("angle")) != attr.end() ? str2double(it->second) : 0;

		Matrix m(1);
		m.rotate(angle).scale(hscale/100, vscale/100).translate(hoffset, voffset);
		BoundingBox bbox(llx, lly, urx, ury);
		bbox.transform(m);

		double sx = rwi/bbox.width();
		double sy = rhi/bbox.height();
		if (sx < 0)	sx = sy;
		if (sy < 0)	sy = sx;
		if (sx < 0) sx = sy = 1.0;

		// save current DVI position (in pt units)
		const double x = _actions->getX();
		const double y = _actions->getY();

		// all following drawings are relative to (0,0)
		_actions->setX(0);
		_actions->setY(0);
		moveToDVIPos();

		_xmlnode = new XMLElementNode("g");
		_psi.execute("\n@beginspecial @setspecial "); // enter \special environment
		_psi.execute(ifs);             // process EPS file
		_psi.execute("\n@endspecial "); // leave special environment
		if (!_xmlnode->empty()) {      // has anything been drawn?
			Matrix m(1);
			m.rotate(angle).scale(hscale/100, vscale/100).translate(hoffset, voffset);
			m.translate(-llx, lly);
			m.scale(sx, sy);      // resize image to width "rwi" and height "rhi"
			m.translate(x, y); // move image to current DVI position
			_xmlnode->addAttribute("transform", m.getSVG());
			_actions->appendToPage(_xmlnode);
		}
		else
			delete _xmlnode;
		_xmlnode = 0;

		// restore DVI position
		_actions->setX(x);
		_actions->setY(y);
		moveToDVIPos();

		// update bounding box
		m.scale(sx, -sy);
		m.translate(x, y);
		bbox = BoundingBox(0, 0, fabs(urx-llx), fabs(ury-lly));
		bbox.transform(m);
		_actions->embed(bbox);
	}
}

///////////////////////////////////////////////////////

void PsSpecialHandler::gsave (vector<double> &p) {
	_clipStack.dup();
}


void PsSpecialHandler::grestore (vector<double> &p) {
	_clipStack.pop();
}


void PsSpecialHandler::grestoreall (vector<double> &p) {
	_clipStack.pop(-1, true);
}


void PsSpecialHandler::save (vector<double> &p) {
	_clipStack.dup(static_cast<int>(p[0]));
}


void PsSpecialHandler::restore (vector<double> &p) {
	_clipStack.pop(static_cast<int>(p[0]));
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
		BoundingBox bbox;
		if (!_actions->getMatrix().isIdentity()) {
			_path.transform(_actions->getMatrix());
			if (!_xmlnode)
				bbox.transform(_actions->getMatrix());
		}

		const double pt = 72.27/72.0;  // factor to convert bp -> pt
		ScalingMatrix scale(pt, pt);
		_path.transform(scale);
		bbox.transform(scale);

		XMLElementNode *path=0;
		Pair<double> point;
		if (_path.isDot(point)) {  // zero-length path?
			if (_linecap == 1) {    // round line ends?  => draw dot
				double x = point.x();
				double y = point.y();
				double r = _linewidth/2.0;
				path = new XMLElementNode("circle");
				path->addAttribute("cx", XMLString(x));
				path->addAttribute("cy", XMLString(y));
				path->addAttribute("r", XMLString(r));
				path->addAttribute("fill", _actions->getColor().rgbString());
				bbox = BoundingBox(x-r, y-r, x+r, y+r);
			}
		}
		else {
			// compute bounding box
			_path.computeBBox(bbox);
			bbox.expand(_linewidth/2);

			ostringstream oss;
			_path.writeSVG(oss);
			path = new XMLElementNode("path");
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
			if (_opacityalpha < 1)
				path->addAttribute("stroke-opacity", XMLString(_opacityalpha));
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
		}
		if (path && _clipStack.top()) {
			// assign clipping path and clip bounding box
			path->addAttribute("clip-path", XMLString("url(#clip")+XMLString(_clipStack.topID())+XMLString(")"));
			BoundingBox clipbox;
			_clipStack.top()->computeBBox(clipbox);
			bbox.intersect(clipbox);
		}

		if (_xmlnode)
			_xmlnode->append(path);
		else {
			_actions->appendToPage(path);
			_actions->embed(bbox);
		}
		_path.newpath();
	}
}


/** Draws a closed path filled with the current color.
 *  @param[in] p not used
 *  @param[in] evenodd true: use even-odd fill algorithm, false: use nonzero fill algorithm */
void PsSpecialHandler::fill (vector<double> &p, bool evenodd) {
	if (!_path.empty() && _actions) {
		// compute bounding box
		BoundingBox bbox;
		_path.computeBBox(bbox);
		if (!_actions->getMatrix().isIdentity()) {
			_path.transform(_actions->getMatrix());
			if (!_xmlnode)
				bbox.transform(_actions->getMatrix());
		}

		const double pt = 72.27/72.0;  // factor to convert bp -> pt
		ScalingMatrix scale(pt, pt);
		_path.transform(scale);
		bbox.transform(scale);

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
		if (_opacityalpha < 1)
			path->addAttribute("fill-opacity", XMLString(_opacityalpha));
		if (_xmlnode)
			_xmlnode->append(path);
		else {
			_actions->appendToPage(path);
			_actions->embed(bbox);
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
		if (!_actions->getMatrix().isIdentity())
			_path.transform(_actions->getMatrix());

		const double pt = 72.27/72.0;  // factor to convert bp -> pt
		ScalingMatrix scale(pt, pt);
		_path.transform(scale);


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
		c.setCMYK((float)p[0], (float)p[1], (float)p[2], (float)p[3]);
		_actions->setColor(c);
	}
}


void PsSpecialHandler::sethsbcolor (vector<double> &p) {
	if (_actions) {
		Color c;
		c.setHSB((float)p[0], (float)p[1], (float)p[2]);
		_actions->setColor(c);
	}
}


/** Sets the dash parameters used for stroking.
 *  @param[in] p dash pattern array m1,...,mn plus trailing dash offset */
void PsSpecialHandler::setdash (vector<double> &p) {
	_dashpattern.clear();
	for (size_t i=0; i < p.size()-1; i++)
		_dashpattern.push_back(scale(p[i]));
	_dashoffset = scale(p.back());
}


/** This method is called by the PSInterpreter if an PS operator has been executed. */
void PsSpecialHandler::executed () {
	if (_actions)
		_actions->progress("ps");
}

////////////////////////////////////////////

void PsSpecialHandler::ClippingStack::push () {
	if (!_stack.empty())
		_stack.push(Entry(0, -1));
}


void PsSpecialHandler::ClippingStack::push (const Path &path, int saveID) {
	if (path.empty())
		_stack.push(Entry(0, saveID));
	else {
		_paths.push_back(path);
		_stack.push(Entry(_paths.size(), saveID));
	}
}


/** Pops a single or several elements from the clipping stack.
 *  The method distingushes between the following cases:
 *  1) saveID < 0 and grestoreall == false:
 *     pop top element if it was pushed by gsave (its saveID is < 0 as well)
 *  2) saveID < 0 and grestoreall == true
 *     repeat popping until stack is empty or the top element was pushed
 *     by save (its saveID is >= 0)
 *  3) saveID >= 0:
 *     pop all elements until the saveID of the top element equals parameter saveID */
void PsSpecialHandler::ClippingStack::pop (int saveID, bool grestoreall) {
	if (!_stack.empty()) {
		if (saveID < 0) {                // grestore?
			if (_stack.top().saveID < 0)  // pushed by 'gsave'?
				_stack.pop();
			// pop all further elements pushed by 'gsave' if grestoreall == true
			while (grestoreall && !_stack.empty() && _stack.top().saveID < 0)
				_stack.pop();
		}
		else {
			// pop elements pushed by 'gsave'
			while (!_stack.empty() && _stack.top().saveID != saveID)
				_stack.pop();
			// pop element pushed by 'save'
			if (!_stack.empty())
				_stack.pop();
		}
	}
}


/** Returns a pointer to the path on top of the stack, or 0 if the stack is empty. */
PsSpecialHandler::Path* PsSpecialHandler::ClippingStack::top () {
	return (!_stack.empty() && _stack.top().pathID)
		? &_paths[_stack.top().pathID-1]
		: 0;
}


/** Pops all elements from the stack. */
void PsSpecialHandler::ClippingStack::clear() {
	_paths.clear();
	while (!_stack.empty())
		_stack.pop();
}


/** Replaces the top element by a new one.
 *  @param[in] path new path to be on top of the stack */
void PsSpecialHandler::ClippingStack::replace (const Path &path) {
	if (_stack.empty())
		push(path, -1);
	else {
		_paths.push_back(path);
		_stack.top().pathID = _paths.size();
	}
}


/** Duplicates the top element, i.e. the top element is pushed again. */
void PsSpecialHandler::ClippingStack::dup (int saveID) {
	_stack.push(_stack.empty() ? Entry(0, -1) : _stack.top());
	_stack.top().saveID = saveID;
}


const char** PsSpecialHandler::prefixes () const {
	static const char *pfx[] = {"header=", "psfile=", "PSfile=", "ps:", "ps::", "!", "\"", 0};
	return pfx;
}

