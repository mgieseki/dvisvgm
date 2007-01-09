$VCDIR = '"c:/Programme/Microsoft Visual Studio 8/vc"';
$_ = `kpsewhich latex.exe`;
s#(.*)/latex.*#\1#;
chomp;
$path = $_;
@dlls = ('MiKTeX-core-3', 'MiKTeX-kpseweb2cemu-3', 'MiKTeX-app-1');
#system "$VCDIR/vcvarsall.bat";
foreach (@dlls) {
	system "pexports $path/$_.dll > $_.def";
	system "link /lib /def:$_.def";
}
