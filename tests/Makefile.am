## This file is part of dvisvgm
## Copyright (C) 2005-2020 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

SUBDIRS = data

check_LTLIBRARIES = libgtest.la
libgtest_la_SOURCES = gtest/src/gtest-all.cc testmain.cpp
libgtest_la_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include -I$(dvisvgm_srcdir)/tests/gtest
libgtest_la_LDFLAGS = -pthread
EXTRA_DIST = gtest/LICENSE \
	gtest/include \
	gtest/src/gtest.cc \
	gtest/src/gtest-death-test.cc \
	gtest/src/gtest-filepath.cc \
	gtest/src/gtest-internal-inl.h \
	gtest/src/gtest_main.cc \
	gtest/src/gtest-matchers.cc \
	gtest/src/gtest-port.cc \
	gtest/src/gtest-printers.cc \
	gtest/src/gtest-test-part.cc \
	gtest/src/gtest-typed-test.cc

include ../libs/defs.am

LIBS_CFLAGS = $(XXHASH_CFLAGS)
LIBS_LIBS   = $(POTRACE_LIBS) $(XXHASH_LIBS)
if ENABLE_WOFF
LIBS_LIBS  += $(WOFF2_LIBS) $(BROTLI_LIBS)
endif

if USE_BUNDLED_MD5
LIBS_CFLAGS += -I$(dvisvgm_srcdir)/libs/md5
LIBS_LIBS   += ../libs/md5/libmd5.a
else
LIBS_CFLAGS += $(LIBCRYPTO_CFLAGS)
LIBS_LIBS   += $(LIBCRYPTO_LIBS)
endif

AM_CXXFLAGS = -I$(dvisvgm_srcdir)/src -I$(dvisvgm_srcdir)/libs/variant/include \
	-Wall -Wno-mismatched-tags \
	-DBUILDDIR='"$(abs_builddir)"' -DSRCDIR='"$(abs_srcdir)"' \
	$(CODE_COVERAGE_CFLAGS)

TESTS = hashcheck
check_PROGRAMS = hashcheck
nodist_hashcheck_SOURCES = hashcheck.cpp
hashcheck_CPPFLAGS = $(LIBS_CFLAGS)
hashcheck_LDADD = $(LIBS_LIBS)

hashcheck.cpp: genhashcheck.py $(dvisvgm_srcdir)/src/AGLTable.hpp $(dvisvgm_srcdir)/libs/xxHash/xxhash.h
	python $^ >$@

TESTLIBS = libgtest.la ../src/libdvisvgm.la $(LIBS_LIBS) -lfreetype
if ENABLE_WOFF
TESTLIBS += ../libs/ff-woff/libfontforge.a
endif
TESTLIBS += $(CODE_COVERAGE_LDFLAGS)

TESTS += BezierTest
check_PROGRAMS += BezierTest
BezierTest_SOURCES = BezierTest.cpp testutil.hpp
BezierTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
BezierTest_LDADD = $(TESTLIBS)

TESTS += BitmapTest
check_PROGRAMS += BitmapTest
BitmapTest_SOURCES = BitmapTest.cpp testutil.hpp
BitmapTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
BitmapTest_LDADD = $(TESTLIBS)

TESTS += BoundingBoxTest
check_PROGRAMS += BoundingBoxTest
BoundingBoxTest_SOURCES = BoundingBoxTest.cpp testutil.hpp
BoundingBoxTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
BoundingBoxTest_LDADD = $(TESTLIBS)

TESTS += CalculatorTest
check_PROGRAMS += CalculatorTest
CalculatorTest_SOURCES = CalculatorTest.cpp testutil.hpp
CalculatorTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
CalculatorTest_LDADD = $(TESTLIBS)

TESTS += CMapManagerTest
check_PROGRAMS += CMapManagerTest
CMapManagerTest_SOURCES = CMapManagerTest.cpp testutil.hpp
CMapManagerTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
CMapManagerTest_LDADD = $(TESTLIBS)

TESTS += CMapReaderTest
check_PROGRAMS += CMapReaderTest
CMapReaderTest_SOURCES = CMapReaderTest.cpp testutil.hpp
CMapReaderTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
CMapReaderTest_LDADD = $(TESTLIBS)

TESTS += CMapTest
check_PROGRAMS += CMapTest
CMapTest_SOURCES = CMapTest.cpp testutil.hpp
CMapTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
CMapTest_LDADD = $(TESTLIBS)

