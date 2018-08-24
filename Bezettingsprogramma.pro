#-------------------------------------------------
#
# Project created by QtCreator 2017-09-16T11:35:14
#
#-------------------------------------------------
TEMPLATE = app
CONFIG += static
win32 {
 QMAKE_LFLAGS +=  -static-libgcc  -static-libstdc++  -s
}
#CONFIG += qt release warn_on console
QT       += core gui sql
CONFIG += static
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bezettingsprogramma


SOURCES += main.cpp\
        mainwindow.cpp \
    ledendialog.cpp \
    teamdialog.cpp

HEADERS  += mainwindow.h \
    ledendialog.h \
    teamdialog.h

FORMS    += mainwindow.ui \
    ledendialog.ui \
    teamdialog.ui
