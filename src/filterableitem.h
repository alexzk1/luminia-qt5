#ifndef FILTERABLEITEM_H
#define FILTERABLEITEM_H
#include <QRegExp>

class Item;
class FilterableItem
{
private:
    QRegExp filter;		//filter for the class name
protected:
    void setFilter(const QString &f);
    FilterableItem() = default;
public:

    FilterableItem(const QRegExp& filter);
    FilterableItem(const QString& f);
    virtual ~FilterableItem() = default;

    bool isForItem(const Item *itm) const;
    bool isForItem(const QString& typeName) const;
};

#endif // FILTERABLEITEM_H
