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
#include "pref.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QSettings>
#include "mainwindow.h"
#include <QtXml/QtXml>
#include <QtGui>
#include <QtSql>
#include <QSqlDatabase>
#include <QSysInfo>
#include "f_rest.h"
#include "articoli.h"
#include "tab_style.h"
#include "resizedialog.h"
#include "../initdb/connessione.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

pref::pref(QWidget *parent) :
    QDialog(parent), Ui::pref()
{
    setupUi(this);
    setModal(true);
    //Esco dalla finestra di dialogo
    connect(chiudi,SIGNAL(clicked()),this,SLOT(close()));
    chiudi->setShortcut(qApp->translate("pref","Ctrl"));
    connect(menu_d,SIGNAL(pressed()),this,SLOT(menu_ex()));
    //Applico le impostazioni
    connect(apply,SIGNAL(clicked()),this,SLOT(applica()));
    apply->setShortcut(qApp->translate("pref","Ctrl+P"));
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
    connect(en_update,SIGNAL(clicked(bool)),this,SLOT(agg_en()));
    connect(ds_update,SIGNAL(clicked(bool)),this,SLOT(agg_dis()));
    connect(sel_tema,SIGNAL(currentIndexChanged(QString)),this,SLOT(seleziona_tema(const QString &)));
    connect(update_b,SIGNAL(clicked()),this,SLOT(self_update()));
    connect(open_image,SIGNAL(clicked()),this,SLOT(apri()));
    connect(open_db,SIGNAL(clicked()),this,SLOT(connetti_database()));
    connect(save_dir,SIGNAL(clicked()),this,SLOT(salva_cartella()));
    connect(es_wizard,SIGNAL(clicked()),this,SLOT(clicca_per_eliminare()));
    connect(use_auth, SIGNAL(clicked(bool)),this,SLOT(visEnAuth(bool)));
    pwd->setEchoMode(QLineEdit::Password);
    pwd_smtp->setEchoMode(QLineEdit::Password);
    settings = new SettingsManager(this);
    imageScene = new ImageScene(this);
    graphicsView = new QGraphicsView(imageScene, groupBox_7);
    graphicsView->setMouseTracking(true);
    gridLayout_11->addWidget(graphicsView, 0, 0, 1, 1);
    graphicsView->setStyleSheet("background-color: transparent");
    tabWidget->tabBar()->setStyle(new Tab_style);
    tabWidget->setTabIcon(0,QIcon(":/images/preferences-system.png"));
    tabWidget->setTabIcon(1,QIcon(":/images/svn-commit.png"));
    tabWidget->setTabIcon(2,QIcon(":/images/mail.png"));
    tabWidget->setTabIcon(3,QIcon(":/images/cash2.png"));
    tabWidget->setTabIcon(4,QIcon(":/images/fattura.png"));
    tabWidget->setTabIcon(5,QIcon(":/images/system-software-update.png"));
    QStringList lista;
    lista << qApp->translate("pref","Default") <<  qApp->translate("pref","Moderno");
    sel_tema->addItems(lista);

    readsettings();
    visagg();
    vis_banca();
    vis_iva();
    visEnAuth(getPreferenzeSql("EmailServerAuthSmtp").toBool());
}

