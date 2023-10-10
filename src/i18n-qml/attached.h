/*
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KI18NQMLATTACHED
#define KI18NQMLATTACHED

#include <QObject>
#include <QQmlEngine>
#include <QVariant>

#include <KLocalizedContext>

#include <memory>
#include <qqmlintegration.h>

class KI18nAttached;

class KI18nAttachedBase : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(KI18nAttachedBase)

    Q_PROPERTY(KLocalizedContext *__context READ context CONSTANT FINAL)

    /*
     * Every function subscribes to this property's change signal. The actual
     * value does not matter. Change signal is emitted from C++ when needed.
     */
    Q_PROPERTY(QVariant __retranslate READ retranslate NOTIFY retranslateChanged FINAL)

public:
    explicit KI18nAttachedBase(QObject *parent = nullptr);
    ~KI18nAttachedBase();

    KLocalizedContext *context() const;

    // dummy getter
    QVariant retranslate() const;

Q_SIGNALS:
    void retranslateChanged();

private:
    mutable KLocalizedContext m_context;
};

class KI18nAttached : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(K)
    QML_UNCREATABLE("Use it as an attached property")
    QML_ATTACHED(KI18nAttachedBase)
public:
    explicit KI18nAttached(QObject *parent = nullptr);
    ~KI18nAttached();

    static KI18nAttachedBase *qmlAttachedProperties(QObject *attachee);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    static QHash<QQmlEngine *, KI18nAttachedBase *> s_map;

    static KI18nAttachedBase *createAttachedObject(QQmlEngine *engine);
    static KLocalizedContext *createLocalizedContext(QQmlEngine *engine);
    static QString translationDomainForEngine(QQmlEngine *engine);
    static void objectDestroyed(QObject *object);
    static void retranslate();
};

struct KLocalizedContextForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(KLocalizedContext)
};

class KI18nTestHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit KI18nTestHelper(QObject *parent = nullptr);

    Q_INVOKABLE static void setLanguages(const QStringList &languages);
};

#endif // KI18NQMLATTACHED
