#ifndef LEXERSCHEME_H
#define LEXERSCHEME_H

#include <memory>

#include <Qsci/qscilexercustom.h>
#include <Qsci/qscistyle.h>
#include <Qsci/qsciapis.h>
#include "no_copy.h"
#include <QStringList>
#include <QPointer>
#include <QSet>

//------------------------------------------------------------------------------
/// @file QsciLexerGlsl.h
/// @brief Custom QScintilla lexer for GLSL
/// @author Phil Rouse
/// @version 1.0
/// @date 07/05/2016
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Predeclare FlexLexer class here since including GlslLexer.h in header breaks
/// flex, including in .cpp works fine though...?
//------------------------------------------------------------------------------
class FlexLexer;
class LexerScheme: public QsciLexerCustom, public utility::NoCopyAssignMove
{
public:
    enum StyleType
    {
        NONE = 0,         ///<No styling
        DEFAULT,      ///<Default
        NUMBER,       ///<Numbers
        KEYWORD,      ///<GLSL Keywords
        IDENTIFIER,   ///something which looks like variable name and not recognized as anything else
        FUNCTION,     ///<Functions
        STRING,       ///<Strings
        COMMENT,      ///<Comments
        OPERATOR,     ///<Operators
        WHITESPACE,   ///<Whitespace
        DATATYPE,     ///<GLSL datatypes
        HASHCOMMENT,  ///<Preprocessor commands starting with #
        GLOBAL,       ///<Global gl_ variables
        MLC_START,    ///multiline comment start
        MLC_END,      ///multiline comment end
        ILLEGAL,      ///<Illegal GLSL characters
        ITALIC = 0x1000,
    };

    LexerScheme(const QStringList &apisToLoad, QsciScintilla *_parent = nullptr);
    //----------------------------------------------------------------------------
    /// @brief QsciLexerGLSL destructor.
    //----------------------------------------------------------------------------
    ~LexerScheme() override = default;

    void styleText( const int _start, const int _end ) final;

    QColor defaultColor( const int _style ) const override;
    //----------------------------------------------------------------------------
    /// @brief virtual function reimplemented from QsciLexer.
    /// @param [in] _style  style number.
    /// @return Default paper colour for style \a _style.
    //----------------------------------------------------------------------------
    QColor defaultPaper( const int _style ) const override;
    //----------------------------------------------------------------------------
    /// @brief virtual function reimplemented from QsciLexer.
    /// @param [in] _style  style number.
    /// @return Default font for style \a _style.
    //----------------------------------------------------------------------------
    QFont defaultFont( const int _style ) const override;
    //----------------------------------------------------------------------------
    /// @brief virtual function reimplemented from QsciLexer.
    /// @param [in] _style  style number.
    /// @return Descriptive for style \a _style.
    //----------------------------------------------------------------------------
    QString description(const int style ) const override;

    virtual void addIdentifiers(const QSet<QString>& newOnes);
protected:
    using ScannerPtr = std::shared_ptr<FlexLexer>;

    //each lexer must have own include to own generated cpp from lex.
    virtual ScannerPtr createScanner() const = 0;
    ScannerPtr getFlex();
    //----------------------------------------------------------------------------
    /// @brief parent QScintilla instance
    //----------------------------------------------------------------------------
    QPointer<QsciScintilla>   m_parent{nullptr};
    //----------------------------------------------------------------------------
    /// @brief GLSL API instance
    //----------------------------------------------------------------------------
    QPointer<QsciAPIs>  m_API{nullptr};

private:
    //----------------------------------------------------------------------------
    /// @brief flex lexer instance
    //----------------------------------------------------------------------------
    ScannerPtr     m_flexScanner{};
    //----------------------------------------------------------------------------
    QSet<QString>     identifiers{};
    std::vector<char> dataBuf;
};

#endif // LEXERSCHEME_H
