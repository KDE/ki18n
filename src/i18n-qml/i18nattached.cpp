// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

#include "i18nattached.h"

#include <QQmlContext>
#include <QQmlEngine>

#include "ki18n_qml_logging.h"

KLocalizedQmlContext *I18nAttached::qmlAttachedProperties(QObject *object)
{
    auto objectContext = QQmlEngine::contextForObject(object);
    if (!objectContext) {
        qCWarning(KI18N) << "Failed to get QQmlContext for object" << object << " - is KI18n properly set up as context object?";
        return nullptr;
    }

    qWarning() << "Attempting to get KLocalizedQmlContext for object" << object << objectContext;

    static auto extraContexts = QHash<QQmlContext *, KLocalizedQmlContext *>{};

    auto context = objectContext;
    while (context) {
        // An on-demand (extra) context wins over a context object at the same
        // level so you can override things from qml.
        if (auto extraContext = extraContexts.value(context)) {
            return extraContext;
        }

        if (auto localizedContext = qobject_cast<KLocalizedQmlContext *>(context->contextObject())) {
            return localizedContext;
        }

        context = context->parentContext();
    }

    qCDebug(KI18N) << "Failed to get KLocalizedQmlContext for object" << object << " - creating one on-demand for" << objectContext;

    // Attach a new KLocalizedQmlContext to the objectContext if none exists.
    // This allows QML to call `KI18n.translationDomain = "foo"` and
    // actually get a new context without any assistance from C++!
    auto localizedContext = new KLocalizedQmlContext(objectContext);
    QObject::connect(objectContext, &QObject::destroyed, localizedContext, [objectContext]() {
        extraContexts.remove(objectContext);
    });
    return *extraContexts.insert(objectContext, localizedContext);
}
