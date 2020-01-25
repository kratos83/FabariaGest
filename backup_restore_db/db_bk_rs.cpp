#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
#include "db_bk_rs.h"
#include "ui_db_bk_rs.h"
#include <QProcess>
#include <QLineEdit>
#include <QtGui>
#include <QTextStream>
#include <QtSql>
#include "../fabaria/databasesql.h"
#include "../fabaria/defines.h"

using namespace Defines;
using namespace DatabaseSql;

db_bk_rs::db_bk_rs(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(bak_but,SIGNAL(clicked()),this,SLOT(crea_backup()));
    connect(ripr_but,SIGNAL(clicked()),this,SLOT(restore_db()));
    connect(chiudi,SIGNAL(clicked()),this,SLOT(close()));
}

db_bk_rs::~db_bk_rs()
{
}

void db_bk_rs::crea_backup()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), "*.sql", tr("Database(*.sql);;Tutti i file(*.*)"));

    // Aggiunge estensione alla fine del file se non c'è
    if (fileName.indexOf(".sql") < 0) {
        fileName += ".sql";
    }


    if(!fileName.isNull()) {
        QString comando;
        QStringList args;
        if(getConfigurazioneSql("DatabaseLocale").toString() == "locale") {
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString())+"> "+fileName);
#endif
#ifdef Q_OS_MAC
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString())+"> "+fileName);
#endif
#ifdef Q_OS_WIN
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysqldump.exe --database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString())+"> "+fileName);
#endif
        }
        else if(getConfigurazioneSql("DatabaseLocale").toString() == "server") {
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString())+" --host="+getConfigurazioneSql("DatabaseServerIP").toString()+"> "+fileName);
#endif
#ifdef Q_OS_MAC
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString())+" --host="+getConfigurazioneSql("DatabaseServerIP").toString()+"> "+fileName);
#endif
#ifdef Q_OS_WIN
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysqldump.exe --database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString())+" --host="+getConfigurazioneSql("DatabaseServerIP").toString()+"> "+fileName);
#endif
        }
        args << comando;
        backup = new QProcess(this);
        backup->start(comando,args);

        QMessageBox MsgBox;
        MsgBox.setText(tr("Backup database..."));
        MsgBox.setInformativeText(tr("Backup database effettuato correttamente..."));
        MsgBox.setIcon(QMessageBox::Information);
        qDebug() << tr("Backup database effettuato correttamente");
        MsgBox.exec();
        this->close();
    }
    else {
        QMessageBox MsgBox;
        MsgBox.setText(tr("Backup database..."));
        MsgBox.setInformativeText(tr("Backup database non effettuato correttamente..."));
        MsgBox.setIcon(QMessageBox::Information);
        MsgBox.exec();
        this->close();
    }


}

void db_bk_rs::restore_db()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), QString(), tr("Database(*.sql);;Tutti i file(*.*)"));
    fileName = QDir::toNativeSeparators(fileName);

    if (!fileName.isNull()) {
        QString comando;
        if(getConfigurazioneSql("DatabaseLocale").toString() == "locale") {
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString())+" < "+fileName);
#endif
#ifdef Q_OS_MAC
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString())+" < "+fileName);
#endif
#ifdef Q_OS_WIN
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysql.exe --one-database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseLocalePWDDB").toString())+" < "+fileName);
#endif
        }
        else if(getConfigurazioneSql("DatabaseLocale").toString() == "server") {
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString())+" --host="+getConfigurazioneSql("DatabaseServerIP").toString()+" < "+fileName);
#endif
#ifdef Q_OS_MAC
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString())+" --host="+getConfigurazioneSql("DatabaseServerIP").toString()+" < "+fileName);
#endif
#ifdef Q_OS_WIN
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysql.exe --one-database fabaria -u root -p"+Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString())+" --host="+getConfigurazioneSql("DatabaseServerIP").toString()+" < "+fileName);
#endif
        }
        QStringList args;
        args << comando;
        backup = new QProcess(this);
        backup->start(comando,args);
        QMessageBox MsgBox;
        MsgBox.setText(tr("Ripristino database..."));
        MsgBox.setInformativeText(tr("Ripristino del database effettuato correttamente..."));
        MsgBox.setIcon(QMessageBox::Information);
        MsgBox.exec();
        this->close();
    }
    else
    {
        QMessageBox MsgBox;
        MsgBox.setText(tr("Ripristino database..."));
        MsgBox.setInformativeText(tr("Impossibile ripristinare il database..."));
        MsgBox.setIcon(QMessageBox::Information);
        qDebug("Impossibile ripristinare il database...");
        MsgBox.exec();
        this->close();
    }

}