HEADERS += \
    $$PWD/QsciLexerGlsl.h \
    $$PWD/Cebitor.h \
    $$PWD/CebErrors.h \
    $$PWD/searchbox.h \
    $$PWD/jslexer.h \
    $$PWD/lexerscheme.h

SOURCES += \
    $$PWD/QsciLexerGlsl.cpp \
    $$PWD/font_family_picker.cpp \
    $$PWD/Cebitor.cpp \
    $$PWD/CebErrors.cpp \
    $$PWD/searchbox.cpp \
    $$PWD/jslexer.cpp \
    $$PWD/lexerscheme.cpp


include("./glsl_gen.pri")
include("./js_gen.pri")

LIBS += -lqscintilla2_qt5

RESOURCES += \
    $$PWD/apis.qrc

FORMS += \
    $$PWD/searchbox.ui
