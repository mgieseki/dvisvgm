## This file is part of dvisvgm
## Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = COPYING LGPL-2.1.txt
SUBDIRS = clipper xxHash src tests doc

if CODE_COVERAGE_ENABLED
lcov_dir=$(top_builddir)/lcov
lcov_file=$(lcov_dir)/lcov.info

lcov-report:
	@mkdir -p $(lcov_dir)
	lcov --capture --directory . --no-external -o $(lcov_file)
	lcov --remove $(lcov_file) "gzstream.*" -o $(lcov_file)
	genhtml --title "dvisvgm" --num-spaces 2 --legend -o $(lcov_dir) $(lcov_file)

lcov-clean:
	@rm -rf $(lcov_dir)
	@find . -name "*.gcda" -exec rm {} \;
	lcov --zerocounters --directory .

coverage: lcov-clean check lcov-report
endif
