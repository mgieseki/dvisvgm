man_MANS = dvisvgm.1
EXTRA_DIST = $(man_MANS)

pdf:
	groff -Tps -man dvisvgm.1 >dvisvgm.ps.tmp
	psselect -p2- dvisvgm.ps.tmp dvisvgm.ps
	ps2pdf dvisvgm.ps dvisvgm.pdf
	rm -f dvisvgm.ps.tmp dvisvgm.ps

pdf-dvi:
	groff -Tdvi -man dvisvgm.1 >dvisvgm.dvi
	dvipdfm -s 2- dvisvgm
	rm -f dvisvgm.dvi

dvi:
	groff -Tdvi -man dvisvgm.1 >dvisvgm.dvi

html:
	groff -Thtml -man dvisvgm.1 >dvisvgm.html

clean:
	rm -f dvisvgm.pdf dvisvgm.html
