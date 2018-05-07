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
#include "lumfile.h"
#include "mainwindow.h"
#include "tree.h"
#include "item.h"
#include "timewidget.h"
#include "console.h"
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QStatusBar>
#include <QMenuBar>
#include <QApplication>
#include <QVBoxLayout>

MainWindow::MainWindow()
{
    setDockNestingEnabled(true);
    Item::ws = this;
    treeview = new TreeView (this);
    setCentralWidget(treeview);

    treeview->world->profiler = new Profiler();
    console = new Console (treeview->world);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createScriptToolBar(treeview->world->launcher);

    timeToolBar = addToolBar(tr("Time"));
    time = new TimeWidget(this);
    timeToolBar->addWidget(time);
    connect(time, SIGNAL(timeChanged(double)), treeview->world, SLOT(setTime(double)));

    readSettings(this);
}

MainWindow::~MainWindow()
{
    writeSettings(this);
    delete time;
    delete timeToolBar;
    delete console;
    delete treeview;
}

void MainWindow::createScriptToolBar( QList<ScriptLauncher*>  launcher)
{
    scriptToolBar = addToolBar(tr("ScriptTools"));
    for (int i = 0; i < launcher.size(); ++i)
    {
        if (launcher.at(i)->filter->exactMatch("ScriptToolBar"))
        {
            scriptToolBar->addAction (launcher.at(i)->a);
        }
    }
}

void MainWindow::recurseWrite(QSettings &settings, QObject*)
{
    settings.setValue("LastPath", lastPath);
}

void MainWindow::recurseRead(QSettings &settings, QObject*)
{
    lastPath = settings.value("LastPath", QDir::homePath()).toString();
}

void MainWindow::showFileName(const QString &name)
{
    if (name.isEmpty())
        setWindowTitle(tr("Lumina Modern"));
    else
        setWindowTitle(QString(tr("Lumina Modern [%1]")).arg(name));
}


void MainWindow::open(const QString & fn)
{
    if(fn == ""){
        clear();
        fileName = QFileDialog::getOpenFileName(this, tr("Lumina Open File"), lastPath, tr("Lumina Project Files (*.lum *.xml)"));
    }
    else{ // append mode....
        fileName = fn;
    }

    if (fileName.isEmpty())
        return;

    lastPath = QFileInfo(fileName).absolutePath();


    //treeWidget->clear();
    LumHandler handler(treeview->world, lastPath);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    //	reader.setErrorHandler(&handler);

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Lumina Project Files"),tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }

    QXmlInputSource xmlInputSource(&file);
    if (reader.parse(xmlInputSource))
        statusBar()->showMessage(tr("File loaded"), 2000);
    showFileName(fileName);
}

void MainWindow::append(){
    QString fnx = fileName; // save the file name


    fileName = QFileDialog::getOpenFileName(this, tr("Lumina Append File"), lastPath, tr("Lumina Project Files (*.lum *.xml)"));
    if(fileName != ""){
        open(fileName);
    }

    if(fnx != ""){
        fileName = fnx; //restore filename
    }
    showFileName(fileName);
}


void MainWindow::saveAs(){
    QString fn = QFileDialog::getSaveFileName(this, tr("Save Lumina File"), lastPath, tr("Lumina project Files (*.lum *.xml)"));
    if (!fn.isEmpty()){
        fileName = fn;
        save();
        showFileName(fileName);
    }
}

void MainWindow::save(){
    if (fileName.isEmpty()){
        fileName = QFileDialog::getSaveFileName(this, tr("Save Lumina File"), lastPath, tr("Lumina project Files (*.lum *.xml)"));
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Lumina Project Files"), tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }
    LumGenerator generator((Item*)treeview->world);
    if (generator.write(&file)) statusBar()->showMessage(tr("File saved"), 2000);
    lastPath = QFileInfo(fileName).absolutePath();
}

void MainWindow::clear(){
    int res = QMessageBox::question ( nullptr, "Clear World", "Clear the world may cause data loss... ", QMessageBox::Ok ,QMessageBox::Cancel );

    if(res == QMessageBox::Ok)
    {
        fileName = "";
        showFileName("");
        treeview->world->destroyAll();
    }
}

void MainWindow::about(){
    QMessageBox::about(this, tr("About Lumina Modern"),tr("Lumina is a flexible plattform independent development envrionment for GLSL shaders. "
                                                          "It uses ECMA-script for tools and emulating opengl engines. This code is distibuted under GNU/GPL license and are made by oc2k1."
                                                          "<br> Contact: <a href='mailto:oc2k1@users.sourceforge.net'>oc2k1@users.sourceforge.net</a><br>"
                                                          "<br>Year 2018 fixes by <a href='https://github.com/alexzk1'>alexzk</a>: <a href = 'mailto:alexzkhr@gmail.com'>email</a><br><br><br>"
                                                          "Old Expired Homepage with Tutorials:<br><a href='http://lumina.sourceforge.net'>http://lumina.sourceforge.net</a>"));
}



void MainWindow::hide_all_editors(){
    QTreeWidgetItemIterator it(treeview->world);
    while (*it) {
        if (Item_edit* edititem = dynamic_cast<Item_edit*>(*it)){
            edititem->dock->hide();
        }
        ++it;
    }
}

void MainWindow::run_all_scripts(){
    QTreeWidgetItemIterator it(treeview->world);
    while (*it) {
        if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it)){
            scriptitem->run();
        }
        ++it;
    }
}

