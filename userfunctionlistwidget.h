#ifndef GUI_USERFUNCTIONLISTWIDGET_H
#define GUI_USERFUNCTIONLISTWIDGET_H

#include <QtCore/QList>
#include <QWidget>

class QEvent;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

class UserFunctionListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserFunctionListWidget(QWidget* parent = 0);
    ~UserFunctionListWidget();

    QTreeWidgetItem* currentItem() const;
    QString getUserFunctionName(const QTreeWidgetItem *);

signals:
    void userFunctionSelected(const QString&);
    void userFunctionEdited(const QString&);

public slots:
    void updateList();
    void retranslateText();

protected slots:
    void activateItem();
    void editItem();
    void deleteItem();
    void deleteAllItems();
    void triggerFilter();

protected:
    void changeEvent(QEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Q_DISABLE_COPY(UserFunctionListWidget)

    QTimer* m_filterTimer;
    QTreeWidget* m_userFunctions;
    QAction* m_insertAction;
    QAction* m_editAction;
    QAction* m_deleteAction;
    QAction* m_deleteAllAction;
    QLabel* m_noMatchLabel;
    QLineEdit* m_searchFilter;
    QLabel* m_searchLabel;
};

#endif
