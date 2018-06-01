#include "jslexer.h"

//this is most important personal include, it contains geberanated code. For another lexer must be another include
#undef yyFlexLexer
#define yyFlexLexer JSFlexLexer

#include "generated/JsLexer.h"
//------------------------------------------------------------------------------
JSLexer::JSLexer(QsciScintilla *_parent):
    LexerScheme({"apis/obj_gl.api"}, _parent)
{
    addIdentifiers({"World", "gl"});
}
//------------------------------------------------------------------------------
const char *JSLexer::language() const
{
    return "JavaScript";
}

QStringList JSLexer::autoCompletionWordSeparators() const
{
    return {"."}; //objects support
}
//------------------------------------------------------------------------------
LexerScheme::ScannerPtr JSLexer::createScanner() const
{
    return LexerScheme::ScannerPtr(new yyFlexLexer);
}
//------------------------------------------------------------------------------
