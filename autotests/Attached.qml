// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

import QtQml
import org.kde.ki18n

QtObject {
    id: root

    KI18n.translationDomain: "ki18n-test"

    // Expose the attached object itself and derived values for C++ assertions.
    readonly property var attached: KI18n
    readonly property string domain: KI18n.translationDomain

    function translateJob() {
        return KI18n.i18n("Job")
    }

    // Objects in the same QML file share the same QQmlContext and therefore
    // must share the same attached context.
    property QtObject child: QtObject {
        readonly property var attached: KI18n

        function translateJob() {
            return KI18n.i18n("Job")
        }
    }
}