void pref::readsettings(){

    menu_d->setChecked(getPreferenzeSql("menu").toBool());

    en_update->setChecked(getPreferenzeSql("AggEn").toBool());
    ds_update->setChecked(getPreferenzeSql("AggDis").toBool());

    ip->setText(getConfigurazioneSql("DatabaseServerIP").toString());
    porta->setValue(getConfigurazioneSql("DatabaseServerPortaDB").toInt());
    db_rete->setText(getConfigurazioneSql("DatabaseServerNomeDB").toString());
    utente->setText(getConfigurazioneSql("DatabaseServerUserDB").toString());
    pwd->setText(Base64ToAscii(getConfigurazioneSql("DatabaseServerPWD").toString()));
    
    ser_smtp->setText(getPreferenzeSql("EmailServerSmtp").toString());
    porta_2->setCurrentText(getPreferenzeSql("EmailServerPortaSmtp").toString());
    prot_ssl->setChecked(getPreferenzeSql("EmailServerSSlSmtp").toBool());
    use_auth->setChecked(getPreferenzeSql("EmailServerAuthSmtp").toBool());
    user_smtp->setText(getPreferenzeSql("EmailServerUserAuthSmtp").toString());
    pwd_smtp->setText(Base64ToAscii(getPreferenzeSql("EmailServerPWDAuthSmtp").toString()));
    all_fatt->setChecked(getPreferenzeSql("EmailServerAllegatiFattura").toBool());
    email->setText(getPreferenzeSql("EmailServerMail").toString());

    dir_image->setText(getPreferenzeSql("ImageResizeDirImage").toString());
    images = image_resize(getPreferenzeSql("ImageResizeDirImage").toString());
    alt_larg = new QLabel("Larghezza logo "+QString::number(getPreferenzeSql("ImageResizeLarghezza").toInt())+" px\nAltezza logo     "+QString::number(getPreferenzeSql("ImageResizeAltezza").toInt())+" px",this);

    dir_save->setText(getPreferenzeSql("CartellaDestinazione").toString());
    sel_tema->setCurrentText(getPreferenzeSql("SelTema").toString());
    
    vis_banca_cmb->setCurrentText(getPreferenzeSql("ListaNomeBanca").toString());
    banca_imposta->setChecked(getPreferenzeSql("ListaBancaCheck").toBool());
    
    combo_iva->setCurrentText(getPreferenzeSql("ListaSelectIva").toString());
    checkBox_iva->setChecked(getPreferenzeSql("ListaIvaCheck").toBool());

    //FONT APPLICATION
    QFont fnt;
    fnt.fromString(getPreferenzeSql("ApplicationFont").toString());
    cmbFontName->setFont(fnt);
    spinFontSize->setValue(fnt.pixelSize());

    QFont appfnt;
    appfnt.fromString(getPreferenzeSql("ApplicationFont2").toString());
    cmbApplicationFontName->setFont(appfnt);
    spinApplicationFontSize->setValue(appfnt.pixelSize());

    //Lingua
    comboBoxLingua->setCurrentText(getPreferenzeSql("LinguaMadre").toString());

    QPixmap px = images.getPixmap();
    imagePixmapItem = imageScene->addPixmap(px);
    gridLayout_8->addWidget(alt_larg,1,0,1,1);
    alt_larg->show();
}

void pref::menu_ex(){
    if(menu_d->isChecked()){
       form->disable_menu();
    }
    else{
        form->enable_menu();
    }
}

void pref::agg_en(){
    if(en_update->isChecked() == true){
        update_b->setVisible(true);
    }
    else
        agg_dis();
}

void pref::agg_dis(){
    if(ds_update->isChecked() == true){
        update_b->setVisible(false);
    }
    else
        agg_en();
}

void pref::visagg(){

    if(getPreferenzeSql("AggEn").toBool() == true){
        update_b->setVisible(true);
    }
    else{
        update_b->setVisible(false);
    }

    if(getPreferenzeSql("AggDis").toBool() == true){
        update_b->setVisible(false);
    }
    else{
        update_b->setVisible(true);
    }

}

