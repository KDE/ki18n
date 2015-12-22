import QtQml 2.1

QtObject
{
    readonly property string testString: i18n("Awesome")
    readonly property string testStringPlural: i18ndp("plasma_lookandfeel_org.kde.lookandfeel", "in 1 second", "in %1 seconds", 3);
}
