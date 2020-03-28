/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2015 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QCoreApplication>
#include <QLocale>
#include <QLibraryInfo>
#include <QTranslator>

// load global Qt translation, needed in KDE e.g. by lots of builtin dialogs (QColorDialog, QFontDialog) that we use
static bool loadTranslation(const QString &localeName)
{
    QTranslator *translator = new QTranslator(QCoreApplication::instance());
    if (!translator->load(QLocale(localeName), QStringLiteral("qt_"), QString(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        delete translator;
        return false;
    }
    QCoreApplication::instance()->installTranslator(translator);
    return true;
}

static void load()
{
    // The way Qt translation system handles plural forms makes it necessary to
    // have a translation file which contains only plural forms for `en`. That's
    // why we load the `en` translation unconditionally, then load the
    // translation for the current locale to overload it.
    loadTranslation(QStringLiteral("en"));

    QLocale locale = QLocale::system();
    if (locale.name() != QStringLiteral("en")) {
        if (!loadTranslation(locale.name())) {
            loadTranslation(locale.bcp47Name());
        }
    }
}

Q_COREAPP_STARTUP_FUNCTION(load)
