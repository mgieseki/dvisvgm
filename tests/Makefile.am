## This file is part of dvisvgm
## Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

SUBDIRS = data

check_LTLIBRARIES = libgtest.la
libgtest_la_SOURCES = gtest/src/gtest-all.cc gtest/src/gtest_main.cc
libgtest_la_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include -I$(top_srcdir)/tests/gtest
libgtest_la_LDFLAGS = -pthread
EXTRA_DIST = gtest/LICENSE \
	gtest/include \
	gtest/src/gtest.cc \
	gtest/src/gtest-death-test.cc \
	gtest/src/gtest-filepath.cc \
	gtest/src/gtest-internal-inl.h \
	gtest/src/gtest-port.cc \
	gtest/src/gtest-printers.cc \
	gtest/src/gtest-test-part.cc \
	gtest/src/gtest-typed-test.cc

AM_CXXFLAGS = -I$(top_srcdir)/src -Wall -DSRCDIR='"$(srcdir)"' $(CODE_COVERAGE_CFLAGS)

TESTS = hashcheck
check_PROGRAMS = hashcheck
nodist_hashcheck_SOURCES = hashcheck.cpp
hashcheck_LDADD = ../libs/xxHash/libxxhash.a
hashcheck_CPPFLAGS = -I$(top_srcdir)/libs/xxHash

hashcheck.cpp: genhashcheck.py $(top_srcdir)/src/AGLTable.hpp $(top_srcdir)/libs/xxHash/xxhash.h
	python $^ >$@

TESTLIBS = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a libgtest.la -lpotrace -lfreetype 
if ENABLE_WOFF
TESTLIBS += \
	../libs/ff-woff/libfontforge.a \
	../libs/woff2/libwoff2.a \
	../libs/woff2/brotli/libbrotli.a
endif
TESTLIBS += $(CODE_COVERAGE_LDFLAGS)

TESTS += BezierTest
check_PROGRAMS += BezierTest
BezierTest_SOURCES = BezierTest.cpp
BezierTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
BezierTest_LDADD = $(TESTLIBS)

TESTS += BitmapTest
check_PROGRAMS += BitmapTest
BitmapTest_SOURCES = BitmapTest.cpp
BitmapTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
BitmapTest_LDADD = $(TESTLIBS)

TESTS += BoundingBoxTest
check_PROGRAMS += BoundingBoxTest
BoundingBoxTest_SOURCES = BoundingBoxTest.cpp
BoundingBoxTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
BoundingBoxTest_LDADD = $(TESTLIBS)

TESTS += CalculatorTest
check_PROGRAMS += CalculatorTest
CalculatorTest_SOURCES = CalculatorTest.cpp
CalculatorTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
CalculatorTest_LDADD = $(TESTLIBS)

TESTS += CMapManagerTest
check_PROGRAMS += CMapManagerTest
CMapManagerTest_SOURCES = CMapManagerTest.cpp
CMapManagerTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
CMapManagerTest_LDADD = $(TESTLIBS)

TESTS += CMapReaderTest
check_PROGRAMS += CMapReaderTest
CMapReaderTest_SOURCES = CMapReaderTest.cpp
CMapReaderTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
CMapReaderTest_LDADD = $(TESTLIBS)

TESTS += CMapTest
check_PROGRAMS += CMapTest
CMapTest_SOURCES = CMapTest.cpp
CMapTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
CMapTest_LDADD = $(TESTLIBS)

TESTS += ColorSpecialTest
check_PROGRAMS += ColorSpecialTest
ColorSpecialTest_SOURCES = ColorSpecialTest.cpp
ColorSpecialTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
ColorSpecialTest_LDADD = $(TESTLIBS)

TESTS += ColorTest
check_PROGRAMS += ColorTest
ColorTest_SOURCES = ColorTest.cpp
ColorTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
ColorTest_LDADD = $(TESTLIBS)

TESTS += CommandLineTest
check_PROGRAMS += CommandLineTest
CommandLineTest_SOURCES = CommandLineTest.cpp
CommandLineTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
CommandLineTest_LDADD = $(TESTLIBS)

TESTS += CRC32Test
check_PROGRAMS += CRC32Test
CRC32Test_SOURCES = CRC32Test.cpp
CRC32Test_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
CRC32Test_LDADD = $(TESTLIBS)

TESTS += DependencyGraphTest
check_PROGRAMS += DependencyGraphTest
DependencyGraphTest_SOURCES = DependencyGraphTest.cpp
DependencyGraphTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
DependencyGraphTest_LDADD = $(TESTLIBS)

TESTS += DirectoryTest
check_PROGRAMS += DirectoryTest
DirectoryTest_SOURCES = DirectoryTest.cpp
DirectoryTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
DirectoryTest_LDADD = $(TESTLIBS)

