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

// type alias for documentary purposes
using I18nQMLType = QObject;

class KI18nAttached : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(K)
    QML_UNCREATABLE("Use it as an attached property")
    QML_ATTACHED(I18nQMLType)
public:
    explicit KI18nAttached(QObject *parent = nullptr);
    ~KI18nAttached();

    static I18nQMLType *qmlAttachedProperties(QObject *attachee);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    static QHash<QQmlEngine *, I18nQMLType *> s_map;

    static I18nQMLType *createAttachedObject(QObject *attachee, QQmlEngine *engine);
    static KLocalizedContext *createLocalizedContext(QObject *attachee, QQmlEngine *engine);
    static QString translationDomainForEngine(QQmlEngine *engine);
    static void objectDestroyed(QObject *object);
    static void retranslate();
    static void retranslate(I18nQMLType *);
};

struct KLocalizedContextForeign {
    Q_GADGET
    QML_FOREIGN(KLocalizedContext)
    QML_NAMED_ELEMENT(KLocalizedContext)
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
