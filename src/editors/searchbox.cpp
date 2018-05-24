#include "searchbox.h"
#include "ui_searchbox.h"
#include <QShortcut>
#include <QKeyEvent>
#include <QEvent>

SearchBox::SearchBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBox)
{
    ui->setupUi(this);
    connect(ui->btnPrev, &QPushButton::clicked, this, &SearchBox::searchPrev);
    connect(ui->btnNext, &QPushButton::clicked, this, &SearchBox::searchNext);
    connect(ui->btnAll, &QPushButton::clicked, this, &SearchBox::highlightAll);

    setVisible(false);
    new QShortcut(QKeySequence("Esc"), this, SLOT(hide()), SLOT(hide()), Qt::WidgetShortcut);

    ui->lineEdit->installEventFilter(this);
}

SearchBox::~SearchBox()
{
    delete ui;
}

QLineEdit *SearchBox::line() const
{
    return ui->lineEdit;
}

bool SearchBox::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->lineEdit && event->type() == QEvent::KeyPress)
    {
        auto ke = dynamic_cast<QKeyEvent*>(event);
        if (ke)
        {
            if (ke->key() == Qt::Key_Return)
            {
                emit searchNext();
                return true;
            }
            if (ke->key() == Qt::Key_Escape)
            {
                hide();
                return true;
            }
        }
    }

    return QWidget::eventFilter(target, event);
}

void SearchBox::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void SearchBox::setVisible(bool a)
{
    if (a)
        ui->lineEdit->clear();
    QWidget::setVisible(a);
}
