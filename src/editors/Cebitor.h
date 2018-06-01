#ifndef _CEBITOR_H__
#define _CEBITOR_H__

#include <QPointer>
#include <Qsci/qsciscintilla.h>

//------------------------------------------------------------------------------
/// @file Cebitor.h
/// @brief CEB text editor, implementation of QScintilla
/// @author Phil Rouse
/// @version 1.0.1
/// @date 03/03/16
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// @class Cebitor
/// @brief CEB text editor class inherits from QsciScintilla
//------------------------------------------------------------------------------
class SearchBox;
class QTimer;
class Cebitor : public QsciScintilla
{
    Q_OBJECT;
public:
    //----------------------------------------------------------------------------
    /// @brief Enum for supported marker types
    //----------------------------------------------------------------------------
    enum MarkerType
    {
        ERROR,    ///< Symbol marker for Errors
        WARNING,  ///< Symbol marker for warnings
    };
    //----------------------------------------------------------------------------
    /// @brief Cebitor constructor, initialises default values
    //----------------------------------------------------------------------------
    Cebitor(SearchBox* searchbox, QWidget *_parent );
    //----------------------------------------------------------------------------
    QSize sizeHint() const override;

    template<class LexerT>
    QPointer<LexerT> makeLexer()
    {
        auto L = new LexerT(this);
        setLexer(L);
        initAfterLexer();
        return L;
    }

public slots:
    //----------------------------------------------------------------------------
    /// @brief Removes all error and warning line markers
    //----------------------------------------------------------------------------
    void clearErrors();
    //----------------------------------------------------------------------------
    /// @brief Search for next occurance
    //----------------------------------------------------------------------------

    /// @brief sets indicator for each search result
    //----------------------------------------------------------------------------
    void highlightAllSearch();
    //----------------------------------------------------------------------------
protected:
    //----------------------------------------------------------------------------
    QPointer<SearchBox> m_searchBox;
    //----------------------------------------------------------------------------
    /// @brief vector of all line markers
    //----------------------------------------------------------------------------
    std::vector<int>  m_fileMarkers;
    //----------------------------------------------------------------------------
    /// @brief indicator used to highlight all search terms
    //----------------------------------------------------------------------------
    int               m_searchIndicator;
    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    /// @brief adds closing character for braces and quotes
    /// @param [in] _close closing character
    /// @returns true if closing character is added
    //----------------------------------------------------------------------------
    bool autoClose( const QString& _close );
    //----------------------------------------------------------------------------
    /// @brief removes duplicates of closing braces and quotes
    /// @param [in] _close closing character
    /// @returns true if duplicate character is replaced
    //----------------------------------------------------------------------------
    bool closing( const QString& _close );
    //----------------------------------------------------------------------------
    /// @brief adds indentation after newline following a "{"
    //----------------------------------------------------------------------------
    void braceIndent();
    //----------------------------------------------------------------------------

protected slots:
    void searchNext();
    //----------------------------------------------------------------------------
    /// @brief Search for previous occurance
    //----------------------------------------------------------------------------
    void searchPrev();
    //----------------------------------------------------------------------------
    /// @brief comment out all selected lines
    //----------------------------------------------------------------------------
    void comment();
    //----------------------------------------------------------------------------
    /// @brief toggle search widget
    //----------------------------------------------------------------------------
    void toggleSearchBox();
    //----------------------------------------------------------------------------
    /// @brief called when a character is added
    //----------------------------------------------------------------------------
    void charAdded( const int _c );
    //----------------------------------------------------------------------------
    /// @brief reset the highlighting colour
    //----------------------------------------------------------------------------
    void resetHighlightColour();
    //----------------------------------------------------------------------------
    void toggleAutocomplete();
private:
    void initAfterLexer();
    QPointer<QTimer> autoCompleteTrigger;
signals:
    void F5pressed(); //F5 pressed
    void F2pressed(); //F2 pressed
}; // end class

#endif  // _CEBITOR_H__
