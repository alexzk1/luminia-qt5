#ifndef DOCK_PREP_H
#define DOCK_PREP_H

#include <QWidget>
#include <QDockWidget>
class QMouseEvent;
class QMoveEvent;

class MyDock : public QDockWidget
{
    Q_OBJECT
public:
    MyDock(const QString& name, QWidget *parent);
    ~MyDock() override = default;
signals:
    void onMousePressed();
    void onMouseRelease();

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
};

namespace nsDocks
{
    //should be called on nested widgets, not a dock itself
    void applyPolicy(QWidget* w, int hs = 10, int vs = 1);
    QDockWidget* createDockFromWidget(QWidget* widget, const QString& dockName = "",  QWidget * parentWidget = nullptr);

}

#endif // DOCK_PREP_H
