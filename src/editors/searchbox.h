#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QWidget>
#include <QLineEdit>

namespace Ui
{
    class SearchBox;
}

class SearchBox : public QWidget
{
    Q_OBJECT

public:
    explicit SearchBox(QWidget *parent = nullptr);
    ~SearchBox() override;
    QLineEdit *line() const;
    bool eventFilter(QObject *target, QEvent *event) override;
protected:
    void changeEvent(QEvent *e) override;
    void setVisible(bool a) override;
private:
    Ui::SearchBox *ui;
signals:
    void searchNext();
    void searchPrev();
    void highlightAll();
};

#endif // SEARCHBOX_H
