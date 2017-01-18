:## This file is part of dvisvgm
## Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

bin_PROGRAMS         = dvisvgm
noinst_LIBRARIES     = libdvisvgm.a

dvisvgm_SOURCES      = dvisvgm.cpp

dvisvgm_LDADD = \
	$(noinst_LIBRARIES) \
	../libs/clipper/libclipper.a \
	../libs/xxHash/libxxhash.a \
	$(FREETYPE_LIBS) \
	$(FONTFORGE_LIBS) \
	$(ZLIB_LIBS)

if ENABLE_WOFF
dvisvgm_LDADD += \
	../libs/ff-woff/libfontforge.a \
	../libs/woff2/libwoff2.a \
	../libs/woff2/brotli/libbrotli.a
endif

dvisvgm_DEPENDENCIES = $(noinst_LIBRARIES)

libdvisvgm_a_SOURCES = \
	AGLTable.hpp \
	BasicDVIReader.cpp \
	BasicDVIReader.hpp \
	Bezier.cpp \
	Bezier.hpp \
	BgColorSpecialHandler.cpp \
	BgColorSpecialHandler.hpp \
	Bitmap.cpp \
	Bitmap.hpp \
	BoundingBox.cpp \
	BoundingBox.hpp \
	Calculator.cpp \
	Calculator.hpp \
	Character.hpp \
	CharMapID.cpp \
	CharMapID.hpp \
	CLCommandLine.cpp \
	CLCommandLine.hpp \
	CLOption.hpp \
	CMap.cpp \
	CMap.hpp \
	CMapManager.cpp \
	CMapManager.hpp \
	CMapReader.cpp \
	CMapReader.hpp \
	Color.cpp \
	Color.hpp \
	ColorSpecialHandler.cpp \
	ColorSpecialHandler.hpp \
	CommandLine.hpp \
	CRC32.cpp \
	CRC32.hpp \
	DependencyGraph.hpp \
	Directory.cpp \
	Directory.hpp \
	DLLoader.cpp \
	DLLoader.hpp \
	DVIActions.hpp \
	DVIReader.cpp \
	DVIReader.hpp \
	DvisvgmSpecialHandler.cpp \
	DvisvgmSpecialHandler.hpp \
	DVIToSVG.cpp \
	DVIToSVG.hpp \
	DVIToSVGActions.cpp \
	DVIToSVGActions.hpp \
	EmSpecialHandler.cpp \
	EmSpecialHandler.hpp \
	EncFile.cpp \
	EncFile.hpp \
	EPSFile.cpp \
	EPSFile.hpp \
	EPSToSVG.cpp \
	EPSToSVG.hpp \
	FileFinder.cpp \
	FileFinder.hpp \
	FilePath.cpp \
	FilePath.hpp \
	FileSystem.cpp \
	FileSystem.hpp \
	FixWord.hpp \
	Font.cpp \
	Font.hpp \
	FontCache.cpp \
	FontCache.hpp \
	FontEncoding.cpp \
	FontEncoding.hpp \
	FontEngine.cpp \
	FontEngine.hpp \
	FontManager.cpp \
	FontManager.hpp \
	FontMap.cpp \
	FontMap.hpp \
	FontMetrics.cpp \
	FontMetrics.hpp \
	FontStyle.hpp \
	FontWriter.cpp \
	FontWriter.hpp \
	GFGlyphTracer.cpp \
	GFGlyphTracer.hpp \
	GFReader.cpp \
	GFReader.hpp \
	GFTracer.cpp \
	GFTracer.hpp \
	Ghostscript.cpp \
	Ghostscript.hpp \
	Glyph.hpp \
	GlyphTracerMessages.hpp \
	GraphicsPath.hpp \
	HtmlSpecialHandler.cpp \
	HtmlSpecialHandler.hpp \
	InputBuffer.cpp \
	InputBuffer.hpp \
	InputReader.cpp \
	InputReader.hpp \
	JFM.cpp \
	JFM.hpp \
	Length.cpp \
	Length.hpp \
	macros.hpp \
	MapLine.cpp \
	MapLine.hpp \
	Matrix.cpp \
	Matrix.hpp \
	Message.cpp \
	Message.hpp \
	MessageException.hpp \
	MetafontWrapper.cpp \
	MetafontWrapper.hpp \
	NoPsSpecialHandler.cpp \
	NoPsSpecialHandler.hpp \
	NumericRanges.hpp \
	PageRanges.cpp \
	PageRanges.hpp \
	PageSize.cpp \
	PageSize.hpp \
	Pair.hpp \
	PapersizeSpecialHandler.cpp \
	PapersizeSpecialHandler.hpp \
	PathClipper.cpp \
	PathClipper.hpp \
	PdfSpecialHandler.cpp \
	PdfSpecialHandler.hpp \
	PreScanDVIReader.cpp \
	PreScanDVIReader.hpp \
	Process.cpp \
	Process.hpp \
	psdefs.cpp \
	PSFilter.hpp \
	PSInterpreter.cpp \
	PSInterpreter.hpp \
	PSPattern.cpp \
	PSPattern.hpp \
	PSPreviewFilter.cpp \
	PSPreviewFilter.hpp \
	PsSpecialHandler.cpp \
	PsSpecialHandler.hpp \
	RangeMap.cpp \
	RangeMap.hpp \
	ShadingPatch.cpp \
	ShadingPatch.hpp \
	SignalHandler.cpp \
	SignalHandler.hpp \
	SpecialActions.hpp \
	SpecialHandler.hpp \
	SpecialManager.cpp \
	SpecialManager.hpp \
	StreamReader.cpp \
	StreamReader.hpp \
	StreamWriter.cpp \
	StreamWriter.hpp \
	Subfont.cpp \
	Subfont.hpp \
	SVGCharHandler.cpp \
	SVGCharHandler.hpp \
	SVGCharHandlerFactory.cpp \
	SVGCharHandlerFactory.hpp \
	SVGCharPathHandler.cpp \
	SVGCharPathHandler.hpp \
	SVGCharTspanTextHandler.cpp \
	SVGCharTspanTextHandler.hpp \
	SVGOutput.cpp \
	SVGOutput.hpp \
	SVGSingleCharTextHandler.cpp \
	SVGSingleCharTextHandler.hpp \
	SVGTree.cpp \
	SVGTree.hpp \
	System.cpp \
	System.hpp \
	TensorProductPatch.cpp \
	TensorProductPatch.hpp \
	Terminal.cpp \
	Terminal.hpp \
	TFM.cpp \
	TFM.hpp \
	ToUnicodeMap.cpp \
	ToUnicodeMap.hpp \
	TpicSpecialHandler.cpp \
	TpicSpecialHandler.hpp \
	TriangularPatch.cpp \
	TriangularPatch.hpp \
	Unicode.cpp \
	Unicode.hpp \
	utility.hpp \
	utility.cpp \
	VectorIterator.hpp \
	VectorStream.hpp \
	version.hpp \
	VFActions.hpp \
	VFReader.cpp \
	VFReader.hpp \
	XMLDocument.cpp \
	XMLDocument.hpp \
	XMLNode.cpp \
	XMLNode.hpp \
	XMLString.cpp \
	XMLString.hpp \
	ZLibOutputStream.hpp

