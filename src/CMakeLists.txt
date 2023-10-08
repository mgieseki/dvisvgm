set(SOURCES
    BasicDVIReader.cpp
    Bezier.cpp
    BgColorSpecialHandler.cpp
    Bitmap.cpp
    BoundingBox.cpp
    Calculator.cpp
    CharMapID.cpp
    CLCommandLine.cpp
    CMap.cpp
    CMapManager.cpp
    CMapReader.cpp
    Color.cpp
    ColorSpecialHandler.cpp
    Directory.cpp
    DLLoader.cpp
    DVIReader.cpp
    DvisvgmSpecialHandler.cpp
    DVIToSVG.cpp
    DVIToSVGActions.cpp
    EllipticalArc.cpp
    EmSpecialHandler.cpp
    EncFile.cpp
    EPSFile.cpp
    FileFinder.cpp
    FilePath.cpp
    FileSystem.cpp
    Font.cpp
    FontCache.cpp
    FontEncoding.cpp
    FontEngine.cpp
    FontManager.cpp
    FontMap.cpp
    FontMetrics.cpp
    fonts/Base14Fonts.cpp
    fonts/Dingbats.cff.cpp
    fonts/NimbusMonoPS-Bold.cff.cpp
    fonts/NimbusMonoPS-BoldItalic.cff.cpp
    fonts/NimbusMonoPS-Italic.cff.cpp
    fonts/NimbusMonoPS-Regular.cff.cpp
    fonts/NimbusRoman-Bold.cff.cpp
    fonts/NimbusRoman-BoldItalic.cff.cpp
    fonts/NimbusRoman-Italic.cff.cpp
    fonts/NimbusRoman-Regular.cff.cpp
    fonts/NimbusSans-Bold.cff.cpp
    fonts/NimbusSans-BoldItalic.cff.cpp
    fonts/NimbusSans-Italic.cff.cpp
    fonts/NimbusSans-Regular.cff.cpp
    fonts/StandardSymbolsPS.cff.cpp
    FontWriter.cpp
    GFGlyphTracer.cpp
    GFReader.cpp
    GFTracer.cpp
    Ghostscript.cpp
    HashFunction.cpp
    HtmlSpecialHandler.cpp
    HyperlinkManager.cpp
    ImageToSVG.cpp
    InputBuffer.cpp
    InputReader.cpp
    JFM.cpp
    Length.cpp
    MapLine.cpp
    Matrix.cpp
    Message.cpp
    MetafontWrapper.cpp
    NoPsSpecialHandler.cpp
    OFM.cpp
    Opacity.cpp
    optimizer/AttributeExtractor.cpp
    optimizer/ClipPathReassigner.cpp
    optimizer/GroupCollapser.cpp
    optimizer/RedundantElementRemover.cpp
    optimizer/SVGOptimizer.cpp
    optimizer/TextSimplifier.cpp
    optimizer/TransformSimplifier.cpp
    optimizer/WSNodeRemover.cpp
    PageRanges.cpp
    PageSize.cpp
    PapersizeSpecialHandler.cpp
    PathClipper.cpp
    PDFHandler.cpp
    PDFParser.cpp
    PdfSpecialHandler.cpp
    PDFToSVG.cpp
    PreScanDVIReader.cpp
    Process.cpp
    psdefs.cpp
    PSInterpreter.cpp
    PSPattern.cpp
    PSPreviewHandler.cpp
    PsSpecialHandler.cpp
    RangeMap.cpp
    ShadingPatch.cpp
    SignalHandler.cpp
    SourceInput.cpp
    SpecialManager.cpp
    StreamReader.cpp
    StreamWriter.cpp
    Subfont.cpp
    SVGCharHandler.cpp
    SVGCharHandlerFactory.cpp
    SVGCharPathHandler.cpp
    SVGCharTspanTextHandler.cpp
    SVGElement.cpp
    SVGOutput.cpp
    SVGSingleCharTextHandler.cpp
    SVGTree.cpp
    System.cpp
    TensorProductPatch.cpp
    Terminal.cpp
    TFM.cpp
    ToUnicodeMap.cpp
    TpicSpecialHandler.cpp
    TriangularPatch.cpp
    ttf/CmapTable.cpp
    ttf/GlyfTable.cpp
    ttf/HeadTable.cpp
    ttf/HheaTable.cpp
    ttf/HmtxTable.cpp
    ttf/MaxpTable.cpp
    ttf/NameTable.cpp
    ttf/OS2Table.cpp
    ttf/PostTable.cpp
    ttf/TTFAutohint.cpp
    ttf/TTFTable.cpp
    ttf/TTFWriter.cpp
    ttf/VheaTable.cpp
    ttf/VmtxTable.cpp
    Unicode.cpp
    utility.cpp
    VFReader.cpp
    XMLDocument.cpp
    XMLNode.cpp
    XMLParser.cpp
    XMLString.cpp
)

