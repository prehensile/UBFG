TARGET = sdf-gen

SOURCES += sdf-gen.cpp \
    ../src/sdf.cpp \
    main.cpp
HEADERS += ../src/sdf.h \
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

QMAKE_CXXFLAGS += -fopenmp -std=c++0x
QMAKE_LINK += -fopenmp
