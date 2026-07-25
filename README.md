# GXDE Wayland GTK Menu

GTK 3 global menu module for the GXDE Wayland session. It exports GTK menu
information through D-Bus and associates it with Wayland surfaces through the
KDE appmenu protocol.

This project supports GTK 3 only. GTK 2 has no native Wayland backend, and
GTK 4 no longer provides the menu widget and module interfaces used by this
implementation.

## Dependencies

- CMake 3.31 or later
- pkgconf
- GTK 3 development files
- Wayland development files
- X11 development files

The required libdbusmenu sources are vendored in this repository.

## Build

```console
$ cmake -S . -B build
$ cmake --build build
```

## Run the tester

```console
$ GTK_MODULES=$PWD/build/libappmenu-gtk-module-wayland.so ./build/unity-gtk-menu-tester
```

## Build the Debian package

Install the dependencies listed in `debian/control`, then run:

```console
$ dpkg-buildpackage --no-sign --build=binary
```

When the package is installed, its maintainer script adds
`appmenu-gtk-module-wayland` to the `gtk-modules` list in
`/etc/gtk-3.0/settings.ini`. Existing settings and GTK modules are retained.
Removing the package removes only this module from the list.

Log out and back in after the first installation. The module can also be
loaded explicitly for one application:

```console
$ GTK_MODULES=appmenu-gtk-module-wayland your-gtk3-application
```
