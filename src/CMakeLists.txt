set(libki18n_SRCS
    klocalizedstring.cpp
    klocalizedtranslator.cpp
    kcatalog.cpp
    kuitmarkup.cpp
    common_helpers.cpp
    main.cpp
)

add_library(KF5I18n ${libki18n_SRCS})
generate_export_header(KF5I18n BASE_NAME KI18n)
add_library(KF5::I18n ALIAS KF5I18n)

target_include_directories(KF5I18n INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KI18n>")
target_include_directories(KF5I18n PRIVATE ${LibIntl_INCLUDE_DIRS})

target_link_libraries(KF5I18n PUBLIC Qt5::Core)
# This is only required for platforms which don't use glibc (with glibc LibIntl_LIBRARIES will be empty)
target_link_libraries(KF5I18n PRIVATE ${LibIntl_LIBRARIES})


set_target_properties(KF5I18n PROPERTIES VERSION   ${KI18N_VERSION_STRING}
                                         SOVERSION ${KI18N_SOVERSION}
                                         EXPORT_NAME I18n
)

install(TARGETS KF5I18n EXPORT KF5I18nTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_headers(KI18n_HEADERS
  HEADER_NAMES
    KLocalizedString
    KLocalizedTranslator
    KuitMarkup
    KuitSetup
    REQUIRED_HEADERS KI18n_HEADERS
)

install(FILES
    ${KI18n_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/ki18n_export.h
    DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF5}/KI18n" COMPONENT Devel
)

### ktranscript plugin
if (BUILD_WITH_QTSCRIPT)

set(ktranscript_LIB_SRCS
    ktranscript.cpp
    common_helpers.cpp
)
add_library(ktranscript MODULE ${ktranscript_LIB_SRCS})
generate_export_header(ktranscript BASE_NAME KTranscript)
target_link_libraries(ktranscript PRIVATE Qt5::Script Qt5::Core)

install(TARGETS ktranscript DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5)

endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KI18n LIB_NAME KF5I18n DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KI18n)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
