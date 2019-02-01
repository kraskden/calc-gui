#include "userfunctionlistwidget.h"
extern "C" {
#include "core/fun.h"
extern fun_list *fun_list_head;
}

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QTreeWidget>
#include <QVBoxLayout>


UserFunctionListWidget::UserFunctionListWidget(QWidget* parent)
    : QWidget(parent)
    , m_filterTimer(new QTimer(this))
    , m_userFunctions(new QTreeWidget(this))
    , m_noMatchLabel(new QLabel(m_userFunctions))
    , m_searchFilter(new QLineEdit(this))
    , m_searchLabel(new QLabel(this))
{
    m_filterTimer->setInterval(500);
    m_filterTimer->setSingleShot(true);

    m_userFunctions->setAlternatingRowColors(true);
    m_userFunctions->setAutoScroll(true);
    m_userFunctions->setColumnCount(2);
    m_userFunctions->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_userFunctions->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_userFunctions->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_userFunctions->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_userFunctions->setRootIsDecorated(false);
    m_userFunctions->setSelectionBehavior(QTreeWidget::SelectRows);
    m_userFunctions->setCursor(QCursor(Qt::PointingHandCursor));

    m_noMatchLabel->setAlignment(Qt::AlignCenter);
    m_noMatchLabel->adjustSize();
    m_noMatchLabel->hide();

    QWidget* searchBox = new QWidget(this);
    QHBoxLayout* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(m_searchLabel);
    searchLayout->addWidget(m_searchFilter);
    searchLayout->setMargin(0);
    searchBox->setLayout(searchLayout);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(3);
    layout->addWidget(searchBox);
    layout->addWidget(m_userFunctions);
    setLayout(layout);

    QMenu* contextMenu = new QMenu(m_userFunctions);
    m_insertAction = new QAction("", contextMenu);
    m_editAction = new QAction("", contextMenu);
    m_deleteAction = new QAction("", contextMenu);
    m_deleteAllAction = new QAction("", contextMenu);
    m_userFunctions->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_userFunctions->addAction(m_insertAction);
    m_userFunctions->addAction(m_editAction);
    m_userFunctions->addAction(m_deleteAction);
    m_userFunctions->addAction(m_deleteAllAction);

    QWidget::setTabOrder(m_searchFilter, m_userFunctions);
    setFocusProxy(m_searchFilter);

    retranslateText();

    connect(m_filterTimer, SIGNAL(timeout()), SLOT(updateList()));
    connect(m_searchFilter, SIGNAL(textChanged(const QString&)), SLOT(triggerFilter()));
    connect(m_userFunctions, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(activateItem()));
    connect(m_insertAction, SIGNAL(triggered()), SLOT(activateItem()));
    connect(m_editAction, SIGNAL(triggered()), SLOT(editItem()));
    connect(m_deleteAction, SIGNAL(triggered()), SLOT(deleteItem()));
    connect(m_deleteAllAction, SIGNAL(triggered()), SLOT(deleteAllItems()));

    updateList();
}

UserFunctionListWidget::~UserFunctionListWidget()
{
    m_filterTimer->stop();
}

void UserFunctionListWidget::updateList()
{
    setUpdatesEnabled(false);

    m_filterTimer->stop();
    m_userFunctions->clear();
    QString term = m_searchFilter->text();

    for (fun_list *i = fun_list_head; i; i = i->next) {
        QString fname;
        if (i->function.type == fun_base)
            fname = i->function.name;
        else {
            fname = i->function.name;
            fname += "(";
            for (var_list *j = i->function.par; j; j = j->next) {
                fname += j->variable.name;
                if (j->next)
                    fname += ";";
            }
            fname += ")";
        }
        QStringList namesAndValues;
        if (i->function.type == fun_base)
            namesAndValues << fname << "Base fun";
        else
            namesAndValues << fname << i->function.expr;

        if (term.isEmpty()
            || namesAndValues.at(0).contains(term, Qt::CaseInsensitive)
            || namesAndValues.at(1).contains(term, Qt::CaseInsensitive))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(m_userFunctions, namesAndValues);
            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            item->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    m_userFunctions->resizeColumnToContents(0);
    m_userFunctions->resizeColumnToContents(1);

    if (m_userFunctions->topLevelItemCount() > 0) {
        m_noMatchLabel->hide();
        m_userFunctions->sortItems(0, Qt::AscendingOrder);
    } else {
        m_noMatchLabel->setGeometry(m_userFunctions->geometry());
        m_noMatchLabel->show();
        m_noMatchLabel->raise();
    }

    setUpdatesEnabled(true);
}

void UserFunctionListWidget::retranslateText()
{
    QStringList titles;
    titles << tr("Name") << tr("Value");
    m_userFunctions->setHeaderLabels(titles);

    m_searchLabel->setText(tr("Search"));
    m_noMatchLabel->setText(tr("No match found"));

    m_insertAction->setText(tr("Insert"));
    m_editAction->setText(tr("Edit"));
    m_deleteAction->setText(tr("Delete"));
    m_deleteAllAction->setText(tr("Delete All"));

    QTimer::singleShot(0, this, SLOT(updateList()));
}

QTreeWidgetItem* UserFunctionListWidget::currentItem() const
{
    return m_userFunctions->currentItem();
}

QString UserFunctionListWidget::getUserFunctionName(const QTreeWidgetItem *item)
{
    return item->text(0).section("(", 0, 0);
}

void UserFunctionListWidget::activateItem()
{
    if (!currentItem() || m_userFunctions->selectedItems().isEmpty())
        return;
    emit userFunctionSelected(currentItem()->text(0).section("(", 0, 0));
}

void UserFunctionListWidget::editItem()
{
    if (!currentItem() || m_userFunctions->selectedItems().isEmpty())
        return;
    emit userFunctionEdited(currentItem()->text(0) + " = " + currentItem()->text(1));
}

void UserFunctionListWidget::deleteItem()
{
    if (!currentItem() || m_userFunctions->selectedItems().isEmpty())
        return;
    QString fun_name = getUserFunctionName(currentItem());
    fun_delete(&fun_list_head, fun_name.toLocal8Bit().data());
    updateList();
}

void UserFunctionListWidget::deleteAllItems()
{
    CLEAR_FUNC(fun_list_head);
    fun_list_head = NULL;
    updateList();
}

void UserFunctionListWidget::triggerFilter()
{
    m_filterTimer->stop();
    m_filterTimer->start();
}

void UserFunctionListWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        setLayoutDirection(Qt::LeftToRight);
        retranslateText();
        return;
    }
    QWidget::changeEvent(event);
}

void UserFunctionListWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete)
        deleteItem();
    else if (event->key() == Qt::Key_E)
        editItem();
    else {
        QWidget::keyPressEvent(event);
        return;
    }
    event->accept();
}
