cmake_minimum_required(VERSION 3.31)

project(libdbusmenu)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLib REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)

set(CMAKE_C_STANDARD 11)

# variables in config.h
set(ENABLE_NLS 0)
set(GETTEXT_PACKAGE "libdbusmenu")
set(HAVE_BIND_TEXTDOMAIN_CODESET 0)
set(HAVE_DCGETTEXT 0)
set(HAVE_DLFCN_H 1)
set(HAVE_GETTEXT 1)
set(HAVE_INTTYPES_H 1)
set(HAVE_LC_MESSAGES 1)
set(HAVE_LOCALE_H 1)
set(HAVE_MEMORY_H 1)
set(HAVE_STDINT_H 1)
set(HAVE_STDLIB_H 1)
set(HAVE_STRINGS_H 1)
set(HAVE_STRING_H 1)
set(HAVE_SYS_STAT_H 1)
set(HAVE_SYS_TYPES_H 1)
set(HAVE_UNISTD_H 1)
set(LT_OBJDIR 0)
set(MASSIVEDEBUGGING 0)
set(STDC_HEADERS 1)

# transform config.h.in to cmake configure_file format
file(READ "config.h.in" CONFIG_H)
string(REPLACE "#undef" "#cmakedefine" CONFIG_H "${CONFIG_H}")
string(REGEX REPLACE "(GETTEXT_PACKAGE)" "\\1 \"@\\1@\"" CONFIG_H "${CONFIG_H}")
# generate config.h
file(CONFIGURE OUTPUT "config.h" CONTENT "${CONFIG_H}" @ONLY)

set(SRC_GLIB "${CMAKE_CURRENT_SOURCE_DIR}/libdbusmenu-glib")

set(HEADERS_GLIB
	"${SRC_GLIB}/enum-types.h"
	"${SRC_GLIB}/dbusmenu-glib.h"
	"${SRC_GLIB}/types.h"
	"${SRC_GLIB}/menuitem.h"
	"${SRC_GLIB}/menuitem-proxy.h"
	"${SRC_GLIB}/server.h"
	"${SRC_GLIB}/client.h"
)

add_library(dbusmenu-glib SHARED
	"${SRC_GLIB}/dbus-menu-clean.xml.c"
	"${SRC_GLIB}/defaults.c"
	"${SRC_GLIB}/enum-types.c"
	"${SRC_GLIB}/menuitem.c"
	"${SRC_GLIB}/menuitem-marshal.c"
	"${SRC_GLIB}/menuitem-proxy.c"
	"${SRC_GLIB}/server.c"
	"${SRC_GLIB}/server-marshal.c"
	"${SRC_GLIB}/client-marshal.c"
	"${SRC_GLIB}/client-menuitem.c"
	"${SRC_GLIB}/client.c"
)

target_compile_definitions(dbusmenu-glib PRIVATE
	HAVE_CONFIG_H
	G_LOG_DOMAIN="LIBDBUSMENU-GLIB"
)
target_link_libraries(dbusmenu-glib PkgConfig::GLib)
target_include_directories(dbusmenu-glib PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(dbusmenu-glib PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set(SRC_GTK3 "${CMAKE_CURRENT_SOURCE_DIR}/libdbusmenu-gtk")

set(HEADERS_GTK
	"${SRC_GTK3}dbusmenu-gtk.h"
	"${SRC_GTK3}client.h"
	"${SRC_GTK3}menu.h"
	"${SRC_GTK3}menuitem.h"
	"${SRC_GTK3}parser.h"
)

add_library(dbusmenu-gtk
	"${SRC_GTK3}/client.c"
	"${SRC_GTK3}/genericmenuitem.c"
	"${SRC_GTK3}/genericmenuitem-enum-types.c"
	"${SRC_GTK3}/menu.c"
	"${SRC_GTK3}/menuitem.c"
	"${SRC_GTK3}/parser.c"
)

target_link_libraries(dbusmenu-gtk PkgConfig::GTK3 dbusmenu-glib)
target_include_directories(dbusmenu-gtk PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(dbusmenu-gtk PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
