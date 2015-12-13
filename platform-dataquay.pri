
exists(config.pri) {
    include(./config.pri)
}

!exists(config.pri) {
    CONFIG += release
    win32-g++ {
        INCLUDEPATH += ../sv-dependency-builds/win32-mingw/include
        LIBS += -L../sv-dependency-builds/win32-mingw/lib
    }
    win32-msvc* {
        INCLUDEPATH += ../sv-dependency-builds/win32-msvc/include
        LIBS += -L../sv-dependency-builds/win32-msvc/lib
    }
    macx* {
        INCLUDEPATH += ../sv-dependency-builds/osx/include
        LIBS += -L../sv-dependency-builds/osx/lib
    }
}

CONFIG += staticlib c++11

DEFINES += USE_SORD
# Libraries and paths should be added by config.pri

