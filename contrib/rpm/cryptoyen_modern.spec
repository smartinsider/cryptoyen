%define bdbv 4.8.30
%global selinux_variants mls strict targeted

%if 0%{?_no_gui:1}
%define _buildqt 0
%define buildargs --with-gui=no
%else
%define _buildqt 1
%if 0%{?_use_qt4}
%define buildargs --with-qrencode --with-gui=qt4
%else
%define buildargs --with-qrencode --with-gui=qt5
%endif
%endif

Name:		cryptoyen_modern
Version:	0.12.0
Release:	2%{?dist}
Summary:	Peer to Peer Cryptographic Currency

Group:		Applications/System
License:	MIT
URL:		https://cryptoyen_modern.icu/
Source0:	https://cryptoyen_modern.icu/bin/cryptoyen_modern-core-%{version}/cryptoyen_modern-%{version}.tar.gz
Source1:	http://download.oracle.com/berkeley-db/db-%{bdbv}.NC.tar.gz

Source10:	https://raw.githubusercontent.com/cryptoyen_modern-project/cryptoyen_modern/v%{version}/contrib/debian/examples/cryptoyen_modern.conf

#man pages
Source20:	https://raw.githubusercontent.com/cryptoyen_modern-project/cryptoyen_modern/v%{version}/doc/man/cryptoyen_modernd.1
Source21:	https://raw.githubusercontent.com/cryptoyen_modern-project/cryptoyen_modern/v%{version}/doc/man/cryptoyen_modern-cli.1
Source22:	https://raw.githubusercontent.com/cryptoyen_modern-project/cryptoyen_modern/v%{version}/doc/man/cryptoyen_modern-qt.1

#selinux
Source30:	https://raw.githubusercontent.com/cryptoyen_modern-project/cryptoyen_modern/v%{version}/contrib/rpm/cryptoyen_modern.te
# Source31 - what about cryptoyen_modern-tx and bench_cryptoyen_modern ???
Source31:	https://raw.githubusercontent.com/cryptoyen_modern-project/cryptoyen_modern/v%{version}/contrib/rpm/cryptoyen_modern.fc
Source32:	https://raw.githubusercontent.com/cryptoyen_modern-project/cryptoyen_modern/v%{version}/contrib/rpm/cryptoyen_modern.if

Source100:	https://upload.wikimedia.org/wikipedia/commons/4/46/Bitcoin.svg

%if 0%{?_use_libressl:1}
BuildRequires:	libressl-devel
%else
BuildRequires:	openssl-devel
%endif
BuildRequires:	boost-devel
BuildRequires:	miniupnpc-devel
BuildRequires:	autoconf automake libtool
BuildRequires:	libevent-devel


Patch0:		cryptoyen_modern-0.12.0-libressl.patch


%description
Bitcoin is a digital cryptographic currency that uses peer-to-peer technology to
operate with no central authority or banks; managing transactions and the
issuing of cryptoyen_moderns is carried out collectively by the network.

%if %{_buildqt}
%package core
Summary:	Peer to Peer Cryptographic Currency
Group:		Applications/System
Obsoletes:	%{name} < %{version}-%{release}
Provides:	%{name} = %{version}-%{release}
%if 0%{?_use_qt4}
BuildRequires:	qt-devel
%else
BuildRequires:	qt5-qtbase-devel
# for /usr/bin/lrelease-qt5
BuildRequires:	qt5-linguist
%endif
BuildRequires:	protobuf-devel
BuildRequires:	qrencode-devel
BuildRequires:	%{_bindir}/desktop-file-validate
# for icon generation from SVG
BuildRequires:	%{_bindir}/inkscape
BuildRequires:	%{_bindir}/convert

%description core
Bitcoin is a digital cryptographic currency that uses peer-to-peer technology to
operate with no central authority or banks; managing transactions and the
issuing of cryptoyen_moderns is carried out collectively by the network.

This package contains the Qt based graphical client and node. If you are looking
to run a Bitcoin wallet, this is probably the package you want.
%endif


%package libs
Summary:	Bitcoin shared libraries
Group:		System Environment/Libraries

%description libs
This package provides the cryptoyen_modernconsensus shared libraries. These libraries
may be used by third party software to provide consensus verification
functionality.

Unless you know need this package, you probably do not.

