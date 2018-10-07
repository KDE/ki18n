cmake_minimum_required(VERSION 3.0)

set(KF5_VERSION "5.51.0") # handled by release scripts
project(KI18n VERSION ${KF5_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.51.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${KI18n_SOURCE_DIR}/cmake ${KI18n_BINARY_DIR}/cmake)


include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)


ecm_setup_version(
  PROJECT
  VARIABLE_PREFIX KI18N
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/ki18n_version.h"
  PACKAGE_VERSION_FILE "${CMAKE_BINARY_DIR}/cmake/KF5I18nConfigVersion.cmake"
  SOVERSION 5)

# Dependencies
set(REQUIRED_QT_VERSION 5.8.0)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core)

option(BUILD_WITH_QML "Build with support for scripted translations (recommended)" ON)
if (BUILD_WITH_QML)
   find_package(Qt5Qml ${REQUIRED_QT_VERSION} CONFIG REQUIRED)
endif()

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

find_package(LibIntl)
set_package_properties(LibIntl PROPERTIES TYPE REQUIRED
	URL "http://gnuwin32.sourceforge.net/packages/libintl.htm"
	PURPOSE "Needed for building KI18n unless glibc is the system libc implementation"
)

# KF5I18nMacros.cmake only needs to know the python executable path.
# Due to CMake caching the variables, and KF5I18nMacros being included by KF5I18nConfig,
# we have to hardcode the PYTHON_EXECUTABLE path or anything depending on KF5I18n
# would be unable to find another Python version.
find_package(PythonInterp REQUIRED)
configure_file(cmake/KF5I18nMacros.cmake.in ${KI18n_BINARY_DIR}/cmake/KF5I18nMacros.cmake @ONLY)
# Needed to build the ki18n translations and run the autotests
file(COPY ${KI18n_SOURCE_DIR}/cmake/build-pofiles.cmake DESTINATION ${KI18n_BINARY_DIR}/cmake)
file(COPY ${KI18n_SOURCE_DIR}/cmake/build-tsfiles.cmake DESTINATION ${KI18n_BINARY_DIR}/cmake)
file(COPY ${KI18n_SOURCE_DIR}/cmake/ts-pmap-compile.py DESTINATION ${KI18n_BINARY_DIR}/cmake)
file(COPY ${KI18n_SOURCE_DIR}/cmake/kf5i18nuic.cmake DESTINATION ${KI18n_BINARY_DIR}/cmake)

# Create dummy file to execute find_package(KF5I18n) within autotests/ki18n_install
file(WRITE ${CMAKE_BINARY_DIR}/cmake/KF5I18nTargets.cmake "")

include(${KI18n_BINARY_DIR}/cmake/KF5I18nMacros.cmake)

remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
if(MSVC)
    remove_definitions(-DQT_STRICT_ITERATORS)
endif()

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()
add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/src/config.h")

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5I18n")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5I18n_QCH
        FILE "${CMAKE_BINARY_DIR}/cmake/KF5I18nQchTargets.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5I18nQchTargets.cmake\")")
endif()

configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/KF5I18nConfig.cmake.in"
                              "${CMAKE_BINARY_DIR}/cmake/KF5I18nConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS  KF5_INCLUDE_INSTALL_DIR LIB_INSTALL_DIR CMAKE_INSTALL_PREFIX)

install(FILES  "${CMAKE_BINARY_DIR}/cmake/KF5I18nConfig.cmake"
               "${CMAKE_BINARY_DIR}/cmake/KF5I18nConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KF5I18nTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5I18nTargets.cmake NAMESPACE KF5::)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ki18n_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel)

install( FILES
    ${KI18n_BINARY_DIR}/cmake/KF5I18nMacros.cmake
    cmake/kf5i18nuic.cmake
    cmake/build-pofiles.cmake
    cmake/build-tsfiles.cmake
    cmake/ts-pmap-compile.py
    DESTINATION ${CMAKECONFIG_INSTALL_DIR} COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
