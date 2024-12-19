/*
    SPDX-FileCopyrightText: 2011 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KLOCALIZEDQMLCONTEXT_H
#define KLOCALIZEDQMLCONTEXT_H

#include <ki18nqml_export.h>

#include <QObject>
#include <QVariant>
#include <memory>

class QQmlEngine;

class KLocalizedQmlContextPrivate;

/*!
 * \class KLocalizedQmlContext
 * \inmodule KI18nQml
 *
 * \brief This class is meant to be used to simplify integration of the KI18n framework
 * in QML.
 *
 * The way to do so, is by creating this object and setting it as a context
 * object:
 *
 * \code
 * QQmlApplicationEngine engine;
 * auto ctx = new KLocalizedQmlContext(&engine);
 * engine->rootContext()->setContextObject(ctx);
 * QQmlEngine::setContextForObject(ctx, engine.rootContext());
 * ctx->setTranslationDomain(...);
 * \endcode
 *
 * In many cases this can be simplified using KLocalization::setupLocalizedContext():
 * \code
 * QQmlApplicationEngine engine;
 * KLocalization::setupLocalizedContext(&engine);
 * \endcode
 *
 * Then i18n*() and xi18n*() functions should be available for use from the code
 * loaded in the engine, for the view.
 *
 * Unlike its predecessor KLocalizedContext this does automatically trigger
 * a binding re-evaluation when the application language is changed at runtime
 * (with Qt 6.6 or higher).
 *
 * \note Plural functions differ from the C/C++ version. On QML/JS we can get a
 * real value easily. To solve warnings on those cases we'll cast the first argument
 * to make sure it's taken into account for the plural.
 *
 * \since 6.8
 */
class KI18NQML_EXPORT KLocalizedQmlContext : public QObject
{
    Q_OBJECT

    /*!
     * \property KLocalizedQmlContext::translationDomain
     *
     * This property only needs to be specified if the context is being run on a library.
     * in an application there is no need to set the translation domain as the application's
     * domain can be used.
     */
    Q_PROPERTY(QString translationDomain READ translationDomain WRITE setTranslationDomain NOTIFY translationDomainChanged)

public:
    /*!
     *
     */
    explicit KLocalizedQmlContext(QObject *parent = nullptr);
    ~KLocalizedQmlContext() override;

    /*!
     *
     */
    QString translationDomain() const;

    /*!
     *
     */
    void setTranslationDomain(const QString &domain);

