# Put the spec in ~/rpmbuild/SPECS and clone the repo as ~/rpmbuild/SOURCES
#
# Credit to original file goes to Github Users:
# https://github.com/bensallen
# https://github.com/ozky
#
# Copied-with-modification from: https://github.com/pyamsoft/pstate-frequency/issues/25

Name:      pstate-frequency
Version:   3.17.0
Release:   1
License:   GPLv2
Summary:   Easily control moden CPU p-state driver on Linux
Url:       https://pyamsoft.github.io/pstate-frequency
Group:     System
Source0:   https://github.com/pyamsoft/%{name}/archive/%{version}.tar.gz#/%{name}-%{version}.tar.gz
Requires:  coreutils
Requires:  grep
Requires:  awk
BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
Pstate-frequency is able to adjust the modern CPU p-state driver values for the minimum and maximum scaling frequencies and the state of turbo boost.

%prep
%setup -q

%install
patch -p1 -i "./assets/pkgbuild/patches/00-fix-prefix.patch"
make PREFIX=%{_prefix} DESTDIR=%{buildroot} install

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
/etc/%{name}.d/*.plan
/usr/lib/systemd/system/%{name}@.service
/usr/lib/systemd/system/%{name}-sleep.service
/usr/lib/udev/rules.d/99-%{name}.rules
/usr/share/bash-completion/completions/%{name}
%doc %{_docdir}/%{name}/README.md
%doc %{_docdir}/%{name}/LICENSE

%changelog
* Sat Jan 24 2026 3.17.0-1
- Read frequencies from all CPU cores to determine system max and min

* Tue Jul 08 2025 3.16.1-1
- systemd and udev scripts run directly instead of via /bin/sh

* Wed May 28 2025 3.16.0-1
- New EPP set behavior
- Default EPP in plans is balance_power

* Wed Sep 04 2024 3.15.2-1
- Fix systemd service file ordering due to new power-profiles-daemon release.

* Sat Apr 06 2024 3.15.1-1
- Initial RPM release (3.15.1-1)

