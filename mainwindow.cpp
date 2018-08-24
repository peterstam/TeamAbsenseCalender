#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractTableModel>.h>
#include <QtSql/QSqlDatabase>
#include <QtGui>
#include <QInputDialog>
#include <QtSql/QSqlQuery>
#include <qmessagebox.h>
#include <QFont>






MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Team Aanwezigheid");
    vulCombo();
    QString pad = qApp->applicationDirPath();
    dbFile = pad+"/Aanwezigheid.sqlite";
    qDebug()<<dbFile;
    OpenDatabase();
    setModel();

    ledenModule = new LedenDialog();
    teamModule = new TeamDialog();
    connect(ui->pbNewDB, SIGNAL(clicked()), this, SLOT(MaakNewJaarTabel()));
    connect(ui->pbLeden, SIGNAL(clicked()), this, SLOT(naarLeden()));
    connect(ui->pbTeamBeheer, SIGNAL(clicked()), this, SLOT(naarTeam()));
    connect(ui->tableView, SIGNAL(pressed (QModelIndex)), this, SLOT(WijzigVeld(const QModelIndex )));
    connect(ui->cmbMaand, SIGNAL(currentIndexChanged(int)), this, SLOT(wijzigMaand()));
    connect(ui->cmbJaar, SIGNAL(currentIndexChanged(int)), this, SLOT(wijzigJaar()));
    connect(ui->cmbTeam, SIGNAL(currentIndexChanged(int)), this, SLOT(wijzigTeam()));


}

QVariant MainWindow::data(const QModelIndex &index, int role) const {
     if (role == Qt::FontRole) {
         // Custom your font
         QFont font;
         font.setBold(true);
         return font;
     } else if (role == Qt::ForegroundRole) {
         return QColor(Qt::red); // QBrush color
     } else {
         qDebug()<<"peter";
     }

     if (role == Qt::BackgroundRole){
         return QColor(Qt::red);
     }
}

void MainWindow::wijzigMaand()
{
 if(signalblokker)return;//voorkomt oneindige loop
    setModel();

}

void MainWindow::wijzigJaar()
{
 if(signalblokker)return;//voorkomt oneindige loop
    setModel();

}

void MainWindow::wijzigTeam()
{   if(signalblokker)return;//voorkomt oneindige loop
    setModel();

}


void MainWindow::setModel()
{
    signalblokker=true;
    int aantDagen;
    getAantDagen(aantDagen);
    model = new QStandardItemModel(0,aantDagen,this); //2 Rows and 3 Columns
    getAantDagen(aantDagen);
    qDebug()<<aantDagen;
    for(int x =1;x<aantDagen-1;x++)
    {
        QString s = QString::number(x);
        getWeekend(x, s);
        QStandardItem *dat = new QStandardItem(QString(s));
        if(s=="Za"|| s=="Zo")
        {
            QBrush b(QColor(Qt::green));
            dat->setData(b, Qt::ForegroundRole);
            qDebug()<<"stap 1";
        }
        int dag = gethuidigdag();
        if(x==dag)
        {
            QBrush b(QColor(Qt::red));
            dat->setData(b, Qt::ForegroundRole);
        }
        model->setHorizontalHeaderItem(x, dat);
    }
    QString quote="'";

    QSqlQuery query(db);
    //Teams ophalen
    if(!eenkeer)
    {
        query.exec("SELECT  *FROM Team");
    //en vul combobox Team
        ui->cmbTeam->clear();
        while (query.next())
        {
            ui->cmbTeam->addItem(query.value(0).toString());
        }
       eenkeer=true;
    }
    QSqlQuery query_wijziging(db);
    //haal leden op
    QString Team = ui->cmbTeam->currentText();
    query.exec("SELECT *FROM Leden WHERE Team = "+ quote + Team + quote + "ORDER BY Persoonid ASC");
    int x =0;
    while (query.next())
    {
        QString VoorNaam = query.value(1).toString();
        QString AchterNaam = query.value(2).toString();
        QString naam = VoorNaam + " " + AchterNaam;
        QString PesoonNr = query.value(0).toString();
        QString jaar = ui->cmbJaar->currentText();
        QString maand = QString::number(ui->cmbMaand->currentIndex()+1);
        model->setVerticalHeaderItem(x, new QStandardItem(QString(naam)));
        //model->setItem(x,0,persNr);
        model->setItem(x,0, new QStandardItem(QString(PesoonNr)));
        QString opdr = "SELECT *FROM  Wijziging WHERE Persoonsnr = " + quote + PesoonNr + quote + " AND Jaar=" + jaar + " AND Maand="  + maand  ;
        query_wijziging.exec(opdr);
        qDebug()<<"stap 3";

        while (query_wijziging.next())
        {
            int dag = query_wijziging.value(4).toInt();


            qDebug()<< "loop";
            QString reden = query_wijziging.value(5).toString();
            QStandardItem *dat = new QStandardItem(QString(reden));
            model->setItem(x,dag,dat);
            qDebug()<< reden;
            QFont f("Times", 8, QFont::Bold);
            //QBrush b(QColor(Qt::red));
            dat->setData(f, Qt::FontRole);
            //dat->setData(b, Qt::ForegroundRole);
            qDebug()<<"stap 4";

        }
        x++;
        qDebug()<<"stap 5";
    }

//Leden geladen

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(0);//kolom met personeelnr is hidden
    for(int x =1;x<aantDagen;x++)
    {
        ui->tableView->setColumnWidth(x,30);
    }

    signalblokker = false;


}

