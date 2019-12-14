/*  This file is part of the KI18N Framework
    Copyright (C) 2014 Kevin Krammer <krammer@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "testhelpers.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTest>

static const QString targetFileName = QStringLiteral("ktranscript.ini");

bool deployTestConfig()
{
    QStandardPaths::setTestModeEnabled(true);

    QDir configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    const QFileInfo targetFile = QFileInfo(configDir, targetFileName);

    if (!configDir.exists()) {
        if (!QDir::current().mkpath(configDir.absolutePath())) {
            qWarning() << "Failed to create config dir" << configDir.absolutePath();
            return false;
        }
    } else if (targetFile.exists()) {
        if (!configDir.remove(targetFileName)) {
            qWarning() << "Failed to remove existing test config file" << targetFile.absoluteFilePath();
            return false;
        }
    }

    QFile sourceFile(QFINDTESTDATA("ktranscript-test.ini"));
    if (!sourceFile.exists()) {
        qWarning() << "Could not locate test data file" << sourceFile.fileName();
        return false;
    }

    if (!sourceFile.copy(targetFile.absoluteFilePath())) {
        qWarning() << "Failed to copy test config file" << sourceFile.fileName()
                   << "to target location" << targetFile.absoluteFilePath();
        return false;
    }

    return true;
}

bool removeTestConfig()
{
    Q_ASSERT(QStandardPaths::isTestModeEnabled());

    QDir configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    if (!configDir.exists()) {
        return true;
    }

    return configDir.remove(targetFileName);
}
