#include <QStringList>
#include <QFontDatabase>

QString pickBestAvailFontFamily()
{
    //more fonts from https://habr.com/post/358992/
    const static QStringList prefFontsFamilies =
    {
        "Source Code Pro",
        "Monaco",
        "Consolas",
        "Ubuntu Mono",
        "Menlo",
        "DejaVu Sans Mono",
        "Droid Sans Mono"
        "Courier",
    };
    static QFontDatabase db;
    static QStringList current = db.families();

    for (const auto& fs : prefFontsFamilies)
    {
        if (current.contains(fs, Qt::CaseInsensitive))
            return fs;
    }
    return "Monospace";
}
