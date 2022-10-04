QT      = core gui widgets
CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/CharTableModel.cpp \
    src/CharTableView.cpp \
    src/GroupsTreeModel.cpp \
    src/GroupsTreeView.cpp \
    src/MainWindow.cpp \
    src/UnicodeGroups.cpp \
    src/UnicodePoints.cpp \
    src/main.cpp

HEADERS += \
    src/CharTableModel.h \
    src/CharTableView.h \
    src/GroupsTreeModel.h \
    src/GroupsTreeView.h \
    src/MainWindow.h \
    src/Settings.h \
    src/Tree.h \
    src/UnicodeGroups.h \
    src/UnicodePoints.h

FORMS += \
    ui/LicenseDialog.ui \
    ui/MainWindow.ui

RESOURCES += \
    resources/SimpleCharmap.qrc

TRANSLATIONS += \
    translations/SimpleCharmap_en.ts \
    translations/SimpleCharmap_it.ts

INCLUDEPATH += src

CONFIG(debug, debug|release) {
    DESTDIR = debug
    OBJECTS_DIR = intermediates/debug/obj
    MOC_DIR = intermediates/debug/moc
    RCC_DIR = intermediates/debug/rcc
    UI_DIR = intermediates/debug/ui
}

CONFIG(release, debug|release) {
    DESTDIR = release
    OBJECTS_DIR = intermediates/release/obj
    MOC_DIR = intermediates/release/moc
    RCC_DIR = intermediates/release/rcc
    UI_DIR = intermediates/release/ui
}

CONFIG += lrelease
QMAKE_LRELEASE_FLAGS = -idbased

# windows stuff

win32-msvc* {
    QMAKE_CXXFLAGS += /MP
}

win32 {
    RC_ICONS = assets/icon.ico

    VERSION = 1.0.0.0
    QMAKE_TARGET_COMPANY = jager012
    QMAKE_TARGET_DESCRIPTION = Just a simple character map
    QMAKE_TARGET_COPYRIGHT = Copyright (C) 2022 jager012. Distributed under the GNU General Public License v3.0
    QMAKE_TARGET_PRODUCT = Simple Charmap
    QMAKE_TARGET_ORIGINAL_FILENAME = SimpleCharmap.exe
    QMAKE_TARGET_INTERNALNAME = SimpleCharmap.exe

    QMAKE_POST_LINK = xcopy /y assets\codepoints.dat \"$$DESTDIR\\\"
    QMAKE_POST_LINK += && xcopy /y assets\groups.xml \"$$DESTDIR\\\"
    QMAKE_POST_LINK += && xcopy /y /i \"$$DESTDIR\\*.qm\" \"$$DESTDIR\\translations\\\"
    QMAKE_POST_LINK += && del \"$$DESTDIR\\*.qm\"

    CONFIG(release) {
        for(lib, QT): QMAKE_POST_LINK += && xcopy /y \"$$[QT_INSTALL_BINS]\\Qt6$${lib}.dll\" \"$$DESTDIR\\\"

        QMAKE_POST_LINK += && xcopy /y \"$$[QT_INSTALL_PLUGINS]\\platforms\\qwindows.dll\" \"$$DESTDIR\\platforms\\\"
        QMAKE_POST_LINK += && xcopy /y \"$$[QT_INSTALL_PLUGINS]\\styles\\qwindowsvistastyle.dll\" \"$$DESTDIR\\styles\\\"
        QMAKE_POST_LINK += && xcopy /y \"$$(VCToolsRedistDir)\\vc_redist.x64.exe\" \"$$DESTDIR\\\"
    }
}

target.files += $$DESTDIR/$$TARGET assets/codepoints.dat assets/groups.xml
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}
!isEmpty(target.path): INSTALLS += target
QM_FILES_INSTALL_PATH = $${target.path}/translations

desktopentry.path = /usr/share/applications
desktopentry.files += SimpleCharmap.desktop
unix:!android: INSTALLS += desktopentry

DISTFILES += \
    assets/codepoints.dat \
    assets/groups.xml