%package devel
Summary:	Development files for cryptoyen_modern
Group:		Development/Libraries
Requires:	%{name}-libs = %{version}-%{release}

%description devel
This package contains the header files and static library for the
cryptoyen_modernconsensus shared library. If you are developing or compiling software
that wants to link against that library, then you need this package installed.

Most people do not need this package installed.

%package server
Summary:	The cryptoyen_modern daemon
Group:		System Environment/Daemons
Requires:	cryptoyen_modern-utils = %{version}-%{release}
Requires:	selinux-policy policycoreutils-python
Requires(pre):	shadow-utils
Requires(post):	%{_sbindir}/semodule %{_sbindir}/restorecon %{_sbindir}/fixfiles %{_sbindir}/sestatus
Requires(postun):	%{_sbindir}/semodule %{_sbindir}/restorecon %{_sbindir}/fixfiles %{_sbindir}/sestatus
BuildRequires:	systemd
BuildRequires:	checkpolicy
BuildRequires:	%{_datadir}/selinux/devel/Makefile

%description server
This package provides a stand-alone cryptoyen_modern-core daemon. For most users, this
package is only needed if they need a full-node without the graphical client.

Some third party wallet software will want this package to provide the actual
cryptoyen_modern-core node they use to connect to the network.

If you use the graphical cryptoyen_modern-core client then you almost certainly do not
need this package.

%package utils
Summary:	Bitcoin utilities
Group:		Applications/System

%description utils
This package provides several command line utilities for interacting with a
cryptoyen_modern-core daemon.

The cryptoyen_modern-cli utility allows you to communicate and control a cryptoyen_modern daemon
over RPC, the cryptoyen_modern-tx utility allows you to create a custom transaction, and
the bench_cryptoyen_modern utility can be used to perform some benchmarks.

This package contains utilities needed by the cryptoyen_modern-server package.


%prep
%setup -q
%patch0 -p1 -b .libressl
cp -p %{SOURCE10} ./cryptoyen_modern.conf.example
tar -zxf %{SOURCE1}
cp -p db-%{bdbv}.NC/LICENSE ./db-%{bdbv}.NC-LICENSE
mkdir db4 SELinux
cp -p %{SOURCE30} %{SOURCE31} %{SOURCE32} SELinux/


%build
CWD=`pwd`
cd db-%{bdbv}.NC/build_unix/
../dist/configure --enable-cxx --disable-shared --with-pic --prefix=${CWD}/db4
make install
cd ../..

./autogen.sh
%configure LDFLAGS="-L${CWD}/db4/lib/" CPPFLAGS="-I${CWD}/db4/include/" --with-miniupnpc --enable-glibc-back-compat %{buildargs}
make %{?_smp_mflags}

pushd SELinux
for selinuxvariant in %{selinux_variants}; do
	make NAME=${selinuxvariant} -f %{_datadir}/selinux/devel/Makefile
	mv cryptoyen_modern.pp cryptoyen_modern.pp.${selinuxvariant}
	make NAME=${selinuxvariant} -f %{_datadir}/selinux/devel/Makefile clean
done
popd


%install
make install DESTDIR=%{buildroot}

mkdir -p -m755 %{buildroot}%{_sbindir}
mv %{buildroot}%{_bindir}/cryptoyen_modernd %{buildroot}%{_sbindir}/cryptoyen_modernd

# systemd stuff
mkdir -p %{buildroot}%{_tmpfilesdir}
cat <<EOF > %{buildroot}%{_tmpfilesdir}/cryptoyen_modern.conf
d /run/cryptoyen_modernd 0750 cryptoyen_modern cryptoyen_modern -
EOF
touch -a -m -t 201504280000 %{buildroot}%{_tmpfilesdir}/cryptoyen_modern.conf

mkdir -p %{buildroot}%{_sysconfdir}/sysconfig
cat <<EOF > %{buildroot}%{_sysconfdir}/sysconfig/cryptoyen_modern
# Provide options to the cryptoyen_modern daemon here, for example
# OPTIONS="-testnet -disable-wallet"

OPTIONS=""

