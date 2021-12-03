remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMAddTests)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Concurrent)

if (BUILD_WITH_QML)
ecm_add_test(ki18ndeclarativetest.cpp
    TEST_NAME "ki18n-declarativetest"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Qml KF5::I18n
)
endif()

ecm_add_test(klocalizedstringtest.cpp
    TEST_NAME "ki18n-klocalizedstringtest"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Concurrent KF5::I18n ${LibIntl_LIBRARIES}
)
# klocalizedstringtest needs the libintl include path
target_include_directories(ki18n-klocalizedstringtest PRIVATE ${LibIntl_INCLUDE_DIRS})

ecm_add_test(klazylocalizedstringtest.cpp
    TEST_NAME "ki18n-klazylocalizedstringtest"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::I18n
)

if (TARGET ktranscript)
ecm_add_test(ktranscripttest.cpp testhelpers.cpp
    TEST_NAME "ki18n-ktranscripttest"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::I18n
)
# ktranscripttest needs the path to ktranscript in the build dir: it cannot rely
# on the installed version since it must be able to run *before* the plugin is
# installed.
target_compile_definitions(ki18n-ktranscripttest PRIVATE "KTRANSCRIPT_PATH=\"$<TARGET_FILE:ktranscript>\"")

# ktranscriptcleantest needs to directly compile ktranscript.cpp with an addition DEFINE
ecm_add_test(ktranscriptcleantest.cpp ../src/i18n/ktranscript.cpp ../src/i18n/common_helpers.cpp
    TEST_NAME ki18n-ktranscriptcleantest
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Qml Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Concurrent KF5::I18n
)
target_compile_definitions(ki18n-ktranscriptcleantest PRIVATE "KTRANSCRIPT_TESTBUILD")
target_include_directories(ki18n-ktranscriptcleantest PRIVATE ..)
endif()

add_test(ki18n_install ${CMAKE_CTEST_COMMAND}
    --build-and-test
    "${CMAKE_CURRENT_SOURCE_DIR}/ki18n_install"
    "${CMAKE_CURRENT_BINARY_DIR}/ki18n_install"
    --build-generator ${CMAKE_GENERATOR}
    --build-makeprogram ${CMAKE_MAKE_PROGRAM}
    --build-target install
    --build-options
        "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
        "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/ki18n_install/destdir"
        "-DKF5I18n_DIR=${CMAKE_BINARY_DIR}/cmake"
    --test-command ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/ki18n_install/test.cmake")

ecm_add_test(kcountrytest.cpp LINK_LIBRARIES KF5::I18nLocaleData Qt${QT_MAJOR_VERSION}::Test)
ecm_add_test(kcountrysubdivisiontest.cpp LINK_LIBRARIES KF5::I18nLocaleData Qt${QT_MAJOR_VERSION}::Test)
ecm_add_test(ktimezonetest.cpp LINK_LIBRARIES KF5::I18nLocaleData Qt${QT_MAJOR_VERSION}::Test)
