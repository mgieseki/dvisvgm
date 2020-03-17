## This file is part of dvisvgm
## Copyright (C) 2005-2020 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

# Don't try to install the manpage file if it can't be built due to missing utilities.
# However, the distribution tarball should always contain a recent manpage. We ensure
# this by adding a dist-hook rule below.

man_MANS = dvisvgm.1

EXTRA_DIST = $(man_MANS) \
	conf-dblatex-man.xsl \
	conf-dblatex-pdf.xsl \
	dvisvgm.txt.in \
	generate-dvisvgm-sty.xsl \
	tweak-dblatex-pdf.xsl \
	tweak-dblatex-tex.py \
	tweak-db-refentry.xsl

if BUILD_MANPAGE

CONF_DBLATEX_MAN  = $(dvisvgm_srcdir)/doc/conf-dblatex-man.xsl
CONF_DBLATEX_PDF  = $(dvisvgm_srcdir)/doc/conf-dblatex-pdf.xsl
TWEAK_DBLATEX_PDF = $(dvisvgm_srcdir)/doc/tweak-dblatex-pdf.xsl
TWEAK_DBLATEX_TEX = $(dvisvgm_srcdir)/doc/tweak-dblatex-tex.py
TWEAK_DB_ARTICLE  = $(dvisvgm_srcdir)/doc/tweak-db-article.xsl
TWEAK_DB_REFENTRY = $(dvisvgm_srcdir)/doc/tweak-db-refentry.xsl
GEN_DBLATEX_STY   = $(dvisvgm_srcdir)/doc/generate-dvisvgm-sty.xsl

DB_VARS = man.endnotes.list.enabled=0 man.endnotes.are.numbered=0 man.authors.section.enabled=0

dvisvgm.1: dvisvgm-man.xml $(CONF_DBLATEX_MAN)
	xmlto -m $(CONF_DBLATEX_MAN) $(addprefix --stringparam , $(DB_VARS)) man $<

dvisvgm.pdf: dvisvgm-man.xml dvisvgm.sty $(CONF_DBLATEX_PDF) $(TWEAK_DBLATEX_PDF) $(TWEAK_DBLATEX_TEX)
	mv $< $<.tmp
	xsltproc -o $< $(TWEAK_DBLATEX_PDF) $<.tmp
	dblatex -bxetex --texstyle=./dvisvgm.sty -p $(CONF_DBLATEX_PDF) -r plugin:$(basename $(TWEAK_DBLATEX_TEX)) $<
	mv $<.tmp $<
	mv $(basename $<).pdf $@

dvi-pdf: dvisvgm.dvi
	dvipdfm $<
	rm -f $<

dvisvgm.dvi: dvisvgm.1
	groff -Tdvi -mandoc ./$< >$@

dvisvgm.html: dvisvgm-man.xml db2html.xsl
	xsltproc -o $@ db2html.xsl $<

dvisvgm.md: dvisvgm-man.xml db2md.xsl
	xsltproc -o $@ db2md.xsl $<

dvisvgm.epub: dvisvgm-article.xml
	a2x -darticle -fepub -L --icons --icons-dir=. $<
	mv $(basename $<).epub $@

.SECONDARY: dvisvgm-article.xml dvisvgm-man.xml dvisvgm.sty

dvisvgm-man.xml: dvisvgm.txt $(TWEAK_DB_REFENTRY)
	asciidoc -a icons -a 'iconsdir=.' -a badges -a 'revnumber=@VERSION@' --unsafe -bdocbook -dmanpage -o $@.tmp $<
	xsltproc -o $@ $(TWEAK_DB_REFENTRY) $@.tmp
	rm $@.tmp

dvisvgm-article.xml: dvisvgm.txt $(TWEAK_DB_ARTICLE)
	asciidoc -a icons -a 'iconsdir=.' -a badges -a 'revnumber=@VERSION@' --unsafe -bdocbook -darticle -o $@.tmp $<
	xsltproc -o $@ $(TWEAK_DB_ARTICLE) $@.tmp
	rm $@.tmp
	sed -i "s/{VERSION}/@VERSION@/" $@

dvisvgm.sty: dvisvgm-man.xml $(GEN_DBLATEX_STY)
	xsltproc -o dvisvgm.sty $(GEN_DBLATEX_STY) $<

dvisvgm.txt: dvisvgm.txt.in ../configure.ac
	sed -e 's/@VERSION[@]/@VERSION@/g' -e 's/@PACKAGE_BUGREPORT[@]/@PACKAGE_BUGREPORT@/g' $< >$@
	if [ $< -nt ../configure.ac ]; \
	then touch -r $< $@; \
	else touch -r ../configure.ac $@; \
	fi

epub: dvisvgm.epub
html: dvisvgm.html
man: dvisvgm.1
md: dvisvgm.md
pdf: dvisvgm.pdf

endif

clean:
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.md dvisvgm-man.xml dvisvgm-article.xml dvisvgm.txt dvisvgm.sty dvisvgm.dvi dvisvgm.epub

distclean-local:
	rm -f dvisvgm.pdf dvisvgm.html dvisvgm.md dvisvgm-man.xml dvisvgm-article.xml dvisvgm.txt dvisvgm.sty dvisvgm.dvi dvisvgm.epub dvisvgm.1

if !BUILD_MANPAGE
# Ensure that distribution tarballs always contain a recent manpage, i.e.
# let "make dist" and "make distcheck" fail if dvisvgm.1 can't be built.
dist-hook:
	@echo "----------------------------------------------------------------"; \
	echo  "manpage file dvisvgm.1 could not be built because one or more of"; \
	echo  "the following utilities are missing: asciidoc, xmlto, xsltproc  "; \
	echo  "----------------------------------------------------------------"; \
	false;
endif
