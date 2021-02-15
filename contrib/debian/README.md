
Debian
====================
This directory contains files used to package cryptoyen_modernd/cryptoyen_modern-qt
for Debian-based Linux systems. If you compile cryptoyen_modernd/cryptoyen_modern-qt yourself, there are some useful files here.

## cryptoyen_modern: URI support ##


cryptoyen_modern-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install cryptoyen_modern-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your cryptoyen_modernqt binary to `/usr/bin`
and the `../../share/pixmaps/cryptoyen_modern128.png` to `/usr/share/pixmaps`

cryptoyen_modern-qt.protocol (KDE)

