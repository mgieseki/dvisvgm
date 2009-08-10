Name:           dvisvgm
Version:        0.8.1
Release:        4%{?dist}
Summary:        A DVI to SVG converter

Group:          Applications/Publishing
License:        GPLv2+
URL:            http://dvisvgm.sourceforge.net
Source0:        http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz

# Patch to use potrace library provided by separate package
Patch0:         dvisvgm-0.8.1-potrace.patch

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  automake kpathsea-devel freetype-devel potrace-devel zlib-devel 
Requires:       texlive-texmf texlive-dvips texlive-fonts ghostscript

%description
dvisvgm is a command line utility that converts DVI files, as created by 
TeX/LaTeX, to the XML-based scalable vector graphics format SVG. Besides 
the basic DVI command set, dvisvgm also supports various special sets, like 
color, PostScript and TPIC.

Fonts that are only available as Metafont source are automatically vectorized 
so that the generated SVG is freely scalable without loss of quality.


%prep
%setup -q
%patch0 -p 1 -b .orig
autoreconf
# Remove bundled potracelib
rm -rf potracelib


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc AUTHORS LICENSE NEWS README
%{_bindir}/dvisvgm
%{_mandir}/man1/dvisvgm.1.*


%changelog
* Tue Aug 03 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-4
- removed bundled potrace library in prep section

* Mon Aug 02 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-3
- don't use bundled potrace library

* Mon Aug 02 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-2
- rebased to upstream with added tarball
- added LICENSE to doc

* Sun Aug 01 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-1
- initial release