void pref::applica(){

    //FONT
    QFont selFont=cmbFontName->currentFont();
    selFont.setPixelSize(spinFontSize->value());

    //FONT APPLICATION
    QFont appfnt=cmbApplicationFontName->currentFont();
    appfnt.setPixelSize(spinApplicationFontSize->value());
    if(getPreferenzeSql("id").toString() == "1")
    {    
        QStringList m_list;
        m_list << QString("menu='")+QVariant(menu_d->isChecked()).toString()+QString("'") << QString(QString("ApplicationFont='")+selFont.toString()+QString("'")) << QString(QString("ApplicationFont2='")+appfnt.toString()+QString("'")) << QString("AggEn='")+QVariant(en_update->isChecked()).toString()+QString("'") <<
        QString("AggDis='")+QVariant(ds_update->isChecked()).toString()+QString("'") << QString("AutoEn='")+QVariant(en_update->isChecked()).toString()+QString("'") << QString("AutoDis='")+QVariant(ds_update->isChecked()).toString()+QString("'") << QString(QString("SelTema='")+sel_tema->currentText()+QString("'")) <<
        QString(QString("EmailServerSmtp='")+ser_smtp->text()+QString("'")) <<  QString(QString("EmailServerPortaSmtp='")+porta_2->currentText()+QString("'")) << QString("EmailServerSSlSmtp='")+QVariant(prot_ssl->isChecked()).toString()+QString("'") << QString("EmailServerAuthSmtp='")+QVariant(use_auth->isChecked()).toString()+QString("'") <<
        QString(QString("EmailServerUserAuthSmtp='")+user_smtp->text()+QString("'")) <<  QString(QString("EmailServerPwdAuthSmtp='")+pwd_smtp->text().toLatin1().toBase64()+QString("'")) << QString("EmailServerAllegatiFattura='")+QVariant(all_fatt->isChecked()).toString()+QString("'") << QString(QString("EmailServerMail='")+email->text()+QString("'")) <<
        QString(QString("CartellaDestinazione='")+dir_save->text()+QString("'")) << QString("ListaBancaCheck='")+QVariant(banca_imposta->isChecked()).toString()+QString("'") << QString(QString("ListaNomeBanca='")+vis_banca_cmb->currentText()+QString("'")) << QString(QString("ListaSelectIva='")+combo_iva->currentText()+QString("'")) <<
        QString("ListaIvaCheck='")+QVariant(checkBox_iva->isChecked()).toString()+QString("'") << QString(QString("LinguaMadre='")+comboBoxLingua->currentText()+QString("'"));
        QString upd = m_list.join(",");
        updatePreferenze(upd);
    }
    else
    {
        QStringList m_list;
        m_list << QString("'")+QVariant(menu_d->isChecked()).toString()+QString("'") << QString(QString("'")+selFont.toString()+QString("'")) << QString(QString("'")+appfnt.toString()+QString("'")) << QString("'")+QVariant(en_update->isChecked()).toString()+QString("'") <<
        QString("'")+QVariant(ds_update->isChecked()).toString()+QString("'") << QString("'")+QVariant(en_update->isChecked()).toString()+QString("'") << QString("'")+QVariant(ds_update->isChecked()).toString()+QString("'") << QString(QString("'")+sel_tema->currentText()+QString("'")) <<
        QString(QString("'")+ser_smtp->text()+QString("'")) <<  QString(QString("'")+porta_2->currentText()+QString("'")) << QString("'")+QVariant(prot_ssl->isChecked()).toString()+QString("'") << QString("'")+QVariant(use_auth->isChecked()).toString()+QString("'") <<
        QString(QString("'")+user_smtp->text()+QString("'")) <<  QString(QString("'")+pwd_smtp->text().toLatin1().toBase64()+QString("'")) << QString("'")+QVariant(all_fatt->isChecked()).toString()+QString("'") << QString(QString("'")+email->text()+QString("'")) <<
        QString(QString("'")+dir_save->text()+QString("'")) << QString("'")+QVariant(banca_imposta->isChecked()).toString()+QString("'") << QString(QString("'")+vis_banca_cmb->currentText()+QString("'")) << QString(QString("'")+combo_iva->currentText()+QString("'")) <<
        QString("'")+QVariant(checkBox_iva->isChecked()).toString()+QString("'") << QString(QString("'")+comboBoxLingua->currentText()+QString("'")) << QString("'")+QString("")+QString("'") << QString("'")+QString("")+QString("'") << QString("'")+QString("")+QString("'");
        QString ins = m_list.join(",");
        insertPreferenze(ins);
    }

    dir_server();
    readsettings();
    emit load_plugins();
}

void pref::apri(){

if(getPreferenzeSql("id").toString() == "1"){
    QString fileName = QFileDialog::getOpenFileName(this,
                                                        qApp->translate("pref","Open File"), QString(),
                                        qApp->translate("pref","Images (*.png *.xpm *.jpg *.bmp);;Tutti i file(*.*);;JPG(*.jpg);;PNG(*.png);;BMP(*.bmp);;XPM(*.xpm)"));
        if (!fileName.isEmpty()) {
            QImage image(fileName);
            if (image.isNull()) {
                QMessageBox::information(this, qApp->translate("pref","FabariaGest"),
                                         qApp->translate("pref","Impossibile aprire %1.").arg(fileName));
                return;
            }
            QApplication::setOverrideCursor(Qt::WaitCursor);
            imageScene->clear();
            images = image_resize(fileName);
            QPixmap pix = images.getPixmap().mask();
            imagePixmapItem = imageScene->addPixmap(pix);
            imageScene->setSceneRect(imagePixmapItem->boundingRect());

            dir_image->setText(fileName);
            QApplication::restoreOverrideCursor();
            label_file();

            QStringList m_up;
            m_up << QString("ImageResizeDirImage='")+QString(fileName)+QString("'") << QString("ImageResizeLarghezza='")+QString::number(images.getPixmap().width())+QString("'")
                 << QString("ImageResizeAltezza='")+QString::number(images.getPixmap().height())+QString("'");
            QString _up;
            _up = m_up.join(",");
            updatePreferenze(_up);
            
        }
        imageScene->clear();
        readsettings();
    }
    else
        QMessageBox::warning(this,"FabariaGest",qApp->translate("pref","Per aprire il logo per la fattura bisogna salvare i dati."));
}

