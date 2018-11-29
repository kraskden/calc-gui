#include "variablelistwidget.h"
extern "C" {
#include "core/var.h"
extern var_list *var_list_head;
}

#include <QEvent>
#include <QTimer>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QTreeWidget>
#include <QVBoxLayout>

VariableListWidget::VariableListWidget(QWidget* parent)
    : QWidget(parent)
    , m_filterTimer(new QTimer(this))
    , m_variables(new QTreeWidget(this))
    , m_noMatchLabel(new QLabel(m_variables))
    , m_searchFilter(new QLineEdit(this))
    , m_searchLabel(new QLabel(this))
{
    m_filterTimer->setInterval(500);
    m_filterTimer->setSingleShot(true);

    m_variables->setAlternatingRowColors(true);
    m_variables->setAutoScroll(true);
    m_variables->setColumnCount(2);
    m_variables->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_variables->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_variables->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_variables->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_variables->setRootIsDecorated(false);
    m_variables->setSelectionBehavior(QTreeWidget::SelectRows);
    m_variables->setCursor(QCursor(Qt::PointingHandCursor));

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
    layout->addWidget(m_variables);
    setLayout(layout);

    QMenu* contextMenu = new QMenu(m_variables);
    m_insertAction = new QAction("", contextMenu);
    m_deleteAction = new QAction("", contextMenu);
    m_deleteAllAction = new QAction("", contextMenu);
    m_variables->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_variables->addAction(m_insertAction);
    m_variables->addAction(m_deleteAction);
    m_variables->addAction(m_deleteAllAction);

    QWidget::setTabOrder(m_searchFilter, m_variables);
    setFocusProxy(m_searchFilter);

    retranslateText();

    connect(m_filterTimer, SIGNAL(timeout()), SLOT(updateList()));
    connect(m_searchFilter, SIGNAL(textChanged(const QString&)), SLOT(triggerFilter()));
    connect(m_variables, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(activateItem()));
    connect(m_insertAction, SIGNAL(triggered()), SLOT(activateItem()));
    connect(m_deleteAction, SIGNAL(triggered()), SLOT(deleteItem()));
    connect(m_deleteAllAction, SIGNAL(triggered()), SLOT(deleteAllItems()));

    updateList();
}

VariableListWidget::~VariableListWidget()
{
    m_filterTimer->stop();
}

void VariableListWidget::updateList()
{
    setUpdatesEnabled(false);

    m_filterTimer->stop();
    m_variables->clear();
    QString term = m_searchFilter->text();
    for (var_list *i = var_list_head; i; i = i->next) {
        QString varName(i->variable.name);
        QString varValue;
        if (i->variable.type == var_double)
            varValue = QString::number(i->variable.value.double_val);
        else
            varValue = QString::number(i->variable.value.int_val);
        QStringList namesAndValues;
        namesAndValues << varName << varValue; //Need format!

        if (term.isEmpty()
            || namesAndValues.at(0).contains(term, Qt::CaseInsensitive)
            || namesAndValues.at(1).contains(term, Qt::CaseInsensitive))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(m_variables, namesAndValues);
            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            item->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    m_variables->resizeColumnToContents(0);
    m_variables->resizeColumnToContents(1);

    if (m_variables->topLevelItemCount() > 0) {
        m_noMatchLabel->hide();
        m_variables->sortItems(0, Qt::AscendingOrder);
    } else {
        m_noMatchLabel->setGeometry(m_variables->geometry());
        m_noMatchLabel->show();
        m_noMatchLabel->raise();
    }

    setUpdatesEnabled(true);
}

void VariableListWidget::retranslateText()
{
    QStringList titles;
    titles << tr("Name") << tr("Value");
    m_variables->setHeaderLabels(titles);

    m_searchLabel->setText(tr("Search"));
    m_noMatchLabel->setText(tr("No match found"));

    m_insertAction->setText(tr("Insert"));
    m_deleteAction->setText(tr("Delete"));
    m_deleteAllAction->setText(tr("Delete All"));

    QTimer::singleShot(0, this, SLOT(updateList()));
}

QTreeWidgetItem* VariableListWidget::currentItem() const
{
    return m_variables->currentItem();
}

void VariableListWidget::activateItem()
{
    if (!currentItem() || m_variables->selectedItems().isEmpty())
        return;
    emit variableSelected(currentItem()->text(0));
}

void VariableListWidget::deleteItem()
{
    if (!currentItem() || m_variables->selectedItems().isEmpty())
        return;
    const char* varName = currentItem()->text(0).toLocal8Bit().data();
    var_delete(&var_list_head, varName);
    updateList();
}

void VariableListWidget::deleteAllItems()
{
    CLEAR_VAR(var_list_head); //Unsafe! Delete pi, e, etc...
    var_list_head = NULL;
    updateList();
}

void VariableListWidget::triggerFilter()
{
    m_filterTimer->stop();
    m_filterTimer->start();
}

void VariableListWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        setLayoutDirection(Qt::LeftToRight);
        retranslateText();
        return;
    }
    QWidget::changeEvent(event);
}

void VariableListWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        deleteItem();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

