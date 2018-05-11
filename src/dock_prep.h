#ifndef DOCK_PREP_H
#define DOCK_PREP_H

#include <QWidget>
class QDockWidget;

namespace nsDocks
{
    //should be called on nested widgets, not a dock itself
    void applyPolicy(QWidget* w, int hs = 10, int vs = 1);
    QDockWidget* createDockFromWidget(QWidget* widget, const QString& dockName = "",  QWidget * parentWidget = nullptr);
}

#endif // DOCK_PREP_H
