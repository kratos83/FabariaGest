/****************************************************************************
**
** Copyright (C) 2006-2018 Angelo e Calogero Scarna
** Contact: Angelo Scarnà (angelo.scarna@codelinsoft.it)
**          Calogero Scarnà (calogero.scarna@codelinsoft.it)
**          Team Codelinsoft (info@codelinsoft.it)
**
** This file is part of the project FabariaGest
**
** LGPLv3 License
**
**  You may use this file under the terms of the LGPLv3 license as follows:
*
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Codelinsoft and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/
#include <QDesktopServices>
#include <QDebug>
#include "DropboxFabaria.h"
#include "ui_DropboxFabaria.h"
#include "defines.h"
#include <iostream>
//Client dropbox
#include "dropbox/endpoint/DropboxAppInfo.h"
#include "dropbox/endpoint/DropboxWebAuth.h"
#include "dropbox/DropboxClient.h"
#include "dropbox/files/FilesRoutes.h"
#include "dropbox/files/FilesWriteMode.h"

//Chiavi pubbliche
QString consumer_key = "9ia55bxr6x0sswb";
QString consumer_secret = "byj1bygz4np11f2";

using namespace std;
using namespace Defines;
using namespace dropboxQt;

DropboxFabaria::DropboxFabaria(QWidget* parent) : 
	QDialog(parent),
	ui(new Ui::DropboxFabaria)
{
    ui->setupUi(this);
    m_manager = new SettingsManager(this);
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(esciDropBox()));
    connect(ui->connetti,SIGNAL(clicked()),this,SLOT(connetti()));
    connect(ui->carico_file,SIGNAL(clicked()),this,SLOT(upload_file()));
    connect(ui->scarico_file,SIGNAL(clicked()),this,SLOT(download_file()));
    connect(ui->up_fatture,SIGNAL(clicked()),this,SLOT(up_fatture()));
    connect(ui->save_fatture,SIGNAL(clicked()),this,SLOT(dw_fatture()));
}

void DropboxFabaria::connetti()
{    
    DropboxAppInfo appInfo;
   appInfo.setKeySecret(consumer_key,consumer_secret);
    
   QString url = DropboxWebAuth::getCodeAuthorizeUrl(appInfo);
   QDesktopServices::openUrl(QUrl(QString(url)));
	
   bool ok;
    QString text = QInputDialog::getText(this, qApp->translate("DropboxFabaria","Inserisci codice"),
                                         qApp->translate("DropboxFabaria","Inserisci il codice\nriportato sul browser:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
    {
         try
        {
            QString token = "token.info";
            DropboxAuthInfo authInfo = DropboxWebAuth::getTokenFromCode(appInfo, QString::fromStdString(text.toStdString()));
            if(!authInfo.storeToFile(m_manager->settingsDir->path()+"/"+token)){
                std::cout << qApp->translate("DropboxFabaria","Errore, impossibile scrivere il token sul file: ").toStdString() << std::endl;
            }
            std::cout << qApp->translate("DropboxFabaria","Perfetto, hai scritto il token").toStdString() <<  std::endl;
        }
        catch(DropboxException& e)
        {
            std::cout << qApp->translate("DropboxFabaria","Oops, è successo qualcosa di brutto: ").toStdString() << e.what() << std::endl;
        }
    }
}

void DropboxFabaria::upload_file()
{
   ui->label_up_dw->clear();
    ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Caricamento in corso del database..."));
  QSqlQuery query;
  query.exec("select nome_azienda,partita_iva,cod_fisc from azienda where id=1");
  QString comando;
  QStringList args;
  if(query.next()){
    QString nome_azienda = query.value(0).toString().mid(0,10).replace(" ","_");
    QString partita_iva = query.value(1).toString();
    QString codice_fiscale = query.value(2).toString();
    //Creo il dump del database
    #if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
    comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --databases fabaria -u root -p"+Base64ToAscii(m_manager->generalValue("Database/password").toString())+"> "+
      cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql");
    #endif
    #ifdef Q_OS_MAC
    comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysqldump --databases fabaria -u root -p"+Base64ToAscii(m_manager->generalValue("Database/password").toString())+"> "+
      cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql");
    #endif
    #ifdef Q_OS_WIN
    comando=getLineFromCommandOutput("mysqldump.exe --databases fabaria -u root -p"+Base64ToAscii(m_manager->generalValue("Database/password").toString())+"> "+
      cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql");
    #endif
  args << comando;
  //Eseguo i comandi
  backup = new QProcess(this);
  backup->start(comando,args);
      
  QString file = cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql";
  QString all = "/fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql";
  QString argAuthFile =  m_manager->settingsDir->path()+"/token.info";
    DropboxAuthInfo authInfo;
    if(!authInfo.readFromFile(argAuthFile)){
        std::cout << qApp->translate("DropboxFabaria","Errore lettura accesso token").toStdString() << std::endl;    
    }

    QFile file_in(file);
    if(!file_in.open(QFile::ReadOnly)){
        std::cout << qApp->translate("DropboxFabaria","Errore apertura file: ").toStdString() << file.toStdString() << std::endl;
    }

    DropboxClient c(authInfo.getAccessToken());
    QObject::connect(&c, &DropboxClient::uploadProgress,this, &DropboxFabaria::progressUpload);
    downloadTime.start();
    try
        {
            files::CommitInfo arg(all);
            arg.setMode(files::WriteMode:: WriteMode_OVERWRITE);
            std::unique_ptr<files::Metadata> res = c.getFiles()->upload(arg, &file_in);
            if(res)
                ui->label_up_dw->setText(qApp->translate("DropboxFabaria","File caricato con successo..."));
            else
                ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Impossibile caricare il file..."));
        }
    catch(DropboxException& e)
        {
            std::cout << qApp->translate("DropboxFabaria","Oops, è successo qualcosa di brutto: ").toStdString() << e.what() << std::endl;
        }

    file_in.close();
   QFile::remove(file);
  }
}

void DropboxFabaria::download_file()
{
  ui->label_up_dw->clear();
  ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Scaricamento in corso del database..."));
  
  //Scarico il file
  QSqlQuery query;
  query.exec("select nome_azienda,partita_iva,cod_fisc from azienda where id=1");
  QString comando;
  QStringList args;
  if(query.next()){
    QString nome_azienda = query.value(0).toString().mid(0,10).replace(" ","_");
    QString partita_iva = query.value(1).toString();
    QString codice_fiscale = query.value(2).toString();
    QString file = cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql";
    QString all = "/fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql";
    QString authFile = m_manager->settingsDir->path()+"/token.info";
    DropboxAuthInfo authInfo;
    // Read auth info file.
    if(!authInfo.readFromFile(authFile)){
        std::cout << qApp->translate("DropboxFabaria","Errore lettura accesso token").toStdString() << std::endl;
    }

    QFile out(file);
    if(!out.open(QFile::WriteOnly|QIODevice::Truncate)){
        std::cout << qApp->translate("DropboxFabaria","Errore apertura file: ").toStdString() << file.toStdString() << std::endl;
    }
    
    DropboxClient c(authInfo.getAccessToken());
    QObject::connect(&c, &DropboxClient::downloadProgress,this, &DropboxFabaria::progressDownload);
    downloadTime.start();
    try
        {
            files::DownloadArg arg(all);
            std::unique_ptr<files::Metadata> res = c.getFiles()->download(arg, &out);
            if(res)
            {
                ui->label_up_dw->update();
                ui->label_up_dw->setText(qApp->translate("DropboxFabaria","File scaricato con successo...\nDatabase inizializzato con successo..."));
            }
            else
                ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Impossibile scaricare il file..."));
        }
    catch(DropboxException& e)
        {
            std::cout << qApp->translate("DropboxFabaria","Oops, è successo qualcosa di brutto: ").toStdString() << e.what() << std::endl;
        }
    out.close();
        #if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(m_manager->generalValue("Database/password").toString())+"< "+
        cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql");
        #endif
        #ifdef Q_OS_MAC
        comando=getLineFromCommandOutput(GetRunCommandMySql()+"/mysql --one-database fabaria -u root -p"+Base64ToAscii(m_manager->generalValue("Database/password").toString())+"< "+
        cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql");
        #endif
        #ifdef Q_OS_WIN
        comando=getLineFromCommandOutput("mysql.exe --one-database fabaria -u root -p"+Base64ToAscii(m_manager->generalValue("Database/password").toString())+"< "+
        cartellaDoc()+"fabariadump_"+nome_azienda+"_"+partita_iva+"_"+codice_fiscale+".sql");
        #endif
        args << comando;
        restore = new QProcess(this);
        restore->start(comando,args);
        QFile::remove(file);
   }
}

void DropboxFabaria::up_fatture()
{
         ui->label_up_dw->clear();
         ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Caricamento fatture in corso..."));
        QMessageBox *box= new QMessageBox(this);
        box->setWindowTitle("FabariaGest");
        box->setText(qApp->translate("DropboxFabaria","Crea file zip fatture"));
        box->setInformativeText(qApp->translate("DropboxFabaria","Crea un archivio in formato zip e inserisci all'interno le fatture.\nIl file deve risiedere all'interno della cartella di\nsalvataggio delle fatture di FabariaGest.\n L'archivio deve chiamarsi fatture.zip"));
        box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box->setDefaultButton(QMessageBox::Ok);
        int ret = box->exec();
        switch(ret){
        case QMessageBox::Ok:{
            QString file = m_manager->generalValue("Cartella/destinazione",QVariant()).toString()+"/fatture.zip";
            QString all = "/fatture.zip";
            QString argAuthFile =  m_manager->settingsDir->path()+"/token.info";
            DropboxAuthInfo authInfo;
            if(!authInfo.readFromFile(argAuthFile)){
                std::cout << qApp->translate("DropboxFabaria","Errore lettura accesso token").toStdString() << std::endl;    
            }

            QFile file_in(file);
            if(!file_in.open(QFile::ReadOnly)){
                std::cout << qApp->translate("DropboxFabaria","Errore apertura file: ").toStdString() << file.toStdString() << std::endl;
            }

            DropboxClient c(authInfo.getAccessToken());
            QObject::connect(&c, &DropboxClient::uploadProgress,this, &DropboxFabaria::progressUpload);
            downloadTime.start();
            try
                {
                    files::CommitInfo arg(all);
                    arg.setMode(files::WriteMode:: WriteMode_OVERWRITE);
                    std::unique_ptr<files::Metadata> res = c.getFiles()->upload(arg, &file_in);
                    if(res){
                        ui->label_up_dw->setText(qApp->translate("DropboxFabaria","File caricato con successo..."));
                    }
                    else
                        ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Impossibile caricare il file..."));
                }
            catch(DropboxException& e)
                {
                    std::cout << qApp->translate("DropboxFabaria","Oops, è successo qualcosa di brutto: ").toStdString() << e.what() << std::endl;
                }

            file_in.close();
            QFile::remove(file);
           box->close();
            break;
        }
        case QMessageBox::Cancel:
            box->close();
            break;
        }
}

void DropboxFabaria::dw_fatture()
{
  ui->label_up_dw->clear();
  ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Scaricamento in corso delle fatture..."));
  
  //Scarico il file
  QString file = m_manager->generalValue("Cartella/destinazione",QVariant()).toString()+"/fatture.zip";
    QString all = "/fatture.zip";
    QString authFile = m_manager->settingsDir->path()+"/token.info";
    DropboxAuthInfo authInfo;
    // Read auth info file.
    if(!authInfo.readFromFile(authFile)){
        std::cout << qApp->translate("DropboxFabaria","Errore lettura accesso token").toStdString() << std::endl;
    }

    QFile out(file);
    if(!out.open(QFile::WriteOnly|QIODevice::Truncate)){
        std::cout << qApp->translate("DropboxFabaria","Errore apertura file: ").toStdString() << file.toStdString() << std::endl;
    }
    
    DropboxClient c(authInfo.getAccessToken());
    QObject::connect(&c, &DropboxClient::downloadProgress,this, &DropboxFabaria::progressDownload);
    downloadTime.start();
    try
        {
            files::DownloadArg arg(all);
            std::unique_ptr<files::Metadata> res = c.getFiles()->download(arg, &out);
            if(res)
            {
                ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Fatture scaricate con successo......"));
                ui->test_agg->setHtml(qApp->translate("DropboxFabaria","L'archivio fatture.zip si trova nella seguente cartella:<p><b>")+file+"</b></p>");
            }
            else
                ui->label_up_dw->setText(qApp->translate("DropboxFabaria","Impossibile scaricare il file..."));
        }
    catch(DropboxException& e)
        {
            std::cout << qApp->translate("DropboxFabaria","Oops, è successo qualcosa di brutto: ").toStdString() << e.what() << std::endl;
        }
    out.close();
}

void DropboxFabaria::progressDownload(qint64 bytesProcessed, qint64 total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(bytesProcessed);

        // calcolo la velocità di download
        double speed = bytesProcessed * 1000.0 / downloadTime.elapsed();
        QString unit;
        if (speed < 1024) {
            unit = "bytes/sec";
        } else if (speed < 1024*1024) {
            speed /= 1024;
            unit = "kB/s";
        } else {
            speed /= 1024*1024;
            unit = "MB/s";
        }

        //Calcolo la dimensione del file da scaricare
        double peso = bytesProcessed;

        QString dt;
        if (peso < 1024) {
            dt = "bytes";
        } else if (peso < 1024*1024) {
            peso /= 1024;
            dt = "kB";
        } else {
            peso /= 1024*1024;
            dt = "MB";
        }

        int secs = downloadTime.elapsed()/1000;
        int mins = (secs/60)%60;
        int hours = (secs/3600);
        secs = secs%60;

        QString ts = QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit);
        QString ts1 = QString::fromLatin1("%1 %2").arg(peso,3,'f',1).arg(dt);
        ui->test_agg->setPlainText(QString::fromUtf8(qApp->translate("DropboxFabaria","Velocità di scaricamento: ").toStdString().c_str())+ts+qApp->translate("DropboxFabaria","  Dimensione: ")+ts1+qApp->translate("DropboxFabaria"," Tempo stimato: ")+QString("%1:%2:%3").arg(hours,2,10, QLatin1Char('0')).arg(mins,2,10, QLatin1Char('0')).arg(secs,2,10, QLatin1Char('0')));
        ui->progressBar->update();
}

void DropboxFabaria::progressUpload(qint64 bytesProcessed, qint64 total)
{
      ui->progressBar->setMaximum(total);
     ui->progressBar->setValue(bytesProcessed);
        // calcolo la velocità di download
        double speed = bytesProcessed * 1000.0 / downloadTime.elapsed();
        QString unit;
        if (speed < 1024) {
            unit = "bytes/sec";
        } else if (speed < 1024*1024) {
            speed /= 1024;
            unit = "kB/s";
        } else {
            speed /= 1024*1024;
            unit = "MB/s";
        }

        //Calcolo la dimensione del file da scaricare
        double peso = bytesProcessed;

        QString dt;
        if (peso < 1024) {
            dt = "bytes";
        } else if (peso < 1024*1024) {
            peso /= 1024;
            dt = "kB";
        } else {
            peso /= 1024*1024;
            dt = "MB";
        }

        int secs = downloadTime.elapsed()/1000;
        int mins = (secs/60)%60;
        int hours = (secs/3600);
        secs = secs%60;

        QString ts = QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit);
        QString ts1 = QString::fromLatin1("%1 %2").arg(peso,3,'f',1).arg(dt);
        ui->test_agg->setPlainText(QString::fromUtf8(qApp->translate("DropboxFabaria","Velocità di caricamento: ").toStdString().c_str())+ts+qApp->translate("DropboxFabaria","  Dimensione: ")+ts1+qApp->translate("DropboxFabaria"," Tempo stimato: ")+QString("%1:%2:%3").arg(hours,2,10, QLatin1Char('0')).arg(mins,2,10, QLatin1Char('0')).arg(secs,2,10, QLatin1Char('0')));
        ui->progressBar->update();
}

void DropboxFabaria::esciDropBox()
{
    close();
}

DropboxFabaria::~DropboxFabaria()
{
    delete ui;
}
