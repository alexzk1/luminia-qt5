#ifndef DOCK_PREP_H
#define DOCK_PREP_H

#include <QWidget>

//should be called on nested widgets, not a dock itself
void inline applyPolicy(QWidget* w, int hs = 10, int vs = 1)
{
    auto p = w->sizePolicy();
    p.setVerticalStretch(vs);
    p.setHorizontalStretch(hs);
    w->setSizePolicy(p);
}



#endif // DOCK_PREP_H
