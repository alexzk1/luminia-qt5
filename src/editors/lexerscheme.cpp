#include "lexerscheme.h"
#include "QsciLexerGlsl.h"
#include <Qsci/qsciscintilla.h>
#include "../loaderpaths.h"
#include <sstream>
#include <FlexLexer.h>
#include <QDebug>

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
    //return if no QsciScintilla editor
    if (!editor())
        return;
    //return if no text to be styled
    if (start == end)
        return;

    dataBuf.resize((end - start + 1) * 3); //gona support UTF-8

    //get text to be styled

    editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, dataBuf.data());
    std::istringstream istr (dataBuf.data());
    //style keywords



    std::vector<int> tokens;
    auto flex = getFlex();


    for (int tok = flex->yylex(&istr), loc = 0; tok > 0; tok = flex->yylex())
    {
        tokens.push_back(tok); //style
        tokens.push_back(loc); //position
        auto len = flex->YYLeng();
        tokens.push_back(len); //length
        loc += len;
    }

    bool is_mlc{false};

    const auto update_mlc = [&is_mlc](int s)
    {
        if (StyleType::MLC_START == s)
            is_mlc = true;

        if (StyleType::MLC_END == s)
            is_mlc = false;
    };

    if (start > 0)
        update_mlc(editor()->SendScintilla(QsciScintilla::SCI_GETSTYLEAT, start - 1));

    // qDebug() << "START\n" << dataBuf.data() << "\nEND\n";
    for (int i = 0, tokensCount = tokens.size(); i < tokensCount; i = i + 3)
    {
        int style = tokens[i];
        startStyling(start + tokens[i + 1]);

        if (!is_mlc && style == StyleType::MLC_END)
            style = StyleType::ILLEGAL;

        update_mlc(style);

        if (is_mlc)
            style = StyleType::MLC_START;

        auto tmp = QString::fromUtf8(dataBuf.data() + tokens[i + 1], tokens[i + 2]);
        if (identifiers.contains(tmp))
        {
            if (style == StyleType::DEFAULT)
                style = StyleType::IDENTIFIER;
            //else
            // style = style | StyleType::ITALIC;
        }
        setStyling(tokens[i + 2], style);
    }
}

QString LexerScheme::description(const int style) const
{
    switch (style & ~StyleType::ITALIC)
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
        case StyleType::IDENTIFIER:
        {
            return "IDENTIFIER";
        }
        default:
        {
            return QString(style);
        }
    }
}

void LexerScheme::setIdentifiers(const QSet<QString> &newOnes)
{
    identifiers = newOnes;
}

QColor LexerScheme::defaultColor(const int style) const
{
    switch (style & ~StyleType::ITALIC)
    {
        case StyleType::IDENTIFIER:
        {
            return {"#109bd4"};
        }
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
        case StyleType::MLC_START:
        case StyleType::MLC_END:
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
    switch (style & ~StyleType::ITALIC)
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
    switch (style & ~StyleType::ITALIC)
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
    if (style & StyleType::ITALIC)
        italic = true;

    return QFont(pickBestAvailFontFamily(), size, weight, italic);
}

//-----------------------------------------------------------------------------

LexerScheme::ScannerPtr LexerScheme::getFlex()
{
    if (!m_flexScanner)
        m_flexScanner = createScanner();
    return m_flexScanner;
}
