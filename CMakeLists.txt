cmake_minimum_required(VERSION 3.31)

project(appmenu-gtk-module-wayland VERSION 0.1.0 LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET x11)

include(GNUInstallDirs)
include(CTest)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(libdbusmenu)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/wayland/generated")

add_library(appmenu-gtk-module-wayland  SHARED
    "${SRC_DIR}/appmenu-gtk-module.c"
    "${SRC_DIR}/datastructs.c"
    "${SRC_DIR}/hijack.c"
    "${SRC_DIR}/support.c"
    "${SRC_DIR}/blacklist.c"
    "${SRC_DIR}/platform.c"
    "${GENERATED_DIR}/appmenu.c"
)

target_include_directories(appmenu-gtk-module-wayland PRIVATE "${GENERATED_DIR}")
target_link_libraries(appmenu-gtk-module-wayland
    PkgConfig::GTK3
    PkgConfig::WAYLAND_CLIENT
    PkgConfig::X11
    dbusmenu-gtk
)

install(
    TARGETS appmenu-gtk-module-wayland
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/gtk-3.0/modules"
)

add_executable(update-gtk-settings tools/update-gtk-settings.c)
target_link_libraries(update-gtk-settings PkgConfig::GLIB)
install(
    TARGETS update-gtk-settings
    RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}/gxde-wayland-gtk-menu"
)

if(BUILD_TESTING)
    set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_executable(unity-gtk-menu-tester "${TEST_DIR}/demos/unity-gtk-menu-tester.c")
    target_link_libraries(unity-gtk-menu-tester PkgConfig::GTK3)
endif()
