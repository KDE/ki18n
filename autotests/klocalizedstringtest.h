/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2005 Thomas Braxton <brax108@cox.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KLOCALIZEDSTRINGTEST_H
#define KLOCALIZEDSTRINGTEST_H

#include <ki18n_export.h>
#include <QObject>
#include <QTemporaryDir>

class KLocalizedStringTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void correctSubs();
    void wrongSubs();
    void removeAcceleratorMarker();
    void miscMethods();
    void translateToFrenchLowlevel();
    void translateToFrench();
#if KI18N_ENABLE_DEPRECATED_SINCE(5, 0)
    void translateQt();
#endif
    void addCustomDomainPath();

    void testThreads();

    void testLocalizedTranslator();
    void semanticTags();
    void setFormatForMarker();
    void multipleLanguages();
    void untranslatedText();
    void brokenTags();

private:
    bool m_hasFrench;
    bool m_hasCatalan;
    QTemporaryDir m_tempDir;
    bool compileCatalogs(const QStringList &catalogs, const QDir &dataDir, const QString &language);
};

#endif // KLOCALIZEDSTRINGTEST_H
