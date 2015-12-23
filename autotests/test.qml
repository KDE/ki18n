import QtQml 2.1

QtObject
{
    readonly property real numOne: 1
    readonly property real numThree: 3
    readonly property string testString: i18n("Awesome")
    readonly property string testStringSingular: i18np("and %1 other window", "and %1 other windows", numOne);
    readonly property string testStringPlural: i18np("and %1 other window", "and %1 other windows", numThree);
    readonly property string testStringPluralWithDomain: i18ndp("plasma_lookandfeel_org.kde.lookandfeel", "in 1 second", "in %1 seconds", 3);
}
