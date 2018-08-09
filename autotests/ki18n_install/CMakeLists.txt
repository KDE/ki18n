project(ki18n_install)
cmake_minimum_required(VERSION 3.0)

find_package(KF5I18n)

ki18n_install(${CMAKE_CURRENT_SOURCE_DIR}/po)
