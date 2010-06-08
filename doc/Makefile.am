man_MANS = dvisvgm.1
EXTRA_DIST = $(man_MANS)

dvisvgm.1: dvisvgm.txt
	if [ `type -p a2x` ]; then \
		a2x -dmanpage -fmanpage -L $<; \
	fi

dvisvgm.pdf: dvisvgm.1
	man -t ./$< | ps2pdf - $@

dvi-pdf: dvisvgm.dvi
	dvipdfm $<
	rm -f $<

dvisvgm.dvi: dvisvgm.1
	groff -Tdvi -mandoc ./$< >$@

dvisvgm.html: dvisvgm.txt
	asciidoc --unsafe -dmanpage $<

html: dvisvgm.html
man: dvisvgm.1
pdf: dvisvgm.pdf

clean:
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.xml dvisvgm.1 dvisvgm.txt

distclean-local:
	rm -f dvisvgm.1
