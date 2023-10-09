# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

ecm_add_qml_module(ki18nqmlplugin URI "org.kde.i18n" GENERATE_PLUGIN_SOURCE)

target_sources(ki18nqmlplugin PRIVATE
    attached.cpp
    attached.h
)

ecm_qt_declare_logging_category(ki18nqmlplugin
    HEADER ki18nqml_logging.h
    IDENTIFIER KI18N_QML
    CATEGORY_NAME kf.i18n.qml
    DESCRIPTION "KI18n (QML)"
    EXPORT KI18N
)

# Can not mark it as QT_QML_INTERNAL_TYPE, otherwise this module won't be able
# to load it from C++ either.
ecm_target_qml_sources(ki18nqmlplugin SOURCES
    KI18nImpl.qml
)

target_link_libraries(ki18nqmlplugin PRIVATE
    Qt6::Qml
    KF6::I18n
)

ecm_finalize_qml_module(ki18nqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR})
