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
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#=============================================================================

find_path(LibIntl_INCLUDE_DIRS NAMES libintl.h)
find_library(LibIntl_LIBRARIES NAMES intl libintl)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(dngettext libintl.h LibIntl_SYMBOL_FOUND)

include(FindPackageHandleStandardArgs)
if(LibIntl_SYMBOL_FOUND)
    message(STATUS "libintl is part of libc, no extra library is required.")
    set(LibIntl_LIBRARIES "")
    if(LibIntl_INCLUDE_DIRS)
        find_package_handle_standard_args(LibIntl REQUIRED_VARS LibIntl_INCLUDE_DIRS)
    else()
        # in the default search path but not found by find_path, e.g. host glibc when cross-compiling
        set(LibIntl_INCLUDE_DIRS "")
        set(LibIntl_FOUND TRUE)
    endif()
else()
    message(STATUS "libintl is a separate library.")
    find_package_handle_standard_args(LibIntl REQUIRED_VARS LibIntl_INCLUDE_DIRS LibIntl_LIBRARIES)
endif()
