#ifndef FONTSTYLE
#define FONTSTYLE

struct FontStyle {
	FontStyle () : bold(0), extend(1), slant(0) {}
	FontStyle (float b, float e, float s) : bold(b), extend(e), slant(s) {}
	double bold;   ///< stroke width in pt used to draw the glyph outlines
	double extend; ///< factor to strech/shrink the glyphs horizontally
	double slant;  ///< horizontal slanting/skewing value (= tan(phi))
};

#endif
