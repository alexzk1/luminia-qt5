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

#include "script_extender.h"
#include "script_extender_engine.h"
#include "loaderpaths.h"

QList<SAction> ScriptExtender::actionlist;
QList<SSlot> ScriptExtender::slotlist;

QList<QPointer<SEngine>> ScriptExtender::engineList;


void ScriptExtender::scanFile(const QString& filename)
{

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        qDebug() << "Error launching script";
        return;
    }
    QString in = file.readAll();

    QRegExp rx;


    rx = QRegExp( "<FILTER>(.+)</FILTER>" );
    rx.setMinimal(true);
    if (!rx.indexIn(in,0))return;
    QString filter = rx.cap(1);

    //for each found function
    rx = QRegExp( "<FUNCTION>(.+)</FUNCTION>" );
    rx.setMinimal(true);
    for (int pos = 0; (pos = rx.indexIn(in, pos)) != -1; pos += rx.matchedLength())
    {
        QStringList split = rx.cap(1).split(" ");

        switch (split.size())
        {
            case 1:
                slotlist << SSlot("", split.at(0), slotlist.size(), QRegExp(filter),filename );
                break;
            case 2:
                slotlist << SSlot(split.at(0), split.at(1), slotlist.size(), QRegExp(filter),filename );
                break;
            default:
                qDebug() << "<FUNCTION>....</FUNCTION> with illegal content";
        }

    }



    rx = QRegExp( "<ACTION>(.+)</ACTION>" );
    rx.setMinimal(true);
    for (int pos = 0; (pos = rx.indexIn(in, pos)) != -1; pos += rx.matchedLength()) {

        QRegExp tx;


        tx = QRegExp( "<TEXT>(.+)</TEXT>" );
        if (!tx.indexIn(rx.cap(1),0))return;
        QString text = tx.cap(1);

        tx = QRegExp( "<SLOT>(.+)</SLOT>" );
        if (!tx.indexIn(rx.cap(1),0))return;
        QString slot = tx.cap(1);

        const char** icon = xpm(rx.cap(1));
        actionlist << SAction(icon,text,QString("1%1").arg(slot),QRegExp(filter));

    }

    //qDebug() << " sssssss" <<  SLOT(test());



}

const char** ScriptExtender::xpm(const QString& in){

    QRegExp rx( "<XPM>.+\"(.+)\".+</XPM>" );
    rx.indexIn(in,0);
    QString captured = rx.cap( 1 );

    captured.replace( QRegExp("\"[^\"&.]+\""), "\n" );
    QByteArray ascii = captured.toUtf8();
    const char *xpmbytes=ascii.data();

    char * xpmfinal = new char[captured.length() + 1]; //memory will never be deleted
    int lines = 1;
    for (int i=0;i<captured.length();i++){
        if (xpmbytes[i]=='\n'){
            xpmfinal[i]=0;
            lines++;
        }
        else xpmfinal[i]=xpmbytes[i];
    }

    const char ** xpmptrs = new const char*[lines];  //memory will never be deleted
    xpmptrs[0] = xpmfinal;
    int j = 1;

    for (int i=0;i<captured.length();i++){
        if (xpmfinal[i]=='\0'){
            xpmptrs[j]=xpmfinal + i + 1;
            j++;
            //qDebug() << j << " "<< i;
        }
    }

    return xpmptrs;
}

void ScriptExtender::addActions(QMenu *menu, const QString &type)
{
    for (const auto& as : ScriptExtender::actionlist)
    {
        if(as.filter.exactMatch(type))
        {
             QAction *a = menu->addAction(QIcon(QPixmap(as.icon)), "*" + as.text);

             //, ScriptExtender::actionlist.at(i).text, this, ScriptExtender::actionlist.at(i).slot.toLatin1().constData());
        }
    }
}

void ScriptExtender::setup()
{
    QStringList paths = LoaderPaths::listFilesInSubfolder(LoaderPaths::SCRIPTS);
    for(const auto& p : paths)
        scanFile(p);
}


