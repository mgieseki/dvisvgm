## This file is part of dvisvgm
## Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

SUBDIRS = data

AM_CXXFLAGS = -I$(top_srcdir)/src -Wall -DSRCDIR='"$(srcdir)"' $(CODE_COVERAGE_CFLAGS)

TESTS = hashcheck
check_PROGRAMS = hashcheck
nodist_hashcheck_SOURCES = hashcheck.cpp
hashcheck_LDADD = ../libs/xxHash/libxxhash.a
hashcheck_CPPFLAGS = -I$(top_srcdir)/libs/xxHash
hashcheck.cpp: genhashcheck.py $(top_srcdir)/src/AGLTable.hpp $(top_srcdir)/libs/xxHash/xxhash.h
	python $^ >$@

TESTS += BezierTest
check_PROGRAMS += BezierTest
BezierTest_SOURCES = BezierTest.cpp
BezierTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += BitmapTest
check_PROGRAMS += BitmapTest
BitmapTest_SOURCES = BitmapTest.cpp
BitmapTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += BoundingBoxTest
check_PROGRAMS += BoundingBoxTest
BoundingBoxTest_SOURCES = BoundingBoxTest.cpp
BoundingBoxTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += CalculatorTest
check_PROGRAMS += CalculatorTest
CalculatorTest_SOURCES = CalculatorTest.cpp
CalculatorTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += CMapManagerTest
check_PROGRAMS += CMapManagerTest
CMapManagerTest_SOURCES = CMapManagerTest.cpp
CMapManagerTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += CMapReaderTest
check_PROGRAMS += CMapReaderTest
CMapReaderTest_SOURCES = CMapReaderTest.cpp
CMapReaderTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += CMapTest
check_PROGRAMS += CMapTest
CMapTest_SOURCES = CMapTest.cpp
CMapTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += ColorSpecialTest
check_PROGRAMS += ColorSpecialTest
ColorSpecialTest_SOURCES = ColorSpecialTest.cpp
ColorSpecialTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += ColorTest
check_PROGRAMS += ColorTest
ColorTest_SOURCES = ColorTest.cpp
ColorTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += CommandLineTest
check_PROGRAMS += CommandLineTest
CommandLineTest_SOURCES = CommandLineTest.cpp
CommandLineTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += CRC32Test
check_PROGRAMS += CRC32Test
CRC32Test_SOURCES = CRC32Test.cpp
CRC32Test_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += DependencyGraphTest
check_PROGRAMS += DependencyGraphTest
DependencyGraphTest_SOURCES = DependencyGraphTest.cpp
DependencyGraphTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += DirectoryTest
check_PROGRAMS += DirectoryTest
DirectoryTest_SOURCES = DirectoryTest.cpp
DirectoryTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += DvisvgmSpecialTest
check_PROGRAMS += DvisvgmSpecialTest
DvisvgmSpecialTest_SOURCES = DvisvgmSpecialTest.cpp
DvisvgmSpecialTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += EmSpecialTest
check_PROGRAMS += EmSpecialTest
EmSpecialTest_SOURCES = EmSpecialTest.cpp
EmSpecialTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += FileFinderTest
check_PROGRAMS += FileFinderTest
FileFinderTest_SOURCES = FileFinderTest.cpp
FileFinderTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += FilePathTest
check_PROGRAMS += FilePathTest
FilePathTest_SOURCES = FilePathTest.cpp
FilePathTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += FileSystemTest
check_PROGRAMS += FileSystemTest
FileSystemTest_SOURCES = FileSystemTest.cpp
FileSystemTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += FontManagerTest
check_PROGRAMS += FontManagerTest
FontManagerTest_SOURCES = FontManagerTest.cpp
FontManagerTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += FontMapTest
check_PROGRAMS += FontMapTest
FontMapTest_SOURCES = FontMapTest.cpp
FontMapTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += GFGlyphTracerTest
check_PROGRAMS += GFGlyphTracerTest
GFGlyphTracerTest_SOURCES = GFGlyphTracerTest.cpp
GFGlyphTracerTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += GFReaderTest
check_PROGRAMS += GFReaderTest
GFReaderTest_SOURCES = GFReaderTest.cpp
GFReaderTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += GhostscriptTest
check_PROGRAMS += GhostscriptTest
GhostscriptTest_SOURCES = GhostscriptTest.cpp
GhostscriptTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += GraphicsPathTest
check_PROGRAMS += GraphicsPathTest
GraphicsPathTest_SOURCES = GraphicsPathTest.cpp
GraphicsPathTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += JFMReaderTest
check_PROGRAMS += JFMReaderTest
JFMReaderTest_SOURCES = JFMReaderTest.cpp
JFMReaderTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += LengthTest
check_PROGRAMS += LengthTest
LengthTest_SOURCES = LengthTest.cpp
LengthTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += MapLineTest
check_PROGRAMS += MapLineTest
MapLineTest_SOURCES = MapLineTest.cpp
MapLineTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += MatrixTest
check_PROGRAMS += MatrixTest
MatrixTest_SOURCES = MatrixTest.cpp
MatrixTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += MessageExceptionTest
check_PROGRAMS += MessageExceptionTest
MessageExceptionTest_SOURCES = MessageExceptionTest.cpp
MessageExceptionTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += PageRagesTest
check_PROGRAMS += PageRagesTest
PageRagesTest_SOURCES = PageRagesTest.cpp
PageRagesTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += PageSizeTest
check_PROGRAMS += PageSizeTest
PageSizeTest_SOURCES = PageSizeTest.cpp
PageSizeTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += PairTest
check_PROGRAMS += PairTest
PairTest_SOURCES = PairTest.cpp
PairTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += PSInterpreterTest
check_PROGRAMS += PSInterpreterTest
PSInterpreterTest_SOURCES = PSInterpreterTest.cpp
PSInterpreterTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += RangeMapTest
check_PROGRAMS += RangeMapTest
RangeMapTest_SOURCES = RangeMapTest.cpp
RangeMapTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += ShadingPatchTest
check_PROGRAMS += ShadingPatchTest
ShadingPatchTest_SOURCES = ShadingPatchTest.cpp
ShadingPatchTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += SplittedCharInputBufferTest
check_PROGRAMS += SplittedCharInputBufferTest
SplittedCharInputBufferTest_SOURCES = SplittedCharInputBufferTest.cpp
SplittedCharInputBufferTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += StreamInputBufferTest
check_PROGRAMS += StreamInputBufferTest
StreamInputBufferTest_SOURCES = StreamInputBufferTest.cpp
StreamInputBufferTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += StreamReaderTest
check_PROGRAMS += StreamReaderTest
StreamReaderTest_SOURCES = StreamReaderTest.cpp
StreamReaderTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += StreamWriterTest
check_PROGRAMS += StreamWriterTest
StreamWriterTest_SOURCES = StreamWriterTest.cpp
StreamWriterTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += SubfontTest
check_PROGRAMS += SubfontTest
SubfontTest_SOURCES = SubfontTest.cpp
SubfontTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += SVGOutputTest
check_PROGRAMS += SVGOutputTest
SVGOutputTest_SOURCES = SVGOutputTest.cpp
SVGOutputTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += TensorProductPatchTest
check_PROGRAMS += TensorProductPatchTest
TensorProductPatchTest_SOURCES = TensorProductPatchTest.cpp
TensorProductPatchTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += TFMReaderTest
check_PROGRAMS += TFMReaderTest
TFMReaderTest_SOURCES = TFMReaderTest.cpp
TFMReaderTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += ToUnicodeMapTest
check_PROGRAMS += ToUnicodeMapTest
ToUnicodeMapTest_SOURCES = ToUnicodeMapTest.cpp
ToUnicodeMapTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += TriangularPatchTest
check_PROGRAMS += TriangularPatchTest
TriangularPatchTest_SOURCES = TriangularPatchTest.cpp
TriangularPatchTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += UnicodeTest
check_PROGRAMS += UnicodeTest
UnicodeTest_SOURCES = UnicodeTest.cpp
UnicodeTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += UtilityTest
check_PROGRAMS += UtilityTest
UtilityTest_SOURCES = UtilityTest.cpp
UtilityTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += VectorIteratorTest
check_PROGRAMS += VectorIteratorTest
VectorIteratorTest_SOURCES = VectorIteratorTest.cpp
VectorIteratorTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += VectorStreamTest
check_PROGRAMS += VectorStreamTest
VectorStreamTest_SOURCES = VectorStreamTest.cpp
VectorStreamTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += XMLNodeTest
check_PROGRAMS += XMLNodeTest
XMLNodeTest_SOURCES = XMLNodeTest.cpp
XMLNodeTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

TESTS += XMLStringTest
check_PROGRAMS += XMLStringTest
XMLStringTest_SOURCES = XMLStringTest.cpp
XMLStringTest_LDADD = ../src/libdvisvgm.a ../libs/xxHash/libxxhash.a -L../potracelib -lpotrace -lfreetype -lgtest -lgtest_main -lpthread $(CODE_COVERAGE_LDFLAGS)

EXTRA_DIST = check-conv genhashcheck.py normalize.xsl
TESTS += check-conv

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno hashcheck.cpp
