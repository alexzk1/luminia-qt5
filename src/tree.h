/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef TREE_H
#define TREE_H


#include <QTreeWidget>
#include "item.h"

class Item;
class Item_world;

class TreeView : public QTreeWidget
{
    Q_OBJECT
public:
    TreeView ( QWidget * parent = nullptr );
    virtual ~TreeView ()override = default;

    Item_world* world;

protected:
    virtual void contextMenuEvent ( QContextMenuEvent * event )override;
    virtual void dragEnterEvent ( QDragEnterEvent * event )override;
    virtual void dropEvent ( QDropEvent * event )override;
    virtual void dragMoveEvent ( QDragMoveEvent * event )override;
    virtual void mousePressEvent(QMouseEvent*)override;
    virtual void mouseMoveEvent ( QMouseEvent *)override;
private:
    Item* dragSource;

};


#endif
