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

db_bk_rs::db_bk_rs(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(bak_but,SIGNAL(clicked()),this,SLOT(crea_backup()));
    connect(ripr_but,SIGNAL(clicked()),this,SLOT(restore_db()));
    connect(chiudi,SIGNAL(clicked()),this,SLOT(esci()));
#ifdef Q_OS_WIN
    settingsDir = new QDir(QDir::homePath()+"/fabaria/");
#else
    settingsDir = new QDir(QDir::homePath()+"/.fabaria/");
#endif

    general = new QSettings(settingsDir->path()+"/config.conf",QSettings::IniFormat);
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


    if(!fileName.isNull()){
        QString comando;
        QStringList args;
        if(general->value("Database/databaselocale").toString() == "locale"){
        #if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(general->value("Database/password").toString())+"> "+fileName);
        #endif
        #ifdef Q_OS_MAC
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(general->value("Database/password").toString())+"> "+fileName);
        #endif
        #ifdef Q_OS_WIN
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysqldump.exe --database fabaria -u root -p"+Base64ToAscii(general->value("Database/password").toString())+"> "+fileName);
        #endif
        }
        else if(general->value("Database/databaselocale").toString() == "server"){
        #if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(general->value("DatabaseServer/passdb").toString())+" --host="+general->value("DatabaseServer/indirizzoip").toString()+"> "+fileName);
        #endif
        #ifdef Q_OS_MAC
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --database fabaria -u root -p"+Base64ToAscii(general->value("DatabaseServer/passdb").toString())+" --host="+general->value("DatabaseServer/indirizzoip").toString()+"> "+fileName);
        #endif
        #ifdef Q_OS_WIN
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysqldump.exe --database fabaria -u root -p"+Base64ToAscii(general->value("DatabaseServer/passdb").toString())+" --host="+general->value("DatabaseServer/indirizzoip").toString()+"> "+fileName);
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
    else{
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

    if (!fileName.isNull()){
        QString comando;
        if(general->value("Database/databaselocale").toString() == "locale"){
        #if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
            comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(general->value("Database/password").toString())+" < "+fileName);
        #endif
        #ifdef Q_OS_MAC
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(general->value("Database/password").toString())+" < "+fileName);
        #endif
        #ifdef Q_OS_WIN
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysql.exe --one-database fabaria -u root -p"+Base64ToAscii(general->value("Database/password").toString())+" < "+fileName);
        #endif
        }
        else if(general->value("Database/databaselocale").toString() == "server"){
        #if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(general->value("DatabaseServer/passdb").toString())+" --host="+general->value("DatabaseServer/indirizzoip").toString()+" < "+fileName);
        #endif
        #ifdef Q_OS_MAC
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(general->value("DatabaseServer/passdb").toString())+" --host="+general->value("DatabaseServer/indirizzoip").toString()+" < "+fileName);
        #endif
        #ifdef Q_OS_WIN
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"\\mysql.exe --one-database fabaria -u root -p"+Base64ToAscii(general->value("DatabaseServer/passdb").toString())+" --host="+general->value("DatabaseServer/indirizzoip").toString()+" < "+fileName);
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

QString db_bk_rs::getLineFromCommandOutput(QString command){
    FILE *file = popen(command.toLatin1(),"r");

                char buffer[100];
                QString line = "";
                char firstChar;

                if ((firstChar = fgetc(file)) != -1){
                        line += firstChar;
                        line += fgets(buffer,100,file);
                }

                pclose(file);


                return line;
}

QString db_bk_rs::Base64ToAscii(QString String){

    QByteArray ByteArray1=String.toUtf8();

    const char* chArr1=ByteArray1.constData();

    QByteArray ByteArray2=QByteArray::fromBase64(chArr1);

    const char* chArr2=ByteArray2.constData();

    return QString::fromUtf8(chArr2);
}

QString db_bk_rs::GetRunCommandMySql()
{
    QSqlDatabase db = QSqlDatabase::database("QMYSQL");
    QString cartella_db="";
    QString sep = QDir::separator();
    QSqlQuery query("SHOW VARIABLES WHERE Variable_Name LIKE 'basedir'");
    query.exec();
    if(query.next()){
#if defined(Q_OS_LINUX)
    cartella_db = query.value(1).toString()+"/bin";
#elif defined(Q_OS_WIN)
    cartella_db = query.value(1).toString()+"\\bin";
#elif defined(Q_OS_MACX)
    cartella_db = query.value(1).toString()+"/bin";
#elif defined(Q_OS_FREEBSD)
    cartella_db = query.value(1).toString()+"/bin";
#endif
    }
    else{
    }
    return cartella_db;
}

void db_bk_rs::esci()
{
    exit(0);
}
