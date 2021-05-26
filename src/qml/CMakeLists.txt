# SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_library(ki18nqmlplugin ki18nqmlplugin.cpp)
target_link_libraries(ki18nqmlplugin PRIVATE
    Qt5::Qml
    KF5::I18n
)

# make examples work without installation
set_property(TARGET ki18nqmlplugin PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/ki18n)
file(COPY qmldir DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/ki18n)

install(TARGETS ki18nqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/ki18n)
install(FILES qmldir ${qml_SRC} DESTINATION ${QML_INSTALL_DIR}/org/kde/ki18n)
