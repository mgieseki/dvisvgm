Name:           dvisvgm
Version:        0.8.4
Release:        1%{?dist}
Summary:        A DVI to SVG converter

Group:          Applications/Publishing
License:        GPLv3+
URL:            http://dvisvgm.sourceforge.net
Source0:        http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  automake freetype-devel ghostscript-devel potrace-devel zlib-devel 

%if 0%{?rhel} == 4 || 0%{?rhel} == 5
BuildRequires:  tetex-fonts
Requires:       tetex-fonts tetex-dvips
%else
BuildRequires:  kpathsea-devel
Requires:       texlive-texmf texlive-dvips texlive-fonts
%endif


%description
dvisvgm is a command line utility that converts DVI files, as created by 
TeX/LaTeX, to the XML-based scalable vector graphics format SVG. Besides 
the basic DVI command set, dvisvgm also supports various special sets, like 
color, PostScript and TPIC.

Fonts that are only available as Metafont source are automatically vectorized 
so that the generated SVG is freely scalable without loss of quality.


%prep
%setup -q
# Remove bundled potracelib
rm -rf potracelib
# Remove bundled Ghostscript API headers
rm -f src/iapi.h src/ierrors.h


%build
# remove references to bundled potrace library
sed -i "s/ potracelib\/Makefile / /" configure.ac
sed -i "s/potracelib //" Makefile.am
autoreconf
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS README
%{_bindir}/dvisvgm
%{_mandir}/man1/dvisvgm.1.*


%changelog
* Thu Sep 17 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.4-1
- updated to latest upstream release

* Mon Aug 24 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.3-1
- updated to latest upstream release
- conditional Requires and BuildRequires to satisfy F-xx and EL5

* Wed Aug 12 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-5
- adapted Build and BuildRequires to requirements of EL5

* Tue Aug 03 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-4
- removed bundled potrace library in prep section

* Mon Aug 02 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-3
- don't use bundled potrace library

* Mon Aug 02 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-2
- rebased to upstream with added tarball
- added LICENSE to doc

* Sun Aug 01 2009 Martin Gieseking <martin.gieseking@uos.de> - 0.8.1-1
- initial release
