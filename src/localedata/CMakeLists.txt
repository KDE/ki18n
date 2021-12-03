# SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config-localedata.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config-localedata.h")

add_library(KF5I18nLocaleData)
add_library(KF5::I18nLocaleData ALIAS KF5I18nLocaleData)

target_sources(KF5I18nLocaleData PRIVATE
    isocodes.cpp
    isocodescache.cpp
    kcountry.cpp
    kcountrysubdivision.cpp
    ktimezone.cpp
    spatial_index.cpp
    spatial_index_entry.cpp
    spatial_index_property.cpp
    timezonedata.cpp
)

ecm_generate_export_header(KF5I18nLocaleData
    BASE_NAME KI18nLocaleData
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

ecm_qt_declare_logging_category(KF5I18nLocaleData
    HEADER logging.h
    IDENTIFIER KI18NLD
    CATEGORY_NAME kf.i18n.localeData
    DESCRIPTION "KI18n Locale Data"
    EXPORT KI18N
)

target_include_directories(KF5I18nLocaleData INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KI18nLocaleData>")

target_link_libraries(KF5I18nLocaleData PUBLIC Qt${QT_MAJOR_VERSION}::Core)
target_link_libraries(KF5I18nLocaleData PRIVATE Qt${QT_MAJOR_VERSION}::CorePrivate KF5I18n)
target_compile_options(KF5I18n PRIVATE -DTRANSLATION_DOMAIN=\"ki18n5\")

set_target_properties(KF5I18nLocaleData PROPERTIES
    VERSION ${KI18N_VERSION}
    SOVERSION ${KI18N_SOVERSION}
    EXPORT_NAME I18nLocaleData
)

install(TARGETS KF5I18nLocaleData EXPORT KF5I18nTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_headers(KI18nLocaleData_HEADERS
  HEADER_NAMES
    KCountry
    KCountrySubdivision
    KTimeZone
    REQUIRED_HEADERS KI18nLocaleData_HEADERS
)

install(FILES
    ${KI18nLocaleData_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/ki18nlocaledata_export.h
    DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF}/KI18nLocaleData" COMPONENT Devel
)