void MainWindow::stop_all_scripts(){
    QTreeWidgetItemIterator it(treeview->world);
    while (*it) {
        if (Item_script* scriptitem = dynamic_cast<Item_script*>(*it)){
            scriptitem->stop();
        }
        ++it;
    }
}

void MainWindow::createActions()
{
    clearAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    clearAct->setShortcut(tr("Ctrl+N"));
    clearAct->setStatusTip(tr("Start a new project"));
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));


    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the current project"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/save.png"), tr("Save As"), this);
    saveAsAct->setStatusTip(tr("Save the current project as file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open a Lumina project"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    appendAct = new QAction(QIcon(":/images/open.png"), tr("Append"), this);
    appendAct->setShortcut(tr("Ctrl+A"));
    appendAct->setStatusTip(tr("Append a Lumina project"));
    connect(appendAct, SIGNAL(triggered()), this, SLOT(append()));



    profilerToggleAct = new QAction(QIcon(":/images/profiler.png"), tr("&Profiler"), this);
    profilerToggleAct->setShortcut(tr("Ctrl+P"));
    profilerToggleAct->setStatusTip(tr("Toggle profiling"));
    profilerToggleAct->setCheckable (true);
    connect(profilerToggleAct, SIGNAL(toggled(bool)),treeview->world->profiler , SLOT(toggle(bool)));

    consoleToggleAct = new QAction(QIcon(":/images/console.png"),  tr("&Console"), this);
    consoleToggleAct->setShortcut(tr("Ctrl+C"));
    consoleToggleAct->setStatusTip(tr("Toggle console"));
    consoleToggleAct->setCheckable (true);
    connect(consoleToggleAct, SIGNAL(toggled(bool)),console , SLOT(toggle(bool)));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    hide_all_editors_Act = new QAction(tr("Hide all Editors"), this);
    connect(hide_all_editors_Act, SIGNAL(triggered()), this, SLOT(hide_all_editors()));

    run_all_scripts_Act = new QAction(tr("Run all Scripts"), this);
    connect(run_all_scripts_Act, SIGNAL(triggered()), this, SLOT(run_all_scripts()));

    stop_all_scripts_Act = new QAction(tr("Stop all Spripts"), this);
    connect(stop_all_scripts_Act, SIGNAL(triggered()), this, SLOT(stop_all_scripts()));

}

void MainWindow::createMenus(){
    fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(openAct);
    fileMenu->addAction(appendAct);

    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();
    fileMenu->addAction(clearAct);
    fileMenu->addSeparator();

    fileMenu->addAction(quitAct);

    menuBar()->addSeparator();

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(hide_all_editors_Act);
    editMenu->addAction(run_all_scripts_Act);
    editMenu->addAction(stop_all_scripts_Act);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

}

void MainWindow::createToolBars(){
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(appendAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(profilerToggleAct);
    fileToolBar->addAction(consoleToggleAct);

}

void MainWindow::createStatusBar(){
    statusBar()->showMessage(tr("Ready"));
}


#include <math.h>
void MainWindow::spaceballEvent(int *val){

    Item_matrix *c = dynamic_cast<Item_matrix*>(treeview->selectedItems().at(0));


    if(!c) return;
    //qDebug() << c << val[0] << val[1] << val[2] << val[3] << val[4] << val[5];

    c->Translate(val[0]/100.0, -val[2]/100.0 ,val[1]/100.0);

    float l = 0.01* sqrt(val[3]*val[3] + val[4]*val[4] + val[5] * val[5]);
    c->Rotate(l,val[3]/100.0, val[5]/100.0 ,val[4]/100.0);


    if (val[6]!=0)c->LoadIdenty();
    // handle buttons toggle 6dof / translate/ rotate  mode

    if(!time->isPlaying()){
        treeview->world->setTime(treeview->world->getTime()); //trigger screen refresh
    }
}






