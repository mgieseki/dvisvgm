## This file is part of dvisvgm
## Copyright (C) 2005-2024 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

SUBDIRS = boost clipper md5 variant xxHash

if !TEXLIVE_BUILD
SUBDIRS += potrace
endif

if ENABLE_WOFF
SUBDIRS += brotli woff2
endif
