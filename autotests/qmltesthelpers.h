// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

#pragma once

#include <chrono>

#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTest>

#include <KLocalizedQmlContext>
#include <KLocalizedString>

namespace QmlTestHelpers
{

inline QObject *loadQml(QQmlEngine *engine, const QString &qmlFile)
{
    auto *applicationEngine = qobject_cast<QQmlApplicationEngine *>(engine);
    Q_ASSERT(applicationEngine);
    applicationEngine->load(QUrl::fromLocalFile(QFINDTESTDATA(qmlFile)));
    if (applicationEngine->hasError() || applicationEngine->rootObjects().isEmpty()) {
        return nullptr;
    }
    // Return the most recently loaded root (load() appends to rootObjects()).
    return applicationEngine->rootObjects().constLast();
}

inline std::shared_ptr<QObject> createQml(QQmlEngine *engine, QQmlContext *context, const QString &qmlFile)
{
    QQmlComponent component(engine, QUrl::fromLocalFile(QFINDTESTDATA(qmlFile)));
    if (!component.isReady()) {
        qWarning() << "Failed to load" << qmlFile << ":" << component.errorString();
        return nullptr;
    }
    return std::shared_ptr<QObject>(context ? component.create(context) : component.create());
}

inline KLocalizedQmlContext *attachedContextOf(QObject *qmlObject, const char *property)
{
    return qobject_cast<KLocalizedQmlContext *>(qmlObject->property(property).value<QObject *>());
}

inline QString invokeString(QObject *qmlObject, const char *method)
{
    QVariant result;
    if (!QMetaObject::invokeMethod(qmlObject, method, Q_RETURN_ARG(QVariant, result))) {
        return {};
    }
    return result.toString();
}

inline bool deployFrenchCatalogs(QTemporaryDir &tempDir)
{
    using namespace Qt::Literals;
    using namespace std::chrono_literals;

    const QString msgfmt = QStandardPaths::findExecutable(u"msgfmt"_s);
    if (msgfmt.isEmpty() || !tempDir.isValid()) {
        return false;
    }

    const QString lcMessagesDir = u"locale/fr/LC_MESSAGES"_s;
    QDir dataDir(tempDir.path());
    if (!dataDir.mkpath(lcMessagesDir)) {
        return false;
    }

    const QStringList poFiles = {u"po/fr/ki18n-test.po"_s, u"po/fr/ki18n-test2.po"_s};
    for (const QString &poFile : poFiles) {
        const QString poPath = QFINDTESTDATA(poFile);
        if (poPath.isEmpty()) {
            return false;
        }
        const QString domain = QFileInfo(poPath).baseName();
        QProcess process;
        process.start(msgfmt, {poPath, u"-o"_s, dataDir.filePath(lcMessagesDir + u'/' + domain + u".mo"_s)});
        if (!process.waitForFinished((10000ms).count()) || process.exitCode() != 0) {
            return false;
        }
    }

    const QByteArray dataDirs = qgetenv("XDG_DATA_DIRS") + ":" + QFile::encodeName(tempDir.path());
    qputenv("XDG_DATA_DIRS", dataDirs);
    KLocalizedString::setLanguages({u"fr"_s});
    return true;
}

} // namespace QmlTestHelpers
