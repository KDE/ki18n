// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

#include <memory>

#include <QPointer>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTest>

#include <KLocalizedQmlContext>

#include "qmltesthelpers.h"

using namespace Qt::Literals;

// Tests for the KI18n attached type (KLocalizedQmlContext as provided by
// I18nAttached::qmlAttachedProperties()). Everything is driven through QML:
// the QML engine invokes the attached function, and Attached.qml exposes the
// attached object itself for identity assertions. Separate engines provide
// separate root contexts, QQmlComponent::create(context) injects arbitrary
// contexts.
class KI18nQmlAttachedTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void testQmlAttachedWithContextObject();
    void testQmlAttachedAutoCreated();
    void testQmlAttachedPerContext();
    void testQmlAttachedParentContextResolution();
    void testQmlAttachedInnerContextObject();
    void testQmlAttachedLifetime();
    void testQmlTranslationThroughAttachedDomain();

private:
    static KLocalizedQmlContext *attachedOf(QObject *qmlObject)
    {
        return QmlTestHelpers::attachedContextOf(qmlObject, "attached");
    }

    QTemporaryDir m_tempDir;
    bool m_hasFrench = false;
};

void KI18nQmlAttachedTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    m_hasFrench = QmlTestHelpers::deployFrenchCatalogs(m_tempDir);
}

void KI18nQmlAttachedTest::testQmlAttachedWithContextObject()
{
    QQmlApplicationEngine engine;
    auto *ctx = KLocalization::setupLocalizedContext(&engine);
    ctx->setTranslationDomain(u"ki18n-unrelated-domain"_s); // overwritten from QML below

    auto *root = QmlTestHelpers::loadQml(&engine, u"Attached.qml"_s);
    QVERIFY(root);

    // The attached object is the context object that was set up from C++ ...
    auto *attached = attachedOf(root);
    QVERIFY(attached);
    QCOMPARE(attached, ctx);
    // ... and the domain assigned from QML landed on it, not on a new context.
    QCOMPARE(attached->translationDomain(), u"ki18n-test"_s);
    QCOMPARE(root->property("domain").toString(), u"ki18n-test"_s);

    // Objects in the same QML context share the attached context.
    auto *child = root->property("child").value<QObject *>();
    QVERIFY(child);
    QCOMPARE(attachedOf(child), attached);
}

void KI18nQmlAttachedTest::testQmlAttachedAutoCreated()
{
    // No C++ setup: the attached context must be created on demand.
    QQmlApplicationEngine engine;
    QVERIFY(!engine.rootContext()->contextObject());

    auto *root = QmlTestHelpers::loadQml(&engine, u"Attached.qml"_s);
    QVERIFY(root);

    auto *attached = attachedOf(root);
    QVERIFY(attached);
    // Parented to the object's QQmlContext so that it dies with it.
    QVERIFY(attached->parent() == QQmlEngine::contextForObject(root));
    // The domain was set purely from QML (KI18n.translationDomain: "ki18n-test").
    QCOMPARE(attached->translationDomain(), u"ki18n-test"_s);

    // Cached per context: the child in the same file shares it.
    auto *child = root->property("child").value<QObject *>();
    QVERIFY(child);
    QCOMPARE(attachedOf(child), attached);

    // Another engine has different contexts and gets a different attached context.
    QQmlApplicationEngine engine2;
    auto *root2 = QmlTestHelpers::loadQml(&engine2, u"Attached.qml"_s);
    QVERIFY(root2);
    auto *attached2 = attachedOf(root2);
    QVERIFY(attached2);
    QVERIFY(attached2 != attached);
}

void KI18nQmlAttachedTest::testQmlAttachedPerContext()
{
    // Within one engine, objects in different QQmlContexts (e.g. different QML
    // files) each get their own auto-created attached context.
    QQmlApplicationEngine engine;
    QVERIFY(!engine.rootContext()->contextObject());

    auto *root1 = QmlTestHelpers::loadQml(&engine, u"Attached.qml"_s);
    QVERIFY(root1);
    auto *attached1 = attachedOf(root1);
    QVERIFY(attached1);
    QVERIFY(attached1->parent() == QQmlEngine::contextForObject(root1));

    QQmlContext otherContext(engine.rootContext());
    auto *root2 = QmlTestHelpers::createQml(&engine, &otherContext, u"Attached.qml"_s);
    QVERIFY(root2);
    auto *attached2 = attachedOf(root2);
    QVERIFY(attached2);
    QVERIFY(attached2 != attached1);
    QVERIFY(attached2->parent() == QQmlEngine::contextForObject(root2));
}

