## This file is part of dvisvgm
## Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>
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
	echo '[ /Author (Martin Gieseking) /Title (dvisvgm manual page) /Subject (manual page of dvisvgm @VERSION@) /DOCINFO pdfmark' >tmp.ps
	man -t ./$< >>tmp.ps
	ps2pdf tmp.ps $@
	rm -f tmp.ps

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
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.xml dvisvgm.txt

distclean-local:
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.xml dvisvgm.txt dvisvgm.1
