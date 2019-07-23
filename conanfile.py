from conans import ConanFile, CMake, tools


class Ki18nConan(ConanFile):
    name = "ki18n"
    version = "5.61.0"
    license = "GPLv2"
    url = "https://api.kde.org/frameworks/ki18n/html/index.html"
    description = "KDE Gettext-based UI text internationalization"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    options = {"with_qml": [True, False]}
    default_options = { "with_qml" : False}

    requires = (
        "extra-cmake-modules/[>=5.60.0]@kde/testing", # CMakeLists.txt requires 5.49.0
        "qt/[>=5.11.0]@bincrafters/stable",
        "gettext_installer/0.20.1@bincrafters/stable"
    )

    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
     }

    def configure(self):
        if self.options.with_qml:
            self.options["qt"].qtdeclarative = True

    def build(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_WITH_QML"] = "ON" if self.options.with_qml else "OFF"
        cmake.configure()
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.resdirs = ["share"]
