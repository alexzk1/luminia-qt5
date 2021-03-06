#ifndef _QSCILEXERGLSL_H__
#define _QSCILEXERGLSL_H__
#include "lexerscheme.h"

class QsciLexerGLSL: public LexerScheme
{
public:
    QsciLexerGLSL( QsciScintilla *_parent = nullptr );
    //----------------------------------------------------------------------------
    /// @brief QsciLexerGLSL destructor.
    //----------------------------------------------------------------------------
    ~QsciLexerGLSL() override = default;
    //----------------------------------------------------------------------------
    const char* language() const override;
    //----------------------------------------------------------------------------
protected:
    ScannerPtr createScanner() const override;
};// end class

#endif
