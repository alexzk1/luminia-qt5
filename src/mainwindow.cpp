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
#include "script_extender_engine.h"
#include "dock_prep.h"
#include "loaderpaths.h"

QPointer<MainWindow> MainWindow::instance;
const QString MainWindow::fileNameFilter = tr("Lumina project Files (*.lum)");


MainWindow::MainWindow()
{
    instance = this;
    Item::ws = this;
    treeview = new TreeView (this);
    QPointer<QDockWidget> dockTree = nsDocks::createDockFromWidget(treeview, tr("World Tree"), this);


    addDockWidget(Qt::LeftDockWidgetArea, dockTree);

    auto dumb = new QLabel(this);
    setCentralWidget(dumb);
    dumb->setVisible(false);

    switchDockTree = new QAction(QIcon(":/images/xpm/world.xpm"), tr("Show/Hide World Tree"), this);
    switchDockTree->setShortcut(QKeySequence("ctrl+W"));
    switchDockTree->setCheckable(true);
    switchDockTree->setChecked(true);

    connect(switchDockTree, &QAction::triggered, [dockTree](bool check)
    {
        if (dockTree)
        {
            if (check)
                dockTree->show();
            else
                dockTree->hide();
        }
    });

    treeview->world->profiler = new Profiler(this);
    console = new Console (this);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    //createScriptToolBar();

    timeToolBar = addToolBar(tr("Time"));
    time = new TimeWidget(this);
    timeToolBar->addWidget(time);
    connect(time, SIGNAL(timeChanged(double)), treeview->world, SLOT(setTime(double)));
    readSettings(this);
    updateTitle();

    if (QCoreApplication::arguments().count() > 1)
    {
        const auto a1 = QCoreApplication::arguments().at(1);
        //qDebug() << "File to open: " << a1;
        if (!a1.isEmpty()) //not really sure, direct call to open from constructor fails >:
            QTimer::singleShot(250, this, std::bind(&MainWindow::open, this, QCoreApplication::arguments().at(1)));
    }


}

MainWindow::~MainWindow()
{
    writeSettings(this);
    delete time;
    delete timeToolBar;
    delete console;
    delete treeview;
}

void MainWindow::setupErrorHandler(SEngine *engine)
{
    connect(engine, &SEngine::scriptError, this, [this](const QString & err, const QStringList & trace)
    {
        QString msg = QString("%1\n%2").arg(err).arg(trace.join("\n"));

        QTimer::singleShot(150, this, [this, msg]()
        {
            QMessageBox::critical(this, tr("Script Error"), msg);
        });
    });
}

void MainWindow::createScriptToolBar()
{
    scriptToolBar = addToolBar(tr("ScriptTools"));
    ScriptExtender::buildToolbar(scriptToolBar);
}

void MainWindow::recurseWrite(QSettings &settings, QObject*)
{
    settings.setValue("LastPath", lastPath);
}

void MainWindow::recurseRead(QSettings &settings, QObject*)
{
    lastPath = settings.value("LastPath", QDir::homePath()).toString();
}

void MainWindow::updateTitle()
{
    if (fileName.isEmpty())
        setWindowTitle("");
    else
    {
        setWindowTitle(QString(tr("[%1]")).arg(fileName)); //qt automatically appends program name at the end for such [%1] text...
        updatedLastFilesList();
    }
}

const static QString last_proj_key = "LastProjects";

void MainWindow::updatedLastFilesList()
{
    QSettings settings;
    settings.beginGroup(getSettingsGroupName(this));
    QStringList stored = settings.value(last_proj_key).toStringList();

    //making current file on top of list
    if (stored.contains(fileName))
        stored.removeOne(fileName);
    stored.insert(0, fileName);

    while (stored.size() > 15)
        stored.removeLast();
    settings.setValue(last_proj_key, stored);
    settings.endGroup();
    settings.sync();
}

