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

#include <ktranscriptcleantest.h>

#include <QtTest>

#include <QDebug>

#include <ktranscript_p.h>

QTEST_MAIN(KTranscriptCleanTest)

KTranscriptCleanTest::KTranscriptCleanTest()
    : m_transcript(nullptr)
{
}

void KTranscriptCleanTest::init()
{
    m_transcript = autotestCreateKTranscriptImp();
}

void KTranscriptCleanTest::cleanup()
{
    autotestDestroyKTranscriptImp();
    m_transcript = nullptr;
}

void KTranscriptCleanTest::test_data()
{
    QTest::addColumn<QVariantList>("argv");
    QTest::addColumn<bool>("fallsBack");
    QTest::addColumn<QString>("expected");

    // Example test case, replace with first clean-slate test
    QTest::newRow("test_basic")
        << (QVariantList() << "test_basic" << "foo")
        << false
        << "foo bar";
}

void KTranscriptCleanTest::test()
{
    QFETCH(QVariantList, argv);
    QFETCH(bool, fallsBack);
    QFETCH(QString, expected);

    QString language = "fr";
    QString country = "fr";
    QString msgctxt = "a-context";
    QHash<QString, QString> dynamicContext;
    dynamicContext.insert("origin", "neverwhere");
    QString msgid = "source-text";
    QStringList subs;
    subs << "10" << "qwyx";
    QList<QVariant> values;
    values << 10 << "qwyx";
    QString ordinaryTranslation = "translated-text";

    QString testJs = QFINDTESTDATA("test.js");
    QList<QStringList> modules;
    modules << (QStringList() << testJs << language);

    QString error;
    bool fallback;
    QString result = m_transcript->eval(
                         argv, language, country,
                         msgctxt, dynamicContext, msgid,
                         subs, values, ordinaryTranslation,
                         modules, error, fallback);

    if (!error.isEmpty()) {
        QFAIL(qPrintable(error));
    }
    if (!fallsBack) {
        QVERIFY(!fallback);
        QCOMPARE(result, expected);
    } else {
        QVERIFY(fallback);
    }
}
