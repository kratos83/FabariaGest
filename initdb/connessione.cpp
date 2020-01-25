#include "connessione.h"
#include "ui_connessione.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QTextStream>
#include <stdio.h>
#include <stdlib.h>
#include <QSettings>
#include <QtGui/QtGui>
#include <QProcess>
#include "../fabaria/defines.h"
#include "../fabaria/databasesql.h"
#include "../fabaria/queryexception.h"

using namespace Defines;
using namespace DatabaseSql;

connessione::connessione(QString inizio_update,QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent),
    m_inizio(inizio_update)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(f);
    //Centro la schermata
    QRect reale = screenFromWidget(this)->geometry();
    int x = (reale.width()-width())/2;
    int y = (reale.height()-height())/2;
    move(x,y);
    connect(p_crea,SIGNAL(clicked()),this,SLOT(connetti()));
    connect(open_db,SIGNAL(clicked()),this,SLOT(connettidatabase()));
    connect(dat_locale,SIGNAL(clicked(bool)),this,SLOT(add_vis_groupbox(bool)));
    connect(dat_remoto,SIGNAL(clicked(bool)),this,SLOT(add_vis_groupbox(bool)));
    connect(m_ind,SIGNAL(clicked(bool)),this,SLOT(vis_main()));
    connect(m_ind_1,SIGNAL(clicked(bool)),this,SLOT(vis_main()));
    timer = new QTimer(this);
    manager = new SettingsManager;
    prog=0;
    textEdit->setReadOnly(true);
    setModal(true);
    password->setEchoMode(QLineEdit::Password);
    pwd_lan->setEchoMode(QLineEdit::Password);
    confread();
}

connessione::~connessione()
{
}

QSqlDatabase connessione::connetti(){

    open_db->setEnabled(false);
    ip_dat->setEnabled(false);
    port->setEnabled(false);
    n_dat_lan->setEnabled(false);
    user->setEnabled(false);
    pwd_lan->setEnabled(false);
    add_vis_groupbox(true);
    QString db_fabaria = "fabaria";
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("mysql");
    db.setUserName("root");
    db.setPassword(password->text());
    QDate date = QDate::currentDate();
    creadb(db,db_fabaria,date.year());

    return db;
}

bool connessione::isNull(){
    return false;
}

