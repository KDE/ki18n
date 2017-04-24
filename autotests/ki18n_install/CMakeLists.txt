project(ki18n_install)
cmake_minimum_required(VERSION 3.0)

include(KF5I18NMacros)

ki18n_install(${CMAKE_CURRENT_SOURCE_DIR}/po)
