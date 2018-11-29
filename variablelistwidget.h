#ifndef GUI_VARIABLELISTWIDGET_H
#define GUI_VARIABLELISTWIDGET_H

#include <QList>
#include <QWidget>

class QEvent;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;
class Variable;

class VariableListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VariableListWidget(QWidget* parent = 0);
    ~VariableListWidget();

    QTreeWidgetItem* currentItem() const;

signals:
    void variableSelected(const QString&);

public slots:
    void updateList();
    void retranslateText();

protected slots:
    void activateItem();
    void deleteItem();
    void deleteAllItems();
    void triggerFilter();

protected:
    void changeEvent(QEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Q_DISABLE_COPY(VariableListWidget)

    QTimer* m_filterTimer;
    QTreeWidget* m_variables;
    QAction* m_insertAction;
    QAction* m_deleteAction;
    QAction* m_deleteAllAction;
    QLabel* m_noMatchLabel;
    QLineEdit* m_searchFilter;
    QLabel* m_searchLabel;
};

#endif