# System service defaults.
# Don't change these unless you know what you're doing.
CONFIG_FILE="%{_sysconfdir}/cryptoyen_modern/cryptoyen_modern.conf"
DATA_DIR="%{_localstatedir}/lib/cryptoyen_modern"
PID_FILE="/run/cryptoyen_modernd/cryptoyen_modernd.pid"
EOF
touch -a -m -t 201504280000 %{buildroot}%{_sysconfdir}/sysconfig/cryptoyen_modern

mkdir -p %{buildroot}%{_unitdir}
cat <<EOF > %{buildroot}%{_unitdir}/cryptoyen_modern.service
[Unit]
Description=Bitcoin daemon
After=syslog.target network.target

[Service]
Type=forking
ExecStart=%{_sbindir}/cryptoyen_modernd -daemon -conf=\${CONFIG_FILE} -datadir=\${DATA_DIR} -pid=\${PID_FILE} \$OPTIONS
EnvironmentFile=%{_sysconfdir}/sysconfig/cryptoyen_modern
User=cryptoyen_modern
Group=cryptoyen_modern

Restart=on-failure
PrivateTmp=true
TimeoutStopSec=120
TimeoutStartSec=60
StartLimitInterval=240
StartLimitBurst=5

[Install]
WantedBy=multi-user.target
EOF
touch -a -m -t 201504280000 %{buildroot}%{_unitdir}/cryptoyen_modern.service
#end systemd stuff

mkdir %{buildroot}%{_sysconfdir}/cryptoyen_modern
mkdir -p %{buildroot}%{_localstatedir}/lib/cryptoyen_modern

#SELinux
for selinuxvariant in %{selinux_variants}; do
	install -d %{buildroot}%{_datadir}/selinux/${selinuxvariant}
	install -p -m 644 SELinux/cryptoyen_modern.pp.${selinuxvariant} %{buildroot}%{_datadir}/selinux/${selinuxvariant}/cryptoyen_modern.pp
done

