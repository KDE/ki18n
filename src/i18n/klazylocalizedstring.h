/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KLAZYLOCALIZEDSTRING_H
#define KLAZYLOCALIZEDSTRING_H

#include "ki18n_export.h"

#include <cstddef>

class KLocalizedString;

/**
 *  @class KLazyLocalizedString klazylocalizedstring.h <KLazyLocalizedString>
 *
 *  Lazy-initialized variant of KLocalizedString.
 *
 *  This is a safer replacement for the I18N_NOOP set of macros and allows
 *  marking strings for extraction without runtime-initializing KLocalizedString instances,
 *  for example for storing in static data tables.
 *
 *  Instances of KLazyLocalizedString are not created directly, unless they should be empty.
 *  Instead they have to be obtained via the kli18n* functions (similar to KLocalizedString
 *  and the ki18n* functions).
 *
 *  Example usage in a static message table:
 *  @code
 *  struct {
 *      MyClass::VehicleType type;
 *      KLazyLocalizedString msg;
 *  } static constexpr const vehicle_msg_table[] = {
 *      { MyClass::Train, kli18np("%1 train", "%1 trains") },
 *      { MyClass::Bus, kli18ncp("the vehicle, not the USB one", "%1 bus", "%1 buses") },
 *      ...
 *  };
 *
 *  ...
 *
 *  const auto it = std::find_if(std::begin(vehicle_msg_table), std::end(vehicle_msg_table), [vehicleType](const auto &m) { return m.type == vehicleType; });
 *  QString translatedMessage = KLocalizedString((*it).msg).subs(vehicleCount).toString();
 *  @endcode
 *
 *  @since 5.89
 */
class KLazyLocalizedString
{
public:
    /**
     * Construct an empty message.
     *
     * Direct construction is used when e.g. a KLazyLocalizedString field in
     * a data table needs to be set, but there is not message to be used.
     * Directly constructed instances are not valid for
     * finalization by \c toString methods.
     *
     * \see isEmpty
     */
    constexpr inline KLazyLocalizedString() = default;

    /** Convert to a KLocalizedString to actually perform the translation and obtain a concrete string to show. */
    KI18N_EXPORT operator KLocalizedString() const;

    /**
     * Check whether the message is empty.
     *
     * The message is considered empty if the object was constructed
     * via the default constructor.
     *
     * Empty messages are not valid for finalization.
     * The behavior of calling toString() on them is undefined.
     * In debug mode, an error mark may appear in the returned string.
     *
     * \return \c true if the message is empty, \c false otherwise
     */
    constexpr inline bool isEmpty() const
    {
        return (m_text == nullptr) || (m_text[0] == '\0');
    }

    /** Returns the raw untranslated text as passed to @p kli18n*. */
    constexpr inline const char *untranslatedText() const
    {
        return m_text;
    }

private:
    template<std::size_t TextSize>
    friend inline constexpr KLazyLocalizedString kli18n(const char (&text)[TextSize]);
    template<std::size_t ContextSize, std::size_t TextSize>
    friend constexpr inline KLazyLocalizedString kli18nc(const char (&context)[ContextSize], const char (&text)[TextSize]);
    template<std::size_t SingularSize, std::size_t PluralSize>
    friend constexpr inline KLazyLocalizedString kli18np(const char (&singular)[SingularSize], const char (&plural)[PluralSize]);
    template<std::size_t ContextSize, std::size_t SingularSize, std::size_t PluralSize>
    friend constexpr inline KLazyLocalizedString
    kli18ncp(const char (&context)[ContextSize], const char (&singular)[SingularSize], const char (&plural)[PluralSize]);
    template<std::size_t TextSize>
    friend constexpr inline KLazyLocalizedString klxi18n(const char (&text)[TextSize]);
    template<std::size_t ContextSize, std::size_t TextSize>
    friend constexpr inline KLazyLocalizedString klxi18nc(const char (&context)[ContextSize], const char (&text)[TextSize]);
    template<std::size_t SingularSize, std::size_t PluralSize>
    friend constexpr inline KLazyLocalizedString klxi18np(const char (&singular)[SingularSize], const char (&plural)[PluralSize]);
    template<std::size_t ContextSize, std::size_t SingularSize, std::size_t PluralSize>
    friend constexpr inline KLazyLocalizedString
    klxi18ncp(const char (&context)[ContextSize], const char (&singular)[SingularSize], const char (&plural)[PluralSize]);

