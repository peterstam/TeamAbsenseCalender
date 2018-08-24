#ifndef LEDENDIALOG_H
#define LEDENDIALOG_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtGui>
#include <qsqlquerymodel.h>


namespace Ui {
class LedenDialog;
}

class LedenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LedenDialog(QWidget *parent = 0);
    ~LedenDialog();
    void setData(QString file);
    void setQuery();


    QSqlDatabase db;
    QSqlQueryModel *model;
    QString PersoonId, VoorNaam, AchterNaam, Team;
    bool Admin, Beheer, newLid;
    QString dbFile;



private:
    Ui::LedenDialog *ui;
    void InputGegevens(bool &ok);

private slots:
    void newItem();
    void showIndex(QModelIndex ind);
    void WijzigRecord();
    void deleteItem();
    void Test();

};

#endif // LEDENDIALOG_H