TESTS += ColorSpecialTest
check_PROGRAMS += ColorSpecialTest
ColorSpecialTest_SOURCES = ColorSpecialTest.cpp testutil.hpp
ColorSpecialTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
ColorSpecialTest_LDADD = $(TESTLIBS)

TESTS += ColorTest
check_PROGRAMS += ColorTest
ColorTest_SOURCES = ColorTest.cpp testutil.hpp
ColorTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
ColorTest_LDADD = $(TESTLIBS)

TESTS += CommandLineTest
check_PROGRAMS += CommandLineTest
CommandLineTest_SOURCES = CommandLineTest.cpp testutil.hpp
CommandLineTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
CommandLineTest_LDADD = $(TESTLIBS)

TESTS += DependencyGraphTest
check_PROGRAMS += DependencyGraphTest
DependencyGraphTest_SOURCES = DependencyGraphTest.cpp testutil.hpp
DependencyGraphTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
DependencyGraphTest_LDADD = $(TESTLIBS)

TESTS += DirectoryTest
check_PROGRAMS += DirectoryTest
DirectoryTest_SOURCES = DirectoryTest.cpp testutil.hpp
DirectoryTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
DirectoryTest_LDADD = $(TESTLIBS)

TESTS += DVIReaderTest
check_PROGRAMS += DVIReaderTest
DVIReaderTest_SOURCES = DVIReaderTest.cpp testutil.hpp
DVIReaderTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
DVIReaderTest_LDADD = $(TESTLIBS)

TESTS += DvisvgmSpecialTest
check_PROGRAMS += DvisvgmSpecialTest
DvisvgmSpecialTest_SOURCES = DvisvgmSpecialTest.cpp testutil.hpp
DvisvgmSpecialTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
DvisvgmSpecialTest_LDADD = $(TESTLIBS)

TESTS += EllipticalArcTest
check_PROGRAMS += EllipticalArcTest
EllipticalArcTest_SOURCES = EllipticalArcTest.cpp testutil.hpp
EllipticalArcTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
EllipticalArcTest_LDADD = $(TESTLIBS)

TESTS += EmSpecialTest
check_PROGRAMS += EmSpecialTest
EmSpecialTest_SOURCES = EmSpecialTest.cpp testutil.hpp
EmSpecialTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
EmSpecialTest_LDADD = $(TESTLIBS)

TESTS += FileFinderTest
check_PROGRAMS += FileFinderTest
FileFinderTest_SOURCES = FileFinderTest.cpp testutil.hpp
FileFinderTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
FileFinderTest_LDADD = $(TESTLIBS)

TESTS += FilePathTest
check_PROGRAMS += FilePathTest
FilePathTest_SOURCES = FilePathTest.cpp testutil.hpp
FilePathTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
FilePathTest_LDADD = $(TESTLIBS)

TESTS += FileSystemTest
check_PROGRAMS += FileSystemTest
FileSystemTest_SOURCES = FileSystemTest.cpp testutil.hpp
FileSystemTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
FileSystemTest_LDADD = $(TESTLIBS)

TESTS += FontCacheTest
check_PROGRAMS += FontCacheTest
FontCacheTest_SOURCES = FontCacheTest.cpp testutil.hpp
FontCacheTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
FontCacheTest_LDADD = $(TESTLIBS)

TESTS += FontManagerTest
check_PROGRAMS += FontManagerTest
FontManagerTest_SOURCES = FontManagerTest.cpp testutil.hpp
FontManagerTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
FontManagerTest_LDADD = $(TESTLIBS)

TESTS += FontMapTest
check_PROGRAMS += FontMapTest
FontMapTest_SOURCES = FontMapTest.cpp testutil.hpp
FontMapTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
FontMapTest_LDADD = $(TESTLIBS)

TESTS += GFGlyphTracerTest
check_PROGRAMS += GFGlyphTracerTest
GFGlyphTracerTest_SOURCES = GFGlyphTracerTest.cpp testutil.hpp
GFGlyphTracerTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
GFGlyphTracerTest_LDADD = $(TESTLIBS)

TESTS += GFReaderTest
check_PROGRAMS += GFReaderTest
GFReaderTest_SOURCES = GFReaderTest.cpp testutil.hpp
GFReaderTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
GFReaderTest_LDADD = $(TESTLIBS)

TESTS += GhostscriptTest
check_PROGRAMS += GhostscriptTest
GhostscriptTest_SOURCES = GhostscriptTest.cpp testutil.hpp
GhostscriptTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
GhostscriptTest_LDADD = $(TESTLIBS)