void MainWindow::getWeekend(int x, QString &weekenddag)
{
    int jaar = ui->cmbJaar->currentIndex()+2017;
    int maand = ui->cmbMaand->currentIndex()+1;
    QDate d(jaar,maand,x);
    int dag = d.dayOfWeek();
    if(dag==6) weekenddag = "Za";
    if(dag==7) weekenddag = "Zo";
}

void MainWindow::getAantDagen(int &aantDagen)
{
    int jaar = ui->cmbJaar->currentIndex()+2017;
    int maand = ui->cmbMaand->currentIndex()+1;
    QDate d(jaar,maand,1);
    aantDagen = d.daysInMonth();

}

int MainWindow::gethuidigdag()
{
    QDate d(QDate::currentDate());
    int maand = ui->cmbMaand->currentIndex()+1;
    int dag;
    int m = d.month();
    //als huidige maand niet voorstaat dan een 0 terugeven
    if(!(m==maand))dag=0;
    else dag=d.day();
    return dag;
}


void MainWindow::WijzigVeld(const QModelIndex &index)
{
    int row = index.row();
    int dag = index.column();
    //ui->lbltest->setText(QString::number(row));//werkt
    //ui->lbltest->setText(index.data().toString());//werkt
    QModelIndex i = model->index(row,0 , QModelIndex());
    QString PersNr = ui->tableView->model()->data(i).toString();
    ui->lbltest->setText(PersNr);

    bool ok;
    QString Reden = QInputDialog::getText(this, tr("Voer een afwezigheidsreden in"),
                                          tr("Reden"), QLineEdit::Normal,
                                          "VK", &ok);
    if (Reden.isEmpty())return;
    int jaar = ui->cmbJaar->currentText().toInt();
    int maand = ui->cmbMaand->currentIndex()+1;
    //QDate d (jaar,maand,dag);
    //qDebug()<<jaar<< " "<<maand<< " "<< dag;

   //kijk of er een dubbelng is

    //als er een dubbeling is dan updaten

    //anders nieuw toevoegen
    QSqlQuery query(db);

    ok=query.prepare("INSERT INTO Wijziging (Persoonsnr, Jaar, Maand ,Dag, Reden) " "VALUES (?,?,?,?,?)");
    query.addBindValue(PersNr);
    query.addBindValue(jaar);
    query.addBindValue(maand);
    query.addBindValue(dag);
    query.addBindValue(Reden);
    ok=query.exec();
    if(!ok)
    {
        QSqlQuery query1(db);
        ok=query1.exec("CREATE TABLE Wijziging (id int, Persoonsnr varchar(20), Jaar int, Maand int, Dag int, Reden varchar(20))");
        query1.exec("SELECT *FROM Wijziging");
        ok=query.prepare("INSERT INTO Wijziging (Persoonsnr, Jaar, Maand ,Dag, Reden) " "VALUES (?,?,?,?,?)");
        query.addBindValue(PersNr);
        query.addBindValue(jaar);
        query.addBindValue(maand);
        query.addBindValue(dag);
        query.addBindValue(Reden);
        ok=query.exec();
        if(!ok)QMessageBox::information(0, tr("SQL fout"),tr("kon item niet toevoegen"), QMessageBox::Ok);

    }
    setModel();

}

