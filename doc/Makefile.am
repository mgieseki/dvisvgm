man_MANS = dvisvgm.1
EXTRA_DIST = $(man_MANS)

dvisvgm.1: dvisvgm.txt
	if test -f buildman; then \
		a2x -dmanpage -fmanpage dvisvgm.txt; \
		sed -i '/^\.SH "Notes"/{N;N;N;N;d}' dvisvgm.1; \
		sed -i '/^\.nr an-break-flag 1/{N;N;d}' dvisvgm.1; \
		sed -i "s/\\\\'/'/g" dvisvgm.1; \
	fi

dvisvgm.pdf: dvisvgm.1
	man -l -Tps dvisvgm.1 >dvisvgm.ps
	ps2pdf dvisvgm.ps dvisvgm.pdf
	rm -f dvisvgm.ps

dvi-pdf: dvisvgm.dvi
	dvipdfm dvisvgm
	rm -f dvisvgm.dvi

dvisvgm.dvi: dvisvgm.1
	man -l -Tdvi dvisvgm.1 >dvisvgm.dvi

dvisvgm.html: dvisvgm.txt
	asciidoc --unsafe -dmanpage dvisvgm.txt

html: dvisvgm.html
man: dvisvgm.1
pdf: dvisvgm.pdf

clean:
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.xml dvisvgm.1 dvisvgm.txt

distclean-local:
	rm -f dvisvgm.1
