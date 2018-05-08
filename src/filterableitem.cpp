#include "filterableitem.h"
#include "item.h"


FilterableItem::FilterableItem(const QRegExp &filter)
    :filter(filter)
{

}

FilterableItem::FilterableItem(const QString &f):
    filter()
{
    setFilter(f);
}

bool FilterableItem::isForItem(const Item *itm) const
{
    return isForItem(itm->getType());
}

bool FilterableItem::isForItem(const QString &typeName) const
{
    return filter.exactMatch(typeName);
}

void FilterableItem::setFilter(const QString &f)
{
    filter = QRegExp((f.isEmpty())?"(.+)":f); //if empty string - then match any
}
