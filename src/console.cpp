/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007-2008  oc2k1
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
*USA.
*********************************************************************************/

#include "console.h"
#include "factory/factory.h"
#include "glwrapper.h"
#include <QApplication>
#include <QMainWindow>
#include <QtScript>

#include "prohibited_filter.h"
#include "dock_prep.h"

//***********************ConsoleCompletionBox*****************************

ConsoleCompletionBox::ConsoleCompletionBox(ConsoleLine *_line, const QStringList &_completions, const QString &_searchString):
    AbstractCompletionBox(_line, _completions, _searchString)
{
    line = _line;
    line->completationOpen = true;
}

ConsoleCompletionBox::~ConsoleCompletionBox()
{
    line->completationOpen = false;
}

void ConsoleCompletionBox::finishCompletion()
{
    QListWidgetItem *item = listwidget->currentItem();
    if (!item)
        return;
    QString s = item->data(Qt::UserRole).toString().mid(searchString.length());
    line->insert(s.replace(QRegExp("\\(.+\\)"), "("));
}

//***********************ConsoleLine*********************************
/*!
Keypress Eventhandler for consoles input. Implements history and completionbox
*/

void ConsoleLine ::keyPressEvent(QKeyEvent *e)
{

    int size = history.size();
    if (!completationOpen)
    {
        switch (e->key())
        {
            case Qt::Key_Down:
                if (count == 1)
                    this->setText(temp);
                else
                    if (count > 1)

                        setText(history.at(size - count));
                count = count != 0 ? count - 1 : 0;
                break;
            case Qt::Key_Up:
                count = count < size ? count + 1 : size;
                if (count == 1)
                    temp = text();
                if (count != 0)
                    setText(history.at(size - count));
                break;
            case Qt::Key_Return:
                count = 0;
                history.append(text());
                break;
        }
    }
    QLineEdit::keyPressEvent(e);

    if (completationOpen)
        return; // don't open a second completationbox

    QStringList comp = QStringList() << "World"
                       << "Math";

    QString last = text().split(QRegExp("[^\\w\\.]")).last();

    if (last.length() < 2)
        return;
    // qDebug() << last;

    auto& eng = console->getEngine()->getEngine();

    QStringList objnames = last.split(".");
    // qDebug() << objnames;

    QScriptValue obj = eng.globalObject();

    for (int i = 0; i < objnames.size() - 1; i++)
    {
        qDebug() << obj.isValid() << objnames.at(i);
        obj = obj.property(objnames.at(i));
    }

    if (last.endsWith("."))
    {
        comp.clear();
        // append child objects
        if (obj.isQObject())
        {
            QObject *qobj = obj.toQObject();
            if (qobj == nullptr)
                return; // Fix for deleted objects
            for (const auto i : qobj->children())
                comp << i->objectName();
            meta = qobj->metaObject();
        }
        // append propertys
        QScriptValueIterator it(obj);
        while (it.hasNext())
        {
            it.next();
            comp.append(it.name()); //.replace(QRegExp("\\(.*\\)"),"")) ;
        }
    }

    QString s = last.split(".").last();

    filterProhibitedCompletion(comp);

    QWidget *box = new ConsoleCompletionBox(this, comp, s);
    box->move(mapToGlobal(rect().bottomLeft()));
    box->show();
    connect(this, SIGNAL(setHelpString(const QString &)), box,
            SLOT(setHelpString(const QString &)));
    connect(box, SIGNAL(requestHelpString(const QString &)), this,
            SLOT(helpHandler(const QString &)));
}

/*!
handler that reads the doxygen XML documentation an pass the help to the
consoles complettation box. Similar code like for the script editor,
*/
void ConsoleLine::helpHandler(const QString &_string)
{
    QString string = _string;
    string.replace(QRegExp("\\(.*\\)"), "");

    if (meta == nullptr)
    {
        emit setHelpString("");
        return;
    }

    static QString cacheFilename("");
    static QString lines("");
    // qDebug() << string << meta->className();
    QString Filename =
        QString("doc/doxygen/xml/class") + meta->className() + ".xml";
    Filename.replace("_", "__");

    if (Filename != cacheFilename)
    {
        // qDebug() << Filename;
        QFile file(Filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);

        lines = in.readAll();
        cacheFilename = Filename;
    }

    QRegExp parser1(QString("<memberdef.*<name>") + string +
                    "</name>.*</memberdef>");
    parser1.setMinimal(true);
    parser1.indexIn(lines);

    QString block = parser1.cap(0).right(
                        parser1.cap(0).size() - parser1.cap(0).lastIndexOf("<memberdef"));
    // qDebug() << block;

    QRegExp parser2("<para>(.*)</para>");
    parser2.indexIn(block);

    QRegExp remove("<.*>");
    remove.setMinimal(true);
    emit setHelpString(parser2.cap(1).replace(remove, ""));
}

//***********************Console*********************************
Console::Console(QObject *parent) :
    QObject(parent)
{
    active = false;

    out = new QTextEdit();
    out->setReadOnly(true);

    in = new ConsoleLine(this);

    widget = new QWidget();
    layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(out);
    layout->addWidget(in);

    widget->setLayout(layout);

    dock = nsDocks::createDockFromWidget(widget, tr("Console"));
    dock->hide();
    connect(in, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
}

Console::~Console()
{
    delete in;
    delete out;
    if (dock)
        dock->deleteLater();

    if (eng)
        delete eng;
}

QPointer<SEngine> Console::getEngine()
{
    if (!eng)
        eng = new SEngine(this);
    return eng;
}

void Console::toggle(bool b)
{
    active = b;
    if (active)
        dock->show();
    else
        dock->hide();
}

void Console::returnPressed()
{
    out->setTextColor(QColor(Qt::blue));
    out->append(in->text());

    QString line = in->text();
    if (line.isNull())
        return;

    if (line.startsWith("reset()"))
    {
        if (eng)
        {
            line.clear();
            delete eng;
        }
        if (line.isEmpty())
            return;
    }

    code += line;
    code += QLatin1Char('\n');

    if (line.trimmed().isEmpty())
        return;
    else
        if (!getEngine()->getEngine().canEvaluate(code))
            in->setText("   ");
        else
        {
            for (const auto& p : getProhibitedMethods())
                if (code.contains(p))
                {
                    out->setTextColor(QColor(Qt::red));
                    out->append("prohibited command");
                    code.clear();
                    in->clear();
                    return;
                }
            QScriptValue result = getEngine()->getEngine().evaluate(code, QLatin1String("typein"));
            code.clear();
            in->clear();
            if (!result.isUndefined())
            {
                out->setTextColor(QColor(Qt::darkGreen));
                if (result.toString().contains("Error:"))
                    out->setTextColor(QColor(Qt::red));
                out->append(result.toString());
            }
        }
    getEngine()->getEngine().collectGarbage();
}
