
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
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

find_package(Gettext REQUIRED)
find_package(PythonInterp REQUIRED)

set(_ki18n_pmap_compile_script ${CMAKE_CURRENT_LIST_DIR}/ts-pmap-compile.py)
set(_ki18n_uic_script ${CMAKE_CURRENT_LIST_DIR}/kf5i18nuic.cmake)

#create the implementation files from the ui files and add them to the list of sources
#usage: KI18N_WRAP_UI(foo_SRCS ${ui_files})
macro (KI18N_WRAP_UI _sources )
   foreach (_current_FILE ${ARGN})

      get_filename_component(_tmp_FILE ${_current_FILE} ABSOLUTE)
      get_filename_component(_basename ${_tmp_FILE} NAME_WE)
      set(_header ${CMAKE_CURRENT_BINARY_DIR}/ui_${_basename}.h)

      get_target_property(QT_UIC_EXECUTABLE Qt5::uic LOCATION)
      # we need to run uic and replace some things in the generated file
      # this is done by executing the cmake script kf5i18nuic.cmake
      add_custom_command(OUTPUT ${_header}
         COMMAND ${CMAKE_COMMAND}
         ARGS
         -DKDE_UIC_EXECUTABLE:FILEPATH=${QT_UIC_EXECUTABLE}
         -DKDE_UIC_FILE:FILEPATH=${_tmp_FILE}
         -DKDE_UIC_H_FILE:FILEPATH=${_header}
         -DKDE_UIC_BASENAME:STRING=${_basename}
         -P ${_ki18n_uic_script}
         MAIN_DEPENDENCY ${_tmp_FILE}
      )
      list(APPEND ${_sources} ${_header})
   endforeach (_current_FILE)
endmacro (KI18N_WRAP_UI)

# KI18N_INSTALL(podir)
# Search for .po files and scripting modules and install them to the standard
# location.
#
# This is a convenience function which relies on the following directory
# structure:
#
#  <podir>/
#    <lang>/
#      scripts/
#        <domain>/
#          *.js
#      *.po
#
# .po files are passed to build-pofiles.cmake
#
# .js files are installed using build-tsfiles.cmake
#
# For example, given the following directory structure:
#
#  po/
#    fr/
#      scripts/
#        kfoo/
#          kfoo.js
#      kfoo.po
#
# KI18N_INSTALL(po) does the following:
# - Compiles kfoo.po into kfoo.mo and installs it in
#   ${LOCALE_INSTALL_DIR}/fr/LC_MESSAGES or share/locale/fr/LC_MESSAGES if
#   ${LOCALE_INSTALL_DIR} is not set.
# - Installs kfoo.js in ${LOCALE_INSTALL_DIR}/fr/LC_SCRIPTS/kfoo
#
# KI18N_INSTALL_TS_FILES() is deprecated, use KI18N_INSTALL()
#
function(KI18N_INSTALL podir)
    if (NOT LOCALE_INSTALL_DIR)
        set(LOCALE_INSTALL_DIR share/locale)
    endif()

    get_filename_component(dirname ${LOCALE_INSTALL_DIR} NAME)
    get_filename_component(destname ${LOCALE_INSTALL_DIR} DIRECTORY)

    get_filename_component(absolute_podir ${podir} ABSOLUTE)
    string(MD5 pathmd5 ${absolute_podir})

    add_custom_target(pofiles-${pathmd5} ALL
        COMMENT "Generating mo..."
        COMMAND ${CMAKE_COMMAND}
                -DGETTEXT_MSGFMT_EXECUTABLE=${GETTEXT_MSGFMT_EXECUTABLE}
                -DCOPY_TO=${CMAKE_CURRENT_BINARY_DIR}/${dirname}
                -DPO_DIR=${absolute_podir}
                -P ${KF5I18n_DIR}/build-pofiles.cmake
    )
    add_custom_target(tsfiles-${pathmd5} ALL
        COMMENT "Generating ts..."
        COMMAND ${CMAKE_COMMAND}
                -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
                -D_ki18n_pmap_compile_script=${_ki18n_pmap_compile_script}
                -DCOPY_TO=${CMAKE_CURRENT_BINARY_DIR}/${dirname}
                -DPO_DIR=${absolute_podir}
                -P ${KF5I18n_DIR}/build-tsfiles.cmake
    )

    if (NOT TARGET pofiles)
        add_custom_target(pofiles)
        add_custom_target(tsfiles)
    endif()
    add_dependencies(pofiles pofiles-${pathmd5})
    add_dependencies(tsfiles tsfiles-${pathmd5})

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dirname})
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dirname} DESTINATION ${destname})
endfunction()

function(ki18n_install_ts_files _lang)
    message(AUTHOR_WARNING "KI18N_INSTALL_TS_FILES is deprecated!")
    ki18n_install(${_lang})
endfunction()
