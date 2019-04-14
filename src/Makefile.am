## This file is part of dvisvgm
## Copyright (C) 2005-2019 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

bin_PROGRAMS     = dvisvgm
noinst_LTLIBRARIES = libdvisvgm.la
SUBDIRS = optimizer

dvisvgm_SOURCES = \
	CommandLine.hpp \
	dvisvgm.cpp

include ../libs/defs.am

dvisvgm_LDADD = \
	$(noinst_LTLIBRARIES) \
	../libs/clipper/libclipper.a \
	$(FREETYPE_LIBS) \
	$(FONTFORGE_LIBS) \
	$(POTRACE_LIBS) \
	$(XXHASH_LIBS) \
	$(ZLIB_LIBS)

if ENABLE_WOFF
dvisvgm_LDADD += \
	../libs/ff-woff/libfontforge.a \
	$(WOFF2_LIBS) \
	$(BROTLI_LIBS)
endif

if USE_BUNDLED_MD5
dvisvgm_LDADD += ../libs/md5/libmd5.a
else
dvisvgm_LDADD += $(LIBCRYPTO_LIBS)
endif

dvisvgm_DEPENDENCIES = $(noinst_LTLIBRARIES)

libdvisvgm_la_SOURCES = \
	AGLTable.hpp \
	BasicDVIReader.hpp           BasicDVIReader.cpp \
	Bezier.hpp                   Bezier.cpp \
	BgColorSpecialHandler.hpp    BgColorSpecialHandler.cpp \
	Bitmap.hpp                   Bitmap.cpp \
	BoundingBox.hpp              BoundingBox.cpp \
	Calculator.hpp               Calculator.cpp \
	Character.hpp \
	CharMapID.hpp                CharMapID.cpp \
	CLCommandLine.hpp            CLCommandLine.cpp \
	CMap.hpp                     CMap.cpp \
	CMapManager.hpp              CMapManager.cpp \
	CMapReader.hpp               CMapReader.cpp \
	CLOption.hpp \
	Color.hpp                    Color.cpp \
	ColorSpecialHandler.hpp      ColorSpecialHandler.cpp \
	CommandLine.hpp \
	CRC32.hpp                    CRC32.cpp \
	Directory.hpp                Directory.cpp \
	DVIActions.hpp \
	DLLoader.hpp                 DLLoader.cpp \
	DVIReader.hpp                DVIReader.cpp \
	DvisvgmSpecialHandler.hpp    DvisvgmSpecialHandler.cpp \
	DVIToSVG.hpp                 DVIToSVG.cpp \
	DVIToSVGActions.hpp          DVIToSVGActions.cpp \
	EmSpecialHandler.hpp         EmSpecialHandler.cpp \
	EncFile.hpp                  EncFile.cpp \
	EPSFile.hpp                  EPSFile.cpp \
	EPSToSVG.hpp \
	FileFinder.hpp               FileFinder.cpp \
	FilePath.hpp                 FilePath.cpp \
	FileSystem.hpp               FileSystem.cpp \
	FixWord.hpp \
	Font.hpp                     Font.cpp \
	FontCache.hpp                FontCache.cpp \
	FontEncoding.hpp             FontEncoding.cpp \
	FontEngine.hpp               FontEngine.cpp \
	FontManager.hpp              FontManager.cpp \
	FontMap.hpp                  FontMap.cpp \
	FontMetrics.hpp              FontMetrics.cpp \
	FontStyle.hpp \
	FontWriter.hpp               FontWriter.cpp \
	GFGlyphTracer.hpp            GFGlyphTracer.cpp \
	GFReader.hpp                 GFReader.cpp \
	GFTracer.hpp                 GFTracer.cpp \
	Ghostscript.hpp              Ghostscript.cpp \
	Glyph.hpp \
	GlyphTracerMessages.hpp \
	GraphicsPath.hpp \
	HashFunction.hpp             HashFunction.cpp \
	HtmlSpecialHandler.hpp       HtmlSpecialHandler.cpp \
	HyperlinkManager.hpp         HyperlinkManager.cpp \
	ImageToSVG.hpp               ImageToSVG.cpp \
	InputBuffer.hpp              InputBuffer.cpp \
	InputReader.hpp              InputReader.cpp \
	JFM.hpp                      JFM.cpp \
	Length.hpp                   Length.cpp \
	macros.hpp \
	MapLine.hpp                  MapLine.cpp \
	Matrix.hpp                   Matrix.cpp \
	MD5HashFunction.hpp \
	Message.hpp                  Message.cpp \
	MessageException.hpp \
	MetafontWrapper.hpp          MetafontWrapper.cpp \
	NoPsSpecialHandler.hpp       NoPsSpecialHandler.cpp \
	NumericRanges.hpp \
	PageRanges.hpp               PageRanges.cpp \
	PageSize.hpp                 PageSize.cpp \
	Pair.hpp \
	PapersizeSpecialHandler.hpp  PapersizeSpecialHandler.cpp \
	PathClipper.hpp              PathClipper.cpp \
	PDFParser.hpp                PDFParser.cpp \
	PdfSpecialHandler.hpp        PdfSpecialHandler.cpp \
	PDFToSVG.hpp \
	PreScanDVIReader.hpp         PreScanDVIReader.cpp \
	Process.hpp                  Process.cpp \
	psdefs.cpp \
	PSFilter.hpp \
	PSInterpreter.hpp            PSInterpreter.cpp \
	PSPattern.hpp                PSPattern.cpp \
	PSPreviewFilter.hpp          PSPreviewFilter.cpp \
	PsSpecialHandler.hpp         PsSpecialHandler.cpp \
	RangeMap.hpp                 RangeMap.cpp \
	ShadingPatch.hpp             ShadingPatch.cpp \
	SignalHandler.hpp            SignalHandler.cpp \
	SourceInput.hpp              SourceInput.cpp \
	SpecialActions.hpp \
	SpecialHandler.hpp \
	SpecialManager.hpp           SpecialManager.cpp \
	StreamReader.hpp             StreamReader.cpp \
	StreamWriter.hpp             StreamWriter.cpp \
	Subfont.hpp                  Subfont.cpp \
	SVGCharHandler.hpp           SVGCharHandler.cpp \
	SVGCharHandlerFactory.hpp    SVGCharHandlerFactory.cpp \
	SVGCharPathHandler.hpp       SVGCharPathHandler.cpp \
	SVGCharTspanTextHandler.hpp  SVGCharTspanTextHandler.cpp \
	SVGOutput.hpp                SVGOutput.cpp \
	SVGSingleCharTextHandler.hpp SVGSingleCharTextHandler.cpp \
	SVGTree.hpp                  SVGTree.cpp \
	System.hpp                   System.cpp \
	TensorProductPatch.hpp       TensorProductPatch.cpp \
	Terminal.hpp                 Terminal.cpp \
	TFM.hpp                      TFM.cpp \
	ToUnicodeMap.hpp             ToUnicodeMap.cpp \
	TpicSpecialHandler.hpp       TpicSpecialHandler.cpp \
	TriangularPatch.hpp          TriangularPatch.cpp \
	TrueTypeFont.hpp             TrueTypeFont.cpp \
	TTFAutohint.hpp              TTFAutohint.cpp \
	Unicode.hpp                  Unicode.cpp \
	utility.hpp                  utility.cpp \
	VectorIterator.hpp \
	VectorStream.hpp \
	VFActions.hpp \
	VFReader.hpp                 VFReader.cpp \
	windows.hpp \
	XMLDocument.hpp              XMLDocument.cpp \
	XMLNode.hpp                  XMLNode.cpp \
	XMLString.hpp                XMLString.cpp \
	XXHashFunction.hpp \
	ZLibOutputStream.hpp

