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

KI18nAttachedBase::KI18nAttachedBase(QObject *parent)
    : QObject(parent)
{
}

KI18nAttachedBase::~KI18nAttachedBase() = default;

KLocalizedContext *KI18nAttachedBase::context() const
{
    return &m_context;
}

QVariant KI18nAttachedBase::retranslate() const
{
    return {};
}

QHash<QQmlEngine *, KI18nAttachedBase *> KI18nAttached::s_map = QHash<QQmlEngine *, KI18nAttachedBase *>();

Q_APPLICATION_STATIC(KI18nAttached, s_globalLanguageChangeEventFilter);

namespace
{
void ensureGlobalLanguageChangeEventFilterExists()
{
    // dereference to create if needed.
    *s_globalLanguageChangeEventFilter;
}
}

KI18nAttached::KI18nAttached(QObject *parent)
    : QObject(parent)
{
    // Watch for LanguageChange
    QCoreApplication::instance()->installEventFilter(this);
}

KI18nAttached::~KI18nAttached() = default;

KI18nAttachedBase *KI18nAttached::qmlAttachedProperties(QObject *attachee)
{
    Q_ASSERT(attachee);
    auto engine = qmlEngine(attachee);
    Q_ASSERT(engine);
    KI18nAttachedBase *ki18n = s_map.value(engine, nullptr);
    if (!ki18n) {
        ki18n = createAttachedObject(engine);
        if (ki18n) {
            connect(engine, &QObject::destroyed, &KI18nAttached::objectDestroyed);
            s_map[engine] = ki18n;
            ensureGlobalLanguageChangeEventFilterExists();
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

KI18nAttachedBase *KI18nAttached::createAttachedObject(QQmlEngine *engine)
{
    QQmlComponent component(engine, uri, "KI18nImpl");
    Q_ASSERT(component.status() == QQmlComponent::Ready);

    // Attached object is shared across all attachees within an engine, so the
    // context should be nullptr (which means the global one). Ownership is
    // transferred to the caller.
    QObject *created = component.create();
    KI18nAttachedBase *ki18n = qobject_cast<KI18nAttachedBase *>(created);
    if (!ki18n || component.isError()) {
        qCCritical(KI18N_QML).noquote() << "Failed to create I18n object:" << component.errorString().trimmed();
        delete created;
        return nullptr;
    }

    QString domain = translationDomainForEngine(engine);
    if (!domain.isEmpty()) {
        ki18n->context()->setTranslationDomain(domain);
    }

    return ki18n;
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
    KI18nAttachedBase *ki18n = s_map.take(engine);
    if (ki18n) {
        delete ki18n;
    }
}

void KI18nAttached::retranslate()
{
    qCDebug(KI18N_QML) << "Filtered LanguageChange event, retranslating";
    // Assume that change of strings might mutate this map (i.e. create new
    // engines or destroy existing ones). It's unlikely but not impossible.

    QList<std::pair<QPointer<QQmlEngine>, KI18nAttachedBase *>> copy;

    for (auto it = s_map.constBegin(); it != s_map.constEnd(); it++) {
        copy.append(std::make_pair(QPointer(it.key()), it.value()));
    }

    for (const auto &[engine, ki18n] : std::as_const(copy)) {
        if (engine) {
            qCDebug(KI18N_QML) << "Retranslating engine" << engine;
            Q_EMIT ki18n->retranslateChanged();
        }
    }
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