if ENABLE_WOFF
libdvisvgm_a_SOURCES += ffwrapper.c ffwrapper.h
endif

EXTRA_DIST = options.xml options.dtd iapi.h ierrors.h MiKTeXCom.hpp MiKTeXCom.cpp

AM_CFLAGS = -Wall \
	$(ZLIB_CFLAGS) \
	$(CODE_COVERAGE_CFLAGS)

AM_CXXFLAGS = -Wall -Wnon-virtual-dtor \
	-I$(top_srcdir)/libs/clipper \
	-I$(top_srcdir)/libs/xxHash \
	$(FREETYPE_CFLAGS) \
	$(ZLIB_CFLAGS) \
	$(CODE_COVERAGE_CFLAGS)

if ENABLE_WOFF
AM_CFLAGS +=  \
	-I$(top_srcdir)/libs/ff-woff/fontforge \
	-I$(top_srcdir)/libs/ff-woff/inc

AM_CXXFLAGS +=  \
	-I$(top_srcdir)/libs/woff2/brotli/enc \
	-I$(top_srcdir)/libs/woff2/src
endif

AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

# the command-line parser class is generated from options.xml by opt2cpp
$(srcdir)/CommandLine.hpp: options.xml
	rm -f $@ $*.hpp
	python $(srcdir)/opt2cpp.py $< >$@

# Create a C string definition containing the PostScript routines psdefs.ps needed by class PSInterpreter
$(srcdir)/psdefs.cpp: psdefs.ps
	if test -f $<; then \
		ps2c PSInterpreter::PSDEFS $< >$@; \
	fi

psdefs.ps: ;

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
