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

namespace
{
constexpr const char *uri = "org.kde.i18n";
constexpr const char *translationDomainPropertyName = "_k_translationDomain";
}

QList<KI18nBase *> KI18nBase::s_instances = {};

KI18nBase::KI18nBase(QObject *parent)
    : QObject(parent)
{
    s_instances.append(this);
}

KI18nBase::~KI18nBase()
{
    s_instances.removeOne(this);
};

KLocalizedContext *KI18nBase::context() const
{
    return &m_context;
}

QVariant KI18nBase::retranslate() const
{
    return {};
}

void KI18nBase::doRetranslate()
{
    for (const auto &ki18n : std::as_const(s_instances)) {
        qCDebug(KI18N_QML) << "Retranslating" << ki18n;
        Q_EMIT ki18n->retranslateChanged();
    }
}

KI18nBase *KI18nSingleton::create(QQmlEngine *engine, QJSEngine *)
{
    Q_ASSERT(engine);

    QQmlComponent component(engine, uri, "KI18nImpl");
    Q_ASSERT(component.status() == QQmlComponent::Ready);

    // Attached object is shared across all attachees within an engine, so the
    // context should be nullptr (which means the global one). Ownership is
    // transferred to the caller.
    QObject *created = component.create();
    KI18nBase *ki18n = qobject_cast<KI18nBase *>(created);
    if (!ki18n || component.isError()) {
        qCCritical(KI18N_QML).noquote() << "Failed to create I18n object:" << component.errorString().trimmed();
        delete created;
        return nullptr;
    }

    QString domain = translationDomainForEngine(engine);
    if (!domain.isEmpty()) {
        ki18n->context()->setTranslationDomain(domain);
    }

    // ensure filter is created and installed.
    KI18nLanguageChangeEventFilter::instance();

    return ki18n;
}

QString KI18nSingleton::translationDomainForEngine(QQmlEngine *engine)
{
    return engine->property(translationDomainPropertyName).toString();
}

Q_APPLICATION_STATIC(KI18nLanguageChangeEventFilter, s_globalLanguageChangeEventFilter);

KI18nLanguageChangeEventFilter::KI18nLanguageChangeEventFilter(QObject *parent)
    : QObject(parent)
{
    // Watch for LanguageChange
    QCoreApplication::instance()->installEventFilter(this);
}

KI18nLanguageChangeEventFilter::~KI18nLanguageChangeEventFilter() = default;

KI18nLanguageChangeEventFilter *KI18nLanguageChangeEventFilter::instance()
{
    return s_globalLanguageChangeEventFilter;
}

bool KI18nLanguageChangeEventFilter::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    return QObject::eventFilter(object, event);
}

void KI18nLanguageChangeEventFilter::retranslate()
{
    qCDebug(KI18N_QML) << "Filtered LanguageChange event, retranslating";

    KI18nBase::doRetranslate();
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
