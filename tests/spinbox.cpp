/*
    SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KLocalization>

#include <QApplication>
#include <QDoubleSpinBox>
#include <QSpinBox>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QSpinBox spinBox;
    KLocalization::setupFormatString(&spinBox, ki18np("Baking %v pizza", "Baking %v pizzas"));
    KLocalization::setupFormatString(&spinBox, ki18np("Baking %v cake", "Baking %v cakes"));
    KLocalization::retranslateFormatString(&spinBox);
    spinBox.show();

    QSpinBox doubleSpinBox;
    KLocalization::setupFormatString(&doubleSpinBox, ki18n("%v%"));
    KLocalization::retranslateFormatString(&doubleSpinBox);
    doubleSpinBox.show();

    return app.exec();
}
