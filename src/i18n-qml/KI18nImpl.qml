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
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
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

    function i18nc(
        context: string,
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.i18nc(
            context,
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

    function i18np(
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.i18np(
            singular,
            plural,
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

    function i18ncp(
        context: string,
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.i18ncp(
            context,
            singular,
            plural,
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
        domain: string,
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
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

    function i18ndc(
        domain: string,
        context: string,
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.i18ndc(
            domain,
            context,
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

    function i18ndp(
        domain: string,
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.i18ndp(
            domain,
            singular,
            plural,
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

    function i18ndcp(
        domain: string,
        context: string,
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.i18ndcp(
            domain,
            context,
            singular,
            plural,
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

    function xi18n(
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18n(
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

    function xi18nc(
        context: string,
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18nc(
            context,
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

    function xi18np(
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18np(
            singular,
            plural,
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

    function xi18ncp(
        context: string,
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18ncp(
            context,
            singular,
            plural,
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

    function xi18nd(
        domain: string,
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18nd(
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

    function xi18ndc(
        domain: string,
        context: string,
        message: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18ndc(
            domain,
            context,
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

    function xi18ndp(
        domain: string,
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18ndp(
            domain,
            singular,
            plural,
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

    function xi18ndcp(
        domain: string,
        context: string,
        singular: string,
        plural: string,
        p1: var,
        p2: var,
        p3: var,
        p4: var,
        p5: var,
        p6: var,
        p7: var,
        p8: var,
        p9: var,
        p10: var,
    ): string
    {
        void __retranslate;
        return __context.xi18ndcp(
            domain,
            context,
            singular,
            plural,
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
