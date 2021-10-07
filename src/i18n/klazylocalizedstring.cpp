/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "klazylocalizedstring.h"

#include <KLocalizedString>

KLazyLocalizedString::operator KLocalizedString() const
{
    if (!m_text) {
        return KLocalizedString();
    }
    return KLocalizedString(nullptr, m_context, m_text, m_plural, m_markupAware);
}
