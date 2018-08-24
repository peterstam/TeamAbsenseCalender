#include "teamdialog.h"
#include "ui_teamdialog.h"
#include <qmessagebox.h>
#include <qsqlquery.h>
#include <qdebug.h>
#include <qinputdialog.h>

TeamDialog::TeamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TeamDialog)
{
    ui->setupUi(this);

    connect(ui->pbToevoegen, SIGNAL(clicked()), this, SLOT(newItem()));
    connect(ui->pbWijzigen, SIGNAL(clicked()), this, SLOT(WijzigRecord()));
    connect(ui->pbVerwijderen, SIGNAL(clicked()), this, SLOT(deleteItem()));
    connect(ui->tableView, SIGNAL(pressed (QModelIndex)), this, SLOT(showIndex(const QModelIndex )));

}

void TeamDialog::showIndex(const QModelIndex index)
{
    //data zetten als er op geklikt wordt
    int row = index.row();
    Team = index.sibling(row, 0).data().toString();

    ui->label->setText(Team);

}

void TeamDialog::newItem()
{

    bool ok;
    QString text;
    newTeam = true;
    InputGegevens(ok);
    if(!ok)return;
    QSqlQuery query(db);
//controleer op dubbelingen
    int x=0;QString quote="'";
        qDebug()<<Team<<" team3";
    text="SELECT *FROM Team WHERE Team = " +quote+Team+quote;
    ok=query.exec(text);
    if(ok)while (query.next()){x++;qDebug()<<Team;}
    else QMessageBox::critical(0, tr("SQL fout"),tr("kon niet controleren op dubbelingen"), QMessageBox::Ok);
    if(x>>0)
    {
        QMessageBox::information(0, tr("Dubbel Naam"),tr("PersoonId bestaat al, probeer het opnieuw"), QMessageBox::Ok);
        return;
    }
    ok=query.prepare("INSERT INTO Team (Team) " "VALUES (?)");
    qDebug()<<Team<<" team";
    query.addBindValue(Team);
    ok=query.exec();
    if(!ok)QMessageBox::information(0, tr("SQL fout"),tr("kon item niet toevoegen"), QMessageBox::Ok);
    query.exec("SELECT *FROM Team");
    model->setQuery(query);
    ui->tableView->setModel(model);
}

void TeamDialog::WijzigRecord()
{
    newTeam = false;
    bool ok;
    oudTeam = Team;
    InputGegevens(ok);
    QSqlQuery query(db);
    QString text;

    QString quote="'";
    text="UPDATE Team SET Team = '"+ Team +"' WHERE Team = " + quote+oudTeam+quote ;
    qDebug()<<text;
    ok=query.exec(text );if(!ok){QMessageBox::information(0, tr("SQL fout"),tr("kon item niet opslaan"), QMessageBox::Ok);return;}

    query.exec("SELECT *FROM Team");
    model->setQuery(query);
    ui->tableView->setModel(model);
}


void TeamDialog::InputGegevens(bool &ok)
{
    if(newTeam)
    {
        Team = "";
    }
    ok=true;
    QString text;
    text = QInputDialog::getText(this, tr("Voer TeamNaam in"), tr("TeamNaam"), QLineEdit::Normal,Team, &ok);
    if(text.isEmpty())ok=false;
    Team = text;
}


void TeamDialog::setData(QString file)//voorbereiding
{
    if (file =="")return;
    db = QSqlDatabase::addDatabase("QSQLITE","bezetting");
    //QString file = qApp->applicationDirPath()+"/kb.sql";
    db.setDatabaseName(file);
    bool ok=db.open();
    if(!ok){QMessageBox::information(0, tr("SQL fout"),tr("kon DB niet openen"), QMessageBox::Ok);return;}
    setQuery();
}

void TeamDialog::setQuery()//voorbereiding
{
    bool ok;
    QSqlQuery query(db);
    ok=query.exec("SELECT *FROM Team " );
    if(!ok)
    {
        ok=query.exec("CREATE TABLE Team (Team varchar)");
        query.exec("SELECT *FROM Team ");
    }
    model = new QSqlQueryModel();
    model->setQuery(query);
    ui->tableView->setModel(model);
}

void TeamDialog::deleteItem()
{
    QSqlQuery query(db);
    bool ok;QString quote="'", tekst;
    if(Team=="")return;
    switch(QMessageBox::question ( this,"Delete Item", "Wilt u deze Item verwijderen? "+Team, QMessageBox::Yes, QMessageBox::No| QMessageBox::Default ))
    {
    case QMessageBox::Yes:
        tekst = "DELETE FROM Team WHERE Team = " +quote+Team+quote;
        qDebug()<<tekst;
        ok=query.exec(tekst);
        if(!ok)QMessageBox::information(0, tr("SQL fout"),tr("kon item niet verwijderen"), QMessageBox::Ok);
        query.exec("SELECT *FROM Team");
        model->setQuery(query);
        ui->tableView->setModel(model);
        break;
    case QMessageBox::No:
        break;
    }
}

TeamDialog::~TeamDialog()
{
    delete ui;
}
