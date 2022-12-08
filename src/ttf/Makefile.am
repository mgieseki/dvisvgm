noinst_LTLIBRARIES = libttf.la

libttf_la_SOURCES = \
	CmapTable.hpp   CmapTable.cpp \
	GlyfTable.hpp   GlyfTable.cpp \
	HeadTable.hpp   HeadTable.cpp \
	HheaTable.hpp   HheaTable.cpp \
	HmtxTable.hpp   HmtxTable.cpp \
	LocaTable.hpp   MaxpTable.cpp \
	MaxpTable.hpp   NameTable.cpp \
	NameTable.hpp   OS2Table.cpp \
	OS2Table.hpp    PostTable.cpp \
	PostTable.hpp \
	TTFAutohint.hpp TTFAutohint.cpp \
	TTFTable.hpp    TTFTable.cpp \
	TTFWriter.hpp   TTFWriter.cpp \
	VheaTable.hpp   VheaTable.cpp \
	VmtxTable.hpp   VmtxTable.cpp

include ../../libs/defs.am

AM_CXXFLAGS = \
	-I$(dvisvgm_srcdir)/libs/boost \
	-I$(dvisvgm_srcdir)/libs/variant/include \
	$(BROTLI_CFLAGS) $(WOFF2_CFLAGS)
