#include "ledendialog.h"
#include "ui_ledendialog.h"
#include <qmessagebox.h>
#include <QtSql/QSqlQuery>
#include <QStandardItemModel>
#include<qinputdialog.h>

LedenDialog::LedenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LedenDialog)
{
    ui->setupUi(this);

    connect(ui->pbToevoegen, SIGNAL(clicked()), this, SLOT(newItem()));
    connect(ui->pbWijzig, SIGNAL(clicked()), this, SLOT(WijzigRecord()));
    connect(ui->pbTest, SIGNAL(clicked()), this, SLOT(Test()));
    connect(ui->pbVerwijderen, SIGNAL(clicked()), this, SLOT(deleteItem()));
    connect(ui->tableView, SIGNAL(pressed (QModelIndex)), this, SLOT(showIndex(const QModelIndex )));

    dbFile = "peter.sql";

}

void LedenDialog::WijzigRecord()
{
    newLid = false;
    bool ok;
    InputGegevens(ok);
    QSqlQuery query(db);
    QString text;

    QString quote="'";
    text="UPDATE Leden SET Voornaam = '"+ VoorNaam +"' WHERE PersoonId = " + quote+PersoonId+quote ;
    qDebug()<<text;
    ok=query.exec(text );if(!ok){QMessageBox::information(0, tr("SQL fout"),tr("kon item niet opslaan"), QMessageBox::Ok);return;}

    query.exec("SELECT *FROM Leden");
    model->setQuery(query);
    ui->tableView->setModel(model);
}

void LedenDialog::deleteItem()
{
    QSqlQuery query(db);
    bool ok;QString quote="'", tekst;
    if(PersoonId=="")return;
    switch(QMessageBox::question ( this,"Delete Item", "Wilt u deze Item verwijderen? "+PersoonId, QMessageBox::Yes, QMessageBox::No| QMessageBox::Default ))
    {
    case QMessageBox::Yes:
        tekst = "DELETE FROM Leden WHERE PersoonId = " +quote+PersoonId+quote;
        qDebug()<<tekst;
        ok=query.exec(tekst);
        if(!ok)QMessageBox::information(0, tr("SQL fout"),tr("kon item niet verwijderen"), QMessageBox::Ok);
        query.exec("SELECT *FROM Leden");
        model->setQuery(query);
        ui->tableView->setModel(model);
        break;
    case QMessageBox::No:
        break;
    }
}




void LedenDialog::setData(QString file)//voorbereiding
{
    if (file =="")return;
    db = QSqlDatabase::addDatabase("QSQLITE","bezetting");
    //QString file = qApp->applicationDirPath()+"/kb.sql";
    db.setDatabaseName(file);
    bool ok=db.open();
    if(!ok){QMessageBox::information(0, tr("SQL fout"),tr("kon DB niet openen"), QMessageBox::Ok);return;}
    setQuery();
}

void LedenDialog::showIndex(const QModelIndex index)
{
    //data zetten als er op geklikt wordt
    int row = index.row();
    PersoonId = index.sibling(row, 0).data().toString();
    VoorNaam = index.sibling(row, 1).data().toString();
    AchterNaam = index.sibling(row, 2).data().toString();
    Admin = index.sibling(row, 3).data().toBool();
    Beheer = index.sibling(row, 4).data().toBool();

    ui->lblRec->setText(PersoonId);

}

void LedenDialog::setQuery()//voorbereiding
{
    bool ok;
    QSqlQuery query(db);
    ok=query.exec("SELECT *FROM Leden " );
    if(!ok)
    {
        ok=query.exec("CREATE TABLE Leden (Persoonid varchar, VoorNaam varchar, Achternaam varchar, Admin bolean, Beheer bolean, Team varchar(20))");
        query.exec("SELECT *FROM Leden");
    }
    model = new QSqlQueryModel();
    model->setQuery(query);
    ui->tableView->setModel(model);
}

