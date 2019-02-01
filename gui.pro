#-------------------------------------------------
#
# Project created by QtCreator 2018-11-26T21:51:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    keypad.cpp \
    main.cpp \
    mainwindow.cpp \
    core/calc.c \
    core/define.c \
    core/extmath.c \
    core/fun.c \
    core/list.c \
    core/mparser.c \
    core/operations.c \
    core/token.c \
    core/var.c \
    variablelistwidget.cpp \
    userfunctionlistwidget.cpp

HEADERS += \
    mainwindow.h \
    keypad.h \
    core/calc.h \
    core/define.h \
    core/extmath.h \
    core/fun.h \
    core/list.h \
    core/mparser.h \
    core/operations.h \
    core/token.h \
    core/var.h \
    variablelistwidget.h \
    genericdock.h \
    userfunctionlistwidget.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
