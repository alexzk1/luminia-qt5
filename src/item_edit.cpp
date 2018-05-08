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
#include "sourceedit.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QCoreApplication>
#include <QDebug>
#include "loaderpaths.h"
#include "mainwindow.h"

Item_edit::Item_edit( Item *parent, const QString& name) :
    Item( parent, name ),
    commonActions()
{
    //fileNormal = new QPixmap( pix_file );

    edit =  new SourceEdit(nullptr);
    appendToWs(edit);
    setIcon(0, QIcon(":/images/xpm/edit.xpm"));
    setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable | Qt::ItemIsEditable| Qt::ItemIsDragEnabled);

    const auto addAction = [this](auto a, auto b, auto c)
    {
        QAction *act = new QAction(a, b, this);
        connect(act, &QAction::triggered, this, c);
        commonActions.push_back(act);
    };
    addAction ( QIcon(":/images/xpm/load.xpm"),  tr("Load File"),    std::bind(&Item_edit::load,   this, QString()));
    addAction ( QIcon(":/images/xpm/save.xpm"),  tr("Save as File"), std::bind(&Item_edit::saveas, this, QString()));
    addAction ( QIcon(":/images/xpm/reload.xpm"),tr("Reload File"),  &Item_edit::reload);

    for (const auto& a : commonActions)
        edit->appendActionToBar(a);

}

QString Item_edit::getType() const
{
    return QString("Text");
}

Item_edit::~Item_edit()
{
    if (edit)
        delete edit;
    else qDebug() << "~Item_edit: edit already deleted";
}

void Item_edit::addMenu(QMenu *menu)
{
    for (const auto& a : commonActions)
    {
        if (a)
            menu->addAction(a);
    }
}

/*!
function for saving the editors content into a file. A filename can be used as first argument
*/
void Item_edit::saveas(const QString& filename)
{
    fn = filename;
    if (filename=="")
    {
        fn = QFileDialog::getSaveFileName(ws, tr("Save File"), "",tr("Text (*.*)"));
    }
    if (fn != "")
    {
        QFile file(fn);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))return;

        QTextStream out(&file);
        out << edit->getText();
    }
}

/*!
function for loading a file. A filename can be used as first argument:
*/
void Item_edit::load(const QString& filename)
{
    fn = filename;
    if (filename == "")
    {
        fn = QFileDialog::getOpenFileName(ws, tr("Open File"), "",tr("Text (*.*)"));
    }
    else
    {
        fn = LoaderPaths::findObject(filename);
    }
    reload();
}

/*!
function for reloading a file
*/
void Item_edit::reload()
{
    if ( !fn.isEmpty())
    {
        QFile file( fn );
        if ( !file.open(QIODevice::ReadOnly | QIODevice::Text ) )
            return;
        QTextStream ts( &file );
        edit->setText( ts.readAll() );
        edit->show();
    }
}

/*!
returns the editors text as string without processing #include
*/
QString Item_edit::raw_text() const
{
    return edit->getText();
}

/*!
This function returns the text of the editor with proceessed "#include"

*/
QString Item_edit::text()const
{
    static int rec_protection = 0;
    rec_protection ++;
    QRegExp rx( "(#include .+\n)" );
    rx.setMinimal(true);
    QString txt = edit->getText();

    int pos = 0;

    if(rec_protection < 16)while ((pos = rx.indexIn(txt, pos)) != -1)
    {
        QString path = txt.mid(pos, rx.matchedLength()).split(QRegExp("\\s+")).at(1);
        QStringList p = path.split(".");
        QObject *obj;

        if (p.at(0) == "World")
        {
            obj = world;
        }
        else
        {
            obj = QObject::parent();
        }

        for ( int i = 1; i < p.size(); i++)
        {
            if (!obj) break;
            obj = obj->findChild<QObject *>(p.at(i));
        }

        Item_edit* inc = dynamic_cast<Item_edit*>(obj);
        if(inc)
        {
            QString inctxt = inc->text();
            txt.replace(pos, rx.matchedLength()-1, inctxt); //don't replace the \n
            pos += inctxt.length() + 1;
        }
        else
        {
            pos += rx.matchedLength();
        }
    }

    rec_protection --;
    return txt;
}



/*!

*/
void Item_edit::setText(const QString& text)
{
    edit->setText(text);
}
