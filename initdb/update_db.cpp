#include "update_db.h"
#include "ui_update_db.h"
#include "../fabaria/databasesql.h"
#include "../fabaria/defines.h"

using namespace Defines;
using namespace DatabaseSql;

update_db::update_db(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::update_db)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Aggiornamento database"));
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    connect(ui->p_aggiorna,SIGNAL(clicked()),this,SLOT(connetti()));
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    ui->textEdit->setReadOnly(true);
}


QSqlDatabase update_db::connetti()
{
    QString db_fabaria = "fabaria";
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("mysql");
    db.setUserName("root");
    db.setPassword(Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString()));
    aggiorna_db(db,db_fabaria);

    return db;
}

void update_db::aggiorna_db(QSqlDatabase db, QString db_fabaria)
{
    if (db.open())
    {
        QSqlQuery query;
        db.setDatabaseName(db_fabaria);
        db.open();
#if defined (Q_OS_LINUX) || defined (Q_OS_FREBSD)
        QFile file("/opt/FabariaGest/updatedb/update_db.sql");
#else
        QFile file("updatedb/update_db.sql");
#endif
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            qDebug() << tr("file non trovato");

        QTextStream in(&file);
        QString linea;
        QString sql_stato = "";

        while(!in.atEnd()){
            linea = in.readLine();
            if((linea.startsWith("/*") && linea.startsWith("--")) || linea.isEmpty())
                continue;
            sql_stato += linea;
            if(sql_stato.trimmed().endsWith(";")){

                aggiorna_tabelle(sql_stato);
                sql_stato = "";
                ui->progressBar->setMaximum(linea.size());
                ui->progressBar->setValue(sql_stato.count());
                ui->progressBar->setValue(ui->progressBar->maximum());
            }
        }

        queryExec("GRANT ALL ON fabaria.* TO 'fabaria'@'localhost' IDENTIFIED BY 'fabaria'","GRANT_TABLE_UPDATE");
        ui->textEdit->append(tr("Aggiornamento database locale"));

        ui->textEdit->append(tr("Aggiornamento database in corso..."));
        messaggio();
        db.close();

        if(db.open()){

            ui->p_aggiorna->setEnabled(false);
        }
    }

    else
    {
                    QMessageBox MsgBox1;
                    MsgBox1.setText(tr("Errore di connessione al DB"));
                    MsgBox1.setInformativeText(tr("Impossibile aggiornare il database"));
                    MsgBox1.setWindowTitle("Fabaria");
                    QIcon icon;
                    icon.addFile(QString::fromUtf8(":/images/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
                    MsgBox1.setWindowIcon(icon);
                    MsgBox1.setIcon(QMessageBox::Warning);
                    MsgBox1.exec();
     }
}

void update_db::aggiorna_tabelle(const QString &sql) const{
    QSqlQuery query_prep;
    query_prep.prepare(sql);
    query_prep.exec();
}

void update_db::messaggio()
{
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("Fabaria");
    box->setText("Database");
    box->setInformativeText(tr("Aggiornamento db strutturato con successo...."));
    box->setStandardButtons(QMessageBox::Ok);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
    case QMessageBox::Ok:
         //Close
        #if defined (Q_OS_LINUX) || defined (Q_OS_FREBSD)
            QProcess *proc = new QProcess();
            proc->start("/opt/FabariaGest/FabariaGest");
        #endif
        #ifdef Q_OS_WIN
            QProcess *proc = new QProcess();
            proc->start("FabariaGest.exe");
        #endif
        #ifdef Q_OS_MAC
            QProcess *proc = new QProcess();
            proc->start("/Applications/Fabaria/FabariaGest.app");
        #endif
         box->close();
         exit(0);
         break;
    }
}

void update_db::load_style(const QString &sheetName){

    QFile file(":/qss/" + sheetName.toLower() + ".qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    qApp->setStyleSheet(styleSheet);
    qApp->setStyle(styleSheet);
}

QString update_db::Base64ToAscii(QString String){

    QByteArray ByteArray1=String.toUtf8();

    const char* chArr1=ByteArray1.constData();

    QByteArray ByteArray2=QByteArray::fromBase64(chArr1);

    const char* chArr2=ByteArray2.constData();

    return QString::fromUtf8(chArr2);
}

update_db::~update_db()
{
    delete ui;
}
