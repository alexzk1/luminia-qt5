/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007  oc2k1
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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************/
#include "item.h"
#include "glwrapper.h"
#include "sourceedit.h"

#include <QMessageBox>
#include <qscriptengine.h>

#include "factory/factory.h"
#include "prohibited_filter.h"
#include "editors/Cebitor.h"
#include "editors/searchbox.h"
#include "editors/jslexer.h"

Item_script::Item_script( Item *parent, const QString& name) :
    Item_edit(1, parent, name),
    running(false),
    engine(nullptr)
{
    initParent(); //this allows to properly call createTextEditor
    switchIcon(false);

    const auto addAction = [this](auto a, auto b, auto c)
    {
        auto act = new QAction(a, b, this);
        connect(act, &QAction::triggered, this, c);
        commonActions.push_front(act);
    };
    addAction ( QIcon(":/images/xpm/stop.xpm"), tr( "Disable Script"), &Item_script::stop);
    addAction ( QIcon(":/images/xpm/run.xpm"), tr( "Enable Script"),  &Item_script::run);

    appendActionToBar(commonActions.at(0), commonActions.at(2));
    appendActionToBar(commonActions.at(1), commonActions.at(2));
    appendActionToBar(nullptr, commonActions.at(2));
}

void Item_script::deleteEngine()
{
    if (engine)
        engine->deleteLater();
    engine = nullptr;
}

QWidget *Item_script::createTextEditor(QWidget *parent) const
{
    auto l = new Cebitor(searchBox, parent);
    l->makeLexer<JSLexer>();
    return l;
}

Item_script::~Item_script()
{
    deleteEngine();
}