void MainWindow::reloadLastFilesList()
{
    QSettings settings;
    settings.beginGroup(getSettingsGroupName(this));
    QStringList stored = settings.value(last_proj_key).toStringList();
    settings.endGroup();
    if (historyMenu)
    {
        historyMenu->clear();
        for (const auto& fn : stored)
        {
            auto act = historyMenu->addAction(fn);
            connect(act, &QAction::triggered, this, std::bind(&MainWindow::open, this, fn));
        }
    }
}

void MainWindow::open(const QString& file2open)
{
    QString selected = fileNameFilter;
    auto fileN = (file2open.isEmpty()) ? QFileDialog::getOpenFileName(this, tr("Lumina Open File"), lastPath, fileNameFilter, &selected) : file2open;
    if (!fileN.isEmpty())
    {
        if (fileN != fileName)
        {
            if (fileName.isEmpty())
            {
                lastPath = QFileInfo(fileN).absolutePath();

                LumHandler handler(treeview->world, lastPath);
                QXmlSimpleReader reader;
                reader.setContentHandler(&handler);

                QFile file(fileN);
                if (file.open(QFile::ReadOnly | QFile::Text))
                {
                    QXmlInputSource xmlInputSource(&file);
                    if (reader.parse(xmlInputSource))
                        statusBar()->showMessage(tr("File loaded."), 2000);
                    fileName = fileN;
                    updateTitle();
                }
                else
                    QMessageBox::warning(this, tr("Lumina Project Files"), tr("Cannot read file %1:\n%2.").arg(fileN).arg(file.errorString()));
            }
            else
                spawnNewApplication(fileN);
        }
    }
}

void MainWindow::saveAs()
{
    auto tmp = fileName;
    fileName = "";
    save();
    if (fileName.isEmpty())
        fileName = tmp;
}

void MainWindow::save()
{
    QString selected = fileNameFilter;
    QString name = fileName;
    if (name.isEmpty())
        name = QFileDialog::getSaveFileName(this, tr("Save Lumina File"), lastPath, fileNameFilter, &selected);

    if (!name.isEmpty())
    {
        if (!name.endsWith(".lum", Qt::CaseInsensitive))
            name += ".lum";

        QFile file(name);
        if (file.open(QFile::WriteOnly | QFile::Text))
        {
            LumGenerator generator(treeview->world);
            if (generator.write(&file))
                statusBar()->showMessage(tr("File saved."), 2000);
            fileName = name;
            lastPath = QFileInfo(fileName).absolutePath();
            updateTitle();
        }
        else
            QMessageBox::critical(this, tr("Lumina Project Files"), tr("Cannot write file %1:\n%2.").arg(name).arg(file.errorString()));
    }
}

void MainWindow::spawnNewApplication(const QString& file2open)
{
    LoaderPaths::spawnCopy(file2open);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Lumina Modern"), tr("Lumina is a flexible plattform independent development envrionment for GLSL shaders. "
                       "It uses ECMA-script for tools and emulating opengl engines. This code is distibuted under GNU/GPL license and are made by oc2k1."
                       "<br> Contact: <a href='mailto:oc2k1@users.sourceforge.net'>oc2k1@users.sourceforge.net</a><br>"
                       "<br>Year 2018 fixes by <a href='https://github.com/alexzk1'>alexzk</a>: <a href = 'mailto:alexzkhr@gmail.com'>email</a><br><br><br>"
                       "Old Expired Homepage with Tutorials:<br><a href='http://lumina.sourceforge.net'>http://lumina.sourceforge.net</a>"));
}

void MainWindow::hide_all_editors()
{
    QTreeWidgetItemIterator it(treeview->world);
    while (*it)
    {
        if (auto edititem = dynamic_cast<Item_edit*>(*it))
            edititem->hideDock();
        ++it;
    }
}

void MainWindow::run_all_scripts()
{
    QTreeWidgetItemIterator it(treeview->world);
    while (*it)
    {
        if (auto scriptitem = dynamic_cast<Item_script*>(*it))
            scriptitem->run();
        ++it;
    }
}

void MainWindow::stop_all_scripts()
{
    QTreeWidgetItemIterator it(treeview->world);
    while (*it)
    {
        if (auto scriptitem = dynamic_cast<Item_script*>(*it))
            scriptitem->stop();
        ++it;
    }
}

