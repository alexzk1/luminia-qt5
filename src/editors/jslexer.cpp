#include "jslexer.h"

//this is most important personal include, it contains geberanated code. For another lexer must be another include
#undef yyFlexLexer
#define yyFlexLexer JSFlexLexer

#include "generated/JsLexer.h"
//------------------------------------------------------------------------------
JSLexer::JSLexer(QsciScintilla *_parent):
    LexerScheme({}, _parent)
{
}
//------------------------------------------------------------------------------
const char *JSLexer::language() const
{
    return "JavaScript";
}
//------------------------------------------------------------------------------
LexerScheme::ScannerPtr JSLexer::createScanner() const
{
    return LexerScheme::ScannerPtr(new yyFlexLexer);
}
//------------------------------------------------------------------------------