TESTS += DvisvgmSpecialTest
check_PROGRAMS += DvisvgmSpecialTest
DvisvgmSpecialTest_SOURCES = DvisvgmSpecialTest.cpp
DvisvgmSpecialTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
DvisvgmSpecialTest_LDADD = $(TESTLIBS)

TESTS += EmSpecialTest
check_PROGRAMS += EmSpecialTest
EmSpecialTest_SOURCES = EmSpecialTest.cpp
EmSpecialTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
EmSpecialTest_LDADD = $(TESTLIBS)

TESTS += FileFinderTest
check_PROGRAMS += FileFinderTest
FileFinderTest_SOURCES = FileFinderTest.cpp
FileFinderTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
FileFinderTest_LDADD = $(TESTLIBS)

TESTS += FilePathTest
check_PROGRAMS += FilePathTest
FilePathTest_SOURCES = FilePathTest.cpp
FilePathTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
FilePathTest_LDADD = $(TESTLIBS)

TESTS += FileSystemTest
check_PROGRAMS += FileSystemTest
FileSystemTest_SOURCES = FileSystemTest.cpp
FileSystemTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
FileSystemTest_LDADD = $(TESTLIBS)

TESTS += FontCacheTest
check_PROGRAMS += FontCacheTest
FontCacheTest_SOURCES = FontCacheTest.cpp
FontCacheTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
FontCacheTest_LDADD = $(TESTLIBS)

TESTS += FontManagerTest
check_PROGRAMS += FontManagerTest
FontManagerTest_SOURCES = FontManagerTest.cpp
FontManagerTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
FontManagerTest_LDADD = $(TESTLIBS)

TESTS += FontMapTest
check_PROGRAMS += FontMapTest
FontMapTest_SOURCES = FontMapTest.cpp
FontMapTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
FontMapTest_LDADD = $(TESTLIBS)

TESTS += GFGlyphTracerTest
check_PROGRAMS += GFGlyphTracerTest
GFGlyphTracerTest_SOURCES = GFGlyphTracerTest.cpp
GFGlyphTracerTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
GFGlyphTracerTest_LDADD = $(TESTLIBS)

TESTS += GFReaderTest
check_PROGRAMS += GFReaderTest
GFReaderTest_SOURCES = GFReaderTest.cpp
GFReaderTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
GFReaderTest_LDADD = $(TESTLIBS)

TESTS += GhostscriptTest
check_PROGRAMS += GhostscriptTest
GhostscriptTest_SOURCES = GhostscriptTest.cpp
GhostscriptTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
GhostscriptTest_LDADD = $(TESTLIBS)

TESTS += GraphicsPathTest
check_PROGRAMS += GraphicsPathTest
GraphicsPathTest_SOURCES = GraphicsPathTest.cpp
GraphicsPathTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
GraphicsPathTest_LDADD = $(TESTLIBS)

TESTS += JFMReaderTest
check_PROGRAMS += JFMReaderTest
JFMReaderTest_SOURCES = JFMReaderTest.cpp
JFMReaderTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
JFMReaderTest_LDADD = $(TESTLIBS)

TESTS += LengthTest
check_PROGRAMS += LengthTest
LengthTest_SOURCES = LengthTest.cpp
LengthTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
LengthTest_LDADD = $(TESTLIBS)

TESTS += MapLineTest
check_PROGRAMS += MapLineTest
MapLineTest_SOURCES = MapLineTest.cpp
MapLineTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
MapLineTest_LDADD = $(TESTLIBS)

TESTS += MatrixTest
check_PROGRAMS += MatrixTest
MatrixTest_SOURCES = MatrixTest.cpp
MatrixTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
MatrixTest_LDADD = $(TESTLIBS)

TESTS += MessageExceptionTest
check_PROGRAMS += MessageExceptionTest
MessageExceptionTest_SOURCES = MessageExceptionTest.cpp
MessageExceptionTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
MessageExceptionTest_LDADD = $(TESTLIBS)

TESTS += PageRagesTest
check_PROGRAMS += PageRagesTest
PageRagesTest_SOURCES = PageRagesTest.cpp
PageRagesTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
PageRagesTest_LDADD = $(TESTLIBS)

TESTS += PageSizeTest
check_PROGRAMS += PageSizeTest
PageSizeTest_SOURCES = PageSizeTest.cpp
PageSizeTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
PageSizeTest_LDADD = $(TESTLIBS)

TESTS += PairTest
check_PROGRAMS += PairTest
PairTest_SOURCES = PairTest.cpp
PairTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
PairTest_LDADD = $(TESTLIBS)

TESTS += PapersizeSpecialTest
check_PROGRAMS += PapersizeSpecialTest
PapersizeSpecialTest_SOURCES = PapersizeSpecialTest.cpp
PapersizeSpecialTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
PapersizeSpecialTest_LDADD = $(TESTLIBS)

