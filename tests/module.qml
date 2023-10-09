/*
    SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQml
import org.kde.i18n

// Run with:
//
// $ kqml --domain plasma-discover module.qml
//
QtObject {
    readonly property string text: K.i18n("&Home")

    onTextChanged: print("Text:", text, "\n")

    Component.onCompleted: {
        helper.setLanguages(["ru_RU", "ru"]);
        helper.setLanguages(["ar_EG", "ar"]);
        Qt.quit();
    }

    readonly property KI18nTestHelper helper: KI18nTestHelper {
        id: helper
    }
}
