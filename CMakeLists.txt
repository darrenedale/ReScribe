cmake_minimum_required(VERSION 3.16)
project(rescribe LANGUAGES CXX VERSION 0.1)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# TODO these are all temporary overrides that happen to work on my system - we should be able to rely on those
# configured by the cmake modules
# TODO can we use ECM DBUSSYSTEMSERVICEDIR?
set(DBUS_SYSTEM_SERVICES_INSTALL_DIR /usr/share/dbus-1/system-services/)
set(KDE_INSTALL_DBUSDIR /etc/dbus-1)
set(KAUTH_HELPER_INSTALL_DIR /usr/lib/kauth/)

find_package(Qt5 5.7 REQUIRED COMPONENTS Core Widgets DBus)
find_package(KF5WidgetsAddons)
find_package(KF5KIO)
find_package(KF5Solid)
find_package(KF5Auth)

add_subdirectory(src)
