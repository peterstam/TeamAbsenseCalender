#ifndef TEAMDIALOG_H
#define TEAMDIALOG_H

#include <QDialog>
#include <qsqldatabase.h>
#include <qsqlquerymodel.h>

namespace Ui {
class TeamDialog;
}

class TeamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TeamDialog(QWidget *parent = 0);
    ~TeamDialog();
    void setData(QString file);
    void setQuery();
    void InputGegevens(bool &ok);

        QSqlDatabase db;
        QSqlQueryModel *model;
        bool newTeam;
        QString Team, oudTeam;


private:
    Ui::TeamDialog *ui;

private slots:
   void newItem();
   void WijzigRecord();
   void showIndex(QModelIndex ind);
   void deleteItem();
};

#endif // TEAMDIALOG_H
