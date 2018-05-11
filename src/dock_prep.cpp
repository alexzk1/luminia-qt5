#include "dock_prep.h"
#include <QDockWidget>
#include "mainwindow.h"
#include <memory>
#include <QTimer>

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
    QPointer<QDockWidget> dock = new QDockWidget((dockName.isEmpty()) ? srcWidget->objectName() : dockName, (parentWidget) ? : MainWindow::instance);
    dock->setAllowedAreas(DOCK_AREAS);
    dock->setWidget(srcWidget);
    MainWindow::instance->addDockWidget(Qt::RightDockWidgetArea, dock);

    QPointer<QWidget> src(srcWidget);

    std::shared_ptr<bool> once(new bool(true));

    QObject::connect(dock, &QDockWidget::topLevelChanged, MainWindow::instance, [dock, src, once](bool top)
    {
        if (dock && src)
        {
            if (top)
            {
                //giving user some time to move it around
                dock->setAllowedAreas(Qt::NoDockWidgetArea);
                QTimer::singleShot(3000, MainWindow::instance, [dock, src]()
                {
                    if (dock)
                        dock->setAllowedAreas(DOCK_AREAS);
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
