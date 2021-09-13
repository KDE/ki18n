/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ktimezone.h"
#include "data/timezone_country_map.cpp"
#include "data/timezone_name_table.cpp"
#include "kcountry.h"
#include "spatial_index_p.h"

#include <cstring>

const char *KTimeZone::fromLocation(float latitude, float longitude)
{
    const auto entry = SpatialIndex::lookup(latitude, longitude);
    return timezone_name_table + entry.m_tz;
}

KCountry KTimeZone::country(const char *ianaId)
{
    if (!ianaId) {
        return {};
    }

    const auto it = std::lower_bound(std::begin(timezone_country_map), std::end(timezone_country_map), ianaId, [](auto lhs, auto rhs) {
        return std::strcmp(timezone_name_table + lhs.key, rhs) < 0;
    });
    if (it != std::end(timezone_country_map) && std::strcmp(timezone_name_table + (*it).key, ianaId) == 0) {
        KCountry c;
        c.d = (*it).value;
        return c;
    }
    return {};
}