void connessione::creadb(QSqlDatabase db, QString db_fabaria, int year){


        if (db.open())
        {
            queryExec("create database "+db_fabaria+" CHARACTER SET=utf8","CREATE_DATABASE");
            textEdit->append(tr("Creazione database fabaria effettuata...."));
            db.close();


            db.setDatabaseName(db_fabaria);
            db.open();
            textEdit->append(tr("Creazione database fabaria effettuata..."));
#if defined (Q_OS_LINUX) || defined (Q_OS_FREBSD)
            QFile file("/opt/FabariaGest/installdb/install_db.sql");
#else
            QFile file("installdb/install_db.sql");
#endif
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
                qDebug() << tr("file non trovato") << file.fileName();

            QTextStream in(&file);
            QString linea;
            QString sql_stato = "";

            while(!in.atEnd()){
                linea = in.readLine();
                if((linea.startsWith("/*") && linea.startsWith("--")) || linea.isEmpty())
                    continue;
                sql_stato += linea;
                if(sql_stato.trimmed().endsWith(";")){

                    crea_tabelle(sql_stato);
                    sql_stato = "";
		            qint64 size = file.size();
		            progressBar->reset();
                    progressBar->setMaximum(size/1024);
                    progressBar->setValue(sql_stato.count());
                    progressBar->setValue(progressBar->maximum());
                }
            }
	    
	        insertQuery("INSERT INTO anno(anno_ins) VALUES("+QString::number(QDate::currentDate().year())+")","anno");
            
            if(m_inizio == "inizio"){
            QStringList m_conf;
            m_conf << "'1'"  <<  "''" << "''" << "''" << "''" << "''" << "''" << "''" << "''"  << "''" << "''" << "''" <<
                    "''" << QString("'locale'") << QString("'localhost'") <<
                    QString("'fabaria'") << QString("'root'") <<
                    QString("'"+password->text().toLatin1().toBase64()+"'") <<
                    "''" << "''" << "''" << "''" << "''" << "''" << "''"  << "''" << "''" <<
                    QString("'"+QString::number(year)+"'") << "''";
            QString m_cc = m_conf.join(",");
            insertConfigurazione(m_cc);
            }
            queryExec("GRANT ALL ON fabaria.* TO 'fabaria'@'localhost' IDENTIFIED BY 'fabaria'","GRANT TABLE");
            textEdit->append(tr("Creazione database locale"));

            textEdit->append(tr("Configurazione database in corso..."));
            barra_progresso();
            db.close();
	    
            if(db.open()){

                p_crea->setEnabled(false);
            }
        }

        else
        {
            QMessageBox MsgBox1;
            MsgBox1.setText(tr("Errore di connessione al DB"));
            MsgBox1.setInformativeText(tr("Controllare di aver installato Mysql e di aver creato il DB fabaria"));
            MsgBox1.setWindowTitle("Fabaria");
            QIcon icon;
            icon.addFile(QString::fromUtf8(":/images/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
            MsgBox1.setWindowIcon(icon);
            MsgBox1.setIcon(QMessageBox::Warning);
            MsgBox1.exec();
         }
}

void connessione::load_style(const QString &sheetName){

        QFile file(":/qss/" + sheetName.toLower() + ".qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());

        qApp->setStyleSheet(styleSheet);
        qApp->setStyle(styleSheet);
}

void connessione::barra_progresso(){

        textEdit->append(tr("Configurazione database completata con successo..."));
        messaggio();

}

void connessione::messaggio(){
    textEdit->append(tr("Creazione db strutturata con successo...."));
    #if defined (Q_OS_LINUX) || defined (Q_OS_FREBSD)
      QProcess *proc = new QProcess();
      proc->start("/opt/fabaria_gest/fabaria_gest");
    #endif
    #ifdef Q_OS_WIN
      QProcess *proc = new QProcess();
      proc->start("fabaria_gest.exe");
    #endif
    #ifdef Q_OS_MAC
      QProcess *proc = new QProcess();
      proc->start("/Applications/Fabaria/fabaria_gest.app");
    #endif
    exit(0);
}

void connessione::connettidatabase(){

    if(ip_dat->text().length() == 0 || n_dat_lan->text().length() == 0 || user->text().length() == 0 || pwd_lan->text().length() == 0){
        QMessageBox MsgBox2;
        MsgBox2.setText(tr("Errore di connessione al DB"));
        MsgBox2.setInformativeText(tr("Verificare che i dati siano corretti"));
        MsgBox2.setWindowTitle("Fabaria");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
        MsgBox2.setWindowIcon(icon);
        MsgBox2.setIcon(QMessageBox::Warning);
        MsgBox2.exec();
        ip_dat->setStyleSheet("background-color: rgb(249, 22, 5)");
        n_dat_lan->setStyleSheet("background-color: rgb(249, 22, 5)");
        user->setStyleSheet("background-color: rgb(249, 22, 5)");
        pwd_lan->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else{
    p_crea->setEnabled(false);
    db = QSqlDatabase::database("QMYSQL");
    db.setHostName(ip_dat->text());
    db.setPort(port->value());
    db.setDatabaseName(n_dat_lan->text());
    db.setUserName(user->text());
    db.setPassword(pwd_lan->text());

    if(m_inizio == "inizio"){
        QStringList m_list;
        m_list << QString("DatabaseServerIP='"+ip_dat->text()+"'") <<
                  QString("DatabaseServerPortaDB='"+QString::number(port->value())+"'") <<
                  QString("DatabaseServerNomeDB='"+n_dat_lan->text()+"'") <<
                  QString("DatabaseServerUserDB='"+user->text()+"'") <<
                  QString("DatabaseServerPWD='"+pwd_lan->text().toLatin1().toBase64()+"'") <<
                  QString("DatabaseLocale='server'");
        QString m_clist = m_list.join(",");
        insertConfigurazione(m_clist);
    }

    if (db.open())
    {
        messaggio();
    }
    else{
        QMessageBox MsgBox2;
        MsgBox2.setText(tr("Errore di connessione al DB"));
        MsgBox2.setInformativeText(tr("Impossibile connettersi al db.Controllare le impostazioni."));
        MsgBox2.setWindowTitle("FabariaGest");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
        MsgBox2.setWindowIcon(icon);
        MsgBox2.setIcon(QMessageBox::Warning);
        MsgBox2.exec();
    }
    }
}

void connessione::confread(){

    ip_dat->setText(getConfigurazioneSql("DatabaseServerIP").toString());
    port->setValue(getConfigurazioneSql("DatabaseServerPortaDB").toInt());
    n_dat_lan->setText(getConfigurazioneSql("DatabaseServerNomeDB").toString());
    user->setText(getConfigurazioneSql("DatabaseServerUserDB").toString());
    pwd_lan->setText(Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString()));
}

bool connessione::add_vis_groupbox(bool vis)
{
    if(dat_locale->isChecked() == vis)
    {
      stackedWidget->setCurrentIndex(1);
    }
    else if(dat_remoto->isChecked() == vis)
    {
      stackedWidget->setCurrentIndex(2);
    }

    return vis;
}

void connessione::closeEvent(QCloseEvent *event){

    event->ignore();

}

void connessione::crea_tabelle(const QString &sql) const{
    QSqlQuery query_prep;
    query_prep.prepare(sql);
    query_prep.exec();
}

QString connessione::Base64ToAscii(QString String){

    QByteArray ByteArray1=String.toUtf8();

    const char* chArr1=ByteArray1.constData();

    QByteArray ByteArray2=QByteArray::fromBase64(chArr1);

    const char* chArr2=ByteArray2.constData();

    return QString::fromUtf8(chArr2);
}

void connessione::vis_main()
{
  stackedWidget->setCurrentIndex(0);
}

