## This file is part of dvisvgm
## Copyright (C) 2005-2011 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

man_MANS = dvisvgm.1
EXTRA_DIST = $(man_MANS)

dvisvgm.1: dvisvgm.txt
	if [ `type -p a2x` ]; then \
		a2x -dmanpage -fmanpage -L -a icons -a 'iconsdir=.' $<; \
		rm -f dvisvgm.xml; \
	fi

dvisvgm.pdf: dvisvgm.1
	man -t ./$< | ps2pdf - $@

dvi-pdf: dvisvgm.dvi
	dvipdfm $<
	rm -f $<

dvisvgm.dvi: dvisvgm.1
	groff -Tdvi -mandoc ./$< >$@

dvisvgm.html: dvisvgm.txt
	asciidoc -a icons -a 'iconsdir=.' -a badges -a 'revnumber=@VERSION@' --unsafe -dmanpage $<

html: dvisvgm.html
man: dvisvgm.1
pdf: dvisvgm.pdf

clean:
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.xml dvisvgm.1 dvisvgm.txt

distclean-local:
	rm -f dvisvgm.1