%if %{_buildqt}
# qt icons
install -D -p share/pixmaps/cryptoyen_modern.ico %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern.ico
install -p share/pixmaps/nsis-header.bmp %{buildroot}%{_datadir}/pixmaps/
install -p share/pixmaps/nsis-wizard.bmp %{buildroot}%{_datadir}/pixmaps/
install -p %{SOURCE100} %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern.svg
%{_bindir}/inkscape %{SOURCE100} --export-png=%{buildroot}%{_datadir}/pixmaps/cryptoyen_modern16.png -w16 -h16
%{_bindir}/inkscape %{SOURCE100} --export-png=%{buildroot}%{_datadir}/pixmaps/cryptoyen_modern32.png -w32 -h32
%{_bindir}/inkscape %{SOURCE100} --export-png=%{buildroot}%{_datadir}/pixmaps/cryptoyen_modern64.png -w64 -h64
%{_bindir}/inkscape %{SOURCE100} --export-png=%{buildroot}%{_datadir}/pixmaps/cryptoyen_modern128.png -w128 -h128
%{_bindir}/inkscape %{SOURCE100} --export-png=%{buildroot}%{_datadir}/pixmaps/cryptoyen_modern256.png -w256 -h256
%{_bindir}/convert -resize 16x16 %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern256.png %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern16.xpm
%{_bindir}/convert -resize 32x32 %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern256.png %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern32.xpm
%{_bindir}/convert -resize 64x64 %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern256.png %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern64.xpm
%{_bindir}/convert -resize 128x128 %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern256.png %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern128.xpm
%{_bindir}/convert %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern256.png %{buildroot}%{_datadir}/pixmaps/cryptoyen_modern256.xpm
touch %{buildroot}%{_datadir}/pixmaps/*.png -r %{SOURCE100}
touch %{buildroot}%{_datadir}/pixmaps/*.xpm -r %{SOURCE100}

# Desktop File - change the touch timestamp if modifying
mkdir -p %{buildroot}%{_datadir}/applications
cat <<EOF > %{buildroot}%{_datadir}/applications/cryptoyen_modern-core.desktop
[Desktop Entry]
Encoding=UTF-8
Name=Bitcoin
Comment=Bitcoin P2P Cryptocurrency
Comment[fr]=Bitcoin, monnaie virtuelle cryptographique pair à pair
Comment[tr]=Bitcoin, eşten eşe kriptografik sanal para birimi
Exec=cryptoyen_modern-qt %u
Terminal=false
Type=Application
Icon=cryptoyen_modern128
MimeType=x-scheme-handler/cryptoyen_modern;
Categories=Office;Finance;
EOF
# change touch date when modifying desktop
touch -a -m -t 201511100546 %{buildroot}%{_datadir}/applications/cryptoyen_modern-core.desktop
%{_bindir}/desktop-file-validate %{buildroot}%{_datadir}/applications/cryptoyen_modern-core.desktop

# KDE protocol - change the touch timestamp if modifying
mkdir -p %{buildroot}%{_datadir}/kde4/services
cat <<EOF > %{buildroot}%{_datadir}/kde4/services/cryptoyen_modern-core.protocol
[Protocol]
exec=cryptoyen_modern-qt '%u'
protocol=cryptoyen_modern
input=none
output=none
helper=true
listing=
reading=false
writing=false
makedir=false
deleting=false
EOF
# change touch date when modifying protocol
touch -a -m -t 201511100546 %{buildroot}%{_datadir}/kde4/services/cryptoyen_modern-core.protocol
%endif

# man pages
install -D -p %{SOURCE20} %{buildroot}%{_mandir}/man1/cryptoyen_modernd.1
install -p %{SOURCE21} %{buildroot}%{_mandir}/man1/cryptoyen_modern-cli.1
%if %{_buildqt}
install -p %{SOURCE22} %{buildroot}%{_mandir}/man1/cryptoyen_modern-qt.1
%endif

# nuke these, we do extensive testing of binaries in %%check before packaging
rm -f %{buildroot}%{_bindir}/test_*

%check
make check
srcdir=src test/cryptoyen_modern-util-test.py
test/functional/test_runner.py --extended

%post libs -p /sbin/ldconfig

%postun libs -p /sbin/ldconfig

%pre server
getent group cryptoyen_modern >/dev/null || groupadd -r cryptoyen_modern
getent passwd cryptoyen_modern >/dev/null ||
	useradd -r -g cryptoyen_modern -d /var/lib/cryptoyen_modern -s /sbin/nologin \
	-c "Bitcoin wallet server" cryptoyen_modern
exit 0

%post server
%systemd_post cryptoyen_modern.service
# SELinux
if [ `%{_sbindir}/sestatus |grep -c "disabled"` -eq 0 ]; then
for selinuxvariant in %{selinux_variants}; do
	%{_sbindir}/semodule -s ${selinuxvariant} -i %{_datadir}/selinux/${selinuxvariant}/cryptoyen_modern.pp &> /dev/null || :
done
%{_sbindir}/semanage port -a -t cryptoyen_modern_port_t -p tcp 8332
%{_sbindir}/semanage port -a -t cryptoyen_modern_port_t -p tcp 8333
%{_sbindir}/semanage port -a -t cryptoyen_modern_port_t -p tcp 18332
%{_sbindir}/semanage port -a -t cryptoyen_modern_port_t -p tcp 18333
%{_sbindir}/semanage port -a -t cryptoyen_modern_port_t -p tcp 18443
%{_sbindir}/semanage port -a -t cryptoyen_modern_port_t -p tcp 18444
%{_sbindir}/fixfiles -R cryptoyen_modern-server restore &> /dev/null || :
%{_sbindir}/restorecon -R %{_localstatedir}/lib/cryptoyen_modern || :
fi

%posttrans server
%{_bindir}/systemd-tmpfiles --create

%preun server
%systemd_preun cryptoyen_modern.service

%postun server
%systemd_postun cryptoyen_modern.service
# SELinux
if [ $1 -eq 0 ]; then
	if [ `%{_sbindir}/sestatus |grep -c "disabled"` -eq 0 ]; then
	%{_sbindir}/semanage port -d -p tcp 8332
	%{_sbindir}/semanage port -d -p tcp 8333
	%{_sbindir}/semanage port -d -p tcp 18332
	%{_sbindir}/semanage port -d -p tcp 18333
	%{_sbindir}/semanage port -d -p tcp 18443
	%{_sbindir}/semanage port -d -p tcp 18444
	for selinuxvariant in %{selinux_variants}; do
		%{_sbindir}/semodule -s ${selinuxvariant} -r cryptoyen_modern &> /dev/null || :
	done
	%{_sbindir}/fixfiles -R cryptoyen_modern-server restore &> /dev/null || :
	[ -d %{_localstatedir}/lib/cryptoyen_modern ] && \
		%{_sbindir}/restorecon -R %{_localstatedir}/lib/cryptoyen_modern &> /dev/null || :
	fi
fi

%clean
rm -rf %{buildroot}

%if %{_buildqt}
%files core
%defattr(-,root,root,-)
%license COPYING db-%{bdbv}.NC-LICENSE
%doc COPYING cryptoyen_modern.conf.example doc/README.md doc/bips.md doc/files.md doc/multiwallet-qt.md doc/reduce-traffic.md doc/release-notes.md doc/tor.md
%attr(0755,root,root) %{_bindir}/cryptoyen_modern-qt
%attr(0644,root,root) %{_datadir}/applications/cryptoyen_modern-core.desktop
%attr(0644,root,root) %{_datadir}/kde4/services/cryptoyen_modern-core.protocol
%attr(0644,root,root) %{_datadir}/pixmaps/*.ico
%attr(0644,root,root) %{_datadir}/pixmaps/*.bmp
%attr(0644,root,root) %{_datadir}/pixmaps/*.svg
%attr(0644,root,root) %{_datadir}/pixmaps/*.png
%attr(0644,root,root) %{_datadir}/pixmaps/*.xpm
%attr(0644,root,root) %{_mandir}/man1/cryptoyen_modern-qt.1*
%endif

%files libs
%defattr(-,root,root,-)
%license COPYING
%doc COPYING doc/README.md doc/shared-libraries.md
%{_libdir}/lib*.so.*

%files devel
%defattr(-,root,root,-)
%license COPYING
%doc COPYING doc/README.md doc/developer-notes.md doc/shared-libraries.md
%attr(0644,root,root) %{_includedir}/*.h
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/*.la
%attr(0644,root,root) %{_libdir}/pkgconfig/*.pc

%files server
%defattr(-,root,root,-)
%license COPYING db-%{bdbv}.NC-LICENSE
%doc COPYING cryptoyen_modern.conf.example doc/README.md doc/REST-interface.md doc/bips.md doc/dnsseed-policy.md doc/files.md doc/reduce-traffic.md doc/release-notes.md doc/tor.md
%attr(0755,root,root) %{_sbindir}/cryptoyen_modernd
%attr(0644,root,root) %{_tmpfilesdir}/cryptoyen_modern.conf
%attr(0644,root,root) %{_unitdir}/cryptoyen_modern.service
%dir %attr(0750,cryptoyen_modern,cryptoyen_modern) %{_sysconfdir}/cryptoyen_modern
%dir %attr(0750,cryptoyen_modern,cryptoyen_modern) %{_localstatedir}/lib/cryptoyen_modern
%config(noreplace) %attr(0600,root,root) %{_sysconfdir}/sysconfig/cryptoyen_modern
%attr(0644,root,root) %{_datadir}/selinux/*/*.pp
%attr(0644,root,root) %{_mandir}/man1/cryptoyen_modernd.1*

%files utils
%defattr(-,root,root,-)
%license COPYING
%doc COPYING cryptoyen_modern.conf.example doc/README.md
%attr(0755,root,root) %{_bindir}/cryptoyen_modern-cli
%attr(0755,root,root) %{_bindir}/cryptoyen_modern-tx
%attr(0755,root,root) %{_bindir}/bench_cryptoyen_modern
%attr(0644,root,root) %{_mandir}/man1/cryptoyen_modern-cli.1*



%changelog
* Fri Feb 26 2016 Alice Wonder <buildmaster@librelamp.com> - 0.12.0-2
- Rename Qt package from cryptoyen_modern to cryptoyen_modern-core
- Make building of the Qt package optional
- When building the Qt package, default to Qt5 but allow building
-  against Qt4
- Only run SELinux stuff in post scripts if it is not set to disabled

* Wed Feb 24 2016 Alice Wonder <buildmaster@librelamp.com> - 0.12.0-1
- Initial spec file for 0.12.0 release

# This spec file is written from scratch but a lot of the packaging decisions are directly
# based upon the 0.11.2 package spec file from https://www.ringingliberty.com/cryptoyen_modern/
