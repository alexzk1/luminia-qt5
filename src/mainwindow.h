
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class TreeView;
class Console;
class ScriptLauncher;
class TimeWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow() override;
public slots:
    void spaceballEvent(int *val);
private slots:

    void clear();
    void append();
    void open(const QString & appendfilename = "");
    void saveAs();
    void save();
    void about();
    void hide_all_editors();
    void run_all_scripts();
    void stop_all_scripts();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void createScriptToolBar( QList<ScriptLauncher*>);
    QString fileName;

    TreeView *treeview;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *timeToolBar;
    QToolBar *scriptToolBar;

    QAction *clearAct;
    QAction *appendAct;

    QAction *saveAct;
    QAction *saveAsAct;
    QAction *openAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *quitAct;

    QAction *hide_all_editors_Act;
    QAction *run_all_scripts_Act;
    QAction *stop_all_scripts_Act;


    QAction *profilerToggleAct;
    QAction *consoleToggleAct;

    TimeWidget *time;
    Console *console;
};

#endif
