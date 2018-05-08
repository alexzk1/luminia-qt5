#include "script_header_parser.h"

script_header_parser::script_header_parser(const QString &fileText):
    fileText(fileText)
{
}

script_header_parser::script_header_parser(QFile &file)
{
    fileText = loadScriptFile(file);
}

QString script_header_parser::getFileText() const
{
    return fileText;
}

script_header_parser::operator QString() const
{
    return getFileText();
}

QString script_header_parser::tagFilter() const
{
    return getSingleMatch("<FILTER>(.+)</FILTER>", fileText);
}

QString script_header_parser::tagName() const
{
    return getSingleMatch("<NAME>(.+)</NAME>", fileText);
}

QString script_header_parser::tagDescription() const
{
    return getSingleMatch("<DESCRIPTION>(.+)</DESCRIPTION>", fileText);
}

bool script_header_parser::tagToggle() const
{
    QRegExp px( ".*<TOGGLE/>.*" );
    return px.exactMatch(fileText);
}

script_header_parser::FuncsList script_header_parser::tagFunction() const
{
    FuncsList res;
    auto list = getAllMatches("<FUNCTION>(.+)</FUNCTION>", fileText);
    for (auto& s : list)
    {
        FuncProto r(s);
        if (!r.isEmpty())
            res.push_back(r);
    }
    return res;
}

script_header_parser::ActionsList script_header_parser::tagAction() const
{
   ActionsList res;
   auto list = getAllMatches("<ACTION>(.+)</ACTION>", fileText);
   for (const auto& atext : list)
   {
       ScriptAction action(atext);
       if (!action.isEmpty())
           res.push_back(action);
   }
   return res;
}

QIcon script_header_parser::tagXpm() const
{
    auto tag = getSingleMatch("<XPM>.+\"(.+)\".+</XPM>", fileText);
    if (tag.isEmpty())
        return QIcon();
    return getXpm(tag);
}

QString script_header_parser::loadScriptFile(QFile &file)
{
    QString fileText;
    if (!file.isOpen())
        file.open(QIODevice::ReadOnly);

    if (file.isOpen())
    {
        fileText = QString::fromUtf8(file.readAll());
        fileText.replace("\r", "\n").replace("\n\n", "\n"); //windows support ...
    }
    return fileText;
}

QString script_header_parser::getSingleMatch(const QString &exp, const QString& where)
{
    QString res;
    auto rx = QRegExp( exp );
    rx.setMinimal(true);
    if (rx.indexIn(where,0))
        res = rx.cap(1);

    return res;
}

QStringList script_header_parser::getAllMatches(const QString &exp, const QString& where)
{
    QStringList res;

    auto rx = QRegExp( exp );
    rx.setMinimal(true);
    for (int pos = 0; (pos = rx.indexIn(where, pos)) != -1; pos += rx.matchedLength())
        res.push_back(rx.cap(1));

    return res;
}

QIcon script_header_parser::getXpm(QString tag)
{
    tag.replace( QRegExp("\"[^\"&.]+\""), "\n" );
    QByteArray ascii = tag.toUtf8();
    const char *xpmbytes=ascii.data();
    const auto len = tag.length();

    char * xpmfinal = new char[len];
    int lines = 1;
    for (int i = 0; i < len; ++i)
    {
        if (xpmbytes[i]=='\n')
        {
            xpmfinal[i]=0;
            lines++;
        }
        else xpmfinal[i]=xpmbytes[i];
    }

    const char ** xpmptrs = new const char*[lines + 1];
    xpmptrs[0] = xpmfinal;
    int j = 1;
    for (int i = 0; i < len; ++i)
    {
        if (xpmfinal[i]=='\0')
        {
            xpmptrs[j]=xpmfinal + i + 1;
            j++;
        }
    }

    QPixmap pix(xpmptrs);
    QIcon ico(pix);
    delete[] xpmfinal;
    delete[] xpmptrs;

    return ico;
}

script_header_parser::FuncProto::FuncProto(const QString &innerText)
{
    parseProto(innerText);
}

bool script_header_parser::FuncProto::isEmpty() const
{
    return funcFullProto.isEmpty();
}

void script_header_parser::FuncProto::parseProto(QString innerText)
{
    innerText.replace("  ", " ");
    QStringList split = innerText.split(" ");
    switch (split.size())
    {
        case 1:
            funcFullProto  = split.at(0);
            break;
        case 2:
            resultType     = split.at(0);
            funcFullProto  = split.at(1);
            break;
        default:
            break;
    }
    if (!isEmpty())
        funcNameOnly = funcFullProto.split("(").at(0).trimmed();
}

script_header_parser::ScriptAction::ScriptAction(const QString &innerText)
{
    parseProto(innerText);
}

bool script_header_parser::ScriptAction::isEmpty() const
{
    return slot.isEmpty();
}

void script_header_parser::ScriptAction::parseProto(const QString &innerText)
{
    text = script_header_parser::getSingleMatch("<TEXT>(.+)</TEXT>", innerText);
    auto xpm = getSingleMatch("<XPM>.+\"(.+)\".+</XPM>", innerText);
    if (!xpm.isEmpty())
        icon = script_header_parser::getXpm(xpm);
    slot.parseProto(script_header_parser::getSingleMatch("<SLOT>(.+)</SLOT>", innerText));
}