void MainWindow::vulCombo()
{
    ui->cmbMaand->addItem("Jan");
    ui->cmbMaand->addItem("Feb");
    ui->cmbMaand->addItem("Maart");
    ui->cmbMaand->addItem("April");
    ui->cmbMaand->addItem("Mei");
    ui->cmbMaand->addItem("Juni");
    ui->cmbMaand->addItem("Juli");
    ui->cmbMaand->addItem("Aug");
    ui->cmbMaand->addItem("Sept");
    ui->cmbMaand->addItem("Oct");
    ui->cmbMaand->addItem("Nov");
    ui->cmbMaand->addItem("Dec");

    ui->cmbJaar->addItem("2017");
    ui->cmbJaar->addItem("2018");
    ui->cmbJaar->addItem("2019");
    ui->cmbJaar->addItem("2020");
    ui->cmbJaar->addItem("2021");

    // zet op huidige jaar en maand
    QDate d = QDate::currentDate();
    int jaar = d.year();
    int maand = d.month();

    ui->cmbJaar->setCurrentIndex(jaar-2017);
    ui->cmbMaand->setCurrentIndex(maand-1);


}

void MainWindow::naarLeden()
{
    ledenModule->setData(dbFile);
    ledenModule->setQuery();
    ledenModule->exec();
    setModel();
}

void MainWindow::naarTeam()
{
    teamModule->setData(dbFile);
    teamModule->exec();
    setModel();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MaakNewDatabase()//nog niet nodig
{
    bool ok;
     QString text = QInputDialog::getText(this, tr("New DB File"),
                                          tr("Nieuw DB naam"), QLineEdit::Normal,
                                          "", &ok);
     if (!ok || text.isEmpty())return;
     dbFile = qApp->applicationDirPath()+"/"+text+".sql";

    db = QSqlDatabase::addDatabase("QSQLITE","showdb");
    db.setDatabaseName(dbFile);
    ok=db.open();
    QSqlQuery query(db);
    query.exec("create table person (id int primary key, firstname varchar(20), lastname varchar(20), datum varchar)");
    query.exec("create table Wijziging (id int primary key, Persoonsnr varchar(20), Jaar int, Maand int, Dag int, Reden varchar)");

}

void MainWindow::OpenDatabase()
{
/*
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("fdb16.awardspace.net"); // fake ip ;)
    db.setPort(3306);
    db.setDatabaseName("2371422_aanwezigheid");
    db.setUserName("2371422_aanwezigheid");
    db.setPassword("Racefiets9");
    if (!db.open())
        {  QMessageBox::critical(0,QObject::tr("Database Error"),"Fout");
         }

        else qDebug()<<"SUCCESS";
        */
    db = QSqlDatabase::addDatabase("QSQLITE","showdb");
    db.setDatabaseName(dbFile);
    db.open();


}

void MainWindow::MaakNewJaarTabel()//test
{
    /*
bool ok;
    QString text = QInputDialog::getText(this, tr("voer jaartal in"),
                                         tr("Jaar"), QLineEdit::Normal,
                                         "", &ok);
    if (!ok || text.isEmpty())return;
    int jaar = text.toInt();

    QSqlQuery query(db);
    QString opdracht = "create table " + text + "(id int primary key, maand varchar, aantDagen int)" ;
    query.exec(opdracht);
   QDate d(jaar,1, 1);
    QDate newd(jaar,2,1);
    int aantd = d.daysTo(newd);
    */
/*
    QSqlQuery query(db);
    QString text= "create table Wijziging (id int primary key, Persoonsnr varchar(20), Jaar int, Maand int, Dag int, Reden varchar(20))";
    //query.exec(text);
    query.exec("CREATE TABLE Leden (Persoonid varchar, VoorNaam varchar, Achternaam varchar, Admin bolean, Beheer bolean, Team varchar(20))");

qDebug()<<text;
*/
}

