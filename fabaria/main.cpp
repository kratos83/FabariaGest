#include <QApplication>
#include <QString>
#include <QtSql>
#include <QMessageBox>
#include <QProgressBar>
#include <QObject>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QDebug>
#include <QFile>
#include <QVariant>
#include <QSysInfo>
#include <QFrame>
#include <time.h>
#include "mainwindow.h"
#include "splashscreen.h"
#include "settingsmanager.h"
#include "../initdb/update_db.h"
#include "../initdb/connessione.h"
#include "fabariaapplication.h"
#include "queryexception.h"
#include "defines.h"
#include "imposta_anno.h"
#include "minitutorial.h"
#include "splashscreen.h"
//Classi C++
#include <iostream>
#include "databasesql.h"

using namespace std;
using namespace Defines;
using namespace DatabaseSql;

static void addTutorial()
{
    MiniTutorial *mini = new MiniTutorial;
    mini->exec();
}

static void aggiorna_db()
{
    update_db *update = new update_db();
    QMessageBox *box= new QMessageBox;
    box->setWindowTitle("Fabaria Gest");
    box->setText(qApp->translate("main","Aggiornamento database"));
    box->setInformativeText(qApp->translate("main","Prima di effettuare l'aggiornamento al\ndatabase accertati di averne salvato\n"
                            "una copia..."));
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
    case QMessageBox::Ok:
         //Open database update
         update->exec();
         box->close();
         break;
    case QMessageBox::Cancel:
         //Close
         box->close();
         break;
    }
}

static void anno()
{
        imposta_anno *m_anno = new imposta_anno;
        m_anno->exec();
}

static void impostazioni(QSqlDatabase db)
{
    db.open();
    /**********************************
     *Impostazione della porta del db
     **********************************/
    if(getConfigurazioneSql("DatabaseServerPortaDB").toInt() == 0){
        int x = 3306;
        updateConfigurazione("DatabaseServerPortaDB='"+QString::number(x)+"'");
    }

    /**********************************
     *Impostazione della versione
     **********************************/
    if(getConfigurazioneSql("VersioneFabariaGest").toDouble() == 0 || 
       getConfigurazioneSql("VersioneFabariaGest").toDouble() <= 2.0){
        updateConfigurazione("VersioneFabariaGest='2.1'");
    }

    /**********************************
     *Impostazione dei backup
     **********************************/
    if(getConfigurazioneSql("BackupDate").toString().length() == 0){
            QDate curDate = QDate::currentDate();
             updateConfigurazione("BackupDate='"+curDate.toString("yyyy-MM-dd")+"'");
    }

    /**********************************
     *Impostazione degli aggiornamenti
     **********************************/
    if(getPreferenzeSql("AggEn").toBool() == false)
        updatePreferenze("AggEn='1'");

    /*************************
     *Impostazione dei plugin
     *************************/
    if(getConfigurazioneSql("PluginLoad").toString().length() == 0)
        updateConfigurazione("PluginLoad='dbbkrs_plugin, checkdatabase_plugin, inventario_plugin'");
    

    /*************************
     *Impostazione del tema
     *************************/
    if(getPreferenzeSql("SelTema").toString() == "")
    {
        updatePreferenze("SelTema='Default'");
    }
    
    if(getConfigurazioneSql("CartellaLOG").toString() == "")
    {   
	    updateConfigurazione("CartellaLOG='"+QDir::homePath()+"/fabaria/log/FABARIALOG.txt'");
    }
}

void traduzione(QSqlDatabase db)
{
    db.open();
    // Traduzione
    QTranslator *translator = new QTranslator();
    QString locale = QLocale::system().name();
    qDebug() << locale;
    if(locale == "it_IT" || getPreferenzeSql("LinguaMadre").toString() == "Italian"){
        updatePreferenze("LinguaMadre='Italian'");
        translator->load(QString(":/language/")+getPreferenzeSql("LinguaMadre").toString());
    }
    else{
        updatePreferenze("LinguaMadre='English'");
        translator->load(QString(":/language/")+getPreferenzeSql("LinguaMadre").toString());
    }
     qApp->installTranslator(translator);
}

int main(int argc, char *argv[])
{

  try{
    FabariaApplication a(argc, argv);   
    QPixmap image(":/images/splash.png");
    SplashScreen *splash = new SplashScreen(image,0);
    splash->show();

    time_t t;
    t = time(NULL);

    splash->showMessages(QString(qApp->translate("main","Avvio applicazione")));

    a.processEvents();
    splash->showMessages(QString(qApp->translate("main","Avvio database")));

    a.processEvents();
    splash->showMessages(QString(qApp->translate("main","Avvio traduzione"))); 
    
    connessione *conn = new connessione("inizio",0,Qt::FramelessWindowHint);
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        /*************************
         *Impostazione del database
        *************************/
        //if(getConfigurazioneSql("DatabaseLocale").toString()== "locale"){
            db.setHostName("127.0.0.1");
            db.setDatabaseName("fabaria");
            db.setUserName("fabaria");
            db.setPassword("fabaria");
        /*}
        else if(getConfigurazioneSql("DatabaseLocale").toString()== "server"){
            db.setHostName(getConfigurazioneSql("DatabaseServerIP").toString());
            db.setPort(getConfigurazioneSql("DatabaseServerPortaDB").toInt());
            db.setDatabaseName(getConfigurazioneSql("DatabaseServerNomeDB").toString());
            db.setUserName(getConfigurazioneSql("DatabaseServerUserDB").toString());
            db.setPassword(Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString()));
        }*/
     if(!db.open()){
         QMessageBox MsgBox;
         MsgBox.setText(qApp->translate("main","Errore di connessione al DB"));
         MsgBox.setInformativeText(qApp->translate("main","Controllare di aver installato MySql e di aver creato il DB fabaria"));
         MsgBox.setIcon(QMessageBox::Warning);
         MsgBox.exec();
         conn->exec();
         qDebug() << db.lastError().text();
     }
     else
     {     
        QString query = "SHOW TABLES LIKE 'fabarialog'";
        if(!db.exec(query).next()){
            aggiorna_db();
	        LOG_FILE(qApp->translate("main","Aggiornamento database"),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            //Visualizzo le impostazioni
            impostazioni(db);
            //Traduzione
            traduzione(db);
             MainWindow *w = new MainWindow();
             w->leggi_posizione();
             w->show();
             if(getConfigurazioneSql("Tutorial").toBool() == 0)
             {
                    addTutorial();
             }
            LOG_FILE(qApp->translate("main","Avvio FabariaGest"),LOG_INFO_ALL,QIODevice::Append);
	}
     }

     delete splash;
     
     return a.exec();
    }
    catch(FabariaException e)
    {
      qCritical() << qApp->translate("main","Software chiuso inaspettatamente") << e.what();
    }
}
