#Includes common configuration for all subdirectory .pro files.
INCLUDEPATH += . ..
WARNINGS += -Wall

TEMPLATE = lib

# The following keeps the generated files at least somewhat separate 
# from the source files.
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

win32 {
    CONFIG(debug, debug|release) {
        PRAALINE_LIB_POSTFIX = d$${PRAALINE_FILE_VER}
    } else {
        PRAALINE_LIB_POSTFIX = $${PRAALINE_FILE_VER}
    }
    LIB_SUFFIX = a
}
unix {
    CONFIG(debug, debug|release) {
        PRAALINE_LIB_POSTFIX = d$${PRAALINE_FILE_VER}
    } else {
        PRAALINE_LIB_POSTFIX = $${PRAALINE_FILE_VER}
    }
    LIB_SUFFIX = so
}
mac {
    LIB_SUFFIX = dylib
}

# Praaline core has a dependency on VAMP SDK (only for RealTime conversion)
win32-g++ {
    INCLUDEPATH += ../dependency-builds/sv/win32-mingw/include
    LIBS += -L../dependency-builds/sv/win32-mingw/lib
}
win32-msvc* {
    INCLUDEPATH += ../dependency-builds/sv/win32-msvc/include
    LIBS += -L../dependency-builds/sv/win32-msvc/lib
}
macx* {
    INCLUDEPATH += ../dependency-builds/sv/osx/include
    LIBS += -L../dependency-builds/sv/osx/lib
}

DISTFILES += \
    $$PWD/appveyor.yml
