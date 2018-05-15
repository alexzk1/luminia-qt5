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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************/

#include "item.h"
#include "item_virtual.h"
#include <chrono>
#include <stack>
#include "mainwindow.h"

Item_world::Item_world():
    Item(nullptr, "World")
{
    Item::world = this;
    setIcon(0, QIcon(":/images/xpm/world.xpm"));
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled);
    new Item_cam(this, "Cam");
}

QObject *Item_world::addCam(const QString &name)
{
    return makeNewItemNoThis<Item_cam>(this, name);
}

QObject *Item_world::addNode(const QString &name)
{
    return makeNewItem<Item_node>(name);
}

QObject *Item_world::addVirtual(const QString &name)
{
    return makeNewItem<Item_virtual>(name);
}

QString Item_world::timedString(const QString &preffix)
{
    using namespace std::chrono;
    //must be system_clock, steady_clock counting not from 1-Jan-1970
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    return QString("%1_%2").arg(preffix).arg(ms.count());
}

QString Item_world::getType() const
{
    return QString("World");
}

void Item_world::deleteLater()
{
    //do nothing
}

void Item_world::destroyAll()
{
    Item::destroyAll();
}

bool Item_world::isDeletable() const
{
    return false;
}

void Item_world::addMenu(QMenu *menu)
{
    auto a = menu->addAction ( QIcon(":/images/new.png"), tr("Clear World"));
    connect(a, &QAction::triggered, MainWindow::instance, &MainWindow::clearWorld);
}
/*!
accept only Node items for dragging
*/
bool Item_world::dragAccept(Item* i)
{
    return i->getType() == "Node";
}


/*!
returns the time from the timewidget in seconds
*/
double Item_world::getTime()
{
    return timev;
}
/*!
set the worlds time. Usefull for rendering videos
*/
void Item_world::setTime(double t)
{
    timev = t;
    emit update();
}


/*!
retuns the cama width in pixel. Usefull for generating rendertargets for deferred rendering.
*/
int Item_world::getCamWidth()
{
    return CamWidth;
}
/*!
retuns the cam heigh in pixel
*/
int Item_world::getCamHeight()
{
    return CamHeight;
}

void Item_world::setCamSize(int w, int h)
{
    CamWidth = w;
    CamHeight = h;
}

/*!
void Call(String functionname, Array arguments);\n
This function trys to call "functionname" in all Script items in the tree.
Can't be called recursive!
*/
// same code as Item_node::Call();
void Item_world::Call(const QString& function, const QVariantList& args)
{
    //    static int rec = 0;
    //    if(rec > 8)return;
    //    rec++;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {

        if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it))
        {
            glPushMatrix();

            //-simple code to apply the nodes matices, a stack based could be better
            //-hello from 2018, ok, stack as you wanted

            std::stack<float*> matrixlist;
            for (Item_matrix* m = dynamic_cast<Item_matrix*>(scriptitem->parent()); m ;
                    m = dynamic_cast<Item_matrix*>(m->parent()))
                matrixlist.push(m->getMatrix());

            for (; matrixlist.size(); matrixlist.pop())
                glMultMatrixf(matrixlist.top());

            scriptitem->Call(function, args);

            Item_buffer::UnbindAll(); // clear all client and attribute arrays

            glPopMatrix();
        }
        ++it;
    }
    //rec--;
}