TESTS += PSInterpreterTest
check_PROGRAMS += PSInterpreterTest
PSInterpreterTest_SOURCES = PSInterpreterTest.cpp
PSInterpreterTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
PSInterpreterTest_LDADD = $(TESTLIBS)

TESTS += RangeMapTest
check_PROGRAMS += RangeMapTest
RangeMapTest_SOURCES = RangeMapTest.cpp
RangeMapTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
RangeMapTest_LDADD = $(TESTLIBS)

TESTS += ShadingPatchTest
check_PROGRAMS += ShadingPatchTest
ShadingPatchTest_SOURCES = ShadingPatchTest.cpp
ShadingPatchTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
ShadingPatchTest_LDADD = $(TESTLIBS)

TESTS += SplittedCharInputBufferTest
check_PROGRAMS += SplittedCharInputBufferTest
SplittedCharInputBufferTest_SOURCES = SplittedCharInputBufferTest.cpp
SplittedCharInputBufferTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
SplittedCharInputBufferTest_LDADD = $(TESTLIBS)

TESTS += StreamInputBufferTest
check_PROGRAMS += StreamInputBufferTest
StreamInputBufferTest_SOURCES = StreamInputBufferTest.cpp
StreamInputBufferTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
StreamInputBufferTest_LDADD = $(TESTLIBS)

TESTS += StreamReaderTest
check_PROGRAMS += StreamReaderTest
StreamReaderTest_SOURCES = StreamReaderTest.cpp
StreamReaderTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
StreamReaderTest_LDADD = $(TESTLIBS)

TESTS += StreamWriterTest
check_PROGRAMS += StreamWriterTest
StreamWriterTest_SOURCES = StreamWriterTest.cpp
StreamWriterTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
StreamWriterTest_LDADD = $(TESTLIBS)

TESTS += SubfontTest
check_PROGRAMS += SubfontTest
SubfontTest_SOURCES = SubfontTest.cpp
SubfontTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
SubfontTest_LDADD = $(TESTLIBS)

TESTS += SVGOutputTest
check_PROGRAMS += SVGOutputTest
SVGOutputTest_SOURCES = SVGOutputTest.cpp
SVGOutputTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
SVGOutputTest_LDADD = $(TESTLIBS)

TESTS += TensorProductPatchTest
check_PROGRAMS += TensorProductPatchTest
TensorProductPatchTest_SOURCES = TensorProductPatchTest.cpp
TensorProductPatchTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
TensorProductPatchTest_LDADD = $(TESTLIBS)

TESTS += TFMReaderTest
check_PROGRAMS += TFMReaderTest
TFMReaderTest_SOURCES = TFMReaderTest.cpp
TFMReaderTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
TFMReaderTest_LDADD = $(TESTLIBS)

TESTS += ToUnicodeMapTest
check_PROGRAMS += ToUnicodeMapTest
ToUnicodeMapTest_SOURCES = ToUnicodeMapTest.cpp
ToUnicodeMapTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
ToUnicodeMapTest_LDADD = $(TESTLIBS)

TESTS += TriangularPatchTest
check_PROGRAMS += TriangularPatchTest
TriangularPatchTest_SOURCES = TriangularPatchTest.cpp
TriangularPatchTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
TriangularPatchTest_LDADD = $(TESTLIBS)

TESTS += UnicodeTest
check_PROGRAMS += UnicodeTest
UnicodeTest_SOURCES = UnicodeTest.cpp
UnicodeTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
UnicodeTest_LDADD = $(TESTLIBS)

TESTS += UtilityTest
check_PROGRAMS += UtilityTest
UtilityTest_SOURCES = UtilityTest.cpp
UtilityTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
UtilityTest_LDADD = $(TESTLIBS)

TESTS += VectorIteratorTest
check_PROGRAMS += VectorIteratorTest
VectorIteratorTest_SOURCES = VectorIteratorTest.cpp
VectorIteratorTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
VectorIteratorTest_LDADD = $(TESTLIBS)

TESTS += VectorStreamTest
check_PROGRAMS += VectorStreamTest
VectorStreamTest_SOURCES = VectorStreamTest.cpp
VectorStreamTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
VectorStreamTest_LDADD = $(TESTLIBS)

TESTS += XMLNodeTest
check_PROGRAMS += XMLNodeTest
XMLNodeTest_SOURCES = XMLNodeTest.cpp
XMLNodeTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
XMLNodeTest_LDADD = $(TESTLIBS)

TESTS += XMLStringTest
check_PROGRAMS += XMLStringTest
XMLStringTest_SOURCES = XMLStringTest.cpp
XMLStringTest_CPPFLAGS = -I$(top_srcdir)/tests/gtest/include
XMLStringTest_LDADD = $(TESTLIBS)

EXTRA_DIST += check-conv genhashcheck.py normalize.xsl
TESTS += check-conv

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno hashcheck.cpp
