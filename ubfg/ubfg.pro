TARGET = UBFG

TOP = ..
include("../UBFG.pri")

QT_VERSION=$$[QT_VERSION]

contains(QT_VERSION, "^5.*") {
  QT += widgets
}

QT += opengl

exists($$(HOME)/Private/Projekty/_commonRuntime): COMMON_ROOT = $$(HOME)/Private/Projekty/_commonRuntime
else:exists($$(HOME)/Private/Projekty/0.rt/_commonRuntime): COMMON_ROOT = $$(HOME)/Private/Projekty/0.rt/_commonRuntime
else:message("($$TARGET) Missing common runtime path!")
exists($$(HOME)/Private/Projekty/_developerTools): DEVTOOLS_ROOT = $$(HOME)/Private/Projekty/_developerTools
else:message("($$TARGET) Missing devtools path!")

TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    fontrender.cpp \
    fontview.cpp \
    extrawidgets.cpp \
    imagepacker.cpp \
    imagecrop.cpp \
    guillotine.cpp \
    imagesort.cpp \
    sdf.cpp \
    maxrects.cpp \
    watcher.cpp
HEADERS += mainwindow.h \
    fontrender.h \
    fontview.h \
    extrawidgets.h \
    imagepacker.h \
    guillotine.h \
    sdf.h \
    maxrects.h \
    watcher.h
SOURCES += \
    $$COMMON_ROOT/SingleFile/freetype/FreeTypeAmalgam.c $$COMMON_ROOT/SingleFile/freetype/FreeTypeQtFile.cpp

include("$$DEVTOOLS_ROOT/font_tools/font2img-tools/freetypegl/freetype-gl/_freetype-gl-dist/freetype-gl.pri")
include("../third/qctemplatelib/qctemplatelib.pri")

INCLUDEPATH += $$COMMON_ROOT/SingleFile/freetype

FORMS += mainwindow.ui
RESOURCES += res/res.qrc
QMAKE_CXXFLAGS -= -fopenmp
QMAKE_LDFLAGS += -fopenmp
ICON = ../app.icns
