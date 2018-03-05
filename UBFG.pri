isEmpty(TOP):TOP = .

DIR = $$TOP/work/$$[QMAKE_SPEC]
DESTDIR = $$TOP/work/bin

CONFIG(debug, debug|release) {
	DIR = $$DIR-dbg
} else {
	DIR = $$DIR-rel
}

OBJECTS_DIR = $$DIR/obj/$$TARGET
MOC_DIR = $$DIR/ui
RCC_DIR = $$DIR/ui
UI_DIR = $$DIR/ui
