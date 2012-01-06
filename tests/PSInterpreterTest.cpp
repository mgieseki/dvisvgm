/*************************************************************************
** PSInterpreterTest.cpp                                                **
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

#include <gtest/gtest.h>
#include "PSInterpreter.h"

#include <sstream>
#include <vector>

using namespace std;

class PSTestActions : public PSActions
{
	public:
		void applyscalevals (vector<double> &p)  {print("applyscalevals", p);}
		void clip (vector<double> &p)            {print("clip", p);}
		void closepath (vector<double> &p)       {print("closepath", p);}
		void curveto (vector<double> &p)         {print("curveto", p);}
		void eoclip (vector<double> &p)          {print("eoclip", p);}
		void eofill (vector<double> &p)          {print("eofill", p);}
		void fill (vector<double> &p)            {print("fill", p);}
		void gsave (vector<double> &p)           {print("gsave", p);}
		void grestore (vector<double> &p)        {print("grestore", p);}
		void grestoreall(std::vector<double> &p) {print("grestoreall", p);}
		void initclip (vector<double> &p)        {print("initclip", p);}
		void lineto (vector<double> &p)          {print("lineto", p);}
		void moveto (vector<double> &p)          {print("moveto", p);}
		void newpath (vector<double> &p)         {print("newpath", p);}
		void querypos (vector<double> &p)        {print("querypos", p);}
		void restore(std::vector<double> &p)     {print("restore", p);}
		void rotate (vector<double> &p)          {print("rotate", p);}
		void save(std::vector<double> &p)        {print("save", p);}
		void scale (vector<double> &p)           {print("scale", p);}
		void setcmykcolor (vector<double> &p)    {print("setcmykcolor", p);}
		void setdash (vector<double> &p)         {print("setdash", p);}
		void setgray (vector<double> &p)         {print("setgray", p);}
		void sethsbcolor (vector<double> &p)     {print("sethsbcolor", p);}
		void setlinecap (vector<double> &p)      {print("setlinecap", p);}
		void setlinejoin (vector<double> &p)     {print("setlinejoin", p);}
		void setlinewidth (vector<double> &p)    {print("setlinewidth", p);}
		void setmatrix (vector<double> &p)       {print("setmatrix", p);}
		void setmiterlimit (vector<double> &p)   {print("setmiterlimit", p);}
		void setopacityalpha (vector<double> &p) {print("setopacityalpha", p);}
		void setrgbcolor (vector<double> &p)     {print("setrgbcolor", p);}
		void stroke (vector<double> &p)          {print("stroke", p);}
		void translate (vector<double> &p)       {print("translate", p);}


		string result () const {return _oss.str();}
		void clear ()          {_oss.str("");}

	protected:
		void print (const char *op, const vector<double> &p) {
			_oss << op;
			for (size_t i=0; i < p.size(); ++i)
				_oss << ' ' << p[i];
			_oss << ';';
		}

	private:
		ostringstream _oss;
};


TEST(PSInterpreterTest, init) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	ASSERT_TRUE(psi.active());
	ASSERT_EQ(actions.result(), "");
}


TEST(PSInterpreterTest, gsave_grestore) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("gsave ");
	ASSERT_EQ(actions.result(), "gsave;");
	actions.clear();

	psi.execute("grestore ");
	ASSERT_EQ(actions.result(), "setlinewidth 1;setlinecap 0;setlinejoin 0;setmiterlimit 10;setrgbcolor 0 0 0;setmatrix 1 0 0 1 0 0;setdash 0;grestore;");
	actions.clear();

	psi.execute("1 setlinecap 5 setmiterlimit 0 1 0 setrgbcolor gsave 0 setlinecap 10 setmiterlimit ");
	ASSERT_EQ(actions.result(), "setlinecap 1;setmiterlimit 5;setrgbcolor 0 1 0;gsave;setlinecap 0;setmiterlimit 10;");
	actions.clear();

	psi.execute("grestore ");
	ASSERT_EQ(actions.result(), "setlinewidth 1;setlinecap 1;setlinejoin 0;setmiterlimit 5;setrgbcolor 0 1 0;setmatrix 1 0 0 1 0 0;setdash 0;grestore;");
}


TEST(PSInterpreterTest, stroke_fill) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("0 0 moveto 10 10 lineto 0 10 lineto closepath stroke ");
	ASSERT_EQ(actions.result(), "newpath;moveto 0 0;lineto 10 10;lineto 0 10;closepath;stroke;");
	actions.clear();

	psi.execute("0 0 moveto 10 10 lineto 0 10 lineto closepath fill ");
	ASSERT_EQ(actions.result(), "newpath;moveto 0 0;lineto 10 10;lineto 0 10;closepath;fill;");
}


TEST(PSInterpreterTest, clip) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("initclip ");
	ASSERT_EQ(actions.result(), "initclip;");
	actions.clear();

	psi.execute("0 0 moveto 10 10 lineto 0 10 lineto closepath clip ");
	ASSERT_EQ(actions.result(), "newpath;moveto 0 0;lineto 10 10;lineto 0 10;closepath;clip;");
}


TEST(PSInterpreterTest, transform) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("10 10 scale 90 rotate 100 -100 translate ");
	ASSERT_EQ(actions.result(), "scale 10 10;rotate 90;translate 100 -100;");
}


TEST(PSInterpreterTest, calculate) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("2 3 add 4 mul 5 div rotate ");
	ASSERT_EQ(actions.result(), "rotate 4;");
}


TEST(PSInterpreterTest, setlinewidth) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("10 setlinewidth ");
	ASSERT_EQ(actions.result(), "applyscalevals 1 1 1;setlinewidth 10;");
	actions.clear();

	psi.execute("5 5 scale 10 setlinewidth ");
	ASSERT_EQ(actions.result(), "scale 5 5;applyscalevals 5 5 1;setlinewidth 10;");
	actions.clear();

	psi.execute("90 rotate 10 setlinewidth ");
	ASSERT_EQ(actions.result(), "rotate 90;applyscalevals 5 5 0;setlinewidth 10;");
	actions.clear();

	psi.execute("-30 rotate 10 setlinewidth ");
	ASSERT_EQ(actions.result(), "rotate -30;applyscalevals 5 5 0.5;setlinewidth 10;");
}


TEST(PSInterpreterTest, matrix) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("matrix setmatrix ");
	ASSERT_EQ(actions.result(), "setmatrix 1 0 0 1 0 0;");
	actions.clear();
	psi.execute("10 100 translate 30 rotate matrix currentmatrix setmatrix ");
	ASSERT_EQ(actions.result(), "translate 10 100;rotate 30;setmatrix 0.866025 0.5 -0.5 0.866025 10 100;");
}
