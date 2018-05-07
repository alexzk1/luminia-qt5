#ifndef LOADERPATHS_H
#define LOADERPATHS_H

#include <QString>
#include <QStringList>

class LoaderPaths
{
public:

    static QString      findObject(const QString& fullFileName, const QStringList& relTo = {});
    static QStringList  buildDirsList(const QString& forWhat = "", QStringList relTo = {});

    //goes over program's pathes and lists unique file names, i.e. "test.js" will be present in 2 dirs, once will be listed
    const static QString PLUGINS;
    const static QString SCRIPTS;
    static QStringList  listFilesInSubfolder(const QString& subfolder, const QString& extension="js");
};

#endif // LOADERPATHS_H