TESTS += GraphicsPathTest
check_PROGRAMS += GraphicsPathTest
GraphicsPathTest_SOURCES = GraphicsPathTest.cpp testutil.hpp
GraphicsPathTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
GraphicsPathTest_LDADD = $(TESTLIBS)

TESTS += HashFunctionTest
check_PROGRAMS += HashFunctionTest
HashFunctionTest_SOURCES = HashFunctionTest.cpp testutil.hpp
HashFunctionTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
HashFunctionTest_LDADD = $(TESTLIBS)

TESTS += JFMReaderTest
check_PROGRAMS += JFMReaderTest
JFMReaderTest_SOURCES = JFMReaderTest.cpp testutil.hpp
JFMReaderTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
JFMReaderTest_LDADD = $(TESTLIBS)

TESTS += LengthTest
check_PROGRAMS += LengthTest
LengthTest_SOURCES = LengthTest.cpp testutil.hpp
LengthTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
LengthTest_LDADD = $(TESTLIBS)

TESTS += MapLineTest
check_PROGRAMS += MapLineTest
MapLineTest_SOURCES = MapLineTest.cpp testutil.hpp
MapLineTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
MapLineTest_LDADD = $(TESTLIBS)

TESTS += MatrixTest
check_PROGRAMS += MatrixTest
MatrixTest_SOURCES = MatrixTest.cpp testutil.hpp
MatrixTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
MatrixTest_LDADD = $(TESTLIBS)

TESTS += MessageExceptionTest
check_PROGRAMS += MessageExceptionTest
MessageExceptionTest_SOURCES = MessageExceptionTest.cpp testutil.hpp
MessageExceptionTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
MessageExceptionTest_LDADD = $(TESTLIBS)

TESTS += PageRagesTest
check_PROGRAMS += PageRagesTest
PageRagesTest_SOURCES = PageRagesTest.cpp testutil.hpp
PageRagesTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
PageRagesTest_LDADD = $(TESTLIBS)

TESTS += PageSizeTest
check_PROGRAMS += PageSizeTest
PageSizeTest_SOURCES = PageSizeTest.cpp testutil.hpp
PageSizeTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
PageSizeTest_LDADD = $(TESTLIBS)

TESTS += PairTest
check_PROGRAMS += PairTest
PairTest_SOURCES = PairTest.cpp testutil.hpp
PairTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
PairTest_LDADD = $(TESTLIBS)

TESTS += PapersizeSpecialTest
check_PROGRAMS += PapersizeSpecialTest
PapersizeSpecialTest_SOURCES = PapersizeSpecialTest.cpp testutil.hpp
PapersizeSpecialTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
PapersizeSpecialTest_LDADD = $(TESTLIBS)

TESTS += PDFParserTest
check_PROGRAMS += PDFParserTest
PDFParserTest_SOURCES = PDFParserTest.cpp testutil.hpp
PDFParserTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
PDFParserTest_LDADD = $(TESTLIBS)

TESTS += PSInterpreterTest
check_PROGRAMS += PSInterpreterTest
PSInterpreterTest_SOURCES = PSInterpreterTest.cpp testutil.hpp
PSInterpreterTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
PSInterpreterTest_LDADD = $(TESTLIBS)

TESTS += RangeMapTest
check_PROGRAMS += RangeMapTest
RangeMapTest_SOURCES = RangeMapTest.cpp testutil.hpp
RangeMapTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
RangeMapTest_LDADD = $(TESTLIBS)

TESTS += ShadingPatchTest
check_PROGRAMS += ShadingPatchTest
ShadingPatchTest_SOURCES = ShadingPatchTest.cpp testutil.hpp
ShadingPatchTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
ShadingPatchTest_LDADD = $(TESTLIBS)

TESTS += SpecialManagerTest
check_PROGRAMS += SpecialManagerTest
SpecialManagerTest_SOURCES = SpecialManagerTest.cpp testutil.hpp
SpecialManagerTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
SpecialManagerTest_LDADD = $(TESTLIBS)

TESTS += SplittedCharInputBufferTest
check_PROGRAMS += SplittedCharInputBufferTest
SplittedCharInputBufferTest_SOURCES = SplittedCharInputBufferTest.cpp testutil.hpp
SplittedCharInputBufferTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
SplittedCharInputBufferTest_LDADD = $(TESTLIBS)

TESTS += StreamInputBufferTest
check_PROGRAMS += StreamInputBufferTest
StreamInputBufferTest_SOURCES = StreamInputBufferTest.cpp testutil.hpp
StreamInputBufferTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
StreamInputBufferTest_LDADD = $(TESTLIBS)

