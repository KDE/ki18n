/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KCountry>
#include <KCountrySubdivision>

#include <QObject>
#include <QStandardPaths>
#include <QTest>

void initEnvironment()
{
    qputenv("LANG", "fr_CH");
    QStandardPaths::setTestModeEnabled(true);
}

Q_CONSTRUCTOR_FUNCTION(initEnvironment)

class KCountrySubdivisionTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty()
    {
        KCountrySubdivision s;
        QVERIFY(!s.isValid());
        QVERIFY(!s.country().isValid());
        QVERIFY(s.code().isEmpty());
        QVERIFY(!s.parent().isValid());
        QVERIFY(s.subdivisions().isEmpty());
    }

    void testLookup()
    {
        auto s = KCountrySubdivision::fromCode(u"DE-BE");
        QVERIFY(s.isValid());
        QCOMPARE(s.country().alpha2(), QLatin1String("DE"));
        QVERIFY(!s.parent().isValid());
        QCOMPARE(s.name(), QStringLiteral("Berlin"));
        QCOMPARE(s.code(), QLatin1String("DE-BE"));

        s = KCountrySubdivision::fromCode("AT-9");
        QVERIFY(s.isValid());
        QCOMPARE(s.country().alpha2(), QLatin1String("AT"));
        QVERIFY(!s.parent().isValid());
        QCOMPARE(s.name(), QStringLiteral("Vienne"));
        QCOMPARE(s.code(), QLatin1String("AT-9"));

        s = KCountrySubdivision::fromCode(u"FR-ARA");
        QVERIFY(s.isValid());
        QCOMPARE(s.country().alpha2(), QLatin1String("FR"));
        QVERIFY(!s.parent().isValid());
        QCOMPARE(s.name(), QStringLiteral("Auvergne-Rhône-Alpes"));
        QCOMPARE(s.code(), QLatin1String("FR-ARA"));
        QCOMPARE(KCountrySubdivision::fromCode(u"FR-aRa"), s);

        s = KCountrySubdivision::fromCode(u"CZ-101");
        QVERIFY(s.isValid());
        QCOMPARE(s.country().alpha2(), QLatin1String("CZ"));
        QCOMPARE(s.code(), QLatin1String("CZ-101"));

        s = s.parent();
        QVERIFY(s.isValid());
        QCOMPARE(s.country().alpha2(), QLatin1String("CZ"));
        QVERIFY(!s.parent().isValid());
        QCOMPARE(s.code(), QLatin1String("CZ-10"));
        QCOMPARE(KCountrySubdivision::fromCode("cz-10"), s);

        QVERIFY(!KCountrySubdivision::fromCode(u"ZZ-ABC").isValid());
        QVERIFY(!KCountrySubdivision::fromCode("NZ-999").isValid());
        QVERIFY(!KCountrySubdivision::fromCode("AT-9-9").isValid());
        QVERIFY(!KCountrySubdivision::fromCode("").isValid());
        QVERIFY(!KCountrySubdivision::fromCode(nullptr).isValid());
    }

    void testSubList()
    {
        auto s = KCountrySubdivision::fromCode(u"CZ-10");
        QVERIFY(s.isValid());
        const auto l = s.subdivisions();
        QVERIFY(l.size() > 10);
        for (auto sub : l) {
            QVERIFY(sub.isValid());
            QCOMPARE(sub.parent(), s);
            QCOMPARE(sub.country().alpha2(), QLatin1String("CZ"));
        }

        s = KCountrySubdivision::fromCode(u"DE-BE");
        QCOMPARE(s.subdivisions().size(), 0);
    }

    void testTopList()
    {
        auto c = KCountry::fromAlpha2(u"CZ");
        QVERIFY(c.isValid());
        auto l = c.subdivisions();
        QVERIFY(l.size() > 10);
        for (auto s : l) {
            QVERIFY(s.isValid());
            QVERIFY(!s.parent().isValid());
            QCOMPARE(s.country(), c);
        }

        c = KCountry::fromAlpha2(u"DE");
        QVERIFY(c.isValid());
        l = c.subdivisions();
        QCOMPARE(l.size(), 16);
        for (auto s : l) {
            QVERIFY(s.isValid());
            QVERIFY(!s.parent().isValid());
            QCOMPARE(s.subdivisions().size(), 0);
            QCOMPARE(s.country(), c);
        }
    }
};

QTEST_GUILESS_MAIN(KCountrySubdivisionTest)

#include "kcountrysubdivisiontest.moc"
