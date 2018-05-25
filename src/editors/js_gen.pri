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
    FLEXSOURCES_JS = $$PWD/JsLexer.lex

    flexsource_JS.input = FLEXSOURCES_JS
    flexsource_JS.output = $$PWD/generated/${QMAKE_FILE_BASE}.cpp
    flexsource_JS.commands = flex --header-file=$$PWD/generated/${QMAKE_FILE_BASE}.h -o $$PWD/generated/${QMAKE_FILE_BASE}.cpp -+ ${QMAKE_FILE_IN}
    flexsource_JS.variable_out = SOURCES
    flexsource_JS.clean = $$PWD/generated/${QMAKE_FILE_BASE}.cpp
    flexsource_JS.name = Flex Sources ${QMAKE_FILE_IN}
    flexsource_JS.CONFIG += target_predeps

    QMAKE_EXTRA_COMPILERS += flexsource_JS

    flexheader_JS.input = FLEXSOURCES_JS
    flexheader_JS.output = $$PWD/generated/${QMAKE_FILE_BASE}.h
    fleakheader_JS.commands = @true
    flexheader_JS.variable_out = HEADERS
    flexheader_JS.clean = $$PWD/generated/${QMAKE_FILE_BASE}.h
    flexheader_JS.name = Flex Headers ${QMAKE_FILE_IN}
    flexheader_JS.CONFIG += target_predeps no_link

    QMAKE_EXTRA_COMPILERS += flexheader_JS
    # end of Citation
}
##----------------------------------------------------------------------------##
