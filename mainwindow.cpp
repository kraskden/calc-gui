#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "keypad.h"
#include "genericdock.h"

#include <QTimer>

extern "C" {
#include "core/mparser.h"
}

#include "QLayout"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mparser_init();
    ui->setupUi(this);
    QVBoxLayout *root = new QVBoxLayout();
    ui->centralWidget->setLayout(root);
    root->addWidget(ui->plainTextEdit);
    root->addWidget(ui->lineEdit);
    setWindowFlag(Qt::Dialog);
    QHBoxLayout *keypad_layot = new QHBoxLayout();
    keypad = new Keypad();
    keypad_layot->addStretch();
    keypad_layot->addWidget(keypad);
    keypad_layot->addStretch();
    root->addLayout(keypad_layot);
    keypad->show();
    create_var_dock(false);
    create_fun_dock(false);
    connect(ui->actionKeypad, &QAction::triggered, this, &MainWindow::keypad_trigger);
    connect(this->keypad, &Keypad::buttonPressed, this, &MainWindow::keypad_button_pressed);
    connect(ui->actionVariables, &QAction::triggered, this, &MainWindow::vardock_trigger);
    ui->lineEdit->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete keypad;
    mparser_destroy();
}

void MainWindow::on_lineEdit_returnPressed()
{
    QString expr = ui->lineEdit->text();
    calc(expr);
}

void MainWindow::keypad_trigger()
{
    ui->actionKeypad->isChecked() ? keypad->show() : keypad->hide();
}

void MainWindow::vardock_trigger()
{
    ui->actionVariables->isChecked() ? variables->show() : variables->hide();
}

void MainWindow::calc(const QString &expr)
{
    const char *data = expr.toLocal8Bit().data();
    var out;
    switch (mparser_parse(data, &out)) {
    case prs_produse: {
        ui->plainTextEdit->appendPlainText(expr);
        ui->lineEdit->clear();
        if (out.type == var_double) {
            ui->plainTextEdit->appendPlainText("= " + QString::number(out.value.double_val));
        } else {
            ui->plainTextEdit->appendPlainText("= " + QString::number(out.value.int_val));
        }
        ui->plainTextEdit->appendPlainText("");
        emit variables_changed();
        break;
    }
    case prs_def_fun: {
        emit functions_changed();
        ui->lineEdit->clear();
        break;
    }
    case prs_def_var: {
        emit variables_changed();
        ui->lineEdit->clear();
        break;
    }
    case prs_err: {
        QPalette pallete;
        pallete.setColor(QPalette::Text, Qt::red);
        ui->lineEdit->setPalette(pallete);
        QTimer::singleShot(250, this, SLOT(set_def_text()));
        break;
    }
    }
}

void MainWindow::keypad_button_pressed(Keypad::Button btn)
{
    QString append = "";
    switch (btn) {
    case Keypad::Key0: append = "0"; break;
    case Keypad::Key1: append = "1"; break;   
    case Keypad::Key2: append = "2"; break;
    case Keypad::Key3: append = "3"; break;
    case Keypad::Key4: append = "4"; break;
    case Keypad::Key5: append = "5"; break;
    case Keypad::Key6: append = "6"; break;
    case Keypad::Key7: append = "7"; break;
    case Keypad::Key8: append = "8"; break;
    case Keypad::Key9: append = "9"; break;

    case Keypad::KeyPlus: append = "+"; break;
    case Keypad::KeyMinus: append = "-"; break;
    case Keypad::KeyTimes: append = "*"; break;
    case Keypad::KeyDivide: append = "/"; break;

    case Keypad::KeyEE: append = "e"; break;
    case Keypad::KeyLeftPar: append = "("; break;
    case Keypad::KeyRightPar: append = ")"; break;
    case Keypad::KeyRaise: append = "*"; break;
    case Keypad::KeyX: append = "x"; break;
    case Keypad::KeyXEquals: append = "x = "; break;
    case Keypad::KeyPi: append = "pi"; break;
    case Keypad::KeyAns: append = "ans"; break;

    case Keypad::KeySqrt: append = "sqrt("; break;
    case Keypad::KeyLn: append = "ln("; break;
    case Keypad::KeyLg: append = "lg("; break;
    case Keypad::KeyExp: append = "exp("; break;
    case Keypad::KeySin: append = "sin("; break;
    case Keypad::KeyCos: append = "cos("; break;
    case Keypad::KeyTan: append = "tan("; break;
    case Keypad::KeyAsin: append = "asin("; break;
    case Keypad::KeyAcos: append = "acos("; break;
    case Keypad::KeyAtan: append = "atan("; break;

    case Keypad::KeyRadixChar: append = "."; break;
    case Keypad::KeyBS: {
        QString text = ui->lineEdit->text();
        text.chop(1);
        ui->lineEdit->setText(text);
        break;
    }
    case Keypad::KeyClear: ui->lineEdit->clear(); break;
    case Keypad::KeyEquals: {
        QString expr = ui->lineEdit->text();
        calc(expr);
        break;
    }
    }
    ui->lineEdit->insert(append);
}

void MainWindow::set_def_text()
{
    QPalette pallete;
    pallete.setColor(QPalette::Text, Qt::black);
    ui->lineEdit->setPalette(pallete);
}

void MainWindow::insert_var(const QString &v)
{
    ui->lineEdit->insert(v);
    ui->lineEdit->setFocus();
}

void MainWindow::insert_fun(const QString &f)
{
    ui->lineEdit->insert(f);
    ui->lineEdit->insert("()");
    ui->lineEdit->cursorBackward(false);
    ui->lineEdit->setFocus();
}

void MainWindow::edit_fun(const QString &f)
{
    ui->lineEdit->clear();
    ui->lineEdit->insert(f);
    ui->lineEdit->setFocus();
}

void MainWindow::create_var_dock(bool take_focus)
{
    variables = new GenericDock<VariableListWidget>("MainWindow", QT_TR_NOOP("Variables"), this);
    variables->setObjectName("VariablesDock");
    variables->installEventFilter(this);
    variables->setAllowedAreas(Qt::AllDockWidgetAreas);
    // signals-slots connect
    connect(this, &MainWindow::variables_changed, variables->widget(), &VariableListWidget::updateList);
    connect(variables->widget(), &VariableListWidget::variableSelected, this, &MainWindow::insert_var);
    // show
    addDockWidget(Qt::RightDockWidgetArea, variables);
    variables->show();
    variables->raise();
    if (take_focus)
        variables->setFocus();
}

void MainWindow::create_fun_dock(bool take_focus)
{
    functions = new GenericDock<UserFunctionListWidget>("MainWindow", QT_TR_NOOP("Functions"), this);
    functions->setObjectName("FunctionsDock");
    functions->installEventFilter(this);
    functions->setAllowedAreas(Qt::AllDockWidgetAreas);
    // signals + slots
    connect(this, &MainWindow::functions_changed, functions->widget(), &UserFunctionListWidget::updateList);
    connect(functions->widget(), &UserFunctionListWidget::userFunctionSelected, this, &MainWindow::insert_fun);
    connect(functions->widget(), &UserFunctionListWidget::userFunctionEdited, this, &MainWindow::edit_fun);
    addDockWidget(Qt::LeftDockWidgetArea, functions);
    functions->show();
    functions->raise();
    if (take_focus)
        functions->setFocus();
}