    constexpr inline KLazyLocalizedString(const char *context, const char *text, const char *plural, bool markupAware)
        : m_context(context)
        , m_text(text)
        , m_plural(plural)
        , m_markupAware(markupAware)
    {
    }

    const char *m_context = nullptr;
    const char *m_text = nullptr;
    const char *m_plural = nullptr;
    bool m_markupAware = false;
};

/**
 * Mark the string @p text for extraction.
 *
 * \param text string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t TextSize>
constexpr inline KLazyLocalizedString kli18n(const char (&text)[TextSize])
{
    return KLazyLocalizedString(nullptr, text, nullptr, false);
}

/**
 * Mark the string @p text with @p context for extraction.
 *
 * \param context context of the string
 * \param text string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t ContextSize, std::size_t TextSize>
constexpr inline KLazyLocalizedString kli18nc(const char (&context)[ContextSize], const char (&text)[TextSize])
{
    return KLazyLocalizedString(context, text, nullptr, false);
}

/**
 * Mark the string @p singular and @p plural for extraction.
 *
 * \param singular singular form of the string to translate
 * \param plural plural form of the string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t SingularSize, std::size_t PluralSize>
constexpr inline KLazyLocalizedString kli18np(const char (&singular)[SingularSize], const char (&plural)[PluralSize])
{
    return KLazyLocalizedString(nullptr, singular, plural, false);
}

/**
 * Mark the string @p singular and @p plural with @p context for extraction.
 *
 * \param context context of the string
 * \param singular singular form of the string to translate
 * \param plural plural form of the string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t ContextSize, std::size_t SingularSize, std::size_t PluralSize>
constexpr inline KLazyLocalizedString kli18ncp(const char (&context)[ContextSize], const char (&singular)[SingularSize], const char (&plural)[PluralSize])
{
    return KLazyLocalizedString(context, singular, plural, false);
}

/**
 * Mark the markup-aware string @p text for extraction.
 *
 * \param text string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t TextSize>
constexpr inline KLazyLocalizedString klxi18n(const char (&text)[TextSize])
{
    return KLazyLocalizedString(nullptr, text, nullptr, true);
}

/**
 * Mark the markup-aware string @p text with @p context for extraction.
 *
 * \param context context of the string
 * \param text string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t ContextSize, std::size_t TextSize>
constexpr inline KLazyLocalizedString klxi18nc(const char (&context)[ContextSize], const char (&text)[TextSize])
{
    return KLazyLocalizedString(context, text, nullptr, true);
}

/**
 * Mark the markup-aware string @p singular and @p plural for extraction.
 *
 * \param singular singular form of the string to translate
 * \param plural plural form of the string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t SingularSize, std::size_t PluralSize>
constexpr inline KLazyLocalizedString klxi18np(const char (&singular)[SingularSize], const char (&plural)[PluralSize])
{
    return KLazyLocalizedString(nullptr, singular, plural, true);
}

/**
 * Mark the markup-aware string @p singular and @p plural with @p context for extraction.
 *
 * \param context context of the string
 * \param singular singular form of the string to translate
 * \param plural plural form of the string to translate
 * \return KLazyLocalizedString for deferred translation.
 * \since 5.89
 */
template<std::size_t ContextSize, std::size_t SingularSize, std::size_t PluralSize>
constexpr inline KLazyLocalizedString klxi18ncp(const char (&context)[ContextSize], const char (&singular)[SingularSize], const char (&plural)[PluralSize])
{
    return KLazyLocalizedString(context, singular, plural, true);
}

#endif // KLAZYLOCALIZEDSTRING_H