TESTS += StreamReaderTest
check_PROGRAMS += StreamReaderTest
StreamReaderTest_SOURCES = StreamReaderTest.cpp testutil.hpp
StreamReaderTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
StreamReaderTest_LDADD = $(TESTLIBS)

TESTS += StreamWriterTest
check_PROGRAMS += StreamWriterTest
StreamWriterTest_SOURCES = StreamWriterTest.cpp testutil.hpp
StreamWriterTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
StreamWriterTest_LDADD = $(TESTLIBS)

TESTS += StringMatcherTest
check_PROGRAMS += StringMatcherTest
StringMatcherTest_SOURCES = StringMatcherTest.cpp testutil.hpp
StringMatcherTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
StringMatcherTest_LDADD = $(TESTLIBS)

TESTS += SubfontTest
check_PROGRAMS += SubfontTest
SubfontTest_SOURCES = SubfontTest.cpp testutil.hpp
SubfontTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
SubfontTest_LDADD = $(TESTLIBS)

TESTS += SVGOutputTest
check_PROGRAMS += SVGOutputTest
SVGOutputTest_SOURCES = SVGOutputTest.cpp testutil.hpp
SVGOutputTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
SVGOutputTest_LDADD = $(TESTLIBS)

TESTS += TensorProductPatchTest
check_PROGRAMS += TensorProductPatchTest
TensorProductPatchTest_SOURCES = TensorProductPatchTest.cpp testutil.hpp
TensorProductPatchTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
TensorProductPatchTest_LDADD = $(TESTLIBS)

TESTS += TFMReaderTest
check_PROGRAMS += TFMReaderTest
TFMReaderTest_SOURCES = TFMReaderTest.cpp testutil.hpp
TFMReaderTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
TFMReaderTest_LDADD = $(TESTLIBS)

TESTS += ToUnicodeMapTest
check_PROGRAMS += ToUnicodeMapTest
ToUnicodeMapTest_SOURCES = ToUnicodeMapTest.cpp testutil.hpp
ToUnicodeMapTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
ToUnicodeMapTest_LDADD = $(TESTLIBS)

TESTS += TpicSpecialTest
check_PROGRAMS += TpicSpecialTest
TpicSpecialTest_SOURCES = TpicSpecialTest.cpp testutil.hpp
TpicSpecialTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
TpicSpecialTest_LDADD = $(TESTLIBS)

TESTS += TriangularPatchTest
check_PROGRAMS += TriangularPatchTest
TriangularPatchTest_SOURCES = TriangularPatchTest.cpp testutil.hpp
TriangularPatchTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
TriangularPatchTest_LDADD = $(TESTLIBS)

TESTS += UnicodeTest
check_PROGRAMS += UnicodeTest
UnicodeTest_SOURCES = UnicodeTest.cpp testutil.hpp
UnicodeTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
UnicodeTest_LDADD = $(TESTLIBS)

TESTS += UtilityTest
check_PROGRAMS += UtilityTest
UtilityTest_SOURCES = UtilityTest.cpp testutil.hpp
UtilityTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
UtilityTest_LDADD = $(TESTLIBS)

TESTS += VectorIteratorTest
check_PROGRAMS += VectorIteratorTest
VectorIteratorTest_SOURCES = VectorIteratorTest.cpp testutil.hpp
VectorIteratorTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
VectorIteratorTest_LDADD = $(TESTLIBS)

TESTS += VectorStreamTest
check_PROGRAMS += VectorStreamTest
VectorStreamTest_SOURCES = VectorStreamTest.cpp testutil.hpp
VectorStreamTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
VectorStreamTest_LDADD = $(TESTLIBS)

TESTS += XMLNodeTest
check_PROGRAMS += XMLNodeTest
XMLNodeTest_SOURCES = XMLNodeTest.cpp testutil.hpp
XMLNodeTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
XMLNodeTest_LDADD = $(TESTLIBS)

TESTS += XMLStringTest
check_PROGRAMS += XMLStringTest
XMLStringTest_SOURCES = XMLStringTest.cpp testutil.hpp
XMLStringTest_CPPFLAGS = -I$(dvisvgm_srcdir)/tests/gtest/include $(LIBS_CFLAGS)
XMLStringTest_LDADD = $(TESTLIBS)

EXTRA_DIST += check-conv genhashcheck.py normalize.xsl
TESTS += check-conv

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno hashcheck.cpp
