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
    FLEXSOURCES_GLSL = $$PWD/GlslLexer.lex

    flexsource_GLSL.input = FLEXSOURCES_GLSL
    flexsource_GLSL.output = $$PWD/generated/${QMAKE_FILE_BASE}.cpp
    flexsource_GLSL.commands = flex --header-file=$$PWD/generated/${QMAKE_FILE_BASE}.h -o $$PWD/generated/${QMAKE_FILE_BASE}.cpp -+ ${QMAKE_FILE_IN}
    flexsource_GLSL.variable_out = SOURCES
    flexsource_GLSL.clean = $$PWD/generated/${QMAKE_FILE_BASE}.cpp
    flexsource_GLSL.name = Flex Sources ${QMAKE_FILE_IN}
    flexsource_GLSL.CONFIG += target_predeps

    QMAKE_EXTRA_COMPILERS += flexsource_GLSL

    flexheader_GLSL.input = FLEXSOURCES_GLSL
    flexheader_GLSL.output = $$PWD/generated/${QMAKE_FILE_BASE}.h
    fleakheader_GLSL.commands = @true
    flexheader_GLSL.variable_out = HEADERS
    flexheader_GLSL.clean = $$PWD/generated/${QMAKE_FILE_BASE}.h
    flexheader_GLSL.name = Flex Headers ${QMAKE_FILE_IN}
    flexheader_GLSL.CONFIG += target_predeps no_link

    QMAKE_EXTRA_COMPILERS += flexheader_GLSL
    # end of Citation
}
##----------------------------------------------------------------------------##
