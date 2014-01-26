remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMMarkAsTest)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Concurrent)

macro(KI18N_UNIT_TESTS)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} Qt5::Test Qt5::Concurrent KF5::I18n)
        add_test(ki18n-${_testname} ${_testname})
        ecm_mark_as_test(${_testname})
    endforeach()
endmacro()

ki18n_unit_tests(
    klocalizedstringtest
    ktranscripttest
)

# ktranscripttest needs the path to ktranscript in the build dir: it cannot rely
# on the installed version since it must be able to run *before* the plugin is
# installed.
target_compile_definitions(ktranscripttest PRIVATE "KTRANSCRIPT_PATH=\"$<TARGET_FILE:ktranscript>\"")

target_include_directories(klocalizedstringtest PRIVATE ${LibIntl_INCLUDE_DIRS})
target_link_libraries(klocalizedstringtest ${LibIntl_LIBRARIES})
