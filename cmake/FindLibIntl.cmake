#.rst:
# FindLibIntl
# ---------
#
# Find libintl
#
# Find the libintl headers and libraries. On platforms that use glibc this is not required
# and LibIntl_LIBRARIES will be empty
#
# ::
#
#   LibIntl_INCLUDE_DIRS - where to find libintl.h
#   LibIntl_LIBRARIES    - The libintl library if the current platform does not use glibc.
#   LibIntl_FOUND        - True if libintl was found.

#=============================================================================
# Copyright 2014 Alex Richardson <arichardson.kde@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
#=============================================================================

find_path(LibIntl_INCLUDE_DIRS NAMES libintl.h)
find_library(LibIntl_LIBRARIES NAMES libintl)

include(CheckSymbolExists)
check_symbol_exists(dcngettext libintl.h LibIntl_SYMBOL_FOUND)

include(FindPackageHandleStandardArgs)
if(LibIntl_SYMBOL_FOUND)
    message(STATUS "libintl is part of libc, no extra library is required.")
    set(LibIntl_LIBRARIES "")
    find_package_handle_standard_args(LibIntl REQUIRED_VARS LibIntl_INCLUDE_DIRS)
else()
    message(STATUS "libintl is a separate library.")
    find_package_handle_standard_args(LibIntl REQUIRED_VARS LibIntl_INCLUDE_DIRS LibIntl_LIBRARIES)
endif()
