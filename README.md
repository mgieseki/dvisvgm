_dvisvgm_ &ndash; A fast DVI to SVG converter
=============================================
[![C/C++ CI](https://github.com/mgieseki/dvisvgm/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/mgieseki/dvisvgm/actions/workflows/c-cpp.yml)
[![Build Status](https://ci.appveyor.com/api/projects/status/0rbkw88js1on4g2u/branch/master?svg=true)](https://ci.appveyor.com/project/mgieseki/dvisvgm/branch/master)
[![Copr Status](https://copr.fedorainfracloud.org/coprs/mgieseki/dvisvgm/package/dvisvgm/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/mgieseki/dvisvgm/package/dvisvgm)
[![Code Status](https://scan.coverity.com/projects/1099/badge.svg)](https://scan.coverity.com/projects/1099)
[![License](https://img.shields.io/:license-GPL%20v3+-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.en.html)
[![Releases](https://img.shields.io/github/release/mgieseki/dvisvgm.svg)](https://github.com/mgieseki/dvisvgm/releases)

Description
-----------

_dvisvgm_ is a utility for TeX/LaTeX users. It converts
[DVI](https://en.wikipedia.org/wiki/Device_independent_file_format),
[EPS](https://en.wikipedia.org/wiki/Encapsulated_PostScript), and
[PDF](https://en.wikipedia.org/wiki/PDF) files to the
XML-based scalable vector graphics format [SVG](https://www.w3.org/TR/SVG).
The latest releases support standard DVI files (version 2) as well as DVI output
created by [pTeX](https://ctan.org/pkg/ptex) in vertical mode (version 3),
and [XeTeX](http://scripts.sil.org/xetex) (versions 5 to 7, also known as XDV).

To get an impression of the conversion results, have a look at the
[examples](https://dvisvgm.de/Examples) created with _dvisvgm_.

Short overview of the main features:

* Complete font support including [virtual fonts](https://texfaq.org/FAQ-virtualfonts), evaluation of [font encodings](https://texfaq.org/FAQ-whatenc), CMap files, sub-font definitions and font maps.
* Glyph outlines of all required fonts are embedded into the generated SVG files.
* The font data can be embedded in SVG, TrueType, WOFF, or WOFF2 format.
* Glyph outlines of fonts that are not available in a vector format are generated on-the-fly by vectorizing [METAFONT](https://en.wikipedia.org/wiki/Metafont)'s bitmap output.
* _dvisvgm_ allows to replace [font elements](https://www.w3.org/TR/SVG/fonts.html) by [paths](https://www.w3.org/TR/SVG/paths.html) so that applications without SVG font support are enabled to render dvisvgm's output properly.
* Computes tight bounding boxes for the generated graphics, but supports common paper formats and arbitrary user-defined sizes as well.
* Option `--eps` allows to convert [EPS](https://en.wikipedia.org/wiki/Encapsulated_PostScript) files to SVG.
* Option `--pdf` allows to convert [PDF](https://en.wikipedia.org/wiki/PDF) documents to SVG.
* [Intersections of clipping paths](https://dvisvgm.de/Clipping) can be computed directly instead of delegating this task to the SVG renderer which increases the compatibility of the generated SVG files.
* Approximates PostScript [color gradient fills](https://dvisvgm.de/Gradients) not directly supported by SVG 1.1.
* The generated SVG structure and data can be improved with the built-in [SVG optimizer](https://dvisvgm.de/Manpage/#opt-optimize).
* Optionally creates compressed [SVGZ](https://en.wikipedia.org/wiki/Scalable_Vector_Graphics#Compression) files.
* Provides options for applying page transformations, like translation, rotation, scaling, and skewing.
* Evaluates several [specials](https://texfaq.org/FAQ-specials) including color, emTeX, tpic, hyperref/HyperTeX, papersize, PDF hyperlink and mapfile, as well as PostScript specials.
* The converter was successfully tested on various Linux ([TeX Live](https://www.tug.org/texlive)) and Windows ([MiKTeX](https://www.miktex.org), TeX Live) systems.
* _dvisvgm_ has been added to [TeX Live](https://www.tug.org/texlive), [MiKTeX](https://www.miktex.org), and [MacPorts](https://www.macports.org). It is therefore available for a wide range of operating systems.


Usage
-----
For detailed information about the command-line interface and all available
options of _dvisvgm_, see the [manual page](https://dvisvgm.de/Manpage).


Dependencies
------------

_dvisvgm_ relies on the following free libraries:

* [Clipper](http://www.angusj.com/delphi/clipper.php)  
  To compute the intersection of two curved paths, _dvisvgm_ flattens the paths to
  polygons, intersects them using a slightly modified version of Clipper, and reconstructs
  the curves afterwards. A slightly modified version of Clipper is bundled with the sources.

* [FreeType 2](https://www.freetype.org)  
  This library is used to extract the glyph outlines from vector fonts (PFB, OTF, TTF).

* [Ghostscript](https://www.ghostscript.com)  
  The PostScript special handler requires the Ghostscript shared library `libgs.so.N` (Linux)
  or `gsdll32.dll`/`gsdll64.dll` (Windows) to be installed. If the configure script
  finds the corresponding Ghostscript development files on the system, it
  directly links against `libgs.so.N`, where _N_ is the library's ABI version.
  Otherwise, the library is looked up during runtime, and the PostScript support is
  enabled only if the shared library can be found. Due to incompatible changes of the
  Ghostscript API, _dvisvgm_ requires Ghostscript 8.31 or later.

* [Kpathsea](https://tug.org/kpathsea)  
  This library is part of the Web2C package and is usually installed in
  conjunction with a TeX distribution. Kpathsea provides functions for searching
  files in the large `texmf` tree. Please ensure that you use the kpathsea version
  coming with or matching your TeX system.

* [potracelib](http://potrace.sourceforge.net)  
  Peter Selinger's bitmap tracing library is utilized to vectorize Metafont's
  bitmap output. It's also bundled with the _dvisvgm_ sources.

* [ttfautohint](https://www.freetype.org/ttfautohint)  
  _dvisvgm_ can optionally be built with ttfautohint support to allow for autohinting the
  generated TrueType, WOFF, and WOFF2 fonts. Since this is still an experimental feature,
  ttfautohint support is disabled by default. To enable it, run `configure` with option
  `--with-ttfautohint`.

* [woff2](https://github.com/google/woff2) and [brotli](https://github.com/google/brotli)  
  These Google libraries are bundled with the _dvisvgm_ sources. They are used
  to create WOFF2 files from TrueType fonts.

* [xxHash](https://github.com/Cyan4973/xxHash)  
  The xxHash library provides a fast hash algorithm. _dvisvgm_ uses it to create
  unique integer values from PostScript character names in order to store them in
  a compact way together with their Unicode points. A recent version of xxHash is
  bundled with the _dvisvgm_ sources.

* [zlib](http://www.zlib.org)  
  This library is required to create compressed [SVGZ](https://en.wikipedia.org/wiki/Scalable_Vector_Graphics#Compression) files.


Building dvisvgm from source
----------------------------

_dvisvgm_ is written in C++11 ([ISO C++ standard 2011](http://www.iso.org/iso/catalogue_detail.htm?csnumber=50372))
and requires an appropriate compiler, like [GCC](https://gcc.gnu.org) 4.9, [Clang](https://clang.llvm.org) 3.3,
or any later version. The configure script checks for the availability of C++11 support and
adds the proper compiler options if necessary.

Quick installation info:

* ensure that the development packages of all libraries, listed above, are installed
* type `autoreconf -fi` if `configure` is not present in the _dvisvgm_ root folder
* either type `./configure` if you don't want to use the bundled libraries _brotli_, _potrace_, _woff2_, and _xxHash_ but link
against the corresponding system libraries,
* or type `./configure --enable-bundled-libs` in order to build and statically link the bundled libraries.
* type `make`
* type `make install` as root (or `sudo make install`)

For generic configuration instructions see the file INSTALL.

The configure script should recognize all necessary parameters.
If a library is installed but not detected, specify its location
as command-line parameter of configure, e.g.
`./configure --with-freetype=/usr/local/freetype`
(all available options can be displayed with `./configure --help`)

The tracer module calls Metafont in case of lacking TFM or PFB files
via a system call. Please ensure that Metafont is installed and
reachable through the system's search path.

If you don't like compiling the sources yourself, you can download
pre-compiled binaries for Windows and MiKTeX from the
[project website](https://dvisvgm.de) instead.


Additional Information
----------------------

This package is available from [CTAN](https://ctan.org/pkg/dvisvgm), and can be
downloaded from the [project website](https://dvisvgm.de) as well. There you find
the sources, [pre-compiled binaries](https://dvisvgm.de/Downloads), the
[manual page](https://dvisvgm.de/Manpage), [FAQs](https://dvisvgm.de/FAQ) and further
information about the converter and related topics.

If you've found a bug, please let me know. You can either send me an email or
preferably use the [bug tracker at GitHub](https://github.com/mgieseki/dvisvgm/issues).


Copyright
---------

Copyright &copy; 2005&ndash;2024 [Martin Gieseking](mailto:martin.gieseking@uos.de)

This program is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html) for more
details.