void LedenDialog::newItem()
{
    bool ok;
    QString text;
    newLid = true;
    InputGegevens(ok);
    if(!ok)return;
    QSqlQuery query(db);
//controleer op dubbelingen
    int x=0;QString quote="'";
    text="SELECT *FROM Leden WHERE Persoonid = " +quote+PersoonId+quote;
    ok=query.exec(text);
    if(ok)while (query.next()){x++;qDebug()<<PersoonId;}
    else QMessageBox::critical(0, tr("SQL fout"),tr("kon niet controleren op dubbelingen"), QMessageBox::Ok);
    if(x>>0)
    {
        QMessageBox::information(0, tr("Dubbel Naam"),tr("PersoonId bestaat al, probeer het opnieuw"), QMessageBox::Ok);
        return;
    }
    ok=query.prepare("INSERT INTO Leden (PersoonId, VoorNaam, Achternaam, Admin, Beheer, Team) " "VALUES (?,?,?,?,?,?)");
    query.addBindValue(PersoonId);
    query.addBindValue(VoorNaam);
    query.addBindValue(AchterNaam);
    query.addBindValue(Admin);
    query.addBindValue(Beheer);
    query.addBindValue(Team);
    ok=query.exec();
    if(!ok)QMessageBox::information(0, tr("SQL fout"),tr("kon item niet toevoegen"), QMessageBox::Ok);
    query.exec("SELECT *FROM Leden");
    model->setQuery(query);
    ui->tableView->setModel(model);
}

void LedenDialog::InputGegevens(bool &ok)
{
    if(newLid)
    {
        PersoonId = "";
        VoorNaam = "";
        AchterNaam = "";
        Admin = false;
        Beheer = false;
        Team="";
    }
    ok=true;
    QString text;
    if(newLid)// bij wijzig de personnsnr nooit veranderen
    {
         text = QInputDialog::getText(this, tr("Voer personeelsnr in"),
                                              tr("Personeelsnr"), QLineEdit::Normal,
                                              PersoonId, &ok);
         if (ok && !text.isEmpty())
             PersoonId = text.toUpper();
         else {
             ok=false;
             return;
         }
    }
    text = QInputDialog::getText(this, tr("Voer Voornaam in"),
                                          tr("Voornaam"), QLineEdit::Normal,
                                          VoorNaam, &ok);
     if (ok && !text.isEmpty())
         VoorNaam = text;
     else {
         ok=false;
         return;
     }


     text = QInputDialog::getText(this, tr("Voer Achternaam in"),
                                          tr("Achternaam"), QLineEdit::Normal,
                                          AchterNaam, &ok);
     if (ok && !text.isEmpty())
         AchterNaam = text;
     else {
         ok=false;
         return;
     }

     QMessageBox::StandardButton reply;
     reply = QMessageBox::question(this, "Admim", "Is lid een Admin", QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {
       Admin=true;
     } else {
       Admin=false;
     }

     reply = QMessageBox::question(this, "Beheer", "Is lid een Beheer",
                                   QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {
       Beheer=true;
     } else {
       Beheer=false;
     }

     QStringList l;
     QSqlQuery query(db);
     query.exec("SELECT  *FROM Team");
     while (query.next())
     {
         l<<query.value(0).toString();
     }

     //l<<"PVK-Plak"<<"PVK-Vloei"<<"PVK-D&D";
     QInputDialog inpDialog;
     inpDialog.setOption(QInputDialog::UseListViewForComboBoxItems);
     inpDialog.setComboBoxItems(l);
     int ret = inpDialog.exec();
     if (ret == QDialog::Accepted)Team=inpDialog.textValue();
     else ok=false;
}

void LedenDialog::Test()
{
    bool ok;
    qDebug()<<"test";
    QStringList l;
    l<<"PVK-Plak"<<"PVK-Vloei"<<"PVK-D&D";
    QInputDialog inpDialog;
    inpDialog.setOption(QInputDialog::UseListViewForComboBoxItems);
    inpDialog.setComboBoxItems(l);
    int ret = inpDialog.exec();
    if (ret == QDialog::Accepted)
       qDebug() << "Selection:" << inpDialog.textValue();
}

LedenDialog::~LedenDialog()
{
    delete ui;
}
