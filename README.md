_dvisvgm_ &ndash; A fast DVI to SVG converter
=============================================

Description
-----------

_dvisvgm_ is a utility for TeX/LaTeX users. It converts
[DVI files](http://en.wikipedia.org/wiki/Device_independent_file_format) to the
XML-based scalable vector graphics format [SVG](http://www.w3.org/TR/SVG).
The latest releases support classic DVI files (format 2) as well as DVI output
created by [pTeX](http://www.ascii.co.jp/pb/ptex) in vertical mode (format 3),
or [XeTeX](http://scripts.sil.org/xetex) (format 5, also knows as XDV).


Dependencies
------------

_dvisvgm_ relies on the following free libraries:

* [Clipper](http://www.angusj.com/delphi/clipper.php)  
  To compute the insersection of two curved paths, _dvisvgm_ flattens the paths to
  polygons, intersects them using a slightly modified version of Clipper, and reconstructs
  the curves afterwards.

* [FreeType 2](http://www.freetype.org)  
  This library is used to extract the glyph outlines from vector fonts (PFB, OTF, TTF).

* [Ghostscript](http://pages.cs.wisc.edu/~ghost)  
  The PostScript special handler requires the Ghostscript library libgs.so (Linux)
  or gsdll32.dll/gsdll64.dll (Windows) to be installed. If the configure script
  finds the corresponding Ghostscript development files on the system, it
  directly links against libgs.so. Otherwise, the library is looked up during
  runtime and the PostScript support is enabled only if libgs.so could be found.
  Due to incompatible changes of the Ghostscript API, _dvisvgm_ requires
  Ghostscript 8.31 or later.

* [Kpathsea](https://tug.org/kpathsea)  
  This library is part of the Web2C package and is usually installed in
  conjunction with a TeX distribution. Kpathsea provides functions for searching
  files in the large texmf tree.

* [potracelib](http://potrace.sourceforge.net)  
  Peter Selinger's bitmap tracing library is utilized to vectorize Metafont's
  bitmap output.

* [zlib](http://www.zlib.org)  
  Compressed SVG files are produced by using functions of this library and the
  [Gzstream wrapper classes](http://www.cs.unc.edu/Research/compgeom/gzstream)
  which are licensed under LGPL version 2.1 (or optionally any later version)
  and should be included with this package.

If you want to build and run the testsuite located in the _tests_
subdirectory, you also need the [googletest](http://code.google.com/p/googletest)
libraries.


Compilation
-----------

Quick installation info:
* type `./configure`
* type `make`
* type `make install` as root (or `sudo make install`)

For generic configuration instructions see the file INSTALL.

The configure script should recognize all necessary parameters.
If a library is installed but not detected, specify its location
as commandline parameter of configure, e.g.
`./configure --with-freetype=/usr/local/freetype`
(all available options can be displayed with `./configure --help`)

The tracer module calls Metafont in case of lacking TFM or PFB files
via a system call. Please ensure that Metafont is installed and
reachable through the search path.

If you don't like compiling the sources yourself, you can download
pre-compiled binaries for Windows and MiKTeX from the
[project website](http://dvisvgm.sourceforge.net) instead.


Usage
-----
For detailed information about the command-line interface and all available
options of _dvisvgm_, see the [manual page](http://dvisvgm.sourceforge.net/Manpage).


Additional Information
----------------------

This package is available from [CTAN](http://ctan.org), and can be downloaded
from the [project website](http://dvisvgm.sourceforge.net) as well. There you
can find the sources, pre-compiled binaries, the manual page, FAQs and further
information about the converter and related topics.

If you've found a bug, please let me know. You can either send me an email or
preferably use the [bug tracker at Launchpad](https://launchpad.net/dvisvgm).


Copyright
---------

Copyright (C) 2005-2014 [Martin Gieseking](mailto:martin.gieseking@uos.de)

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, see <http://www.gnu.org/licenses/>.

See the file COPYING for details.

