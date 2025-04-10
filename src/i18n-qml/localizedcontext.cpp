// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#include "localizedcontext.h"

#include <QQmlContext>
#include <QQmlEngine>

KLocalizedQmlContext *LocalizedContext::create(QQmlEngine *qmlEngine, [[maybe_unused]] QJSEngine *jsEngine)
{
    auto context = qobject_cast<KLocalizedQmlContext *>(qmlEngine->rootContext()->contextObject());
    Q_ASSERT(context);
    return context;
}
