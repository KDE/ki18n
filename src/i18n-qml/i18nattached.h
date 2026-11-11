// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

#pragma once

#include <QObject>
#include <QtQmlIntegration>

#include "klocalizedqmlcontext.h"

/*!
    \internal
    Assistive class to expose KLocalizedQmlContext as a QML attached type.
    This is used over a singleton because we need to resolve strings in their
    relative QQmlContext, but singletons are per-engine and have no readily
    available way to obtain the current execution context.
*/
class I18nAttached : public QObject
{
    Q_OBJECT
    QML_ATTACHED(KLocalizedQmlContext)
    QML_NAMED_ELEMENT(KI18n)
public:
    static KLocalizedQmlContext *qmlAttachedProperties(QObject *object);
};
