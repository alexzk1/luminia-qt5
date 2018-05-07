#ifndef LOADERPATHS_H
#define LOADERPATHS_H

#include <QString>
#include <QStringList>

class LoaderPaths
{
public:
    static QString      findObject(const QString& fullFileName, const QStringList& relTo = {});
    static QStringList  buildDirsList(const QString& forWhat = "", QStringList relTo = {});
};

#endif // LOADERPATHS_H
