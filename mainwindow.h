#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "keypad.h"
#include "genericdock.h"
#include "variablelistwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_lineEdit_returnPressed();
    void keypad_trigger();
    void vardock_trigger();
    void keypad_button_pressed(Keypad::Button btn);
    void set_def_text();
    void insert_var(const QString &v);

signals:
    void variables_changed();

private:
    //void addTabifiedDock(QDockWidget* newDock, bool takeFocus, Qt::DockWidgetArea = Qt::RightDockWidgetArea);
    void create_var_dock(bool take_focus);
    Keypad *keypad;
    GenericDock<VariableListWidget>* variables;
    Ui::MainWindow *ui;
    void calc(const QString &expr);
};

#endif // MAINWINDOW_H
