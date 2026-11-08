// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

#include <QDir>
#include <QProcess>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtQuickTest/quicktest.h>

#include <KLocalizedQmlContext>
#include <KLocalizedString>

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
        m_hasFrench =
            compileCatalog(QFINDTESTDATA("po/fr/ki18n-test.po"), u"ki18n-test"_s) && compileCatalog(QFINDTESTDATA("po/fr/ki18n-test2.po"), u"ki18n-test2"_s);
        if (m_hasFrench) {
            const QByteArray dataDirs = qgetenv("XDG_DATA_DIRS") + ":" + QFile::encodeName(m_tempDir.path());
            qputenv("XDG_DATA_DIRS", dataDirs);
            KLocalizedString::setLanguages({u"fr"_s});
        } else {
            qWarning() << "French test catalogs not usable, translation assertions will be skipped";
        }

        auto ctx = KLocalization::setupLocalizedContext(engine);
        ctx->setTranslationDomain(u"ki18n-test"_s);
        engine->rootContext()->setContextProperty(u"hasFrenchCatalogs"_s, m_hasFrench);
    }

private:
    bool compileCatalog(const QString &poPath, const QString &domain)
    {
        if (poPath.isEmpty() || !QFile::exists(poPath)) {
            qWarning() << "po file not found:" << poPath;
            return false;
        }
        const QString lcMessages = QStringLiteral("locale/fr/LC_MESSAGES");
        QDir dataDir(m_tempDir.path());
        if (!dataDir.mkpath(lcMessages)) {
            qWarning() << "Failed to create locale subdirectory in" << m_tempDir.path();
            return false;
        }
        const QString msgfmt = QStandardPaths::findExecutable(QStringLiteral("msgfmt"));
        if (msgfmt.isEmpty()) {
            qWarning() << "msgfmt not found in path";
            return false;
        }
        const QString moPath = m_tempDir.path() + u'/' + lcMessages + u'/' + domain + u".mo"_s;
        QProcess process;
        process.start(msgfmt, {poPath, u"--output-file"_s, moPath});
        if (!process.waitForFinished(10000) || process.exitCode() != 0) {
            qWarning() << "msgfmt could not compile" << poPath;
            return false;
        }
        return QFile::exists(moPath);
    }

    QTemporaryDir m_tempDir;
    bool m_done = false;
    bool m_hasFrench = false;
};

QUICK_TEST_MAIN_WITH_SETUP(ki18nsingletontest, KI18nSingletonTestSetup)

#include "ki18nsingletontest.moc"
