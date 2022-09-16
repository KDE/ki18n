configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

add_library(KF5I18n)
add_library(KF5::I18n ALIAS KF5I18n)

target_sources(KF5I18n PRIVATE
    klocalizedstring.cpp
    klocalizedtranslator.cpp
    kcatalog.cpp
    kuitmarkup.cpp
    common_helpers.cpp
    klocalizedcontext.cpp
    main.cpp
)

ecm_qt_declare_logging_category(KF5I18n
    HEADER ki18n_logging.h
    IDENTIFIER KI18N
    CATEGORY_NAME kf.i18n
    OLD_CATEGORY_NAMES kf5.ki18n
    DESCRIPTION "KI18n"
    EXPORT KI18N
)

ecm_qt_declare_logging_category(KF5I18n
    HEADER ki18n_logging_kuit.h
    IDENTIFIER KI18N_KUIT
    CATEGORY_NAME kf.i18n.kuit
    OLD_CATEGORY_NAMES kf5.ki18n.kuit
    DESCRIPTION "KI18n KUIT"
    EXPORT KI18N
)

ecm_generate_export_header(KF5I18n
    BASE_NAME KI18n
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF5I18n INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KI18n>")
target_include_directories(KF5I18n PRIVATE ${LibIntl_INCLUDE_DIRS})

target_link_libraries(KF5I18n PUBLIC Qt${QT_MAJOR_VERSION}::Core)
# This is only required for platforms which don't use glibc (with glibc LibIntl_LIBRARIES will be empty)
target_link_libraries(KF5I18n PRIVATE ${LibIntl_LIBRARIES})
if (ANDROID)
    if (QT_MAJOR_VERSION EQUAL "5")
        target_link_libraries(KF5I18n PRIVATE Qt5::AndroidExtras)
    endif()
    target_link_libraries(KF5I18n PRIVATE android)
endif()
target_compile_options(KF5I18n PRIVATE -DTRANSLATION_DOMAIN=\"ki18n5\")

set_target_properties(KF5I18n PROPERTIES VERSION   ${KI18N_VERSION}
                                         SOVERSION ${KI18N_SOVERSION}
                                         EXPORT_NAME I18n
)

install(TARGETS KF5I18n EXPORT KF5I18nTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_headers(KI18n_HEADERS
  HEADER_NAMES
    KLazyLocalizedString
    KLocalizedContext
    KLocalizedString
    KLocalizedTranslator
    KuitMarkup
    KuitSetup
    REQUIRED_HEADERS KI18n_HEADERS
)

install(FILES
    ${KI18n_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/ki18n_export.h
    DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF}/KI18n" COMPONENT Devel
)


### ktranscript plugin
if (BUILD_WITH_QML)

add_library(ktranscript MODULE)

target_sources(ktranscript PRIVATE
    ktranscript.cpp
    common_helpers.cpp
)
generate_export_header(ktranscript BASE_NAME KTranscript)
target_link_libraries(ktranscript PRIVATE Qt${QT_MAJOR_VERSION}::Qml Qt${QT_MAJOR_VERSION}::Core)

install(TARGETS ktranscript DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION})

endif()

if (BUILD_QCH)
    ecm_add_qch(
        KF5I18n_QCH
        NAME KI18n
        BASE_NAME KF5I18n
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KI18n_HEADERS}
            "${CMAKE_SOURCE_DIR}/docs/programmers-guide.md"
            "${CMAKE_SOURCE_DIR}/docs/translators-guide.md"
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KI18N_EXPORT
            KI18N_DEPRECATED_EXPORT
            KI18N_DEPRECATED
            "KI18N_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KI18n LIB_NAME KF5I18n DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KI18n)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
