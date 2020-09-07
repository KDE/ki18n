# KI18n

KDE Gettext-based UI text internationalization

## Introduction

KI18n provides functionality for internationalizing user interface text
in applications, based on the GNU Gettext translation system.
It wraps the standard Gettext functionality, so that the programmers
and translators can use the familiar Gettext tools and workflows.

KI18n provides additional functionality as well, for both programmers
and translators, which can help to achieve a higher overall quality
of source and translated text. This includes argument capturing,
customizable markup, and translation scripting.


## Usage

If you are using CMake, you need to have

    find_package(KF5I18n NO_MODULE)
    ki18n_install(po)

(or similar) in your CMakeLists.txt file, and you need to link to KF5::I18n.

When using QML, the QML Engine has to be setup for using i18n methods, which is done by creating a KLocalizedContext and registering it.

Information on using KI18n as a translation mechanism can be found in the
[programmer's guide](@ref prg_guide) and the [translator's guide](@ref trn_guide).


