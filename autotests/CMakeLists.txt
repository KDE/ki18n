remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMAddTests)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Qml Concurrent)

ecm_add_test(ki18ndeclarativetest.cpp
    TEST_NAME "ki18n-declarativetest"
    LINK_LIBRARIES Qt5::Test Qt5::Qml KF5::I18n
)

ecm_add_test(klocalizedstringtest.cpp
    TEST_NAME "ki18n-klocalizedstringtest"
    LINK_LIBRARIES Qt5::Test Qt5::Concurrent KF5::I18n ${LibIntl_LIBRARIES}
)
# klocalizedstringtest needs the libintl include path
target_include_directories(ki18n-klocalizedstringtest PRIVATE ${LibIntl_INCLUDE_DIRS})

if (TARGET ktranscript)
ecm_add_test(ktranscripttest.cpp testhelpers.cpp
    TEST_NAME "ki18n-ktranscripttest"
    LINK_LIBRARIES Qt5::Test KF5::I18n
)
# ktranscripttest needs the path to ktranscript in the build dir: it cannot rely
# on the installed version since it must be able to run *before* the plugin is
# installed.
target_compile_definitions(ki18n-ktranscripttest PRIVATE "KTRANSCRIPT_PATH=\"$<TARGET_FILE:ktranscript>\"")

# ktranscriptcleantest needs to directly compile ktranscript.cpp with an addition DEFINE
ecm_add_test(ktranscriptcleantest.cpp ../src/ktranscript.cpp ../src/common_helpers.cpp
    TEST_NAME ki18n-ktranscriptcleantest
    LINK_LIBRARIES Qt5::Script Qt5::Test Qt5::Concurrent KF5::I18n
)
target_compile_definitions(ki18n-ktranscriptcleantest PRIVATE "KTRANSCRIPT_TESTBUILD")
target_include_directories(ki18n-ktranscriptcleantest PRIVATE ..)
endif()