void KI18nQmlAttachedTest::testQmlAttachedParentContextResolution()
{
    // The Button-inside-a-Rectangle case: an object in a child context must
    // resolve the on-demand created attached context of its parent context,
    // not get a new one of its own.
    QQmlApplicationEngine engine;
    QVERIFY(!engine.rootContext()->contextObject());

    auto *root = QmlTestHelpers::loadQml(&engine, u"Attached.qml"_s); // creates the attached context for the file's context
    QVERIFY(root);
    auto *attached = attachedOf(root);
    QVERIFY(attached);

    auto *fileContext = QQmlEngine::contextForObject(root);
    QQmlContext childContext(fileContext);
    auto *childRoot = QmlTestHelpers::createQml(&engine, &childContext, u"Attached.qml"_s);
    QVERIFY(childRoot);
    QCOMPARE(attachedOf(childRoot), attached);

    // Same for a context object: it is resolved through the parent chain.
    QQmlApplicationEngine engine2;
    auto *ctx = KLocalization::setupLocalizedContext(&engine2);
    auto *root2 = QmlTestHelpers::loadQml(&engine2, u"Attached.qml"_s);
    QVERIFY(root2);
    QQmlContext childContext2(QQmlEngine::contextForObject(root2));
    auto *childRoot2 = QmlTestHelpers::createQml(&engine2, &childContext2, u"Attached.qml"_s);
    QVERIFY(childRoot2);
    QCOMPARE(attachedOf(childRoot2), ctx);
}

void KI18nQmlAttachedTest::testQmlAttachedInnerContextObject()
{
    QQmlApplicationEngine engine;
    auto *rootLocalized = KLocalization::setupLocalizedContext(&engine);

    // An inner context with its own context object wins over the root context.
    QQmlContext innerContext(engine.rootContext());
    auto innerLocalized = std::make_unique<KLocalizedQmlContext>(&innerContext);
    innerContext.setContextObject(innerLocalized.get());

    auto *innerRoot = QmlTestHelpers::createQml(&engine, &innerContext, u"Attached.qml"_s);
    QVERIFY(innerRoot);
    QCOMPARE(attachedOf(innerRoot), innerLocalized.get());

    auto *root = QmlTestHelpers::createQml(&engine, engine.rootContext(), u"Attached.qml"_s);
    QVERIFY(root);
    QCOMPARE(attachedOf(root), rootLocalized);
}

void KI18nQmlAttachedTest::testQmlAttachedLifetime()
{
    QPointer<KLocalizedQmlContext> guard;
    {
        auto engine = std::make_unique<QQmlApplicationEngine>();
        auto *root = QmlTestHelpers::loadQml(engine.get(), u"Attached.qml"_s);
        QVERIFY(root);
        guard = attachedOf(root);
        QVERIFY(guard);
    }
    // The auto-created attached context was parented to the QML context and
    // must have been destroyed together with the engine.
    QVERIFY(guard.isNull());
}

void KI18nQmlAttachedTest::testQmlTranslationThroughAttachedDomain()
{
    if (!m_hasFrench) {
        QSKIP("French test catalogs unavailable (msgfmt or po/fr/*.po missing)");
    }

    // With the context object set up from C++.
    {
        QQmlApplicationEngine engine;
        auto *ctx = KLocalization::setupLocalizedContext(&engine);
        ctx->setTranslationDomain(u"ki18n-test"_s);
        auto *root = QmlTestHelpers::loadQml(&engine, u"Attached.qml"_s);
        QVERIFY(root);
        QCOMPARE(QmlTestHelpers::invokeString(root, "translateJob"), u"Tâche"_s);
        QCOMPARE(QmlTestHelpers::invokeString(root->property("child").value<QObject *>(), "translateJob"), u"Tâche"_s);
    }

    // Without C++ setup, translating through the domain assigned from QML.
    {
        QQmlApplicationEngine engine;
        auto *root = QmlTestHelpers::loadQml(&engine, u"Attached.qml"_s);
        QVERIFY(root);
        QCOMPARE(QmlTestHelpers::invokeString(root, "translateJob"), u"Tâche"_s);
        QCOMPARE(QmlTestHelpers::invokeString(root->property("child").value<QObject *>(), "translateJob"), u"Tâche"_s);
    }
}

QTEST_MAIN(KI18nQmlAttachedTest)

#include "ki18nqmlattachedtest.moc"
