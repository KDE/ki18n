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
#include <qobject.h>
#include <qqmlintegration.h>

/*
              Base     <-------- Foreign singleton wrapper
        class with native        creates KI18nImpl instead
       property definitions
               /\
               ||
        Pure QML KI18nImpl
   implementation with methods
 that bind to retranslation signal
*/

class KI18nBase : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(KLocalizedContext *__context READ context CONSTANT FINAL)

    /*
     * Every function subscribes to this property's change signal. The actual
     * value does not matter. Change signal is emitted from C++ when needed.
     */
    Q_PROPERTY(QVariant __retranslate READ retranslate NOTIFY retranslateChanged FINAL)

public:
    explicit KI18nBase(QObject *parent = nullptr);
    ~KI18nBase();

    KLocalizedContext *context() const;

    // dummy getter
    QVariant retranslate() const;

    static void doRetranslate();

Q_SIGNALS:
    void retranslateChanged();

private:
    static QList<KI18nBase *> s_instances;

    mutable KLocalizedContext m_context;
};

struct KI18nSingleton {
    Q_GADGET
    QML_FOREIGN(KI18nBase)
    QML_SINGLETON
    QML_NAMED_ELEMENT(K)

public:
    static KI18nBase *create(QQmlEngine *engine, QJSEngine *);

private:
    static QString translationDomainForEngine(QQmlEngine *engine);
};

class KI18nLanguageChangeEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit KI18nLanguageChangeEventFilter(QObject *parent = nullptr);
    ~KI18nLanguageChangeEventFilter();

    static KI18nLanguageChangeEventFilter *instance();

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
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
