#include <iostream>
#include <sstream>

#include "QsciLexerGlsl.h"
#include <Qsci/qsciscintilla.h>
#include "generated/GlslLexer.h"
#include "../loaderpaths.h"

//from https://github.com/NCCA/ShaderEnvironmentBuilder
//------------------------------------------------------------------------------
/// @file QsciLexerGlsl.cpp
/// @brief implementation for GLSL Lexer class
/// @author Phil Rouse
/// @version 1.0
/// @date 07/05/2016
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QsciLexerGLSL::QsciLexerGLSL(QsciScintilla *_parent):
    QsciLexerCustom(_parent)
{
    m_parent = _parent;
    m_API = new QsciAPIs(this);

    setAPIs(m_API);
    m_API->load(LoaderPaths::findObject("apis/glsl.api"));

    m_API->prepare();
    m_parent->setAutoCompletionThreshold(2);
    m_parent->setAutoCompletionFillupsEnabled(true);
    m_parent->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    m_parent->setSelectionBackgroundColor(QColor(61, 61, 52));
    m_parent->resetSelectionForegroundColor();

    m_flexScanner = new yyFlexLexer;
}

//------------------------------------------------------------------------------
QsciLexerGLSL::~QsciLexerGLSL()
{
    delete m_API;
    delete m_flexScanner;
    return;
}

//------------------------------------------------------------------------------
void QsciLexerGLSL::styleText(const int start, const int end)
{
    m_parent->autoCompleteFromAPIs();
    //return if no QsciScintilla editor
    if (!editor())
        return;
    //return if no text to be styled
    if (start == end)
        return;

    auto * data = new char[end - start + 1];

    //get text to be styled
    editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, data);
    std::istringstream istr (data);
    delete [] data;

    //style keywords

    int tok;
    int loc = 0;

    std::vector<int> tokens;
    tok = m_flexScanner->yylex(&istr);

    while (tok > 0)
    {
        tokens.push_back(tok);
        tokens.push_back(loc);
        tokens.push_back(m_flexScanner->YYLeng());
        loc = loc + m_flexScanner->YYLeng();
        tok = m_flexScanner->yylex();
    }

    int tokensCount = tokens.size();
    for (int i = 0; i < tokensCount; i = i + 3)
    {
        startStyling(start + tokens[i + 1]);
        setStyling(tokens[i + 2], tokens[i]);
    }
}

//------------------------------------------------------------------------------
QColor QsciLexerGLSL::defaultColor(const int style) const
{
    switch (style)
    {
        case StyleType::DEFAULT:
        {
            return {247, 247, 241};
        }
        case StyleType::KEYWORD:
        {
            return {249, 38, 114};
        }
        case StyleType::DATATYPE:
        {
            return {102, 216, 238};
        }
        case StyleType::NUMBER:
        {
            return {174, 129, 255};
        }
        case StyleType::OPERATOR:
        {
            return {249, 38, 114};
        }
        case StyleType::STRING:
        {
            return {230, 219, 116};
        }
        case StyleType::FUNCTION:
        {
            return {166, 226, 46};
        }
        case StyleType::COMMENT:
        {
            return {117, 113, 94};
        }
        case StyleType::HASHCOMMENT:
        {
            return {174, 129, 255};
        }
        default:
        {
            return {247, 247, 241};
        }
    }
}

//------------------------------------------------------------------------------
QColor QsciLexerGLSL::defaultPaper(const int style) const
{
    switch (style)
    {
        case StyleType::ILLEGAL:
        {
            return {249, 38, 114};
        }
        default:
        {
            return {39, 40, 34};
        }
    }
}

//------------------------------------------------------------------------------
extern QString pickBestAvailFontFamily();
QFont QsciLexerGLSL::defaultFont(const int style) const
{
    int weight = 50;
    int size = 12;
    bool italic = false;
    switch (style)
    {
        case StyleType::DEFAULT:
        {
            weight = 50;
            size = 12;
            break;
        }
        case StyleType::DATATYPE:
        {
            italic = true;
            break;
        }
    }

    return QFont(pickBestAvailFontFamily(), size, weight, italic);
}

//------------------------------------------------------------------------------
QString QsciLexerGLSL::description(const int style) const
{
    switch (style)
    {
        case StyleType::NONE:
        {
            return "NONE";
        }
        case StyleType::DEFAULT:
        {
            return "DEFAULT";
        }
        case StyleType::NUMBER:
        {
            return "NUMBER";
        }
        case StyleType::KEYWORD:
        {
            return "KEYWORD";
        }
        case StyleType::FUNCTION:
        {
            return "FUNCTION";
        }
        case StyleType::STRING:
        {
            return "STRING";
        }
        case StyleType::COMMENT:
        {
            return "COMMENT";
        }
        case StyleType::OPERATOR:
        {
            return "OPERATOR";
        }
        case StyleType::WHITESPACE:
        {
            return "WHITESPACE";
        }
        case StyleType::DATATYPE:
        {
            return "DATATYPE";
        }
        default:
        {
            return QString(style);
        }
    }
}

//------------------------------------------------------------------------------
const char *QsciLexerGLSL::language() const
{
    return "GLSL";
}
