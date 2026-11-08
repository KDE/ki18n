// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

import QtTest

import org.kde.ki18n

TestCase {
    name: "KI18nSingleton"

    property string nullString

    function init() {
        // make sure each test function starts with the default domain
        KI18n.translationDomain = "ki18n-test"
    }

    function skipUnlessFrench() {
        if (!hasFrenchCatalogs) {
            skip("French test catalogs not available")
        }
    }

    function test_translationDomain() {
        compare(KI18n.translationDomain, "ki18n-test")
    }

    function test_i18n() {
        compare(KI18n.i18n("Hello %1", "World"), "Hello World")
        compare(KI18n.i18n("%1 and %2", "apples", "oranges"), "apples and oranges")
        compare(KI18n.i18n("I'm %1 years old", 0), "I'm 0 years old")
        compare(KI18n.i18n("Pi is %1", 3.14), "Pi is 3.14")
        compare(KI18n.i18n("Awesome %1", nullString), "Awesome ")
    }

    function test_i18n_translated() {
        skipUnlessFrench()
        compare(KI18n.i18n("Job"), "Tâche")
    }

    function test_i18nc() {
        compare(KI18n.i18nc("Kitten", "Meow"), "Meow")
        compare(KI18n.i18nc("@title", "Welcome %1", "user"), "Welcome user")
    }

    function test_i18np() {
        compare(KI18n.i18np("and %1 other window", "and %1 other windows", 1), "and 1 other window")
        compare(KI18n.i18np("and %1 other window", "and %1 other windows", 3), "and 3 other windows")
        compare(KI18n.i18np("%1 deleted %2 file", "%1 deleted %2 files", 3, "temp"), "3 deleted temp files")
    }

    function test_i18ncp() {
        compare(KI18n.i18ncp("@msg", "%1 file", "%1 files", 1), "1 file")
        compare(KI18n.i18ncp("@msg", "%1 file", "%1 files", 2), "2 files")
    }

    function test_i18nd() {
        compare(KI18n.i18nd("ki18n-test", "Hello %1", "World"), "Hello World")
    }

    function test_i18nd_translated() {
        skipUnlessFrench()
        compare(KI18n.i18nd("ki18n-test2", "Cheese"), "Fromage")
    }

    function test_i18ndc() {
        compare(KI18n.i18ndc("ki18n-test2", "Kitten", "Meow"), "Meow")
    }

    function test_i18ndp() {
        compare(KI18n.i18ndp("ki18n-test", "%1 second", "%1 seconds", 3), "3 seconds")
    }

    function test_i18ndcp() {
        compare(KI18n.i18ndcp("ki18n-test2", "@ctx", "%1 second", "%1 seconds", 1), "1 second")
    }

    function test_xi18n() {
        compare(KI18n.xi18n("Hello %1", "World"), "Hello World")
        // markup-aware: UI markup in the message is formatted, unlike i18n
        compare(KI18n.i18n("Say <emphasis>hi</emphasis>"), "Say <emphasis>hi</emphasis>")
        compare(KI18n.xi18n("Say <emphasis>hi</emphasis>"), "Say *hi*")
    }

    function test_xi18n_translated() {
        skipUnlessFrench()
        compare(KI18n.xi18n("Job"), "Tâche")
    }

    function test_xi18nc() {
        compare(KI18n.xi18nc("Kitten", "Meow"), "Meow")
    }

    function test_xi18np() {
        compare(KI18n.xi18np("and %1 other window", "and %1 other windows", 3), "and 3 other windows")
    }

    function test_xi18ncp() {
        compare(KI18n.xi18ncp("msg", "%1 file", "%1 files", 1), "1 file")
    }

    function test_xi18nd() {
        compare(KI18n.xi18nd("ki18n-test2", "Hello %1", "World"), "Hello World")
    }

    function test_xi18ndc() {
        compare(KI18n.xi18ndc("ki18n-test2", "Kitten", "Meow"), "Meow")
    }

    function test_xi18ndp() {
        compare(KI18n.xi18ndp("ki18n-test", "%1 second", "%1 seconds", 1), "1 second")
    }

    function test_xi18ndcp() {
        compare(KI18n.xi18ndcp("ki18n-test2", "ctx", "%1 second", "%1 seconds", 3), "3 seconds")
    }

    function test_translationDomainSwitch() {
        skipUnlessFrench()
        // writing translationDomain through the singleton switches the
        // domain of the resolved context
        KI18n.translationDomain = "ki18n-test2"
        compare(KI18n.i18n("Cheese"), "Fromage")
        KI18n.translationDomain = "ki18n-test"
        compare(KI18n.translationDomain, "ki18n-test")
        compare(KI18n.i18n("Job"), "Tâche")
    }
}
