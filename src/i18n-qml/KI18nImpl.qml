/*
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQml
import org.kde.i18n

// This wrapper exists to provide runtime bindings on current locale. It works
// because engine binds on anything accessed during binding evaluation in JS,
// including inside methods defined in QML (but not in C++).
KI18nAttachedBase {

    function i18n(
        message,
        p1,
        p2,
        p3,
        p4,
        p5,
        p6,
        p7,
        p8,
        p9,
        p10,
    ) {
        void __retranslate;
        return __context.i18n(
            message,
            p1,
            p2,
            p3,
            p4,
            p5,
            p6,
            p7,
            p8,
            p9,
            p10,
        );
    }

    function i18nd(
        domain,
        message,
        p1,
        p2,
        p3,
        p4,
        p5,
        p6,
        p7,
        p8,
        p9,
        p10,
    ) {
        void __retranslate;
        return __context.i18nd(
            domain,
            message,
            p1,
            p2,
            p3,
            p4,
            p5,
            p6,
            p7,
            p8,
            p9,
            p10,
        );
    }
}