if ENABLE_WOFF
libdvisvgm_la_SOURCES += ffwrapper.c ffwrapper.h
endif

libdvisvgm_la_LIBADD = optimizer/liboptimizer.la

EXTRA_DIST = options.xml options.dtd iapi.h ierrors.h MiKTeXCom.hpp MiKTeXCom.cpp

AM_CFLAGS = -Wall \
	$(ZLIB_CFLAGS) \
	$(CODE_COVERAGE_CFLAGS)

AM_CXXFLAGS = -Wall -Wnon-virtual-dtor \
	-I$(dvisvgm_srcdir)/libs/clipper \
	-I$(dvisvgm_srcdir)/libs/variant/include \
	$(KPSE_CFLAGS) \
	$(FREETYPE_CFLAGS) \
	$(ZLIB_CFLAGS) \
	$(CODE_COVERAGE_CFLAGS)

AM_CXXFLAGS += \
	$(POTRACE_CFLAGS) \
	$(XXHASH_CFLAGS)

AM_LDFLAGS = \
	$(KPSE_LIBS) \
	$(CODE_COVERAGE_LDFLAGS)

if ENABLE_WOFF
AM_CFLAGS +=  \
	-I$(dvisvgm_srcdir)/libs/ff-woff/fontforge \
	-I$(dvisvgm_srcdir)/libs/ff-woff/inc

AM_CXXFLAGS += $(TTFAUTOHINT_CFLAGS)

AM_CXXFLAGS +=  \
	$(BROTLI_CFLAGS) \
	$(WOFF2_CFLAGS)

if USE_BUNDLED_MD5
AM_CXXFLAGS += -I$(dvisvgm_srcdir)/libs/md5
else
AM_CXXFLAGS += $(LIBCRYPTO_CFLAGS)
endif

AM_LDFLAGS += $(TTFAUTOHINT_LIBS)
endif


# the command-line parser class is generated from options.xml by opt2cpp
$(srcdir)/CommandLine.hpp: options.xml
	rm -f $@
	python $(srcdir)/opt2cpp.py $< >$@

# Create a C string definition containing the PostScript routines psdefs.ps needed by class PSInterpreter
$(srcdir)/psdefs.cpp: psdefs.ps
	if test -f $<; then \
		ps2c PSInterpreter::PSDEFS $< >$@; \
	fi

psdefs.ps: ;

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
