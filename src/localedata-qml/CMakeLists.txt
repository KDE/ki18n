# SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_library(ki18nlocaledataqmlplugin ki18nlocaledataqmlplugin.cpp)
target_link_libraries(ki18nlocaledataqmlplugin PRIVATE
    Qt${QT_MAJOR_VERSION}::Qml
    KF5::I18nLocaleData
)

# make examples work without installation
set_property(TARGET ki18nlocaledataqmlplugin PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/i18n/localeData)
file(COPY qmldir DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/i18n/localeData)

install(TARGETS ki18nlocaledataqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/i18n/localeData)
install(FILES qmldir ${qml_SRC} DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/i18n/localeData)