void MainWindow::createActions()
{
    newProjectAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newProjectAct->setShortcut(QKeySequence("Ctrl+N"));
    newProjectAct->setStatusTip(tr("Start a new project."));
    connect(newProjectAct, SIGNAL(triggered()), this, SLOT(spawnNewApplication()));


    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(QKeySequence("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the current project"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/save.png"), tr("Save As"), this);
    saveAsAct->setStatusTip(tr("Save the current project as file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    openAct->setShortcut(QKeySequence("Ctrl+O"));
    openAct->setStatusTip(tr("Open a Lumina project"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    profilerToggleAct = new QAction(QIcon(":/images/profiler.png"), tr("&Profiler"), this);
    profilerToggleAct->setShortcut(QKeySequence("ctrl+f5"));
    profilerToggleAct->setStatusTip(tr("Toggle profiling"));
    profilerToggleAct->setCheckable (true);
    connect(profilerToggleAct, SIGNAL(toggled(bool)), treeview->world->profiler, SLOT(toggle(bool)));

    consoleToggleAct = new QAction(QIcon(":/images/console.png"),  tr("&Console"), this);
    consoleToggleAct->setShortcut(QKeySequence("F4"));
    consoleToggleAct->setStatusTip(tr("Toggle console"));
    consoleToggleAct->setCheckable (true);
    connect(consoleToggleAct, SIGNAL(toggled(bool)), console, SLOT(toggle(bool)));

    quitAct = new QAction(tr("&Exit"), this);
    quitAct->setShortcut(QKeySequence("ctrl+Q"));
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    hide_all_editors_Act = new QAction(QIcon(":/images/closealleditors.png"), tr("Hide all Editors"), this);
    hide_all_editors_Act->setShortcut(QKeySequence("ctrl+H"));
    connect(hide_all_editors_Act, SIGNAL(triggered()), this, SLOT(hide_all_editors()));

    run_all_scripts_Act = new QAction(QIcon(":/images/xpm/run.xpm"), tr("Enable All Scripts"), this);
    run_all_scripts_Act->setShortcut(QKeySequence("ctrl+F5"));
    connect(run_all_scripts_Act, SIGNAL(triggered()), this, SLOT(run_all_scripts()));

    stop_all_scripts_Act = new QAction(QIcon(":/images/xpm/stop.xpm"), tr("Disable All Scripts"), this);
    stop_all_scripts_Act->setShortcut(QKeySequence("ctrl+F2"));
    connect(stop_all_scripts_Act, SIGNAL(triggered()), this, SLOT(stop_all_scripts()));

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newProjectAct);
    fileMenu->addAction(openAct);
    historyMenu = fileMenu->addMenu(tr("&Recent Projects"));
    connect(historyMenu, &QMenu::aboutToShow, this, &MainWindow::reloadLastFilesList);

    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();

    fileMenu->addAction(quitAct);

    menuBar()->addSeparator();

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(hide_all_editors_Act);
    editMenu->addAction(run_all_scripts_Act);
    editMenu->addAction(stop_all_scripts_Act);
    editMenu->addSeparator();
    editMenu->addAction(switchDockTree);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Main"));
    fileToolBar->addAction(newProjectAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    fileToolBar->addSeparator();

    fileToolBar->addAction(switchDockTree);
    fileToolBar->addAction(profilerToggleAct);
    fileToolBar->addAction(consoleToggleAct);

    fileToolBar->addSeparator();
    fileToolBar->addAction(hide_all_editors_Act);
    fileToolBar->addAction(run_all_scripts_Act);
    fileToolBar->addAction(stop_all_scripts_Act);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::hasErrorText(const QString &error)
{
    QTimer::singleShot(150, this, [this, error]()
    {
        QMessageBox::critical(this, tr("Error"), error);
    });
}

void MainWindow::clearWorld()
{
    fileName = "";
    updateTitle();
    treeview->world->destroyAll();
}






