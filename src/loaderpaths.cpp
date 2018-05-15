#include "loaderpaths.h"
#include <QStringList>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <algorithm>
#include <set>
#include <QProcess>

const QString LoaderPaths::PLUGINS = "plugins";
const QString LoaderPaths::SCRIPTS = "scripts";

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
            auto dlist = buildDirsList(QString(), relTo);
            const auto name = (tmp.isRelative()) ? fullFileName : tmp.fileName();
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
    const static QStringList search_pathches =
    {
        QFileInfo(QCoreApplication::arguments().at(0)).absolutePath(),
        QDir::homePath() + QDir::separator() + ".luminia",
        "/usr/share/luminia",
    };
    relTo.append(search_pathches);
    if (!forWhat.isEmpty())
    {
        std::for_each(relTo.begin(), relTo.end(), [&forWhat](auto & v)
        {
            v += forWhat;
        });
    }
    return relTo;
}

QStringList LoaderPaths::listFilesInSubfolder(const QString &subfolder, const QString &extension)
{
    QStringList result;
    std::set<QString> no_same_name;
    auto folders = buildDirsList(QDir::separator() + subfolder + QDir::separator());
    for (const auto& p : folders)
    {
        QDir dir(p);
        dir.setFilter( QDir::Files);
        dir.setSorting( QDir::Size | QDir::Reversed );
        QFileInfoList list = dir.entryInfoList();

        for (const auto & fileInfo : list)
        {
            const auto name = fileInfo.fileName();

            if (fileInfo.suffix() == extension && !no_same_name.count(name))
            {
                result.push_back(fileInfo.absoluteFilePath());
                no_same_name.insert(name);
            }
        }
    }
    return result;
}

void LoaderPaths::spawnCopy(const QString &file2open)
{
    QString my_name = QFileInfo(QCoreApplication::arguments().at(0)).absoluteFilePath();
    //qDebug() << "Spawning: " << my_name << file2open;
    if (file2open.isEmpty())
        QProcess::startDetached(my_name);
    else
        QProcess::startDetached(my_name, {file2open}, QFileInfo(file2open).absolutePath());
}
