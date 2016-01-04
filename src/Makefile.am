:## This file is part of dvisvgm
## Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

bin_PROGRAMS         = dvisvgm
noinst_LIBRARIES     = libdvisvgm.a

dvisvgm_SOURCES      = gzstream.h \
							  dvisvgm.cpp gzstream.cpp

dvisvgm_LDADD = \
	$(noinst_LIBRARIES) \
	../clipper/libclipper.a \
	../xxHash/libxxhash.a \
	$(FREETYPE_LIBS) \
	$(ZLIB_LIBS)

dvisvgm_DEPENDENCIES = $(noinst_LIBRARIES)

libdvisvgm_a_SOURCES = \
	BasicDVIReader.cpp \
	BasicDVIReader.h \
	Bezier.cpp \
	Bezier.h \
	BgColorSpecialHandler.cpp \
	BgColorSpecialHandler.h \
	Bitmap.cpp \
	Bitmap.h \
	BoundingBox.cpp \
	BoundingBox.h \
	Calculator.cpp \
	Calculator.h \
	Character.h \
	CharMapID.cpp \
	CharMapID.h \
	CMap.cpp \
	CMap.h \
	CMapManager.cpp \
	CMapManager.h \
	CMapReader.cpp \
	CMapReader.h \
	CmdLineParserBase.cpp \
	CmdLineParserBase.h \
	Color.cpp \
	Color.h \
	ColorSpecialHandler.cpp \
	ColorSpecialHandler.h \
	CommandLine.cpp \
	CommandLine.h \
	CRC32.cpp \
	CRC32.h \
	DependencyGraph.h \
	Directory.cpp \
	Directory.h \
	DLLoader.cpp \
	DLLoader.h \
	DVIActions.h \
	DVIReader.cpp \
	DVIReader.h \
	DvisvgmSpecialHandler.cpp \
	DvisvgmSpecialHandler.h \
	DVIToSVG.cpp \
	DVIToSVG.h \
	DVIToSVGActions.cpp \
	DVIToSVGActions.h \
	EmSpecialHandler.cpp \
	EmSpecialHandler.h \
	EncFile.cpp \
	EncFile.h \
	EPSFile.cpp \
	EPSFile.h \
	EPSToSVG.cpp \
	EPSToSVG.h \
	FileFinder.cpp \
	FileFinder.h \
	FilePath.cpp \
	FilePath.h \
	FileSystem.cpp \
	FileSystem.h \
	Font.cpp \
	Font.h \
	FontCache.cpp \
	FontCache.h \
	FontEncoding.cpp \
	FontEncoding.h \
	FontEngine.cpp \
	FontEngine.h \
	FontManager.cpp \
	FontManager.h \
	FontMap.cpp \
	FontMap.h \
	FontMetrics.cpp \
	FontMetrics.h \
	FontStyle.h \
	GFGlyphTracer.cpp \
	GFGlyphTracer.h \
	GFReader.cpp \
	GFReader.h \
	GFTracer.cpp \
	GFTracer.h \
	Ghostscript.cpp \
	Ghostscript.h \
	Glyph.h \
	GlyphTracerMessages.h \
	GraphicsPath.h \
	HtmlSpecialHandler.cpp \
	HtmlSpecialHandler.h \
	InputBuffer.cpp \
	InputBuffer.h \
	InputReader.cpp \
	InputReader.h \
	JFM.cpp \
	JFM.h \
	Length.cpp \
	Length.h \
	macros.h \
	MapLine.cpp \
	MapLine.h \
	Matrix.cpp \
	Matrix.h \
	Message.cpp \
	Message.h \
	MessageException.h \
	MetafontWrapper.cpp \
	MetafontWrapper.h \
	NoPsSpecialHandler.cpp \
	NoPsSpecialHandler.h \
	NumericRanges.h \
	PageRanges.cpp \
	PageRanges.h \
	PageSize.cpp \
	PageSize.h \
	Pair.h \
	PathClipper.cpp \
	PathClipper.h \
	PdfSpecialHandler.cpp \
	PdfSpecialHandler.h \
	PreScanDVIReader.cpp \
	PreScanDVIReader.h \
	Process.cpp \
	Process.h \
	psdefs.cpp \
	PSFilter.h \
	PSInterpreter.cpp \
	PSInterpreter.h \
	PSPattern.cpp \
	PSPattern.h \
	PSPreviewFilter.cpp \
	PSPreviewFilter.h \
	PsSpecialHandler.cpp \
	PsSpecialHandler.h \
	RangeMap.cpp \
	RangeMap.h \
	ShadingPatch.cpp \
	ShadingPatch.h \
	SignalHandler.cpp \
	SignalHandler.h \
	SpecialActions.h \
	SpecialHandler.h \
	SpecialManager.cpp \
	SpecialManager.h \
	StreamReader.cpp \
	StreamReader.h \
	StreamWriter.cpp \
	StreamWriter.h \
	Subfont.cpp \
	Subfont.h \
	SVGOutput.cpp \
	SVGOutput.h \
	SVGTree.cpp \
	SVGTree.h \
	System.cpp \
	System.h \
	TensorProductPatch.cpp \
	TensorProductPatch.h \
	Terminal.cpp \
	Terminal.h \
	TFM.cpp \
	TFM.h \
	ToUnicodeMap.cpp \
	ToUnicodeMap.h \
	TpicSpecialHandler.cpp \
	TpicSpecialHandler.h \
	TriangularPatch.cpp \
	TriangularPatch.h \
	types.h \
	Unicode.cpp \
	Unicode.h \
	VectorIterator.h \
	VectorStream.h \
	VFActions.h \
	VFReader.cpp \
	VFReader.h \
	XMLDocument.cpp \
	XMLDocument.h \
	XMLNode.cpp \
	XMLNode.h \
	XMLString.cpp \
	XMLString.h

EXTRA_DIST = options.xml options.dtd iapi.h ierrors.h MiKTeXCom.h MiKTeXCom.cpp

AM_CXXFLAGS = -Wall -Wnon-virtual-dtor \
	-I$(top_srcdir)/clipper \
	-I$(top_srcdir)/xxHash \
	$(FREETYPE_CFLAGS) \
	$(ZLIB_CFLAGS) \
	$(CODE_COVERAGE_CFLAGS)

AM_LDFLAGS = $(CODE_COVERAGE_LDFLAGS)

# the command-line parser is generated from options.xml by opt2cpp
$(srcdir)/CommandLine.cpp: options.xml
	if test -f opt2cpp.xsl; then \
		rm -f $@ $*.h; \
		xsltproc opt2cpp.xsl $<; \
	elif test -f $(srcdir)/opt2cpp.py; then \
		rm -f $@ $*.h; \
		python2 $(srcdir)/opt2cpp.py $< $@ $*.h; \
	fi

# Create a C string definition containing the PostScript routines psdefs.ps needed by class PSInterpreter
$(srcdir)/psdefs.cpp: psdefs.ps
	if test -f $<; then \
		ps2c PSInterpreter::PSDEFS $< >$@; \
	fi

psdefs.ps: ;

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
