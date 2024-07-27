/*
    SPDX-FileCopyrightText: 2024 Lukas Sommer <sommerluk@gmail.com>
    SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KLOCALIZATION_H
#define KLOCALIZATION_H

#include "klocalizedstring.h"

#include <QObject>

#include <type_traits>

class QDoubleSpinBox;
class QSpinBox;

/**
 * @namespace KLocalization
 * @brief Namespace containing helpers for localization.
 * @since 6.5
 */
namespace KLocalization
{

///@cond hidden
namespace Private
{

constexpr inline const char SpinBoxFormatStringProperty[] = "__KLocalizationFormatStringPrivate";

template<typename T>
inline void updateSpinboxPrefixSuffix(T *spinBox)
{
    const auto lString = spinBox->property(SpinBoxFormatStringProperty).template value<KLocalizedString>();
    const auto translation = lString.subs(spinBox->value()).toString();
    const auto parts = translation.split(QLatin1StringView("%v"));
    if (parts.count() == 2) {
        spinBox->setPrefix(parts.at(0));
        spinBox->setSuffix(parts.at(1));
    } else {
        spinBox->setPrefix(QString());
        spinBox->setSuffix(QString());
    }
}

}
///@endcond

/**
 * @brief Sets up a format string for internationalizing spin boxes.
 *
 * This function allows the customization of prefix and suffix for spin boxes
 * (QSpinBox and QDoubleSpinBox), considering internationalization needs.
 *
 * Spin boxes display a number and possibly a prefix and/or suffix. However,
 * in some languages, the position of the prefix and suffix may be reversed
 * compared to English. Example: In English, you write 50%, but in Turkish,
 * you write %50. Qt does not offer an out-of-the-box solution for this. This
 * helper now provides complete internationalization for prefixes and suffixes
 * of spin boxes.
 *
 * For QSpinBox it also provides correct plural handling by installing a
 * handler for the valueChanged() signal to update prefix and suffix whenever
 * the spin box value changes in the future.
 *
 * Example usage:
 * @code
 * QDoubleSpinBox doubleBox;
 * KLocalization::setupFormatString(&doubleBox, ki18nc("@item", "%v%"));
 * // Turkish translation: "%%v"
 *
 * QSpinBox intBox;
 * KLocalization::setupFormatString(&intBox, ki18ncp("@item", "Baking %v cake", "Baking %v cakes"));
 * @endcode
 *
 * @tparam T The type of the spin box, which must be either QSpinBox or QDoubleSpinBox.
 * @param spinBox Pointer to the spin box.
 * @param formatString A localized string in the format "PREFIX%vSUFFIX".
 * For QDoubleSpinBox, it does not handle plural forms. For QSpinBox, it is
 * recommended to handle plural forms.
 *
 * @note It is safe to call this function multiple times on the same spin box.
 *
 * @sa @ref retranslateFormatString
 *
 * @since 6.5
 */

template<typename T>
inline void setupFormatString(T *spinBox, const KLocalizedString &formatString)
{
    if constexpr (std::is_base_of_v<QSpinBox, T>) {
        const bool hasSetup = !spinBox->property(Private::SpinBoxFormatStringProperty).isNull();
        if (!hasSetup) {
            QObject::connect(spinBox, &T::valueChanged, spinBox, [spinBox]() {
                Private::updateSpinboxPrefixSuffix(spinBox);
            });
        }
        // Using relaxSubs() to avoid error marks if the library user did pass
        // a singular-only KLocalizedString.
        spinBox->setProperty(Private::SpinBoxFormatStringProperty, QVariant::fromValue(formatString.relaxSubs()));
        Private::updateSpinboxPrefixSuffix(spinBox);
    } else if constexpr (std::is_base_of_v<QDoubleSpinBox, T>) {
        // Using relaxSubs() to avoid error marks if the library user did pass
        // a singular-only KLocalizedString.
        spinBox->setProperty(Private::SpinBoxFormatStringProperty, QVariant::fromValue(formatString.relaxSubs()));
        Private::updateSpinboxPrefixSuffix(spinBox);
    } else {
        // Android's Clang triggers the else branch unconditionally, even when just including this file...
#ifndef Q_OS_ANDROID
        static_assert(false, "First argument must be a QSpinBox or QDoubleSpinBox.");
#endif
    }
}

/**
 * @brief Retranslates a previously set up format string to the current
 * language and updates the spin box.
 *
 * This function updates the prefix and suffix of a spin box to reflect the
 * current language settings. It is useful for responding to language changes,
 * such as those triggered by QEvent::LanguageChange.
 *
 * @tparam T The type of the spin box, which must be either QSpinBox or
 * QDoubleSpinBox.
 * @param spinBox Pointer to the spin box.
 *
 * @post The prefix and suffix of the spin box are updated to reflect the
 * current language.
 *
 * @sa @ref setupFormatString
 *
 * @since 6.5
 */

template<typename T>
inline void retranslateFormatString(T *spinBox)
{
    if constexpr (std::is_base_of_v<QSpinBox, T> || std::is_base_of_v<QDoubleSpinBox, T>) {
        Private::updateSpinboxPrefixSuffix(spinBox);
    } else {
#ifndef Q_OS_ANDROID
        static_assert(false, "First argument must be a QSpinBox or QDoubleSpinBox.");
#endif
    }
}

}

#endif
