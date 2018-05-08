#ifndef SCRIPT_HEADER_PARSER_H
#define SCRIPT_HEADER_PARSER_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QList>
#include <QIcon>
#include <memory>

class script_header_parser
{
public:

    struct FuncProto
    {
        QString resultType;
        QString funcFullProto;
        QString funcNameOnly;


        FuncProto() = default;
        FuncProto(const QString& innerText);
        bool isEmpty() const;
        void parseProto(QString innerText);
    };

    struct ScriptAction
    {
        QString   text;
        QIcon     icon;
        FuncProto slot;
        ScriptAction() = default;
        ScriptAction(const QString& innerText);

        bool isEmpty() const;
        void parseProto(const QString& innerText);
    };
    using FuncsList   = QList<FuncProto>;
    using ActionsList = QList<ScriptAction>;
public:
    script_header_parser(const QString& fileText);
    script_header_parser(QFile& file);
    QString getFileText() const;
    operator QString () const; //will be using to load scipt files everywhere, so proper utf8, etc.
    QString             tagFilter() const;
    QString             tagName() const;
    QString             tagDescription() const;
    bool                tagToggle() const;
    FuncsList           tagFunction() const;
    ActionsList         tagAction() const;
    QIcon               tagXpm() const;

    static QString      loadScriptFile(QFile& file);
private:
    QString fileText;

    static QString     getSingleMatch(const QString &exp, const QString& where);
    static QStringList getAllMatches(const QString &exp, const QString& where);
    static QIcon       getXpm(QString tag);
};

using ScriptFile    = script_header_parser; //alias, think its better name
using ScriptFilePtr = std::shared_ptr<ScriptFile>;

#endif // SCRIPT_HEADER_PARSER_H
