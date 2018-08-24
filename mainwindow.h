#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ledendialog.h>
#include <teamdialog.h>
#include <QtSql/QSqlDatabase>
#include <QtGui>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void OpenDatabase();
    void vulCombo();
    void setModel();
    void getAantDagen(int &aantDagen);
    QVariant data(const QModelIndex &index, int role)const;
    void getWeekend(int x, QString &weekenddag);
    int gethuidigdag();

    LedenDialog *ledenModule;
    TeamDialog *teamModule;

    QString dbFile;
    QStandardItemModel *model;

    QSqlDatabase db;
    bool signalblokker=false;
    bool eenkeer=false;

private:
    Ui::MainWindow *ui;

public slots:
     void MaakNewDatabase();
     void MaakNewJaarTabel();
     void naarLeden();
     void WijzigVeld(const QModelIndex &index);
     void wijzigMaand();
     void wijzigJaar();
     void wijzigTeam();
     void naarTeam();

};

#endif // MAINWINDOW_H
