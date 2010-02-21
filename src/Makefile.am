## This file is part of dvisvgm
## Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

bin_PROGRAMS         = dvisvgm
noinst_LIBRARIES     = libdvisvgm.a

dvisvgm_SOURCES      = gzstream.h StreamCounter.h \
							  dvisvgm.cpp gzstream.cpp
							  
dvisvgm_LDADD        = $(noinst_LIBRARIES) @EXTRA_LIBS@
dvisvgm_DEPENDENCIES = $(noinst_LIBRARIES) @EXTRA_LIBS@

libdvisvgm_a_SOURCES = Bitmap.h BoundingBox.h BgColorSpecialHandler.h Calculator.h CharmapTranslator.h CmdLineParserBase.h Color.h \
	ColorSpecialHandler.h CommandLine.h Directory.h DLLoader.h DVIActions.h DVIReader.h DvisvgmSpecialHandler.h DVIToSVG.h \
	DVIToSVGActions.h EmSpecialHandler.h FileFinder.h FileSystem.h Font.h FontCache.h FontEmitter.h FontEncoding.h FontEngine.h \
	FontManager.h FontMap.h GFReader.h GFTracer.h GFGlyphTracer.h Ghostscript.h Glyph.h GraphicPath.h InputBuffer.h InputReader.h \
	Length.h macros.h Matrix.h Message.h MessageException.h MetafontWrapper.h PageSize.h Pair.h PSInterpreter.h PsSpecialHandler.h \
	SpecialActions.h SpecialHandler.h SpecialManager.h StreamReader.h SVGFontEmitter.h SVGFontTraceEmitter.h SVGTree.h \
	TpicSpecialHandler.h TFM.h types.h VectorStream.h VFActions.h VFReader.h XMLDocTypeNode.h XMLDocument.h XMLNode.h XMLString.h \
	BgColorSpecialHandler.cpp Bitmap.cpp BoundingBox.cpp Calculator.cpp CharmapTranslator.cpp CmdLineParserBase.cpp Color.cpp \
  	ColorSpecialHandler.cpp CommandLine.cpp Directory.cpp DLLoader.cpp DVIActions.cpp DVIReader.cpp DvisvgmSpecialHandler.cpp \
	DVIToSVG.cpp DVIToSVGActions.cpp EmSpecialHandler.cpp FileFinder.cpp FileSystem.cpp Font.cpp FontCache.cpp FontEncoding.cpp \
	FontEngine.cpp FontManager.cpp FontMap.cpp GFReader.cpp GFGlyphTracer.cpp GFTracer.cpp Ghostscript.cpp \
	InputBuffer.cpp InputReader.cpp Length.cpp Matrix.cpp Message.cpp MetafontWrapper.cpp PageSize.cpp PSInterpreter.cpp \
	PsSpecialHandler.cpp SpecialManager.cpp StreamReader.cpp SVGFontEmitter.cpp SVGFontTraceEmitter.cpp SVGTree.cpp TFM.cpp \
	TpicSpecialHandler.cpp VFReader.cpp XMLDocument.cpp XMLNode.cpp XMLString.cpp

EXTRA_DIST = options.xml psdefs.psc iapi.h ierrors.h

AM_CXXFLAGS = -Wall -Wnon-virtual-dtor

# the commandline parser is generated from options.xml by opt2cpp
CommandLine.cpp: options.xml
	if test -f opt2cpp.xsl; then \
		rm -f $@ $*.h; \
		xsltproc opt2cpp.xsl $<; \
	fi

# The required MikTeX functions are declared in .def files that reference a DLL.
# It's necessary to create import libraries from these .def files. dlltool helps us
# to do that.
.def.a:
	dlltool -d $< -l $@ -U


# Create a C string definition containing the PostScript routines psdefs.ps needed by class PSInterpreter
psdefs.psc: psdefs.ps
	if test -f $<; then \
		ps2c PSInterpreter::PSDEFS $< >$@; \
	fi

psdefs.ps: ;

