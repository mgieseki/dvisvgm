/*************************************************************************
** PSInterpreterTest.cpp                                                **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2020 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "PSInterpreter.hpp"

#include <sstream>
#include <vector>

using namespace std;

class PSTestActions : public PSActions {
	public:
		void applyscalevals (vector<double> &p) override  {print("applyscalevals", p);}
		void clip (vector<double> &p) override            {print("clip", p);}
		void clippath (vector<double> &p) override        {print("clippath", p);}
		void closepath (vector<double> &p) override       {print("closepath", p);}
		void curveto (vector<double> &p) override         {print("curveto", p);}
		void eoclip (vector<double> &p) override          {print("eoclip", p);}
		void eofill (vector<double> &p) override          {print("eofill", p);}
		void fill (vector<double> &p) override            {print("fill", p);}
		void gsave (vector<double> &p) override           {print("gsave", p);}
		void grestore (vector<double> &p) override        {print("grestore", p);}
		void grestoreall (std::vector<double> &p) override{print("grestoreall", p);}
		void image (std::vector<double> &p) override      {print("image", p);}
		void initclip (vector<double> &p) override        {print("initclip", p);}
		void lineto (vector<double> &p) override          {print("lineto", p);}
		void moveto (vector<double> &p) override          {print("moveto", p);}
		void makepattern (vector<double> &p) override     {print("makepattern", p);}
		void setpattern (vector<double> &p) override      {print("setpattern", p);}
		void setpagedevice (vector<double> &p) override   {print("setpagedevice", p);}
		void newpath (vector<double> &p) override         {print("newpath", p);}
		void querypos (vector<double> &p) override        {print("querypos", p);}
		void restore(std::vector<double> &p) override     {print("restore", p);}
		void rotate (vector<double> &p) override          {print("rotate", p);}
		void save(std::vector<double> &p) override        {print("save", p);}
		void scale (vector<double> &p) override           {print("scale", p);}
		void setblendmode (vector<double> &p) override    {print("setblendmode", p);}
		void setcolorspace (vector<double> &p) override   {print("setcolorspace", p);}
		void setcmykcolor (vector<double> &p) override    {print("setcmykcolor", p);}
		void setdash (vector<double> &p) override         {print("setdash", p);}
		void setgray (vector<double> &p) override         {print("setgray", p);}
		void sethsbcolor (vector<double> &p) override     {print("sethsbcolor", p);}
		void setlinecap (vector<double> &p) override      {print("setlinecap", p);}
		void setlinejoin (vector<double> &p) override     {print("setlinejoin", p);}
		void setlinewidth (vector<double> &p) override    {print("setlinewidth", p);}
		void setmatrix (vector<double> &p) override       {print("setmatrix", p);}
		void setmiterlimit (vector<double> &p) override   {print("setmiterlimit", p);}
		void setnulldevice (vector<double> &p) override   {print("setnulldevice", p);}
		void setopacityalpha (vector<double> &p) override {print("setopacityalpha", p);}
		void setshapealpha (vector<double> &p) override   {print("setshapealpha", p);}
		void setrgbcolor (vector<double> &p) override     {print("setrgbcolor", p);}
		void shfill (vector<double> &p) override          {print("shfill", p);}
		void stroke (vector<double> &p) override          {print("stroke", p);}
		void translate (vector<double> &p) override       {print("translate", p);}


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
	EXPECT_EQ(actions.result(), "");
}


TEST(PSInterpreterTest, gsave_grestore) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("gsave ");
	EXPECT_EQ(actions.result(), "gsave;");
	actions.clear();

	psi.execute("grestore ");
	EXPECT_EQ(actions.result(), "setmatrix 1 0 0 1 0 0;applyscalevals 1 1 1;setlinewidth 1;setlinecap 0;setlinejoin 0;setmiterlimit 10;setcolorspace 0;setrgbcolor 0 0 0;setdash 0;grestore;");
	actions.clear();

	psi.execute("1 setlinecap 5 setmiterlimit 0 1 0 setrgbcolor gsave 0 setlinecap 10 setmiterlimit ");
	EXPECT_EQ(actions.result(), "setlinecap 1;setmiterlimit 5;setrgbcolor 0 1 0;gsave;setlinecap 0;setmiterlimit 10;");
	actions.clear();

	psi.execute("grestore ");
	EXPECT_EQ(actions.result(), "setmatrix 1 0 0 1 0 0;applyscalevals 1 1 1;setlinewidth 1;setlinecap 1;setlinejoin 0;setmiterlimit 5;setcolorspace 0;setrgbcolor 0 1 0;setdash 0;grestore;");
}


TEST(PSInterpreterTest, stroke_fill) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("0 0 moveto 10 10 lineto 0 10 lineto closepath stroke ");
	EXPECT_EQ(actions.result(), "setcolorspace 0;setrgbcolor 0 0 0;newpath 0;moveto 0 0;lineto 10 10;lineto 0 10;closepath;stroke;");
	actions.clear();

	psi.execute("0 0 moveto 10 10 lineto 0 10 lineto closepath fill ");
	EXPECT_EQ(actions.result(), "setcolorspace 0;setrgbcolor 0 0 0;newpath 0;moveto 0 0;lineto 10 10;lineto 0 10;closepath;fill;");
}


TEST(PSInterpreterTest, clip) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("initclip ");
	EXPECT_EQ(actions.result(), "initclip;");
	actions.clear();

	psi.execute("0 0 moveto 10 10 lineto 0 10 lineto closepath clip ");
	EXPECT_EQ(actions.result(), "newpath 0;moveto 0 0;lineto 10 10;lineto 0 10;closepath;clip;");
}


TEST(PSInterpreterTest, transform) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("10 10 scale 90 rotate 100 -100 translate ");
	EXPECT_EQ(actions.result(), "scale 10 10;applyscalevals 10 10 1;rotate 90;applyscalevals 10 10 0;translate 100 -100;");
}


TEST(PSInterpreterTest, calculate) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("2 3 add 4 mul 5 div rotate ");
	ASSERT_EQ(actions.result(), "rotate 4;applyscalevals 1 1 0.997564;");
}


TEST(PSInterpreterTest, setlinewidth) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("10 setlinewidth ");
	EXPECT_EQ(actions.result(), "setlinewidth 10;");
	actions.clear();

	psi.execute("5 5 scale 10 setlinewidth ");
	EXPECT_EQ(actions.result(), "scale 5 5;applyscalevals 5 5 1;setlinewidth 10;");
	actions.clear();

	psi.execute("90 rotate 10 setlinewidth ");
	EXPECT_EQ(actions.result(), "rotate 90;applyscalevals 5 5 0;setlinewidth 10;");
	actions.clear();

	psi.execute("-30 rotate 10 setlinewidth ");
	EXPECT_EQ(actions.result(), "rotate -30;applyscalevals 5 5 0.5;setlinewidth 10;");
}


TEST(PSInterpreterTest, matrix) {
	PSTestActions actions;
	PSInterpreter psi(&actions);
	psi.execute("matrix setmatrix ");
	EXPECT_EQ(actions.result(), "setmatrix 1 0 0 1 0 0;applyscalevals 1 1 1;");
	actions.clear();
	psi.execute("10 100 translate 30 rotate matrix currentmatrix setmatrix ");
	EXPECT_EQ(actions.result(), "translate 10 100;rotate 30;applyscalevals 1 1 0.866025;setmatrix 0.866025 0.5 -0.5 0.866025 10 100;applyscalevals 1 1 0.866025;");
}
