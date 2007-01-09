#!/usr/bin/perl
use File::Copy;

if ($ARGV[0] eq "all") {
	chdir "src";
	print `make -f Makefile.orig clean`;
	print `make -f Makefile.orig OPT=-Os`;
	chdir "..";
}

open CONF, "configure.ac";
@conf = <CONF>;
close CONV;
$_ = join('', @conf);
/AC_INIT\(\s*(.+?)\s*,\s*(.+?)\s*, .*?\)/;
$progname = $1;
$version  = $2;

copy("vc/release/$progname.exe", "./$progname.exe");
#`strip $progname.exe`;

copy("doc/$progname.1", "./$progname.1");
`groff -t -e -mandoc -Tps $progname.1 >$progname.ps-tmp`;
`psselect -p"2-" $progname.ps-tmp >$progname.ps`;
`ps2pdf $progname.ps`;
unlink "$progname-$version-win32.zip";
`zip $progname-$version-win32.zip $progname.exe $progname.pdf README COPYING`;
unlink "$progname.exe", "$progname.1", "$progname.pdf";
