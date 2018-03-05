TARGET = sdf-gen

TOP = ..
include("../UBFG.pri")

SOURCES += sdf-gen.cpp \
    ../ubfg/sdf.cpp \
    main.cpp
HEADERS += ../ubfg/sdf.h \
    sdf-gen.h

CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

RESOURCES = sdf-gen.qrc
CONFIG(with_ru_ts): TRANSLATIONS = ru.ts

.SUFFIXES.depends = .ts .qm
.ts.qm.commands = lrelease -qm $@ $<
QMAKE_EXTRA_TARGETS += .SUFFIXES .ts.qm

"$(TARGET)".depends = $$replace(TRANSLATIONS, .ts, .qm)
QMAKE_EXTRA_TARGETS += $(TARGET)

enable_openmp {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LINK += -fopenmp
}
