#ifndef JSLEXER_H
#define JSLEXER_H

#include "lexerscheme.h"

class JSLexer: public LexerScheme
{
public:
    JSLexer( QsciScintilla *_parent = nullptr );
    //----------------------------------------------------------------------------
    /// @brief QsciLexerGLSL destructor.
    //----------------------------------------------------------------------------
    ~JSLexer() override = default;
    //----------------------------------------------------------------------------
    const char* language() const override;
    //----------------------------------------------------------------------------
protected:
    ScannerPtr createScanner() const override;
};// end class


#endif // JSLEXER_H
