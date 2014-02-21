/*
Copyright 2014 Kevin Krammer <krammer@kde.org>

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

#ifndef KTRANSCRIPTCLEANTEST_H
#define KTRANSCRIPTCLEANTEST_H

#include <QLibrary>
#include <QObject>

class KTranscript;

/**
 * @brief Test the KTranscript implementation class
 *
 * Runs tests on the KTranscriptImp scripting facility.
 *
 * The main difference to the test ktranscripttest is that it
 * creates a new instance of KTranscriptImp for each test while
 * the main test re-uses one instance due to internal use of
 * Q_GLOBAL_STATIC
 *
 * Test that require a "clean slate" can be added here, tests that do not
 * should be added to both.
 */
class KTranscriptCleanTest : public QObject
{
    Q_OBJECT
public:
    KTranscriptCleanTest();

private Q_SLOTS:
    void init();
    void cleanup();

    void test_data();
    void test();

private:
    QLibrary m_library;
    KTranscript *m_transcript;
};

#endif /* KTRANSCRIPTCLEANTEST_H */
