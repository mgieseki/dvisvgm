Summary: command-line utility to convert DVI files to SVG
Name: dvisvgm
Version: 0.4.3
Release: 1%{?dist}
#Epoch: 1
Packager: Martin Gieseking <martin.gieseking@uos.de>
License: GPL
Group: Applications/File
Source0: %{name}-%{version}.tar.gz
URL: http://dvisvgm.sourceforge.net
BuildRoot: %{_tmppath}/%{name}-root

BuildRequires: freetype-devel
BuildRequires: tetex
BuildRequires: zlib-devel

Requires: freetype
Requires: tetex
Requires: zlib

%description
dvisvgm is a command-line utility that converts DVI files,
as created by TeX, to the XML-based scalable vector graphics format SVG.

%prep
%setup

%build
%configure
make

%install
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%doc README
%{_bindir}/dvisvgm
%{_mandir}/man1/dvisvgm.1.gz

%changelog
	* Thu Mar 13 2007 Martin Gieseking <martin.gieseking@uos.de>
	- Initial RPM release. 
