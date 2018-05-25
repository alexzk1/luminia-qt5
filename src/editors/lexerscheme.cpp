#include "lexerscheme.h"
#include "QsciLexerGlsl.h"
#include <Qsci/qsciscintilla.h>
#include "../loaderpaths.h"
#include <sstream>
#include <FlexLexer.h>

LexerScheme::LexerScheme(const QStringList& apisToLoad, QsciScintilla *_parent)
{
    m_parent = _parent;
    m_API = new QsciAPIs(this);

    setAPIs(m_API);

    for (const auto& a : apisToLoad)
        m_API->load(LoaderPaths::findObject(a));

    m_API->prepare();
    m_parent->setSelectionBackgroundColor(QColor(61, 61, 52));
    m_parent->resetSelectionForegroundColor();
}

void LexerScheme::styleText(const int start, const int end)
{
    m_parent->autoCompleteFromAll();
    //return if no QsciScintilla editor
    if (!editor())
        return;
    //return if no text to be styled
    if (start == end)
        return;

    dataBuf.resize(end - start + 1); // that will keep memory reserved by biggest chunk happened
    //get text to be styled
    editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, dataBuf.data());
    std::istringstream istr (dataBuf.data());

    //style keywords

    int tok;
    int loc = 0;

    std::vector<int> tokens;
    auto flex = getFlex();
    tok = flex->yylex(&istr);

    while (tok > 0)
    {
        tokens.push_back(tok);
        tokens.push_back(loc);
        tokens.push_back(flex->YYLeng());
        loc = loc + flex->YYLeng();
        tok = flex->yylex();
    }

    int tokensCount = tokens.size();
    for (int i = 0; i < tokensCount; i = i + 3)
    {
        startStyling(start + tokens[i + 1]);
        setStyling(tokens[i + 2], tokens[i]);
    }
}

QString LexerScheme::description(const int style) const
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

QColor LexerScheme::defaultColor(const int style) const
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
        case StyleType::GLOBAL:
        {
            return {"#ffc36f"};
        }
        default:
        {
            return {247, 247, 241};
        }
    }
}

//------------------------------------------------------------------------------
QColor LexerScheme::defaultPaper(const int style) const
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
QFont LexerScheme::defaultFont(const int style) const
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

//-----------------------------------------------------------------------------

LexerScheme::ScannerPtr LexerScheme::getFlex()
{
    if (!m_flexScanner)
        m_flexScanner = createScanner();
    return m_flexScanner;
}
