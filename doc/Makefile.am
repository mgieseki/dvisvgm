man_MANS = dvisvgm.1
EXTRA_DIST = $(man_MANS)

dvisvgm.1: dvisvgm.txt
	a2x -dmanpage -fmanpage dvisvgm.txt

dvisvgm.pdf: dvisvgm.1
	groff -Tps -man dvisvgm.1 >dvisvgm.ps
	ps2pdf dvisvgm.ps dvisvgm.pdf
	rm -f dvisvgm.ps

pdf-dvi: dvisvgm.dvi
	dvipdfm dvisvgm
	rm -f dvisvgm.dvi

dvisvgm.dvi: dvisvgm.1
	groff -Tdvi -man dvisvgm.1 >dvisvgm.dvi

dvisvgm.html: dvisvgm.txt
	asciidoc --unsafe -dmanpage dvisvgm.txt

html: dvisvgm.html
man: dvisvgm.1
pdf: dvisvgm.pdf

clean:
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.xml dvisvgm.1 dvisvgm.txt
