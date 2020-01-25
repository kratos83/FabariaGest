#include "check_database.h"
#include "ui_check_database.h"
#include <QDebug>
#include "../fabaria/defines.h"
#include "../fabaria/databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

check_database::check_database(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::check_database)
{
    ui->setupUi(this);
    setWindowTitle("Analizza e ripara database");
    //Centro la schermata
    QRect reale = screenFromWidget(this)->geometry();
    int x = (reale.width()-width())/2;
    int y = (reale.height()-height())/2;
    move(x,y);
    connect(ui->analize_db,SIGNAL(clicked()),this,SLOT(analizza_database()));
    connect(ui->correct_db,SIGNAL(clicked()),this,SLOT(ripristina_database()));
    connect(ui->chiudi,SIGNAL(clicked()),this,SLOT(close()));
    model = new QStandardItemModel(500,2);
    //Visualizzo lista
    lista();
}

void check_database::analizza_database()
{
    ui->esito->clear();
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
    QString comando=GetRunCommandMySql()+"/mysqlcheck -c --databases fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString());
#endif
#ifdef Q_OS_MAC
    QString comando=GetRunCommandMySql()+"/mysqlcheck -c --databases fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString());
#endif
#ifdef Q_OS_WIN
    QString comando=GetRunCommandMySql()+"\\mysqlcheck -c --databases fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString());
#endif
    analizza = new QProcess(this);
    analizza->setReadChannel(QProcess::StandardOutput);
    analizza->setProcessChannelMode(QProcess::MergedChannels);
    analizza->start(comando);

    if(!analizza->waitForStarted()) {
        ui->esito->setText(tr("Errore, impossbile avviare il servizio mysqlcheck..."));
    }
    analizza->closeWriteChannel();
    ui->esito->setText(tr("Analizzazione tabelle in corso...\n\n"));
    while(analizza->state()!=QProcess::NotRunning) {
        analizza->waitForReadyRead();
        QByteArray result = analizza->readAll();
        read_output(QString(result));
    }
    ui->esito->setText(tr("Fine analizzazione..."));
}

void check_database::ripristina_database()
{
    ui->esito->clear();
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
    QString comando=GetRunCommandMySql()+"/mysqlcheck -r --databases fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString());
#endif
#ifdef Q_OS_MAC
    QString comando=GetRunCommandMySql()+"/mysqlcheck -r --databases fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString());
#endif
#ifdef Q_OS_WIN
    QString comando=GetRunCommandMySql()+"\\mysqlcheck -r --databases fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString());
#endif
    ripristina = new QProcess(this);
    ripristina->setReadChannel(QProcess::StandardOutput);
    ripristina->setProcessChannelMode(QProcess::MergedChannels);
    ripristina->start(comando);

    if(!ripristina->waitForStarted()) {
        ui->esito->setText(tr("Errore, impossbile avviare il servizio mysqlcheck..."));
    }
    ripristina->closeWriteChannel();

    ui->esito->setText(tr("Riparazione tabelle in corso...\n\n"));
    while(ripristina->state()!=QProcess::NotRunning) {
        ripristina->waitForReadyRead();
        QByteArray result = ripristina->readAll();
        read_output(QString(result));
    }
    ui->esito->setText(tr("Fine riparazione..."));
}

void check_database::read_output(QString nameFile)
{
    int count = 0;
    QTextStream stream(&nameFile);
    while(!stream.atEnd()) {
        QString line = stream.readLine();

        //Empty line
        if(line.isEmpty())
            continue;

        //Add
        QStringList list = line.split(" ",QString::SkipEmptyParts);
        for(int y =0; y< list.size(); y++) {
            QString value = list.at(y);
            QStandardItem *m_item1 = new QStandardItem(value);
            model->setItem(count,y,m_item1);
        }
        count++;
    }
}


void check_database::lista()
{
    model->clear();
    model = new QStandardItemModel(500,2);
    model->setRowCount(0);
    model->setHeaderData(0,Qt::Horizontal,tr("Tabelle"));
    model->setHeaderData(1,Qt::Horizontal,tr("Esito"));
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

check_database::~check_database()
{
    delete ui;
}