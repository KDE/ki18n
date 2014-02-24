/*
Copyright 2013 Aurélien Gâteau <agateau@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef KTRANSCRIPTTEST_H
#define KTRANSCRIPTTEST_H

#include <QLibrary>
#include <QObject>

class KTranscript;

/**
 * @brief Test the KTranscript plugin
 *
 * Loads the KTranscript plugin and runs tests on its scripting capabilities.
 *
 * The main difference to the test ktranscriptcleantest is that it does so using
 * a single instance of the KTranscript implementation due to the plugin
 * using Q_GLOBAL_STATIC
 *
 * ktranscriptcleantest on the other hand creates and destroys the instance between
 * tests. Test that require a "clean slate" can be added there.
 */
class KTranscriptTest : public QObject
{
    Q_OBJECT
public:
    KTranscriptTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void test_data();
    void test();

private:
    QLibrary m_library;
    KTranscript *m_transcript;
};

#endif /* KTRANSCRIPTTEST_H */
