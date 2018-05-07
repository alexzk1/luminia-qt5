#include "loaderpaths.h"
#include <QStringList>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <algorithm>

QString LoaderPaths::findObject(const QString &fullFileName, const QStringList &relTo)
{
    qDebug() << "Searching for: " << fullFileName;
    QString result;
    if (!fullFileName.isEmpty())
    {
        QFileInfo tmp(fullFileName);
        if (tmp.exists())
            result = fullFileName;
        else
        {
            auto dlist = buildDirsList();
            const auto name = (tmp.isRelative())?fullFileName:tmp.fileName();
            for (const auto& p : dlist)
            {
                QFileInfo t(p + QDir::separator() + name);
                if (t.exists())
                {
                    result = t.absoluteFilePath();
                    break;
                }
            }
        }
    }
    qDebug() << "Found: " << result;
    return result;
}

QStringList LoaderPaths::buildDirsList(const QString &forWhat, QStringList relTo)
{
    const static QStringList search_pathches = {
        QFileInfo(QCoreApplication::arguments().at(0)).absolutePath(),
        QDir::homePath() + QDir::separator() + ".luminia",
        "/usr/share/luminia",
    };
    relTo.append(search_pathches);
    if (!forWhat.isEmpty())
    {
        std::for_each(relTo.begin(), relTo.end(),[&forWhat](auto& v)
        {
            v += forWhat;
        });
    }
    return relTo;
}
