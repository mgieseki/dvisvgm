noinst_LTLIBRARIES = liboptimizer.la

liboptimizer_la_SOURCES = \
	AttributeExtractor.hpp      AttributeExtractor.cpp \
	ClipPathReassigner.hpp      ClipPathReassigner.cpp \
	DependencyGraph.hpp \
	GroupCollapser.hpp          GroupCollapser.cpp  \
	OptimizerModule.hpp \
	RedundantElementRemover.hpp RedundantElementRemover.cpp \
	SVGOptimizer.hpp            SVGOptimizer.cpp \
	TextSimplifier.hpp          TextSimplifier.cpp \
	TransformSimplifier.hpp     TransformSimplifier.cpp \
  	WSNodeRemover.hpp           WSNodeRemover.cpp

include ../../libs/defs.am

AM_CXXFLAGS = -I$(dvisvgm_srcdir)/libs/variant/include $(XXHASH_CFLAGS)

