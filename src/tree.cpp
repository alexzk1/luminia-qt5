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
#include "tree.h"
#include <iostream>
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QStatusBar>

TreeView::TreeView ( QWidget * parent) : QTreeWidget (parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    //setDragDropMode(QAbstractItemView::DragDrop);
    setColumnCount(1);

    world = new Item_world();
    insertTopLevelItem(0, world);
    world->setExpanded(true);


    header()->hide();

    setMouseTracking(true);
}

void TreeView::contextMenuEvent ( QContextMenuEvent * event )
{
    // indexAt (event->pos ());
    Item *item = static_cast<Item*>(itemFromIndex(indexAt (event->pos ())));
    if (item)
    {
        qDebug("Context event (%s)",qPrintable(item->objectName()));
        item->contextmenu(event->globalPos());
    }
}

void TreeView::mousePressEvent ( QMouseEvent * event )
{

    // hack ... in future drag source should  fetched from QMimeData
    //hint: writting to u from the future, 7 years later - u didnt make it!

    dragSource = static_cast<Item*>(itemFromIndex(indexAt (event->pos ())));
    QTreeView::mousePressEvent(event);
}

void TreeView::mouseMoveEvent ( QMouseEvent * event )
{
    static Item* i;
    // hack ... in future drag source should  fetched from QMimeData
    Item* item = static_cast<Item*>(itemFromIndex(indexAt (event->pos ())));
    if (item != i && item != nullptr){
        i = item;
        static_cast<QMainWindow*>(parent())->statusBar()->showMessage(item->statusText(), 10000);
        //qDebug() << item->statusText();
    }


    QTreeView::mouseMoveEvent(event);
}
/*
bool TreeView::dropMimeData ( QTreeWidgetItem * parent, int index, const QMimeData * data, Qt::DropAction action ){
    qDebug() << "dropMimeData" <<  parent << index << data <<  action;
    }
*/
void TreeView::dragEnterEvent ( QDragEnterEvent * event )
{
    QTreeWidget::dragEnterEvent ( event );
}

void TreeView::dropEvent ( QDropEvent * event ){
    Item *item = static_cast<Item*>(itemFromIndex(indexAt (event->pos ())));
    if (item){
        switch(dropIndicatorPosition()){
            case AboveItem:
            case BelowItem:
                item = item->parent();
                if(!item){
                    event->ignore();
                    return;
                }
                break;
            case OnItem:
            case OnViewport:
                break;
        }
        dragSource->setParent(item);
    }
    QTreeWidget::dropEvent ( event );
}

void TreeView::dragMoveEvent ( QDragMoveEvent * event ){
    QTreeWidget::dragMoveEvent ( event );
    Item *item = static_cast<Item*>(itemFromIndex(indexAt (event->pos ())));
    if (item){
        switch(dropIndicatorPosition()){
            case AboveItem:
            case BelowItem:
                item = item->parent();
                if(!item){
                    event->ignore();
                    return;
                }
                break;
            case OnItem:
            case OnViewport:
                break;
        }
        if(!item->dragAccept(dragSource))event->ignore();
        Item* it = item;
        while (it) {
            if (it == dragSource){
                event->ignore();
            }
            it = it->parent();
        }
    }
    else event->ignore();
}
