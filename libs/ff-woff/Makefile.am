noinst_LIBRARIES = libfontforge.a

libfontforge_a_SOURCES = \
	fontforge/PfEd.h \
	fontforge/alphabet.c \
	fontforge/asmfpst.c \
	fontforge/autohint.c \
	fontforge/char.c \
	fontforge/cjk.c \
	fontforge/configure-fontforge.h \
	fontforge/cvundoes.c \
	fontforge/dumppfa.c \
	fontforge/edgelist.h \
	fontforge/edgelist2.h \
	fontforge/encoding.c \
	fontforge/encoding.h \
	fontforge/fflocale.c \
	fontforge/fflocale.h \
	fontforge/fontforge-config.h \
	fontforge/fontforge.h \
	fontforge/fontforgevw.h \
	fontforge/fvfonts.c \
	fontforge/gwwiconv.c \
	fontforge/libffstamp.h \
	fontforge/lookups.c \
	fontforge/macbinary.c \
	fontforge/macenc.c \
	fontforge/mathconstants.c \
	fontforge/memory.c \
	fontforge/mm.c \
	fontforge/namehash.h \
	fontforge/namelist.c \
	fontforge/nouiutil.c \
	fontforge/nowakowskittfinstr.c \
	fontforge/parsepfa.c \
	fontforge/parsettf.c \
	fontforge/parsettfatt.c \
	fontforge/psfont.h \
	fontforge/psread.c \
	fontforge/pua.c \
	fontforge/sd.h \
	fontforge/sfd.c \
	fontforge/sfd1.c \
	fontforge/sfd1.h \
	fontforge/splinechar.c \
	fontforge/splinefont.c \
	fontforge/splinefont.h \
	fontforge/splineorder2.c \
	fontforge/splineoverlap.c \
	fontforge/splinerefigure.c \
	fontforge/splinesave.c \
	fontforge/splinesaveafm.c \
	fontforge/splineutil.c \
	fontforge/splineutil2.c \
	fontforge/start.c \
	fontforge/stemdb.c \
	fontforge/stemdb.h \
	fontforge/tables.h \
	fontforge/tmpfile2.cpp \
	fontforge/tottf.c \
	fontforge/tottfaat.c \
	fontforge/tottfgpos.c \
	fontforge/tottfvar.c \
	fontforge/ttf.h \
	fontforge/ttfinstrs.c \
	fontforge/ttfinstrs.h \
	fontforge/ttfspecial.c \
	fontforge/uiinterface.h \
	fontforge/unialt.c \
	fontforge/ustring.c \
	fontforge/utype.c \
	fontforge/woff.c \
	inc/basics.h \
	inc/chardata.h \
	inc/charset.h \
	inc/dlist.h \
	inc/ffintl.h \
	inc/gimage.h \
	inc/gnetwork.h \
	inc/gwwiconv.h \
	inc/ustring.h \
	inc/utype.h

EXTRA_DIST = LICENSE

AM_CFLAGS = -I$(srcdir)/inc -I$(srcdir)/fontforge $(ZLIB_CFLAGS)

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
