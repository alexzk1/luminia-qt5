#ifndef PROHIBITED_FILTER_H
#define PROHIBITED_FILTER_H

#include <QStringList>

inline const QStringList&  getProhibitedMethods()
{
    //some methods must not be called ever, or immedial crash
    const static QStringList prohibited_to_use =
    {
        "destroyed()",
        "destroyed(QObject*)",
    };
    return prohibited_to_use;
}

inline void filterProhibitedCompletion(QStringList &list)
{
    for (const auto& p : getProhibitedMethods())
        list.removeAll(p);
}

#endif // PROHIBITED_FILTER_H