void pref::save_file(){

      if(images.getPixmap().width() > 250 && images.getPixmap().height() >135)
      {
	   QMessageBox::critical(this,"FabariaGest",qApp->translate("pref","Impossibile salvare l'immagine.\n"
				    "Il formato deve essere 250x135")
      );
      }
      else{
	QString fileName = QFileDialog::getSaveFileName(this,
                                qApp->translate("pref","Salva le immagini"),
                                QString(),
                                qApp->translate("pref","Images (*.png);;Tutti i file(*.*);;PNG(*.png)"));
            if (fileName.length() != 0) {
                    // Aggiunge estensione alla fine del file se non c'è
                    if (fileName.indexOf(".png") < 0) {
                        fileName += ".png";
                    }
                }

            if (fileName.isEmpty())
                return;

            QImageWriter imageWriter(fileName);
            if (!imageWriter.canWrite()) {
                    QMessageBox::warning(this, qApp->translate("pref","FabariaGest"),
                                             qApp->translate("pref","Impossibile scrivere %1: %2")
                                             .arg(fileName)
                                             .arg(imageWriter.errorString()));
                }

                QApplication::setOverrideCursor(Qt::WaitCursor);
                imageWriter.write(imagePixmapItem->pixmap().toImage());
                images = image_resize(fileName);
                graphicsView->resetMatrix();
                QApplication::restoreOverrideCursor();
                updateDisplay();

                QStringList m_up;
                m_up << QString("ImageResizeDirImage='")+fileName+QString("'") << QString("ImageResizeLarghezza='")+QString::number(images.getPixmap().width())+QString("'")
                    << QString("ImageResizeAltezza='")+QString::number(images.getPixmap().height())+QString("'");
                QString _up;
                _up = m_up.join(",");
                updatePreferenze(_up);
		}
                imageScene->clear();
                readsettings();
}

