#include "QsciLexerGlsl.h"

//this is most important personal include, it contains geberanated code. For another lexer must be another include
#include "generated/GlslLexer.h"
//------------------------------------------------------------------------------
QsciLexerGLSL::QsciLexerGLSL(QsciScintilla *_parent):
    LexerScheme({"apis/glsl.api"}, _parent)
{
}
//------------------------------------------------------------------------------
const char *QsciLexerGLSL::language() const
{
    return "GLSL";
}
//------------------------------------------------------------------------------
LexerScheme::ScannerPtr QsciLexerGLSL::createScanner() const
{
    return LexerScheme::ScannerPtr(new yyFlexLexer);
}
//------------------------------------------------------------------------------
