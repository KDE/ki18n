// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR Unlicense OR CC0-1.0 OR 0BSD

#ifndef HELPERSPINBOX_H
#define HELPERSPINBOX_H

#include <qstring.h>
#include <qstringlist.h>
#include <type_traits>
class QDoubleSpinBox;
class QSpinBox;

/** @internal
 *
 * @file
 *
 * Provides @ref setSpinboxPrefixSuffix */

/** @internal
 *
 * @brief Helper for internationalizing spin boxes.
 *
 * Spin boxes display a number and possibly a prefix and/or suffix. However,
 * in some languages, the position of the prefix and suffix may be reversed
 * compared to English.
 *
 * Example: In English, you write 50%, but in Turkish, you write %50.
 *
 * Qt does not offer an out-of-the-box solution for this. This helper now
 * provides complete internationalization for prefixes and suffixes of
 * spin boxes.
 *
 * @tparam T QSpinBox and QDoubleSpinBox are allowed. This parameter can often
 *           be omitted.
 *
 * @param spinbox Pointer to the spin box.
 *
 * @param format A translated string in the format "prefix%1suffix".
 *
 * @post If the “format” parameter has the correct format, the spin box's
 * prefix and suffix will be set accordingly. Otherwise, they will be set
 * to an empty string.
 *
 * Usage example:
 *
 * <tt>
 * setSpinboxPrefixSuffix(mySpinBox, tr("%1%")); // Turkish: "%%1"
 * </tt>
 */
template<typename T>
void setSpinboxPrefixSuffix(T *spinbox, const QString &format)
{
    constexpr auto isIntSpin = std::is_base_of<QSpinBox, T>::value;
    constexpr auto isDoubleSpin = std::is_base_of<QDoubleSpinBox, T>::value;
    static_assert(isDoubleSpin || isIntSpin, //
                  "This template only supports QSpinBox and QDoubleSpinBox.");

    if (spinbox == nullptr) {
        return;
    }
    // QString::arg() support for %L2, %5 etc which translators might expect:
    const auto l = format.arg(QStringLiteral("%1")).split(QStringLiteral("%1"));
    if (l.count() == 2) {
        spinbox->setPrefix(l.at(0));
        spinbox->setSuffix(l.at(1));
    } else {
        spinbox->setPrefix(QString());
        spinbox->setSuffix(QString());
    }
}

#endif // HELPERSPINBOX_H
