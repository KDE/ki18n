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

class KCountryTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty()
    {
        KCountry c;
        QVERIFY(!c.isValid());
        QVERIFY(c.alpha2().isEmpty());
        QVERIFY(c.alpha3().isEmpty());
        QVERIFY(c.name().isEmpty());
        QVERIFY(c.emojiFlag().isEmpty());
        QVERIFY(c.subdivisions().isEmpty());
        QCOMPARE(c.country(), QLocale::AnyCountry);
        QVERIFY(c.currencyCode().isEmpty());
    }

    void testLookup()
    {
        auto c = KCountry::fromAlpha2(u"NZ");
        QVERIFY(c.isValid());
        QCOMPARE(c.alpha2(), QLatin1String("NZ"));
        QCOMPARE(c.alpha3(), QLatin1String("NZL"));
        QCOMPARE(c.name(), QStringLiteral("Nouvelle-Zélande"));
        QCOMPARE(c.emojiFlag(), QStringLiteral("🇳🇿"));
        QCOMPARE(c.country(), QLocale::NewZealand);
        QCOMPARE(c.currencyCode(), QLatin1String("NZD"));

        c = KCountry::fromAlpha2("nz");
        QVERIFY(c.isValid());
        QCOMPARE(c.alpha2(), QLatin1String("NZ"));

        c = KCountry::fromAlpha3(u"NZL");
        QVERIFY(c.isValid());
        QCOMPARE(c.alpha2(), QLatin1String("NZ"));

        QVERIFY(!KCountry::fromAlpha2(QString()).isValid());
        QVERIFY(!KCountry::fromAlpha2(u"ZZ").isValid());
        QVERIFY(!KCountry::fromAlpha2(u"N").isValid());
        QVERIFY(!KCountry::fromAlpha2(u"NZL").isValid());
        QVERIFY(!KCountry::fromAlpha2(u"42").isValid());
        QVERIFY(!KCountry::fromAlpha2(nullptr).isValid());
        QVERIFY(!KCountry::fromAlpha2("N").isValid());
        QVERIFY(!KCountry::fromAlpha2("nzl").isValid());
        QVERIFY(!KCountry::fromAlpha2("\0\0").isValid());

        QVERIFY(!KCountry::fromAlpha3(u"ZZZ").isValid());
        QVERIFY(!KCountry::fromAlpha3(QString()).isValid());
        QVERIFY(!KCountry::fromAlpha3(u"NZ").isValid());
        QVERIFY(!KCountry::fromAlpha3(u"NEWZL").isValid());
        QVERIFY(!KCountry::fromAlpha3(u"123").isValid());
        QVERIFY(!KCountry::fromAlpha3(nullptr).isValid());
        QVERIFY(!KCountry::fromAlpha3("NZ").isValid());
        QVERIFY(!KCountry::fromAlpha3("nzla").isValid());
        QVERIFY(!KCountry::fromAlpha3("\0\0\0").isValid());

        QCOMPARE(KCountry::fromAlpha2(u"nz").alpha2(), QLatin1String("NZ"));
        QCOMPARE(KCountry::fromAlpha2(u"Nz").alpha2(), QLatin1String("NZ"));
        QCOMPARE(KCountry::fromAlpha3(u"nzl").alpha2(), QLatin1String("NZ"));
        QCOMPARE(KCountry::fromAlpha3(u"NzL").alpha2(), QLatin1String("NZ"));

        c = KCountry::fromQLocale(QLocale::NewZealand);
        QVERIFY(c.isValid());
        QCOMPARE(c.alpha2(), QLatin1String("NZ"));

        QVERIFY(!KCountry::fromQLocale(QLocale::AnyCountry).isValid());
        QVERIFY(!KCountry::fromQLocale(QLocale::World).isValid());
    }

    void benchmarkLookup()
    {
        QBENCHMARK {
            const auto c = KCountry::fromAlpha2(u"NZ");
            QVERIFY(c.isValid());
        }
    }

    void testList()
    {
        const auto l = KCountry::allCountries();
        QVERIFY(l.size() > 200);
        for (auto c : l) {
            QVERIFY(c.isValid());
            QVERIFY(!c.alpha2().isEmpty());
            QVERIFY(!c.alpha3().isEmpty());
            QVERIFY(!c.name().isEmpty());
            QVERIFY(!c.emojiFlag().isEmpty());
            QVERIFY(c.country() != QLocale::AnyCountry);
        }
    }
};

QTEST_GUILESS_MAIN(KCountryTest)

#include "kcountrytest.moc"
