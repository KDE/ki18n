/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KTIMEZONE_H
#define KTIMEZONE_H

#include "ki18n_export.h"

class KCountry;

/** Timezone localization methods.
 *  @since 5.88
 */
namespace KTimeZone // TODO name clash with kdelibs4support!?
{
/** Returns the timezone at the given geo coordinate. */
KI18N_EXPORT const char *fromLocation(float latitude, float longitude);

/** Returns the country a timezone is in. */
KI18N_EXPORT KCountry country(const char *ianaId);

}

#endif // KTIMEZONE_H
