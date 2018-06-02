#include "dock_prep.h"
#include <QDockWidget>
#include "mainwindow.h"
#include <memory>
#include <QTimer>
#include <QMouseEvent>
#include <QMoveEvent>

static const Qt::DockWidgetAreas DOCK_AREAS(Qt::DockWidgetArea::LeftDockWidgetArea | Qt::DockWidgetArea::RightDockWidgetArea);

void nsDocks::applyPolicy(QWidget *w, int hs, int vs)
{
    auto p = w->sizePolicy();
    p.setVerticalStretch(vs);
    p.setHorizontalStretch(hs);
    w->setSizePolicy(p);
}

QDockWidget* nsDocks::createDockFromWidget(QWidget *srcWidget, const QString &dockName,  QWidget * parentWidget)
{
    QPointer<MyDock> dock = new MyDock((dockName.isEmpty()) ? srcWidget->objectName() : dockName, (parentWidget) ? parentWidget : MainWindow::instance);
    dock->setAllowedAreas(DOCK_AREAS);
    dock->setWidget(srcWidget);
    MainWindow::instance->addDockWidget(Qt::RightDockWidgetArea, dock);

    QPointer<QWidget> src(srcWidget);

    std::shared_ptr<bool> once(new bool(true));
    //tried to make it not dockable if hold more then 3s, but no luck
    //    QPointer<QTimer> mouseHold = new QTimer(dock);
    //    mouseHold->setSingleShot(true);
    //    QObject::connect(mouseHold, &QTimer::timeout, MainWindow::instance, std::bind(&QDockWidget::topLevelChanged, dock, true));

    //    QObject::connect(dock, &MyDock::onMousePressed, MainWindow::instance, [mouseHold]()
    //    {
    //        if (mouseHold)
    //            mouseHold->start(3000);
    //    });

    //    QObject::connect(dock, &MyDock::onMouseRelease, MainWindow::instance, [mouseHold]()
    //    {
    //        if (mouseHold)
    //            mouseHold->stop();
    //    });

    QObject::connect(dock, &QDockWidget::topLevelChanged, MainWindow::instance, [dock, src, once, dockName](bool top)
    {
        if (dock && src)
        {
            if (top)
            {
                //giving user some time to move it around
                dock->setAllowedAreas(Qt::NoDockWidgetArea);
                dock->setWindowTitle(QString(QObject::tr("[^] %1").arg(dockName)));
                QTimer::singleShot(3000, MainWindow::instance, [dock, dockName]()
                {
                    if (dock)
                    {
                        dock->setAllowedAreas(DOCK_AREAS);
                        dock->setWindowTitle(dockName);
                    }
                });

                if (*once)
                {
                    *once = false;
                    if (src)
                        src->adjustSize();
                    dock->adjustSize();
                }
            }
        }
    });

    return dock;
}

void MyDock::mousePressEvent(QMouseEvent *ev)
{
    //this is not triggered for dock caption, which i use to drag
    if (ev->button() == Qt::LeftButton)
        emit onMousePressed();
    QDockWidget::mousePressEvent(ev);
}

void MyDock::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
        emit onMouseRelease();
    QDockWidget::mouseReleaseEvent(ev);
}

MyDock::MyDock(const QString &name, QWidget *parent):
    QDockWidget(name, parent)
{}