void pref::label_file(){

        classe *cl = new classe(groupBox_5);
        cl->setText(qApp->translate("pref","Ridimensiona immagine"));
        cl->setCursor(QCursor(Qt::PointingHandCursor));
        cl->setAlignment(Qt::AlignCenter);
        cl->show();
        gridLayout_8->addWidget(cl, 0, 0, 1, 1);
        int altezza = images.getPixmap().height();
        int larghezza = images.getPixmap().width();
        alt_larg->setText(qApp->translate("pref","Larghezza logo ")+QString::number(larghezza)+qApp->translate("pref"," px\nAltezza logo     ")+QString::number(altezza)+" px");
        alt_larg->show();
        gridLayout_8->addWidget(alt_larg,1,0,1,1);
        QPushButton *save = new QPushButton();
        save->setText(qApp->translate("pref","Salva immagine"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/document-save.png"));
        save->setIcon(icon1);
        gridLayout_8->addWidget(save,1,2,1,1);
        QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        gridLayout_8->addItem(spacer,1,1,1,1);
        connect(save,SIGNAL(clicked()),this,SLOT(save_file()));
        connect(cl,SIGNAL(clicked()),this,SLOT(showResizeDialog()));
}

void pref::resizeSwitch(QSize size, int filter){

    QApplication::setOverrideCursor(Qt::WaitCursor);
       QString fType;
       switch(filter)
       {
       case AVERAGE:
           images = images.resizeAverage(size.width(), size.height());
           images.getPixmap().mask();
           fType = qApp->translate("pref","Media");
           break;
       case BILINEAR:
           images = images.resizeBilinear(size.width(), size.height());
           images.getPixmap().mask();
           fType = qApp->translate("pref","Bilineare");
           break;
       }

       QApplication::restoreOverrideCursor();

       updateDisplay();
}

void pref::showResizeDialog(){

    ResizeDialog *diag = new ResizeDialog(this, imagePixmapItem->pixmap().size());
        diag->setModal(true);

        connect(diag, SIGNAL(resizeImage(QSize,int)), this, SLOT(resizeSwitch(QSize, int)));

        diag->exec();
}

void pref::updateDisplay(){

    QPixmap img(images.getPixmap());
    imagePixmapItem->setPixmap(img);
}

QGraphicsItem *pref::getPixmapItem(){

    return imagePixmapItem;
}

void pref::self_update(){

#if defined(Q_OS_LINUX) && defined(__x86_64__)
    QUrl url("http://www.codelinsoft.it/package/fabaria/fabaria-lin64.xml");
    manager->get(QNetworkRequest(QUrl(url)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_LINUX) && defined(__x86)
    QUrl url("http://www.codelinsoft.it/package/fabaria/fabaria-lin32.xml");
    manager->get(QNetworkRequest(QUrl(url)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_WIN64)
    QUrl url("http://www.codelinsoft.it/package/fabaria/fabaria-win64.xml");
    manager->get(QNetworkRequest(QUrl(url)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_WIN32)
    QUrl url("http://www.codelinsoft.it/package/fabaria/fabaria-win32.xml");
    manager->get(QNetworkRequest(QUrl(url)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_MACX)
    QUrl url("http://www.codelinsoft.it/package/fabaria/fabaria-macx.xml");
    manager->get(QNetworkRequest(QUrl(url)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_FREEBSD)
    QUrl url("http://www.codelinsoft.it/package/fabaria/fabaria-unix.xml");
    manager->get(QNetworkRequest(QUrl(url)));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#endif
}

void pref::self_update_parse(QNetworkReply* reply){

            // Check file type
            QDomDocument doc( "XML" );
            if (!doc.setContent(reply->readAll(), true)) {
            return;
            }

            QDomElement root = doc.documentElement();
            if( root.tagName() != "appname" ){
                    return;
            }

            // Loop over main nodes
            latest_version.clear();
            filelist.clear();
            QDomNode mainnode = root.firstChild();
            while( !mainnode.isNull() ){
                    QDomNode subnode = mainnode.firstChild();
            if( !subnode.isNull() ) {
                // Loop over each elements in subnodes
                while(! subnode.isNull()){
                    QDomElement e = subnode.toElement();
                    if(!e.isNull()){
                        // Start parsing, finally
                                            if( e.tagName() == "version" ){
                                                    latest_version = e.attribute("value","");

                                            }
                                            else if( e.tagName() == "file" ){
                                                    filelist.push_back(e.attribute("value",""));
                                            }
                                    }
                                    subnode = subnode.nextSibling();
                            }
                    }
                    mainnode = mainnode.nextSibling();
            }

            // Compare current version with the one on the server
            if( getConfigurazioneSql("VersioneFabariaGest").toString() < latest_version){
                    std::string htmlText =
                            "<html>"+
                            qApp->translate("pref","<b>E' disponibile la nuova versione:</b> ").toStdString()+ latest_version.toStdString() + "<br><br>"
                            "<br><br>";
                    // Add the list of files to download list
                 
                    QString txtt= "http://www.codelinsoft.it/package/";
                    QMessageBox *box= new QMessageBox(this);
		    box->setWindowTitle(qApp->translate("pref","FabariaGest"));
		    box->setText(qApp->translate("pref","Aggiornamento"));
		    box->setInformativeText(qApp->translate("pref","E' disponibile la nuova versione ")+versione+ qApp->translate("pref",",se vuoi aggiornare clicca per aggiornare.\n"
                                        "Se clicchi ok si chiude il programma e si aggiorna il software alla nuova versione"));
		    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		    box->setDefaultButton(QMessageBox::Ok);
		    int ret = box->exec();
		    switch(ret){
                      case QMessageBox::Ok:
                       //Open update
                       up_dw(QString::fromLatin1(filelist.data()->toStdString().c_str()),txtt);
                       box->close();
                       exit(0);
                       break;
                      case QMessageBox::Cancel:
                       //Close
                       box->close();
                       break;
                    }
		

            }
            else if(reply->error()){
                QMessageBox::warning(this,"FabariaGest",qApp->translate("pref","Errore...\n").arg(reply->errorString()));
            }
            else{
                    // Update success dialog
                    QSystemTrayIcon::MessageIcon is1 = QSystemTrayIcon::Information;
                    form->trayIcon->showMessage(qApp->translate("pref",""),qApp->translate("pref","Stai usando la nuova versione"),is1,15*10000);
            }
}


void pref::up_dw(QString package, QString url){
    process = new QProcess(this);
#if defined(Q_OS_LINUX)
    process->start("./update -u "+url+" -p "+package);
#elif defined(Q_OS_WIN)
    process->start("update.exe -u "+url+" -p "+package);
#elif defined(Q_OS_MACX)
    process->start("./update.app/Contents/MacOS/update -u "+url+" -p "+package);
#elif defined (Q_OS_FREEBSD)
    process->start("./update -u "+url+" -p "+package);
#endif
}


bool pref::dir_server(){
    QString sgf = qApp->translate("pref","Devi riavviare per rendere effettive le modifiche....");
    info->setText(sgf);
    return sgf.toStdString().c_str();
}

void pref::connetti_database()
{
  if(getConfigurazioneSql("id").toString() == "1"){
    if(ip->text().length() == 0 || db_rete->text().length() == 0 || utente->text().length() == 0 || pwd->text().length() == 0){
        QMessageBox MsgBox2;
        MsgBox2.setText(qApp->translate("pref","Errore di connessione al DB"));
        MsgBox2.setInformativeText(qApp->translate("pref","Verificare che i dati siano corretti"));
        MsgBox2.setWindowTitle("FabariaGest");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
        MsgBox2.setWindowIcon(icon);
        MsgBox2.setIcon(QMessageBox::Warning);
        MsgBox2.exec();
        ip->setStyleSheet("background-color: rgb(249, 22, 5)");
        db_rete->setStyleSheet("background-color: rgb(249, 22, 5)");
        utente->setStyleSheet("background-color: rgb(249, 22, 5)");
        pwd->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(ip->text());
    db.setPort(porta->value());
    db.setDatabaseName(db_rete->text());
    db.setUserName(utente->text());
    db.setPassword(pwd->text());

    QStringList m_up;
    m_up << QString("DatabaseServerIP='")+ip->text()+QString("'") << QString("DatabaseServerPortaDB='")+QString::number(porta->value())+QString("'") <<
            QString("'DatabaseServerNomeDB=")+db_rete->text()+QString("'") << QString("DatabaseServerUserDB='")+utente->text()+QString("'") <<
            QString("DatabaseServerPWD='")+pwd->text().toLatin1().toBase64()+QString("'") << QString("DatabaseLocale=server='")+QString("'");
    QString _up;
    _up = m_up.join(",");
    updateConfigurazione(_up);

    if (db.open())
    {
        QSqlQuery query1;
        query1.exec("GRANT ALL ON fabaria.* TO 'fabaria'@'"+ip->text()+"' IDENTIFIED BY 'fabaria'");
        messaggio();
    }
    else{
        QMessageBox MsgBox2;
        MsgBox2.setText(qApp->translate("pref","Errore di connessione al DB"));
        MsgBox2.setInformativeText(qApp->translate("pref","Impossibile connettersi al db.Controllare le impostazioni."));
        MsgBox2.setWindowTitle("FabariaGest");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
        MsgBox2.setWindowIcon(icon);
        MsgBox2.setIcon(QMessageBox::Warning);
        MsgBox2.exec();
    }
    }
    }
    else
        QMessageBox::warning(this,"FabariaGest",qApp->translate("pref","Devi prima configurare FabariaGest."));
}

void pref::messaggio()
{
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("FabariaGest");
    box->setText(qApp->translate("pref","Database"));
    box->setInformativeText(qApp->translate("pref","Creazione db strutturata con successo...."));
    box->setStandardButtons(QMessageBox::Ok);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
    case QMessageBox::Ok:
         //Close
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
        QProcess *proc = new QProcess();
        proc->start("./FabariaGest");
        #endif
        #ifdef Q_OS_WIN
        QProcess *proc = new QProcess();
        proc->start("FabariaGest.exe");
        #endif
        #ifdef Q_OS_MAC
        QProcess *proc = new QProcess();
        proc->start("/Applications/FabariaGest/FabariaGest.app");
        #endif
         box->close();
         exit(0);
         break;
    }
}

void pref::seleziona_tema(const QString &sheetName)
{
    QFile file(":/qss/" + sheetName.toLower() + ".qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    qApp->setStyleSheet(styleSheet);
    qApp->setStyle(styleSheet);
}

void pref::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void pref::clicca_per_eliminare()
{
    QMessageBox *box= new QMessageBox;
        box->setWindowTitle("FabariaGest");
        box->setText(qApp->translate("pref","Configurazione"));
        box->setInformativeText(qApp->translate("pref","Attenzione cliccando su ok cancelli i dati relativi\nalla configurazione del database "
                                "e non potrai tornare indietro...\n"
                                "Sei sicuro di voler procedere?"));
        box->setIcon(QMessageBox::Warning);
        box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box->setDefaultButton(QMessageBox::Ok);
        int ret = box->exec();
        switch(ret){
        case QMessageBox::Ok:
             //cancella riga
             cancella_riga();
             box->close();
             break;
        case QMessageBox::Cancel:
             //Close
             box->close();
             break;
        }
}

/*
 * Cancello righe dal file di configurazione
 */
void pref::cancella_riga()
{	
    if(ip->text().length() == 0 || db_rete->text().length() == 0 || utente->text().length() == 0 || pwd->text().length() == 0){
        QMessageBox MsgBox2;
        MsgBox2.setText(tr("Errore di connessione al DB"));
        MsgBox2.setInformativeText(tr("Verificare che i dati siano corretti"));
        MsgBox2.setWindowTitle("Fabaria");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/logo1.png"), QSize(), QIcon::Normal, QIcon::Off);
        MsgBox2.setWindowIcon(icon);
        MsgBox2.setIcon(QMessageBox::Warning);
        MsgBox2.exec();
        ip->setStyleSheet("background-color: rgb(249, 22, 5)");
        db_rete->setStyleSheet("background-color: rgb(249, 22, 5)");
        utente->setStyleSheet("background-color: rgb(249, 22, 5)");
        pwd->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else{
    db = QSqlDatabase::database("QMYSQL");
    db.setHostName(ip->text());
    db.setPort(porta->value());
    db.setDatabaseName(db_rete->text());
    db.setUserName(utente->text());
    db.setPassword(pwd->text());

    QStringList m_list;
    m_list << QString("DatabaseServerIP='"+ip->text()+"'") <<
              QString("DatabaseServerPortaDB='"+QString::number(porta->value())+"'") <<
              QString("DatabaseServerNomeDB='"+db_rete->text()+"'") <<
              QString("DatabaseServerUserDB='"+utente->text()+"'") <<
              QString("DatabaseServerPWD='"+pwd->text().toLatin1().toBase64()+"'") <<
              QString("DatabaseLocale='server'");
    QString m_clist = m_list.join(",");
    insertConfigurazione(m_clist);

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
void pref::salva_cartella()
{
    QString cartella = QFileDialog::getExistingDirectory(this,qApp->translate("pref","Seleziona cartella"),QDir::currentPath());
    if(!cartella.isNull())
    {
      dir_save->setText(cartella);
    }
    else{
      dir_save->setText(QDir::currentPath());
    }
}

void pref::vis_banca()
{
    vis_banca_cmb->clear();
    QStringList list;
    QSqlQuery query("select * from banca");
    if(query.exec()){
    while(query.next()){
        list << query.value(2).toString();
    }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(vis_banca_cmb->model());
    vis_banca_cmb->setCompleter(complete);
    vis_banca_cmb->addItems(list);
}

void pref::vis_iva()
{
    QStringList list;
    QSqlQuery query("select iva from aliquota");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString();
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(combo_iva->model());
    combo_iva->setCompleter(complete);
    combo_iva->addItems(list);
    }
}

void pref::visEnAuth(bool vero)
{
    if(vero == false)
    {
        user_smtp->setEnabled(false);
        pwd_smtp->setEnabled(false);
        porta_2->setCurrentIndex(0);
    }
    else if(vero == true){
        user_smtp->setEnabled(true);
        pwd_smtp->setEnabled(true);
        porta_2->setCurrentIndex(1);
    }
}