# ===== dvisvgm library =====
if (BUILD_SHARED)
    add_library(dvisvgm SHARED ${SOURCES})
else()
    add_library(dvisvgm ${SOURCES})
endif()

target_compile_options(dvisvgm PRIVATE -Wall)
target_link_libraries(dvisvgm PUBLIC boost brotli clipper freetype2 kpathsea md5 mpark_variant potrace woff2 xxhash z)
target_include_directories(dvisvgm INTERFACE .)
target_compile_features(dvisvgm PUBLIC cxx_std_11)

include(CheckCXXSourceCompiles)
check_cxx_source_compiles(
    "#include <signal.h>
     struct sigaction test;
     int main() {}"
    HAVE_SIGACTION)
check_cxx_source_compiles(
    "#include <sys/stat.h>
     void f() { umask(0); }
     int main() {}"
    HAVE_UMASK)
check_cxx_source_compiles(
    "int main() { __builtin_clz(1); }"
    HAVE___BUILTIN_CLZ)

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CXX(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE_CXX(sys/timeb.h HAVE_SYS_TIMEB_H)
CHECK_INCLUDE_FILE_CXX(termios.h HAVE_TERMIOS_H)

configure_file(config.h.in config.h)
target_include_directories(dvisvgm PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_compile_definitions(dvisvgm PUBLIC DVISVGM_CMAKE_BUILD)


# ===== dvisvgm executable =====
add_executable(dvisvgm_exe dvisvgm.cpp)
target_link_libraries(dvisvgm_exe PRIVATE dvisvgm)
set_target_properties(dvisvgm_exe PROPERTIES OUTPUT_NAME "dvisvgm")

install(TARGETS dvisvgm_exe)


# ===== texmf.cnf =====
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    option(TEXMF_CNF_PATH "Path to texmf.cnf file")
    option(TEX_DISTRIBUTION_PATH "Path to TeX Live distribution (i. e. /usr/share/texmf-dist)")

    # Try to find determine paths automatically
    if (NOT TEXMF_CNF_PATH AND NOT TEX_DISTRIBUTION_PATH)
        execute_process(COMMAND which kpsewhich OUTPUT_VARIABLE kpsewhich_path)
        execute_process(COMMAND kpsewhich -format=cnf texmf.cnf OUTPUT_VARIABLE texmf_path)

        if (NOT kpsewhich_path STREQUAL "" AND NOT texmf_path STREQUAL "")
            string(STRIP "${texmf_path}" texmf_stripped_path)
            set(TEXMF_CNF_PATH "${texmf_stripped_path}" CACHE PATH "" FORCE)

            cmake_path(REMOVE_FILENAME kpsewhich_path OUTPUT_VARIABLE kpsewhich_folder)
            set(distribution_raw_path "${kpsewhich_folder}/../share/texmf-dist")
            cmake_path(NORMAL_PATH distribution_raw_path OUTPUT_VARIABLE distribution_path)

            set(TEX_DISTRIBUTION_PATH "${distribution_path}" CACHE PATH "" FORCE)
        endif()
    endif()

    if (TEXMF_CNF_PATH AND TEX_DISTRIBUTION_PATH)
        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/texmf.cnf"
            COMMAND cp "${TEXMF_CNF_PATH}" "${CMAKE_BINARY_DIR}/texmf.cnf"
            COMMAND sed -i "'s:$$SELFAUTODIR:${TEX_DISTRIBUTION_PATH}/../..:g'" "${CMAKE_BINARY_DIR}/texmf.cnf")
        add_custom_target(texmf_generation DEPENDS "${CMAKE_BINARY_DIR}/texmf.cnf")
        add_dependencies(dvisvgm texmf_generation)
    endif()
endif()
