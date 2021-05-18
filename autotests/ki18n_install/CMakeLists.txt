project(ki18n_install)

find_package(KF5I18n)

ki18n_install(${CMAKE_CURRENT_SOURCE_DIR}/po)
