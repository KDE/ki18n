// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

#include <QQmlEngine>
#include <QTemporaryDir>
#include <QtQuickTest/quicktest.h>

#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include "qmltesthelpers.h"

using namespace Qt::Literals;

class KI18nSingletonTestSetup : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        if (m_done) {
            return;
        }
        m_done = true;

        KLocalizedString::setApplicationDomain(QByteArrayLiteral("ki18n-test"));
        m_hasFrench = QmlTestHelpers::deployFrenchCatalogs(m_tempDir);
        if (!m_hasFrench) {
            qWarning() << "French test catalogs not usable, translation assertions will be skipped";
        }

        auto ctx = KLocalization::setupLocalizedContext(engine);
        ctx->setTranslationDomain(u"ki18n-test"_s);
        engine->rootContext()->setContextProperty(u"hasFrenchCatalogs"_s, m_hasFrench);
    }

private:
    QTemporaryDir m_tempDir;
    bool m_done = false;
    bool m_hasFrench = false;
};

QUICK_TEST_MAIN_WITH_SETUP(ki18nsingletontest, KI18nSingletonTestSetup)

#include "ki18nsingletontest.moc"