QStringList Item_script::getApis(QObject * object) const
{
    QStringList result;
    //listing meta-methods "add*", despite what documentation says, it does not list parent's methods. So have to do long way...
    std::set<QString> once; //ensuring that if they fix qt, program will be working yet
    const auto processObject = [&object, &result, &once](const QMetaObject * metaObject)
    {
        for (int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
        {
            const auto method = metaObject->method(i);
            auto n = QString::fromLatin1(method.methodSignature());

            if (!once.count(n) && !n.contains("_q_") && !n.contains("deleteLater") && !n.contains("destroyed") && !n.contains("objectNameChanged"))
            {
                once.insert(n);
                auto ip = qobject_cast<Item*>(object);
                result.push_back(QString("%1.%2").arg((ip) ? ip->getName() : object->objectName()).arg(n));
            }
        };
    };

    auto meta = object->metaObject();
    while (meta)
    {
        processObject(meta);
        meta = meta->superClass();
    }
    return result;
}

/*!
Start a script. QT-4.3 part is not full compatible to QSA
*/
void Item_script::run()
{
    stop();
    deleteEngine();
    engine = new SEngine(parent());
    connect(engine, &SEngine::scriptError, this, [this](const QString&)
    {
        stop();
        //damn, do not delete engine on error
        //deleteEngine();
    });

    auto ed   = getEditor<Cebitor>();
    auto apis = new QsciAPIs(ed->lexer());

    auto insert_apis = [&apis, this](QObject * obj)
    {
        if (apis)
        {
            auto funcs = getApis(obj);
            for (const auto& a : funcs)
                apis->add(a);
        }
    };

    insert_apis(world);
    insert_apis(engine->getGl());
    //doing kinda local bind, so no need full path from root
    QList<Item*> directs = parent()->findChildren<Item*>(QString(), Qt::FindDirectChildrenOnly);
    for (const auto& o : directs)
    {
        engine->bindItem(o, true);
        insert_apis(o);
    }
    ed->lexer()->setAPIs(apis);
    apis->prepare();

    switchIcon(true);
    engine->run(raw_text());

    world->setTime(world->getTime()); //ugly hack for updating the QOpengl widget
}

void Item_script::Call(const QString& function, const QVariantList& args)
{
    if (isRunning())
    {
        //qDebug()<<"Item script, exec for: " << function;
        engine->execJsFunc(function, args);
    }
}

QString Item_script::getType() const
{
    return "Script";
}

/*!
stopt the excution. "render()" will not be called anymore
*/
void Item_script::stop()
{
    switchIcon(false);
}
/*!
returns true if the script is running. A "render()" function is needed or the script will
stop at the end.
*/
bool Item_script::isRunning() const
{
    return running && engine;
}
/*!
complettation handler, that searchs the object by the last line and generates complettations
(childobjects, functions and enums) from the objects metadata.
*/
//void Item_script::completationHandler(const QString& line)
//{
//    glwrapper ogl(nullptr);
//    meta = nullptr;
//    QStringList complettations;
//    hostWidget->setCompleatationList(complettations, 0); //default no completation box

//    QString last = line.split(QRegExp("[^\\w\\.]")).last();

//    if (last.length() < 2)return;

//    if ( last.right(1) == ".")
//    {
//        QObject *obj = nullptr;


//        QStringList parts = last.split(".");
//        if (parts[0] == "World")
//            obj = Item::world;
//        else
//            if (parts[0] == "gl")
//                obj = &ogl;
//            else
//            {
//                //Factory prototypes
//                QRegExp searchProto(parts[0] + R"(\s*=\s*new\s*(\w+)[\(|;])");
//                searchProto.indexIn(text());
//                meta = Factory::metaObjectFrom(searchProto.cap(1));
//                if (meta != nullptr)
//                    goto META_OBJ_KNOWN;

//                //Object from treeview
//                QObjectList QObjectList = parent()->findChildren<QObject *>();
//                for ( int i = 0; i < QObjectList.count(); i++)
//                {
//                    if (QObjectList[i]->parent() == parent())
//                    {
//                        if (parts[0] == QObjectList[i]->objectName())obj = QObjectList[i];
//                    }
//                }
//            }
//        if (obj == nullptr)
//            return;
//        else
//        {
//            bool found = true;
//            //find the correct child object
//            for (int depth = 1; depth < parts.count() - 1; depth++)
//            {
//                found = false;
//                QObjectList QObjectList = obj->findChildren<QObject*>(parts.last());
//                for ( int i = 0; i < QObjectList.count(); i++)
//                {
//                    if (QObjectList[i]->parent() == obj && QObjectList[i]->objectName() == parts[depth])
//                    {
//                        obj = QObjectList[i];
//                        found = true;
//                    }
//                }
//            }
//            qDebug() << obj << found;
//            if (!found)return;

//            //append childobjects
//            QObjectList QObjectList = obj->findChildren<QObject*>();
//            for ( int i = 0; i < QObjectList.count(); i++)
//            {
//                if (QObjectList[i]->parent() == obj)
//                    complettations << QObjectList[i]->objectName();
//            }
//        }
//        meta = obj->metaObject();

//META_OBJ_KNOWN:
//        QStringList Slots;
//        for ( int i = 0; i < meta->methodCount(); ++i)
//        {
//            QString slot = QString(meta->method(i).methodSignature()).replace(QRegExp("\\(.*"), "");
//            if (slot.isEmpty())continue;
//            if (slot == "deleteLater")continue;
//            if (slot.startsWith("_"))continue;
//            Slots << slot;
//        }
//        Slots.sort();
//        Slots.removeDuplicates();
//        complettations << Slots;


//        QStringList Enums;
//        for ( int i = 0; i < meta->enumeratorCount(); ++i)
//        {
//            for (int k = 0; k < meta->enumerator(i).keyCount(); --k)
//                Enums << QString(meta->enumerator(i).key(k));
//        }
//        Enums.sort();
//        Enums.removeDuplicates();
//        complettations << Enums;

//        hostWidget->setCompleatationList(complettations, 0);
//        return;
//    }

//    complettations << "World" << "Math";

//    QObjectList QObjectList = parent()->findChildren<QObject *>();
//    for (int i = 0; i < QObjectList.count(); ++i)
//    {
//        if (QObjectList[i]->parent() == parent())
//            complettations << QObjectList[i]->objectName();
//    }
//    filterProhibitedCompletion(complettations);
//    hostWidget->setCompleatationList(complettations.filter(QRegExp(QString("^") + last)), last.length());
//}

///*!
//handler that reads the doxygen XML documentation an pass the help to the editors complettation box
//*/
//void Item_script::helpHandler(const QString& string)
//{
//    if (meta == nullptr)
//    {
//        hostWidget->setHelpString("");
//        return;
//    }

//    static QString cacheFilename("");
//    static QString lines("");
//    //qDebug() << string << meta->className();
//    QString Filename = QString("doc/doxygen/xml/class") + meta->className() + ".xml";
//    Filename.replace("_", "__");

//    if (Filename != cacheFilename)
//    {
//        //qDebug() << Filename;
//        QFile file(Filename);
//        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//            return;

//        QTextStream in(&file);

//        lines = in.readAll();
//        cacheFilename = Filename;
//    }

//    QRegExp parser1(QString("<memberdef.*<name>") + string + "</name>.*</memberdef>");
//    parser1.setMinimal(true);
//    parser1.indexIn(lines);

//    QString block = parser1.cap(0).right(  parser1.cap(0).size() - parser1.cap(0).lastIndexOf("<memberdef") );
//    //qDebug() << block;

//    QRegExp parser2("<para>(.*)</para>");
//    parser2.indexIn(block);

//    QRegExp remove("<.*>");
//    remove.setMinimal(true);
//    hostWidget->setHelpString(parser2.cap(1).replace(remove, ""));
//}

void Item_script::switchIcon(bool isRunning)
{
    const static QIcon norm(":/images/xpm/script.xpm");
    const static QIcon run (":/images/xpm/script_run.xpm");
    setIcon(0, (isRunning) ? run : norm);
    running = isRunning;
}
