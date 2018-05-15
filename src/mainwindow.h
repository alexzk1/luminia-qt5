
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
#include "saveable_widget.h"

class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class TreeView;
class Console;
class TimeWidget;
class SEngine;

class MainWindow : public QMainWindow, public utility::SaveableWidget<MainWindow>
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow() override;

    void setupErrorHandler(SEngine* engine);
    static QPointer<MainWindow> instance;

public slots:
    void hasErrorText(const QString &error);
    void clearWorld();
private slots:

    void spawnNewApplication(const QString &file2open = "");
    void open(const QString &file2open = "");
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
    void createScriptToolBar();
    QString fileName;
    QString lastPath;

    TreeView *treeview;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *timeToolBar;
    QToolBar *scriptToolBar;

    QAction *newProjectAct;

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

    QPointer<QAction> switchDockTree;
protected:
    void recurseWrite(QSettings& settings, QObject* object) override;
    void recurseRead(QSettings& settings, QObject* object) override;
    void updateTitle();
private:
    const static QString fileNameFilter;
};

#endif
