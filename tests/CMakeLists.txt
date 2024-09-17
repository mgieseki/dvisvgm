set(SOURCES
    BezierTest.cpp
    BitmapTest.cpp
    BoundingBoxTest.cpp
    CMapManagerTest.cpp
    CMapReaderTest.cpp
    CMapTest.cpp
    CalculatorTest.cpp
    ColorSpecialTest.cpp
    ColorTest.cpp
    CommandLineTest.cpp
    DVIReaderTest.cpp
    DependencyGraphTest.cpp
    DirectoryTest.cpp
    DvisvgmSpecialTest.cpp
    EllipticalArcTest.cpp
    EmSpecialTest.cpp
    FileFinderTest.cpp
    FilePathTest.cpp
    FileSystemTest.cpp
    FontCacheTest.cpp
    FontManagerTest.cpp
    FontMapTest.cpp
    GFGlyphTracerTest.cpp
    GFReaderTest.cpp
    GhostscriptTest.cpp
    GraphicsPathParserTest.cpp
    GraphicsPathTest.cpp
    HashFunctionTest.cpp
    JFMReaderTest.cpp
    LengthTest.cpp
    MapLineTest.cpp
    MatrixTest.cpp
    MessageExceptionTest.cpp
    OFMReaderTest.cpp
    PDFParserTest.cpp
    PSInterpreterTest.cpp
    PageRagesTest.cpp
    PageSizeTest.cpp
    PairTest.cpp
    PapersizeSpecialTest.cpp
    RangeMapTest.cpp
    SVGOutputTest.cpp
    ShadingPatchTest.cpp
    SpecialManagerTest.cpp
    SplittedCharInputBufferTest.cpp
    StreamInputBufferTest.cpp
    StreamReaderTest.cpp
    StreamWriterTest.cpp
    StringMatcherTest.cpp
    SubfontTest.cpp
    TFMReaderTest.cpp
    TensorProductPatchTest.cpp
    ToUnicodeMapTest.cpp
    TpicSpecialTest.cpp
    TriangularPatchTest.cpp
    UnicodeTest.cpp
    UtilityTest.cpp
    VectorIteratorTest.cpp
    VectorStreamTest.cpp
    XMLNodeTest.cpp
    XMLStringTest.cpp
    testmain.cpp
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

add_executable(UnitTests ${SOURCES})
target_link_libraries(UnitTests PRIVATE dvisvgm GTest::gtest)
# Modern googletest requires at least C++17
target_compile_features(UnitTests PRIVATE cxx_std_17)
gtest_discover_tests(UnitTests)

target_compile_definitions(UnitTests PRIVATE SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")
