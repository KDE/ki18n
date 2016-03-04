cmake_minimum_required(VERSION 2.8.12)

project(KI18n)

# ECM setup
include(FeatureSummary)
find_package(ECM 5.20.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

set(KF5_VERSION "5.20.0") # handled by release scripts

ecm_setup_version(
  ${KF5_VERSION}
  VARIABLE_PREFIX KI18N
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/ki18n_version.h"
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5I18nConfigVersion.cmake"
  SOVERSION 5)

# Dependencies
set(REQUIRED_QT_VERSION "5.3.0")

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core)

option(BUILD_WITH_QTSCRIPT "Build with support for scripted translations (recommended)" ON)
if (BUILD_WITH_QTSCRIPT)
   find_package(Qt5Script ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
endif()

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(LibIntl)
set_package_properties(LibIntl PROPERTIES TYPE REQUIRED
	URL "http://gnuwin32.sourceforge.net/packages/libintl.htm"
	PURPOSE "Needed for building KI18n unless glibc is the system libc implementation"
)

include(cmake/KF5I18NMacros.cmake)

remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
if(MSVC)
    remove_definitions(-DQT_STRICT_ITERATORS)
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"ki18n5\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()
add_subdirectory(src)
add_subdirectory(autotests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5I18n")

ecm_configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/KF5I18nConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5I18nConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS  KF5_INCLUDE_INSTALL_DIR LIB_INSTALL_DIR CMAKE_INSTALL_PREFIX)

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5I18nConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5I18nConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KF5I18nTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5I18nTargets.cmake NAMESPACE KF5::)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ki18n_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel)

install( FILES
    cmake/KF5I18NMacros.cmake
    cmake/kf5i18nuic.cmake
    cmake/ts-pmap-compile.py
    DESTINATION ${CMAKECONFIG_INSTALL_DIR} COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
