#ifndef FONTWRITER_HPP
#define FONTWRITER_HPP

#include <ostream>
#include <set>
#include "GFGlyphTracer.hpp"

class PhysicalFont;

class FontWriter {
	public:
		enum class FontFormat {UNKNOWN, SVG, TTF, WOFF};

	public:
		FontWriter (const PhysicalFont &font);
		std::string createFontFile (FontFormat format, const std::set<int> &charcodes, GFGlyphTracer::Callback *cb=0) const;
		bool writeCSSFontFace (FontFormat format, const std::set<int> &charcodes, std::ostream &os, GFGlyphTracer::Callback *cb=0) const;
		static FontFormat toFontFormat (std::string formatstr);
		static std::vector<std::string> supportedFormats ();

	protected:
		struct FontFormatInfo {
			FontWriter::FontFormat format;
			const char *mimetype;
			const char *formatstr_short;
			const char *formatstr_long;
		};

		static const FontFormatInfo* fontFormatInfo (FontFormat format);

	private:
		const PhysicalFont &_font;
		static const std::array<FontFormatInfo, 4> _formatInfos;
};

#endif

