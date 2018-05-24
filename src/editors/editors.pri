HEADERS += \
    $$PWD/sci_incs.h \
    $$PWD/QsciLexerGlsl.h \
    $$PWD/FlexLexer.h \
    $$PWD/Cebitor.h \
    $$PWD/CebErrors.h \
    $$PWD/searchbox.h

SOURCES += \
    $$PWD/QsciLexerGlsl.cpp \
    $$PWD/font_family_picker.cpp \
    $$PWD/Cebitor.cpp \
    $$PWD/CebErrors.cpp \
    $$PWD/searchbox.cpp

#GSLS support from https://github.com/NCCA/ShaderEnvironmentBuilde

##----------------------------------------------------------------------------##
##--Flex
##----------------------------------------------------------------------------##

# Set up Flex to run on build
# Test if Flex is installed
FLEX_BIN = $$system(which flex)
isEmpty(FLEX_BIN) {
    error("Flex not found")
} else {
    message("Found Flex")

    # run Flex on .lex file to generate lexer

    # The following section is from :-
    # hipersayan_x (March 1, 2013)  The Samurai Code: Using Flex and Bison with Qt. [online]
    # [Accessed 18 Feb. 2016] Available at: <http://hipersayanx.blogspot.co.uk/2013/03/using-flex-and-bison-with-qt.html>.
    FLEXSOURCES = $$PWD/GlslLexer.lex

    flexsource.input = FLEXSOURCES
    flexsource.output = $$PWD/generated/${QMAKE_FILE_BASE}.cpp
    flexsource.commands = flex --header-file=$$PWD/generated/${QMAKE_FILE_BASE}.h -o $$PWD/generated/${QMAKE_FILE_BASE}.cpp -+ ${QMAKE_FILE_IN}
    flexsource.variable_out = SOURCES
    flexsource.clean = $$PWD/generated/${QMAKE_FILE_BASE}.cpp
    flexsource.name = Flex Sources ${QMAKE_FILE_IN}
    flexsource.CONFIG += target_predeps

    QMAKE_EXTRA_COMPILERS += flexsource

    flexheader.input = FLEXSOURCES
    flexheader.output = $$PWD/generated/${QMAKE_FILE_BASE}.h
    fleakheader.commands = @true
    flexheader.variable_out = HEADERS
    flexheader.clean = $$PWD/generated/${QMAKE_FILE_BASE}.h
    flexheader.name = Flex Headers ${QMAKE_FILE_IN}
    flexheader.CONFIG += target_predeps no_link

    QMAKE_EXTRA_COMPILERS += flexheader
    # end of Citation
}
##----------------------------------------------------------------------------##
LIBS += -lqscintilla2_qt5

RESOURCES += \
    $$PWD/apis.qrc

FORMS += \
    $$PWD/searchbox.ui
