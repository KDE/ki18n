/*
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attached.h"

#include <QCoreApplication>
#include <QQmlComponent>
#include <QQmlContext>
#include <qapplicationstatic.h>

#include <KLocalizedString>

#include "ki18nqml_logging.h"

using namespace Qt::Literals::StringLiterals;

namespace
{
constexpr const char *uri = "org.kde.i18n";
const QString contextPropertyName = u"__context"_s;
constexpr const char *retranslatePropertyName = "__retranslate";
constexpr const char *translationDomainPropertyName = "_k_translationDomain";
}

QHash<QQmlEngine *, I18nQMLType *> KI18nAttached::s_map = QHash<QQmlEngine *, I18nQMLType *>();

Q_APPLICATION_STATIC(KI18nAttached, s_globalLanguageChangeEventFilter);

KI18nAttached::KI18nAttached(QObject *parent)
    : QObject(parent)
{
    // Watch for LanguageChange
    QCoreApplication::instance()->installEventFilter(this);
}

KI18nAttached::~KI18nAttached() = default;

I18nQMLType *KI18nAttached::qmlAttachedProperties(QObject *attachee)
{
    Q_ASSERT(attachee);
    auto engine = qmlEngine(attachee);
    Q_ASSERT(engine);
    I18nQMLType *ki18n = s_map.value(engine, nullptr);
    if (!ki18n) {
        ki18n = createAttachedObject(attachee, engine);
        if (ki18n) {
            connect(engine, &QObject::destroyed, &KI18nAttached::objectDestroyed);
            s_map[engine] = ki18n;
        }
    }
    return ki18n;
}

bool KI18nAttached::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    return QObject::eventFilter(object, event);
}

I18nQMLType *KI18nAttached::createAttachedObject(QObject *attachee, QQmlEngine *engine)
{
    QQmlComponent component(engine, uri, "KI18nImpl");
    Q_ASSERT(component.status() == QQmlComponent::Ready);

    I18nQMLType *ki18n = component.beginCreate(qmlContext(attachee));
    auto context = createLocalizedContext(attachee, engine);
    component.setInitialProperties(ki18n, {{contextPropertyName, QVariant::fromValue(context)}});
    component.completeCreate();

    if (!ki18n) {
        qCWarning(KI18N_QML).noquote() << "Failed to create I18n object:" << component.errorString().trimmed();
        return nullptr;
    }

    // dereference to create if needed.
    *s_globalLanguageChangeEventFilter;

    return ki18n;
}

KLocalizedContext *KI18nAttached::createLocalizedContext(QObject *attachee, QQmlEngine *engine)
{
    KLocalizedContext *context = new KLocalizedContext(attachee);
    QString domain = translationDomainForEngine(engine);
    if (!domain.isEmpty()) {
        context->setTranslationDomain(domain);
    }
    return context;
}

QString KI18nAttached::translationDomainForEngine(QQmlEngine *engine)
{
    return engine->property(translationDomainPropertyName).toString();
}

void KI18nAttached::objectDestroyed(QObject *object)
{
    auto engine = qobject_cast<QQmlEngine *>(object);
    if (!engine) {
        return;
    }
    I18nQMLType *ki18n = s_map.take(engine);
    if (ki18n) {
        delete ki18n;
    }
}

void KI18nAttached::retranslate()
{
    qCDebug(KI18N_QML) << "Filtered LanguageChange event, retranslating";
    // Assume that change of strings might mutate this map (i.e. create new
    // engines or destroy existing ones). It's unlikely but not impossible.

    QList<std::pair<QPointer<QQmlEngine>, I18nQMLType *>> copy;
    for (auto it = s_map.constBegin(); it != s_map.constEnd(); it++) {
        copy.append(std::make_pair(QPointer(it.key()), it.value()));
    }
    for (const auto &[engine, ki18n] : std::as_const(copy)) {
        if (engine) {
            qCDebug(KI18N_QML) << "Retranslating engine" << engine;
            retranslate(ki18n);
        }
    }
}

void KI18nAttached::retranslate(I18nQMLType *ki18n)
{
    Q_ASSERT(ki18n);
    // This is not a hot path, it doesn't need the complexity of static
    // caching of meta-property indices per engine etc.
    const auto mo = ki18n->metaObject();
    const auto index = mo->indexOfProperty(retranslatePropertyName);
    const auto property = mo->property(index);
    const auto notify = property.notifySignal();
    notify.invoke(ki18n);
}

KI18nTestHelper::KI18nTestHelper(QObject *parent)
    : QObject(parent)
{
}

void KI18nTestHelper::setLanguages(const QStringList &languages)
{
    qDebug() << "Set Languages:" << KLocalizedString::languages() << "->" << languages;
    KLocalizedString::setLanguages(languages);

    QEvent le(QEvent::LanguageChange);
    QCoreApplication::sendEvent(QCoreApplication::instance(), &le);
}

#include "moc_attached.cpp"