    Q_INVOKABLE QString i18n(const QString &message,
                             const QVariant &param1 = QVariant(),
                             const QVariant &param2 = QVariant(),
                             const QVariant &param3 = QVariant(),
                             const QVariant &param4 = QVariant(),
                             const QVariant &param5 = QVariant(),
                             const QVariant &param6 = QVariant(),
                             const QVariant &param7 = QVariant(),
                             const QVariant &param8 = QVariant(),
                             const QVariant &param9 = QVariant(),
                             const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString i18nc(const QString &context,
                              const QString &message,
                              const QVariant &param1 = QVariant(),
                              const QVariant &param2 = QVariant(),
                              const QVariant &param3 = QVariant(),
                              const QVariant &param4 = QVariant(),
                              const QVariant &param5 = QVariant(),
                              const QVariant &param6 = QVariant(),
                              const QVariant &param7 = QVariant(),
                              const QVariant &param8 = QVariant(),
                              const QVariant &param9 = QVariant(),
                              const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString i18np(const QString &singular,
                              const QString &plural,
                              const QVariant &param1 = QVariant(),
                              const QVariant &param2 = QVariant(),
                              const QVariant &param3 = QVariant(),
                              const QVariant &param4 = QVariant(),
                              const QVariant &param5 = QVariant(),
                              const QVariant &param6 = QVariant(),
                              const QVariant &param7 = QVariant(),
                              const QVariant &param8 = QVariant(),
                              const QVariant &param9 = QVariant(),
                              const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString i18ncp(const QString &context,
                               const QString &singular,
                               const QString &plural,
                               const QVariant &param1 = QVariant(),
                               const QVariant &param2 = QVariant(),
                               const QVariant &param3 = QVariant(),
                               const QVariant &param4 = QVariant(),
                               const QVariant &param5 = QVariant(),
                               const QVariant &param6 = QVariant(),
                               const QVariant &param7 = QVariant(),
                               const QVariant &param8 = QVariant(),
                               const QVariant &param9 = QVariant(),
                               const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString i18nd(const QString &domain,
                              const QString &message,
                              const QVariant &param1 = QVariant(),
                              const QVariant &param2 = QVariant(),
                              const QVariant &param3 = QVariant(),
                              const QVariant &param4 = QVariant(),
                              const QVariant &param5 = QVariant(),
                              const QVariant &param6 = QVariant(),
                              const QVariant &param7 = QVariant(),
                              const QVariant &param8 = QVariant(),
                              const QVariant &param9 = QVariant(),
                              const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString i18ndc(const QString &domain,
                               const QString &context,
                               const QString &message,
                               const QVariant &param1 = QVariant(),
                               const QVariant &param2 = QVariant(),
                               const QVariant &param3 = QVariant(),
                               const QVariant &param4 = QVariant(),
                               const QVariant &param5 = QVariant(),
                               const QVariant &param6 = QVariant(),
                               const QVariant &param7 = QVariant(),
                               const QVariant &param8 = QVariant(),
                               const QVariant &param9 = QVariant(),
                               const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString i18ndp(const QString &domain,
                               const QString &singular,
                               const QString &plural,
                               const QVariant &param1 = QVariant(),
                               const QVariant &param2 = QVariant(),
                               const QVariant &param3 = QVariant(),
                               const QVariant &param4 = QVariant(),
                               const QVariant &param5 = QVariant(),
                               const QVariant &param6 = QVariant(),
                               const QVariant &param7 = QVariant(),
                               const QVariant &param8 = QVariant(),
                               const QVariant &param9 = QVariant(),
                               const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString i18ndcp(const QString &domain,
                                const QString &context,
                                const QString &singular,
                                const QString &plural,
                                const QVariant &param1 = QVariant(),
                                const QVariant &param2 = QVariant(),
                                const QVariant &param3 = QVariant(),
                                const QVariant &param4 = QVariant(),
                                const QVariant &param5 = QVariant(),
                                const QVariant &param6 = QVariant(),
                                const QVariant &param7 = QVariant(),
                                const QVariant &param8 = QVariant(),
                                const QVariant &param9 = QVariant(),
                                const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18n(const QString &message,
                              const QVariant &param1 = QVariant(),
                              const QVariant &param2 = QVariant(),
                              const QVariant &param3 = QVariant(),
                              const QVariant &param4 = QVariant(),
                              const QVariant &param5 = QVariant(),
                              const QVariant &param6 = QVariant(),
                              const QVariant &param7 = QVariant(),
                              const QVariant &param8 = QVariant(),
                              const QVariant &param9 = QVariant(),
                              const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18nc(const QString &context,
                               const QString &message,
                               const QVariant &param1 = QVariant(),
                               const QVariant &param2 = QVariant(),
                               const QVariant &param3 = QVariant(),
                               const QVariant &param4 = QVariant(),
                               const QVariant &param5 = QVariant(),
                               const QVariant &param6 = QVariant(),
                               const QVariant &param7 = QVariant(),
                               const QVariant &param8 = QVariant(),
                               const QVariant &param9 = QVariant(),
                               const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18np(const QString &singular,
                               const QString &plural,
                               const QVariant &param1 = QVariant(),
                               const QVariant &param2 = QVariant(),
                               const QVariant &param3 = QVariant(),
                               const QVariant &param4 = QVariant(),
                               const QVariant &param5 = QVariant(),
                               const QVariant &param6 = QVariant(),
                               const QVariant &param7 = QVariant(),
                               const QVariant &param8 = QVariant(),
                               const QVariant &param9 = QVariant(),
                               const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18ncp(const QString &context,
                                const QString &singular,
                                const QString &plural,
                                const QVariant &param1 = QVariant(),
                                const QVariant &param2 = QVariant(),
                                const QVariant &param3 = QVariant(),
                                const QVariant &param4 = QVariant(),
                                const QVariant &param5 = QVariant(),
                                const QVariant &param6 = QVariant(),
                                const QVariant &param7 = QVariant(),
                                const QVariant &param8 = QVariant(),
                                const QVariant &param9 = QVariant(),
                                const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18nd(const QString &domain,
                               const QString &message,
                               const QVariant &param1 = QVariant(),
                               const QVariant &param2 = QVariant(),
                               const QVariant &param3 = QVariant(),
                               const QVariant &param4 = QVariant(),
                               const QVariant &param5 = QVariant(),
                               const QVariant &param6 = QVariant(),
                               const QVariant &param7 = QVariant(),
                               const QVariant &param8 = QVariant(),
                               const QVariant &param9 = QVariant(),
                               const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18ndc(const QString &domain,
                                const QString &context,
                                const QString &message,
                                const QVariant &param1 = QVariant(),
                                const QVariant &param2 = QVariant(),
                                const QVariant &param3 = QVariant(),
                                const QVariant &param4 = QVariant(),
                                const QVariant &param5 = QVariant(),
                                const QVariant &param6 = QVariant(),
                                const QVariant &param7 = QVariant(),
                                const QVariant &param8 = QVariant(),
                                const QVariant &param9 = QVariant(),
                                const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18ndp(const QString &domain,
                                const QString &singular,
                                const QString &plural,
                                const QVariant &param1 = QVariant(),
                                const QVariant &param2 = QVariant(),
                                const QVariant &param3 = QVariant(),
                                const QVariant &param4 = QVariant(),
                                const QVariant &param5 = QVariant(),
                                const QVariant &param6 = QVariant(),
                                const QVariant &param7 = QVariant(),
                                const QVariant &param8 = QVariant(),
                                const QVariant &param9 = QVariant(),
                                const QVariant &param10 = QVariant()) const;

    Q_INVOKABLE QString xi18ndcp(const QString &domain,
                                 const QString &context,
                                 const QString &singular,
                                 const QString &plural,
                                 const QVariant &param1 = QVariant(),
                                 const QVariant &param2 = QVariant(),
                                 const QVariant &param3 = QVariant(),
                                 const QVariant &param4 = QVariant(),
                                 const QVariant &param5 = QVariant(),
                                 const QVariant &param6 = QVariant(),
                                 const QVariant &param7 = QVariant(),
                                 const QVariant &param8 = QVariant(),
                                 const QVariant &param9 = QVariant(),
                                 const QVariant &param10 = QVariant()) const;

Q_SIGNALS:
    void translationDomainChanged(const QString &translationDomain);

private:
    bool eventFilter(QObject *watched, QEvent *event) override;
    std::unique_ptr<KLocalizedQmlContextPrivate> const d;
};

/*!
 * \namespace KLocalization
 * \inmodule KI18nQml
 * \brief Namespace containing helpers for localization.
 */
namespace KLocalization
{
namespace Internal
{
[[nodiscard]] KI18NQML_EXPORT KLocalizedQmlContext *createLocalizedContext(QQmlEngine *engine);
}

/*!
 * Creates a KLocalizedQmlContext engine and sets it up in the
 * root context of \a engine.
 *
 * If \c TRANSLATION_DOMAIN is defined, that is also set on
 * the created context.
 *
 * \since 6.8
 * \relates KLocalizedQmlContext
 */
inline KLocalizedQmlContext *setupLocalizedContext(QQmlEngine *engine)
{
    auto ctx = Internal::createLocalizedContext(engine);
#ifdef TRANSLATION_DOMAIN
    ctx->setTranslationDomain(QStringLiteral(TRANSLATION_DOMAIN));
#endif
    return ctx;
}
}

#endif
