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
/*********************************
  Classi FabariaGest
 ********************************/
#include "ui_mainwindow.h"
#include "DropboxFabaria.h"
#include "mainwindow.h"
#include "classe.h"
#include "about.h"
#include "user.h"
#include "articoli.h"
#include "esci.h"
#include "pluginmanager.h"
#include "luxuryim.h"
#include "button.h"
#include "minitutorial.h"
#include "../initdb/connessione.h"

/************************************
  Classi QT
 ************************************/
#include <QDateTime>
#include <QtCore>
#include <QCloseEvent>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QtSql/QtSql>
#include <QToolTip>
#include <QMessageBox>
#include <QDate>
#include <QSettings>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QApplication>
#include <QProcess>

/*******************************
  Classi c++
 ******************************/
#include <fstream>
#include <iostream>
#include "defines.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

MainWindow *form = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    if(form)
        delete form;
    form = this;

    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QString st = "";
    titolo(st);
    interfaccia();
    read();
    core();
    orologio();
    createStatusBar();
    gestioneplugin();
    load_style(getPreferenzeSql("SelTema").toString());

}
void MainWindow::createActions(){

    minimizeAction = new QAction(qApp->translate("MainWindow","Mi&nimiza"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(qApp->translate("MainWindow","Ma&ssimiza"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(qApp->translate("MainWindow","&Ripristina"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(qApp->translate("MainWindow","&Esci"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(onclose()));

    ag_tray = new QAction(qApp->translate("MainWindow","Aggiorna"),this);

    impo = new QAction(qApp->translate("MainWindow","Impostazioni"),this);
    connect(impo,SIGNAL(triggered()),this,SLOT(settings()));
}

void MainWindow::createTrayIcon(){
    trayMenu = new QMenu(this);
    trayMenu->addAction(minimizeAction);
    trayMenu->addAction(maximizeAction);
    trayMenu->addAction(restoreAction);
    trayMenu->addSeparator();
    trayMenu->addAction(impo);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/images/icons32x32.png")), QIcon::Normal, QIcon::Off);

    trayIcon = new QSystemTrayIcon(icon,this);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

}

void MainWindow::messageClicked(QSystemTrayIcon::ActivationReason reason){

    switch(reason){
    case QSystemTrayIcon::Trigger:
        hide();
        break;
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    default:
        ;
    }
}

void MainWindow::setVisible(bool visible){
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::interfaccia(){

    pluginDir = new QDir( QCoreApplication::applicationDirPath()+"/plugin/" );
    loader = new QPluginLoader;

    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->dumpObjectTree();
    setCentralWidget(mdiArea);

    cl1 = new classe(this);
    cl2 = new classe(this);
    cl3 = new classe(this);
    cl4 = new classe(this);
    cl5 = new classe(this);

    dockWidget->setObjectName("Top");
    dockWidget->setFloating(false);
    dockWidget->setTitleBarWidget(new QWidget);
    dock = new QDockWidget(this);
    dock->setObjectName("Top");
    dock->setFloating(false);
    dock->setTitleBarWidget(new QWidget);
    dock->setWidget(tabWidget);
    addDockWidget(Qt::TopDockWidgetArea, dock);

    connect(actionAbout_2,SIGNAL(triggered()),this,SLOT(onactionAbout()));
    actionAbout_2->setIconVisibleInMenu(true);
    actionAbout_2->setShortcut(qApp->translate("MainWindow","Ctrl+H"));
    connect(close_ac,SIGNAL(triggered()),this,SLOT(onclose()));
    close_ac->setIconVisibleInMenu(true);
    close_ac->setShortcut(qApp->translate("MainWindow","Ctrl+ESC"));
    connect(action_articoli,SIGNAL(triggered()),this,SLOT(onarticoli()));
    action_articoli->setIconVisibleInMenu(true);
    action_articoli->setShortcut(qApp->translate("MainWindow","Ctrl+A"));
    connect(clienti,SIGNAL(triggered()),this,SLOT(onclienti()));
    clienti->setIconVisibleInMenu(true);
    clienti->setShortcut(qApp->translate("MainWindow","Ctrl+G"));
    actionCaterorie_libri->setIconVisibleInMenu(true);
    connect(actionAbout_Qt,SIGNAL(triggered()),this,SLOT(onactiondatabasetriggered()));
    actionAbout_Qt->setIconVisibleInMenu(true);
    connect(actionPreferenze,SIGNAL(triggered()),this,SLOT(settings()));
    actionPreferenze->setIconVisibleInMenu(true);
    connect(preferenze_2,SIGNAL(clicked()),this,SLOT(settings()));
    connect(about_luxury_2,SIGNAL(clicked()),this,SLOT(onactionAbout()));
    connect(actionCaterorie_libri,SIGNAL(triggered()),this,SLOT(categorie()));;
    connect(about_qt_2,SIGNAL(clicked()),this,SLOT(onactiondatabasetriggered()));
    connect(categoria_2,SIGNAL(clicked()),this,SLOT(categorie()));
    connect(cl2_2,SIGNAL(clicked()),this,SLOT(gest_fatt_acq()));
    connect(scadenze_2,SIGNAL(clicked()),this,SLOT(gest_fattura_v()));
    connect(clienti_3,SIGNAL(clicked()),this,SLOT(onclienti()));
    connect(libri_2,SIGNAL(clicked()),this,SLOT(onarticoli()));
    connect(actionCalcola_codicefiscale,SIGNAL(triggered()),this,SLOT(gest_codfisc()));
    connect(actionAnagrafica_pagamenti,SIGNAL(triggered()),this,SLOT(gest_pagamento()));
    connect(actionCausali_di_trasporto,SIGNAL(triggered()),this,SLOT(gest_causali()));
    connect(actionAnagrafica_iva,SIGNAL(triggered()),this,SLOT(gest_iva()));
    connect(actionAnagrafica_azienda,SIGNAL(triggered()),this,SLOT(azienda_cod()));
    connect(actionAnagrafica_banche,SIGNAL(triggered()),this,SLOT(gest_banca()));
    connect(actionFattura_d_aquisto,SIGNAL(triggered()),this,SLOT(gest_fatt_acq()));
    connect(actionAnagraficafornitori,SIGNAL(triggered()),this,SLOT(gest_fornitori()));
    connect(actionFattura_di_vendita_2,SIGNAL(triggered()),this,SLOT(gest_fattura_v()));
    connect(actionCarico_magazzino,SIGNAL(triggered()),this,SLOT(vis_carico_magazzino()));
    connect(actionScarico_magazzino,SIGNAL(triggered()),this,SLOT(vis_scarico_magazzino()));
    connect(cod_fiscale,SIGNAL(clicked()),this,SLOT(gest_codfisc()));
    connect(actionInstalla_plugin,SIGNAL(triggered()),this,SLOT(installa_plugin()));
    connect(actionGestione_misure,SIGNAL(triggered()),this,SLOT(gest_misure()));
    connect(actionGestione_scadenze,SIGNAL(triggered()),this,SLOT(gest_scadenze()));
    connect(actionRiba_da_emettere,SIGNAL(triggered()),this,SLOT(gest_dist_riba()));
    connect(actionChiudi_anno_contabile,SIGNAL(triggered()),this,SLOT(gest_anno_cont()));
    connect(actionSeleziona_anno_esercizio,SIGNAL(triggered()),this,SLOT(gest_sel_anno()));
    connect(actionGestione_preventivi,SIGNAL(triggered()),this,SLOT(gest_preventivi()));
    connect(actionAnagrafica_listini,SIGNAL(triggered()),this,SLOT(gest_listini()));
    connect(actionGestione_ordini,SIGNAL(triggered()),this,SLOT(gest_ordini()));
    connect(actionGestione_DDT,SIGNAL(triggered()),this,SLOT(gest_ddt()));
    connect(esci_fabaria,SIGNAL(clicked()),this,SLOT(onclose()));
    connect(ana_forn,SIGNAL(clicked()),this,SLOT(gest_fornitori()));
    connect(ana_pagam,SIGNAL(clicked()),this,SLOT(gest_pagamento()));
    connect(ana_iva,SIGNAL(clicked()),this,SLOT(gest_iva()));
    connect(ana_causali,SIGNAL(clicked()),this,SLOT(gest_causali()));
    connect(ana_banche,SIGNAL(clicked()),this,SLOT(gest_banca()));
    connect(ana_azienda,SIGNAL(clicked()),this,SLOT(azienda_cod()));
    connect(ana_listini,SIGNAL(clicked()),this,SLOT(gest_listini()));
    connect(scadenze_gest,SIGNAL(clicked()),this,SLOT(gest_scadenze()));
    connect(misura_gest,SIGNAL(clicked()),this,SLOT(gest_misure()));
    connect(ch_ann_cont,SIGNAL(clicked()),this,SLOT(gest_anno_cont()));
    connect(sel_anno_es,SIGNAL(clicked()),this,SLOT(gest_sel_anno()));
    connect(preventivi_gest,SIGNAL(clicked()),this,SLOT(gest_preventivi()));
    connect(ordine_gest,SIGNAL(clicked()),this,SLOT(gest_ordini()));
    connect(doc_trasporto,SIGNAL(clicked()),this,SLOT(gest_ddt()));
    connect(caric_maga,SIGNAL(clicked()),this,SLOT(vis_carico_magazzino()));
    connect(scar_maga,SIGNAL(clicked()),this,SLOT(vis_scarico_magazzino()));
    connect(riba_em,SIGNAL(clicked()),this,SLOT(gest_dist_riba()));
    connect(install_plugin,SIGNAL(clicked()),this,SLOT(installa_plugin()));
    connect(actionAggiorna_database,SIGNAL(triggered()),this,SLOT(aggiorna_database()));
    connect(agg_database,SIGNAL(clicked()),this,SLOT(aggiorna_database()));
    connect(actionVisualizza_storico_email,SIGNAL(triggered()),this,SLOT(vis_storico_mail()));
    connect(storico_mail,SIGNAL(clicked()),this,SLOT(vis_storico_mail()));
    connect(actionRicerca_aggiornamento,SIGNAL(triggered()),this,SLOT(gest_update()));
    connect(agg_button,SIGNAL(clicked()),this,SLOT(gest_update()));
    connect(vis_statistiche,SIGNAL(clicked()),this,SLOT(gest_esistenza()));
    connect(actionVisualizza_statitiche,SIGNAL(triggered()),this,SLOT(gest_esistenza()));
    connect(actionRegistro_iva,SIGNAL(triggered()),this,SLOT(gest_registro_iva()));
    connect(registro_iva,SIGNAL(clicked()),this,SLOT(gest_registro_iva()));
    connect(actionAnagrafica_agenti,SIGNAL(triggered()),this,SLOT(gest_agenti()));
    connect(button_agenti,SIGNAL(clicked()),this,SLOT(gest_agenti()));
    connect(actionSituazione_agenti_2,SIGNAL(triggered()),this,SLOT(gest_riep_agenti()));
    connect(sit_agent,SIGNAL(clicked()),this,SLOT(gest_riep_agenti()));
    connect(actionBackup_e_ripristino_con_DropBox,SIGNAL(triggered()),this,SLOT(gest_dropbox()));
    connect(add_dropbox,SIGNAL(clicked()),this,SLOT(gest_dropbox()));
    connect(actionSeleziona_azienda,SIGNAL(triggered()),this,SLOT(selAzienda()));
    connect(sel_azienda,SIGNAL(clicked()),this,SLOT(selAzienda()));
    connect(crea_magazzino,SIGNAL(clicked()),this,SLOT(createMagazzino()));
    connect(actionCrea_magazzino,SIGNAL(triggered()),this,SLOT(createMagazzino()));
    connect(sel_magazzino,SIGNAL(clicked()),this,SLOT(setMagazzino()));
    connect(actionSeleziona_magazzino,SIGNAL(triggered()),this,SLOT(setMagazzino()));
    onwid();
    azienda_ok();
    reload_data();
    reload_lista_cmq();
    reload_data_anno();
    reload_data_mese_prec();
    reload_data_mese_succ();
    createActions();
    createTrayIcon();
    conf_link();
    animationIn(tabWidget);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(messageClicked(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::animationIn(QWidget* wid)
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    wid->setGraphicsEffect(eff);
    QPropertyAnimation *animation = new QPropertyAnimation(eff, "opacity");
    animation->setDuration(5000);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutBack);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    if(animation->endValue() == 1.0)
        wid->setVisible(true);
}

void MainWindow::conf_link()
{	
     label_donate = new classe(groupBox);
     label_donate->setObjectName(QString::fromUtf8("label_donate"));
     label_donate->setMaximumSize(QSize(16777215, 80));
     label_donate->setCursor(QCursor(Qt::PointingHandCursor));
     label_donate->setPixmap(QPixmap(QString::fromUtf8(":/images/paypal.png")));
     label_donate->setAlignment(Qt::AlignCenter);

     gridLayout_4->addWidget(label_donate, 5, 1, 3, 1);
     
     connect(label_donate,SIGNAL(clicked()),this,SLOT(open_link()));
}

void MainWindow::open_link()
{
      QString apri_link = "http://www.codelinsoft.it/sito/2015-02-18-23-59-25/donate.html";
      QDesktopServices::openUrl(QUrl(apri_link));
}

void MainWindow::orologio(){
    cl = new AnalogClock(dockWidget);
    cl->show();
}

void MainWindow::onwid(){

    QMdiSubWindow *s = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    s->setWidget(groupBox);

    mdiArea->addSubWindow(s);

    groupBox->showMaximized();
}

void MainWindow::reload_all()
{
      reload_data();
      reload_data_anno();
      reload_data_mese_prec();
      reload_data_mese_succ();
      reload_lista_cmq();
}

void MainWindow::reload_data(){
    QString str,str2, st1,st;
    QSqlQuery query;
    connect(cl1,SIGNAL(clicked()),this,SLOT(gest_scadenze()));
    cl1->clear();
    query.exec("select count(id),DATEDIFF(data_scadenza,CURDATE()),tipo_doc,num_doc from scadenze where flag_sc='N'");
      while(query.next()){

        if(query.value(1).toInt() == 0){
            cl1->clear();
            st.clear();
            str = query.value(0).toString();
            st = qApp->translate("MainWindow","Vi sono ")+str+ qApp->translate("MainWindow"," scadenze");
            cl1->setText(st);
        }
        else if(query.value(1).toString().mid(0, 1) == "-")
        {
            cl1->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Sono passati ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl1->setText(st1);
        }
        else{
            cl1->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Mancano ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl1->setText(st1);
            }
    }
    cl1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font("Arial",14,75);
    font.setUnderline(true);
    cl1->setFont(font);
    cl1->setCursor(QCursor(Qt::PointingHandCursor));
    cl1->show();
    gridLayout_15->addWidget(cl1,6, 0, 1, 8);
}

void MainWindow::reload_data_mese_prec()
{
    QString str,str2, st1,st;
    QSqlQuery query;
    connect(cl2,SIGNAL(clicked()),this,SLOT(gest_scadenze_1()));
    cl2->clear();
    query.exec("select count(id),DATEDIFF(data_scadenza,CURDATE()),tipo_doc,num_doc from scadenze where MONTH(data_scadenza)=month(now())-1 and flag_sc='N' and doc_anno='"+getConfigurazioneSql("EsercizioConfigurazione").toString()+"' order by id");
      while(query.next()){

        if(query.value(1).toInt() == 0){
            cl2->clear();
            st.clear();
            str = query.value(0).toString();
            st = qApp->translate("MainWindow","Vi sono ")+str+ qApp->translate("MainWindow"," scadenze");
            cl2->setText(st);
        }
        else if(query.value(1).toString().mid(0, 1) == "-")
        {
            cl1->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Sono passati ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl1->setText(st1);
        }
        else{
            cl2->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Mancano ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl2->setText(st1);
            }
    }
    cl2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font("Arial",14,75);
    font.setUnderline(true);
    cl2->setFont(font);
    cl2->setCursor(QCursor(Qt::PointingHandCursor));
    cl2->show();
    gridLayout_10->addWidget(cl2);
}

void MainWindow::reload_data_mese_succ()
{
    QString str,str2, st1,st;
    QSqlQuery query;
    connect(cl3,SIGNAL(clicked()),this,SLOT(gest_scadenze_2()));
    cl3->clear();
    query.exec("select count(id),DATEDIFF(data_scadenza,CURDATE()),tipo_doc,num_doc from scadenze where MONTH(data_scadenza)=month(now())+1 and flag_sc='N' and doc_anno='"+getConfigurazioneSql("EsercizioConfigurazione").toString()+"' order by id");
      while(query.next()){

        if(query.value(1).toInt() == 0){
            cl3->clear();
            st.clear();
            str = query.value(0).toString();
            st = qApp->translate("MainWindow","Vi sono ")+str+ qApp->translate("MainWindow"," scadenze");
            cl3->setText(st);
        }
        else if(query.value(1).toString().mid(0, 1) == "-")
        {
            cl1->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Sono passati ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl1->setText(st1);
        }
        else{
            cl3->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Mancano ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl3->setText(st1);
            }
    }
    cl3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font("Arial",14,75);
    font.setUnderline(true);
    cl3->setFont(font);
    cl3->setCursor(QCursor(Qt::PointingHandCursor));
    cl3->show();
    gridLayout_12->addWidget(cl3);
}

void MainWindow::reload_data_anno()
{
    QString str,str2, st1,st;
    QSqlQuery query;
    connect(cl4,SIGNAL(clicked()),this,SLOT(gest_scadenze_3()));
    cl4->clear();
    query.exec("select count(id),DATEDIFF(data_scadenza,CURDATE()),tipo_doc,num_doc from scadenze where YEAR(data_scadenza)=YEAR(CURDATE()) and flag_sc='N' and doc_anno='"+getConfigurazioneSql("EsercizioConfigurazione").toString()+"' order by id");
      while(query.next()){

        if(query.value(1).toInt() == 0){
            cl4->clear();
            st.clear();
            str = query.value(0).toString();
            st = qApp->translate("MainWindow","Vi sono ")+str+ qApp->translate("MainWindow"," scadenze");
            cl4->setText(st);
        }
        else if(query.value(1).toString().mid(0, 1) == "-")
        {
            cl1->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Sono passati ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl1->setText(st1);
        }
        else{
            cl4->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Mancano ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl4->setText(st1);
            }
    }
    cl4->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font("Arial",14,75);
    font.setUnderline(true);
    cl4->setFont(font);
    cl4->setCursor(QCursor(Qt::PointingHandCursor));
    cl4->show();
    gridLayout_17->addWidget(cl4);
}

void MainWindow::reload_lista_cmq()
{
    QString str,str2, st1,st;
    QSqlQuery query;
    connect(cl5,SIGNAL(clicked()),this,SLOT(gest_scadenze_4()));
    cl5->clear();
    query.exec("select count(id),tipo_doc,num_doc from scadenze where doc_anno='"+getConfigurazioneSql("EsercizioConfigurazione").toString()+"' order by id");
      while(query.next()){

        if(query.value(1).toInt() == 0){
            cl5->clear();
            st.clear();
            str = query.value(0).toString();
            st = qApp->translate("MainWindow","Vi sono ")+str+ qApp->translate("MainWindow"," scadenze");
            cl5->setText(st);
        }
        else if(query.value(1).toString().mid(0, 1) == "-")
        {
            cl1->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Sono passati ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl1->setText(st1);
        }
        else{
            cl5->setText("");
            st1.clear();
            str2 = query.value(1).toString();
            st1 = qApp->translate("MainWindow","Mancano ")+str2+qApp->translate("MainWindow"," giorni alla scadenza della ")+query.value(2).toString()+QString::fromUtf8(" \u00B0 ")+query.value(3).toString();
            cl5->setText(st1);
            }
    }
    cl5->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font("Arial",14,75);
    font.setUnderline(true);
    cl5->setFont(font);
    cl5->setCursor(QCursor(Qt::PointingHandCursor));
    cl5->show();
    gridLayout_8->addWidget(cl5);
}

void MainWindow::gest_scadenze()
{
    sc_fatt = new scadenze(this);
    sc_fatt->setWindowModality(Qt::WindowModal);

    if(cl1->isChecked())
    {
        sc_fatt->checkBox->setChecked(false);
        sc_fatt->list_r->setChecked(true);
        sc_fatt->lista();
    }
    QMdiSubWindow *ws_az = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_az->setWidget(sc_fatt);
    mdiArea->addSubWindow(ws_az);
    connect(sc_fatt,SIGNAL(destroyed()),ws_az,SLOT(deleteLater()));
    sc_fatt->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione scadenze...."));

}

void MainWindow::gest_scadenze_1(){
    sc_fatt = new scadenze(this);
    sc_fatt->setWindowModality(Qt::WindowModal);

    if(cl2->isChecked())
    {
        sc_fatt->scad_m_p->setChecked(true);
        sc_fatt->list_r->setChecked(false);
    }

    QMdiSubWindow *ws_az = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_az->setWidget(sc_fatt);
    mdiArea->addSubWindow(ws_az);
    connect(sc_fatt,SIGNAL(destroyed()),ws_az,SLOT(deleteLater()));
    sc_fatt->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione scadenze...."));
}

void MainWindow::gest_scadenze_2(){

    sc_fatt = new scadenze(this);
    sc_fatt->setWindowModality(Qt::WindowModal);

    if(cl3->isChecked())
    {
        sc_fatt->scad_m_s->setChecked(true);
        sc_fatt->scad_m_p->setChecked(false);
        sc_fatt->list_r->setChecked(false);
    }

    QMdiSubWindow *ws_az = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_az->setWidget(sc_fatt);
    mdiArea->addSubWindow(ws_az);
    connect(sc_fatt,SIGNAL(destroyed()),ws_az,SLOT(deleteLater()));
    sc_fatt->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione scadenze...."));
}

void MainWindow::gest_scadenze_3(){

    sc_fatt = new scadenze(this);
    sc_fatt->setWindowModality(Qt::WindowModal);

    if(cl2->isChecked())
    {
        sc_fatt->scad_a_c->setChecked(true);
        sc_fatt->scad_m_s->setChecked(false);
        sc_fatt->list_r->setChecked(false);
    }

    QMdiSubWindow *ws_az = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_az->setWidget(sc_fatt);
    mdiArea->addSubWindow(ws_az);
    connect(sc_fatt,SIGNAL(destroyed()),ws_az,SLOT(deleteLater()));
    sc_fatt->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione scadenze...."));
}

void MainWindow::gest_scadenze_4(){

    sc_fatt = new scadenze(this);
    sc_fatt->setWindowModality(Qt::WindowModal);

    if(cl5->isChecked())
    {
        sc_fatt->checkBox->setChecked(true);
        sc_fatt->list_r->setChecked(false);
        sc_fatt->lista_comp();
    }
    QMdiSubWindow *ws_az = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_az->setWidget(sc_fatt);
    mdiArea->addSubWindow(ws_az);
    connect(sc_fatt,SIGNAL(destroyed()),ws_az,SLOT(deleteLater()));
    sc_fatt->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione scadenze...."));
}

void MainWindow::gestioneplugin(){
    new pluginmanager(this);

    connect(this,SIGNAL(pluginLoaded(const QString &,bool)),manager,SLOT(pluginLoaded(const QString &,bool)));
    connect(manager,SIGNAL(pluginLoad(QObject*,QTreeWidgetItem*)),this,SLOT(pluginLoad(QObject*,QTreeWidgetItem*)));
    connect(manager,SIGNAL(pluginUnload(QObject*,QTreeWidgetItem*)),this,SLOT(pluginUnload(QObject*,QTreeWidgetItem*)));

    connect(gest_plugin,SIGNAL(clicked()),manager,SLOT(exec()));
    connect(actionGestione_plugin,SIGNAL(triggered()),manager,SLOT(exec()));

    manager->leggiplugin();
}

void MainWindow::pluginLoad(QObject *plugin, QTreeWidgetItem *item){

    bool loaded = true;
    
    if (loaded && qobject_cast<Interface_plugin*>(plugin))
        loaded = loadImPlugin( plugin );

    if(loaded){
        item->setText( 2, qApp->translate("MainWindow","Avviato...") );
        QString ic = ":/images/dialog-ok-apply.png";
        QIcon icona(ic);
        item->setIcon(2,icona);

        emit pluginLoaded( plugin->metaObject()->className(), true );
    } else {
       pluginUnload( plugin, item );
    }
    qDebug() << "True plugin" << plugin->metaObject()->className();
}

void MainWindow::pluginUnload(QObject *plugin, QTreeWidgetItem *item){

    if( qobject_cast<Interface_plugin*>(plugin) )
        unloadImPlugin( plugin );

    item->setText( 2, qApp->translate("MainWindow","Disattivato...") );
    QString ic = ":/images/list-remove.png";
    QIcon icona(ic);
    item->setIcon(2,icona);
    item->setSelected(false);
    qDebug() << "False plugin" << plugin->metaObject()->className();
    emit pluginLoaded( plugin->metaObject()->className(), false );
    delete plugin;
}

bool MainWindow::unloadImPlugin(QObject *pl){
    Interface_plugin *in = qobject_cast<Interface_plugin*>(pl);
    //Disabilito plugin
    removePopulateMenus(pl);
    in->pluginUnload();
    return false;
}

bool MainWindow::loadImPlugin(QObject *plug){
    Interface_plugin *inter = qobject_cast<Interface_plugin*>(plug);
    //Avvio plugin
    populateMenus(plug);
    inter->pluginLoad();
    return true;
}

void MainWindow::populateMenus(QObject *plugin){
    Interface_plugin *in = qobject_cast<Interface_plugin *>(plugin);

    if(in->name_menu() == qApp->translate("MainWindow","Strumenti")){
        addToButton(plugin,in->icona(),in->nameplugin(),strumenti,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuStrumenti_2,SLOT(avvia_widget()));
    }
    else if(in->name_menu() == qApp->translate("MainWindow","Anagrafica")){
        addToButton(plugin,in->icona(),in->nameplugin(),file,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuAnagrafica,SLOT(avvia_widget()));
    }
    else if(in->name_menu() == qApp->translate("MainWindow","Gestione")){
        addToButton(plugin,in->icona(),in->nameplugin(),documenti,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuDocumenti,SLOT(avvia_widget()));
    }
    else if(in->name_menu() == qApp->translate("MainWindow","Plugin")){
        addToButton(plugin,in->icona(),in->nameplugin(),plugin_tab,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuPlugin_2,SLOT(avvia_widget()));
    }
}

void MainWindow::addToMenu(QObject *plugin, QIcon ico, const QString &name, QMenu *menu_add, const char *member){

        QAction *action = new QAction(ico,name, plugin);
        connect(action, SIGNAL(triggered()), this, member);
             menu_add->addAction(action);
             menu_add->addSeparator();
}

void MainWindow::addToButton(QObject *plugin, QIcon ico, const QString &name, QWidget *wid_add, const char *member){

	Interface_plugin *in = qobject_cast<Interface_plugin *>(plugin);
	button *button_wid = new button(wid_add,plugin,ico,name);
	if(in->name_menu() == qApp->translate("MainWindow","Plugin"))
	{
	    horizontalLayout->addWidget(button_wid);
	    connect(button_wid,SIGNAL(clicked(QObject*)),this,member);
	}
	else if(in->name_menu() == qApp->translate("MainWindow","Strumenti"))
	{	
	    gridLayout_5->addWidget(button_wid);
	    connect(button_wid,SIGNAL(clicked(QObject*)),this,member);
	}
	else if(in->name_menu() == qApp->translate("MainWindow","Anagrafica"))
	{
	    gridLayout_23->addWidget(button_wid);
	    connect(button_wid,SIGNAL(clicked(QObject*)),this,member);
	}
	else if(in->name_menu() == qApp->translate("MainWindow","Gestione"))
	{
	    gridLayout_24->addWidget(button_wid);
	    connect(button_wid,SIGNAL(clicked(QObject*)),this,member);
	}
}

void MainWindow::removePopulateMenus(QObject *plugin)
{
    Interface_plugin *in = qobject_cast<Interface_plugin *>(plugin);
    if(in->name_menu() == qApp->translate("MainWindow","Strumenti")){
        removeToButton(plugin,in->icona(),in->nameplugin(),strumenti,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuStrumenti_2,SLOT(avvia_widget()));
    }
    else if(in->name_menu() == qApp->translate("MainWindow","Anagrafica")){
        removeToButton(plugin,in->icona(),in->nameplugin(),file,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuAnagrafica,SLOT(avvia_widget()));
    }
    else if(in->name_menu() == qApp->translate("MainWindow","Plugin")){
        removeToButton(plugin,in->icona(),in->nameplugin(),plugin_tab,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuPlugin_2,SLOT(avvia_widget()));
    }
    else if(in->name_menu() == qApp->translate("MainWindow","Gestione")){
        removeToButton(plugin,in->icona(),in->nameplugin(),documenti,SLOT(avvia_button_widget(QObject*)));
        addToMenu(plugin,in->icona(),in->nameplugin(),menuDocumenti,SLOT(avvia_widget()));
    }
}

void MainWindow::removeToButton(QObject *plugin, QIcon ico, const QString &name, QWidget *wid_add, const char *member)
{
    button *button_wid = new button(wid_add,plugin,ico,name);
    disconnect(button_wid,SIGNAL(clicked(QObject*)),this,member);
    QLayoutItem *item = horizontalLayout->takeAt(manager->treeWidget->currentIndex().row());
    if(item != 0)
    {
        delete item->widget();
        delete item;
    }
}

void MainWindow::avvia_button_widget(QObject *plugin)
{
    button *action = qobject_cast<button *>(sender());
    Interface_plugin *iBrush = qobject_cast<Interface_plugin *>(plugin);
    QString brush = action->text();
    brush = iBrush->nameplugin();

    iBrush->creaWidget()->show();
}

void MainWindow::avvia_widget(){

    QAction *action = qobject_cast<QAction *>(sender());
    Interface_plugin *iBrush = qobject_cast<Interface_plugin *>(action->parent());
    QString brush = action->text();
    brush = iBrush->nameplugin();
    iBrush->creaWidget()->show();
}

void MainWindow::avvia_backup(){
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
    QString fileName = "libbackup_restore_db.so";
    loader->setFileName( pluginDir->absoluteFilePath(fileName) );
    QObject *plugin=loader->instance();
    if(plugin){
        Interface_plugin *plif=qobject_cast<Interface_plugin*>( plugin );
        if(plif)
            plif->creaWidget()->show();
    }
#elif defined (Q_OS_WIN)
    QString fileName = "backup_restore_db.dll";
    loader->setFileName( pluginDir->absoluteFilePath(fileName) );
    QObject *plugin=loader->instance();
    if(plugin){
        Interface_plugin *plif=qobject_cast<Interface_plugin*>( plugin );
        if(plif)
            plif->creaWidget()->show();
    }
#elif defined (Q_OS_MACX)
    QString fileName = "backup_restore_db.dylib";
    loader->setFileName( pluginDir->absoluteFilePath(fileName) );
    QObject *plugin=loader->instance();
    if(plugin){
        Interface_plugin *plif=qobject_cast<Interface_plugin*>( plugin );
        if(plif)
            plif->creaWidget()->show();
    }
#endif
}

void MainWindow::core(){
    if(getConfigurazioneSql("BackupDate").toDate().addDays(14) < QDate::currentDate() ){
            QMessageBox *box= new QMessageBox(this);
            box->setWindowTitle("FabariaGest");
            box->setText("Backup");
            box->setInformativeText(qApp->translate("MainWindow","Sono passati 15 giorni senza eseguire un backup.\nVuoi eseguirlo adesso?"));
            box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box->setDefaultButton(QMessageBox::Ok);
            int ret = box->exec();
            switch(ret){
            case QMessageBox::Ok:
                 //Open backup
                 avvia_backup();
                 updateConfigurazione("BackupDate='"+QDate::currentDate().toString("yyyy-MM-dd")+"'");
                 box->close();
                 break;
            case QMessageBox::Cancel:
                 //Close
                 updateConfigurazione("BackupDate='"+QDate::currentDate().toString("yyyy-MM-dd")+"'");
                 box->close();
                 break;
            }
          }
}

void MainWindow::avvio_licenza(){

        lic = new license(this);
        lic->read_conf();
        lic->exec();
}

void MainWindow::azienda_ok(){
    QSqlQuery query;
            query.prepare("select * from azienda");
            query.exec();
            if(query.next()){

            }
            else{
                QMessageBox MsgBox;
                MsgBox.setWindowTitle("FabariaGest");
                MsgBox.setText(qApp->translate("MainWindow","Errore"));
                MsgBox.setInformativeText(qApp->translate("MainWindow","Inserire i campi dell'azienda"));
                MsgBox.setIcon(QMessageBox::Warning);
                MsgBox.exec();
                azienda_cod();
            }
}

void MainWindow::azienda_cod(){

        azUi = new azienda(this);
        azUi->setWindowModality(Qt::WindowModal);
        azUi->exec();
        QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura anagrafica azienda...."));
}

void MainWindow::selAzienda()
{
     m_input = new InputDialogCombo(this);
     connect(m_input,SIGNAL(SetAzienda(QString)),this,SLOT(titolo(QString)));
     m_input->exec();
}

void MainWindow::vis_carico_magazzino()
{
    carico_mag *car_mag = new carico_mag(this);
    car_mag->exec();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura carico magazzino...."));
}

void MainWindow::vis_scarico_magazzino()
{
    scarico_mag *scar_mag = new scarico_mag(this);
    scar_mag->exec();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura scarico magazzino...."));
}

void MainWindow::gest_fattura_v(){
    fattura *fatt_v = new fattura(this);
    QMdiSubWindow *ws_fa = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_fa->setWidget(fatt_v);
    mdiArea->addSubWindow(ws_fa);

    connect(fatt_v,SIGNAL(destroyed()),ws_fa,SLOT(deleteLater()));
    fatt_v->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura fattura di vendita...."));
}

void MainWindow::gest_banca(){

        banca = new banche(this);
        banca->setWindowModality(Qt::WindowModal);
        banca->exec();
        QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura anagrafica banca...."));
}

void MainWindow::read(){

    if(getPreferenzeSql("menu").toBool() == 1)
        enable_menu();
    else
        disable_menu();
    
    if(getConfigurazioneSql("Esercizioconfigurazione").toString() == "")
        gest_sel_anno();
    else
        getConfigurazioneSql("Esercizioconfigurazione").toString();
    /***********************************************************
     * Impostazione font mainWindow
     ***********************************************************/
    QFont appfnt;
    appfnt.fromString(getPreferenzeSql("ApplicationFont").toString());
    QApplication::setFont(appfnt);

   if(getConfigurazioneSql("VersioneMain").toString() == "freeware"){
        QString t2 = "freeware";
        titolo(t2);
        preferenze_2->setEnabled(true);
        actionPreferenze->setEnabled(true);
    }

   if(getPreferenzeSql("AutoEn").toBool() == 1)
       agg_automatico();
   else
       dis_agg_automatico();

   if(getConfigurazioneSql("MainLicenza").toBool() == 0)
       avvio_licenza();
}

void MainWindow::enable_menu(){
    QMainWindow::menuBar()->setVisible(true);
    groupBox->setVisible(true);
    dock->setVisible(false);
    retranslateUi(this);
}

void MainWindow::disable_menu(){
    QMainWindow::menuBar()->setVisible(false);
    groupBox->setVisible(true);
    dock->setVisible(true);
    retranslateUi(this);
}

void MainWindow::settings(){

        sh1 = new pref(this);
        sh1->setModal(true);
        connect(sh1,SIGNAL(load_plugins()),this,SLOT(read()));
        sh1->exec();
        QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura preferenze...."));
}

void MainWindow::agg_automatico(){
    QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
    trayIcon->showMessage("FabariaGest", qApp->translate("MainWindow","Aggiornamento automantico attivato"),ts, 15*10000);
    sh1 = new pref;
    sh1->self_update();
    ag_tray->setVisible(true);
}

void MainWindow::dis_agg_automatico(){
    QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
    trayIcon->showMessage("FabariaGest", qApp->translate("MainWindow","Aggiornamento automantico disattivato"),ts, 15*10000);
    ag_tray->setVisible(false);
}

void MainWindow::load_style(const QString &sheetName){

        QFile file(":/qss/" + sheetName.toLower() + ".qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());

        qApp->setStyleSheet(styleSheet);
        qApp->setStyle(styleSheet);
}


void MainWindow::titolo(QString testo){

    QSqlQuery query;
    query.prepare("select * from azienda where id='"+getConfigurazioneSql("AziendaID").toString()+"'");
    query.exec();

    if(query.next()){
    QString str,title;
        title = "FabariaGest ";
        str = qApp->translate("MainWindow","rilasciato in licenza a ")+query.value(1).toString()+"  v. "+getConfigurazioneSql("VersioneFabariaGest").toString()+"  ";
        setWindowTitle(title+str+testo);
    }
}

void MainWindow::categorie(){

        po = new porto(this);
        po->setWindowModality(Qt::WindowModal);
        po->exec();
        QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura categorie...."));
}

void MainWindow::onclose()
{
    esci *chiudi = new esci(this);
    chiudi->setWindowModality(Qt::WindowModal);
    chiudi->show();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Chiudi FabariaGest...."));
}

void MainWindow::onactionAbout()
{
    aboutUi = new about(this);
    aboutUi->setWindowModality(Qt::WindowModal);
    aboutUi->setWindowTitle(qApp->translate("MainWindow","About"));
    aboutUi->exec();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura about...."));
}

void MainWindow::gest_causali(){

    caus = new causali(this);
    caus->setWindowModality(Qt::WindowModal);
    caus->exec();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura causali di trasporto...."));
}

void MainWindow::gest_codfisc(){

    fisc = new cod_fisc(this);
    fisc->setWindowModality(Qt::WindowModal);
    fisc->show();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura calcolo codice fiscale...."));

}

void MainWindow::gest_pagamento(){

    pagamento = new pag(this);
    pagamento->setWindowModality(Qt::WindowModal);
    pagamento->exec();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura anagrafica pagamento...."));
}

void MainWindow::gest_iva(){

    iv = new iva(this);
    iv->setWindowModality(Qt::WindowModal);
    iv->exec();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura anagrafica iva..."));
}

void MainWindow::gest_fatt_acq(){

    fatt = new fatt_acq(this);
    fatt->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *ws_fatt = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_fatt->setWidget(fatt);
    mdiArea->addSubWindow(ws_fatt);

    connect(fatt,SIGNAL(destroyed()),ws_fatt,SLOT(deleteLater()));
    fatt->show();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura fattura d'acquisto...."));
}

void MainWindow::gest_fornitori(){

    forn = new fornitori(this);
    forn->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *ws_forn = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_forn->setWidget(forn);
    mdiArea->addSubWindow(ws_forn);

    connect(forn,SIGNAL(destroyed()),ws_forn,SLOT(deleteLater()));
    forn->show();
}

void MainWindow::onclienti()
{
    user *user1 = new user(this);
    user1->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *ws_user = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_user->setWidget(user1);
    ws_user->setGeometry(10,10,600,520);
    mdiArea->addSubWindow(ws_user);

    connect(user1,SIGNAL(destroyed()),ws_user,SLOT(deleteLater()));
    user1->show();
    user1->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura utenti...."));
}



void MainWindow::onarticoli()
{
    articoli *art = new articoli(this);
    art->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *ws_art = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    ws_art->setWidget(art);
    mdiArea->addSubWindow(ws_art);

    connect(art,SIGNAL(destroyed()),ws_art,SLOT(deleteLater()));

    art->show();
    art->showMaximized();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura anagrafica prodotti...."));
}

void MainWindow::onactiondatabasetriggered()
{
    MiniTutorial *mini = new MiniTutorial(this);
    mini->exec();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura documentazione FabariaGest...."));
}

void MainWindow::closeEvent(QCloseEvent *event){

    scrivi_posizione();
    event->ignore();
    onclose();
}

void MainWindow::createStatusBar(){
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Database inizializzato correttamente"));

}

void MainWindow::gest_misure()
{
    un = new misure(this);
    un->setWindowModality(Qt::WindowModal);
    un->exec();

    QMainWindow::statusBar()->showMessage(QString::fromUtf8(qApp->translate("MainWindow","Apertura gestione unità di misura...").toStdString().c_str()));
}

void MainWindow::gest_dist_riba()
{
    riba = new distinta_riba(this);
    riba->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *sub_riba = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    sub_riba->setWidget(riba);
    mdiArea->addSubWindow(sub_riba);

    connect(riba,SIGNAL(destroyed()),sub_riba,SLOT(deleteLater()));
    riba->show();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione riba..."));
}

void MainWindow::gest_anno_cont()
{
    anno_cont = new chiudi_anno_contabile(this);
    anno_cont->setWindowModality(Qt::WindowModal);
    anno_cont->show();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione anno contabile..."));
}

void MainWindow::gest_sel_anno()
{
    sel_anno = new imposta_anno(this);
    sel_anno->setWindowModality(Qt::WindowModal);
    sel_anno->show();

    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura selezione anno dell'esercizio..."));
}

void MainWindow::gest_preventivi()
{
    prev = new preventivi(this);
    prev->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *sub_prev = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    sub_prev->setWidget(prev);
    mdiArea->addSubWindow(sub_prev);

    connect(prev,SIGNAL(destroyed()),sub_prev,SLOT(deleteLater()));
    prev->show();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione preventivi..."));
}

void MainWindow::gest_listini()
{
    listino_gen = new listino(this);
    listino_gen->exec();
}

void MainWindow::gest_ordini()
{
    ord_acq_ven = new ordine(this);
    ord_acq_ven->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *sub_ordini = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    sub_ordini->setWidget(ord_acq_ven);
    mdiArea->addSubWindow(sub_ordini);

    connect(ord_acq_ven,SIGNAL(destroyed()),sub_ordini,SLOT(deleteLater()));
    ord_acq_ven->show();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione ordini..."));
}

void MainWindow::gest_ddt()
{
    m_ddt = new ddt(this);
    m_ddt->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *sub_ddt = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    sub_ddt->setWidget(m_ddt);
    mdiArea->addSubWindow(sub_ddt);

    connect(m_ddt,SIGNAL(destroyed()),sub_ddt,SLOT(deleteLater()));
    m_ddt->show();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura gestione ddt..."));
}

void MainWindow::vis_storico_mail()
{
    lista_email = new lista_invio_mail(this);
    lista_email->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *sub_email = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    sub_email->setWidget(lista_email);
    mdiArea->addSubWindow(sub_email);
    
    connect(lista_email,SIGNAL(destroyed()),sub_email,SLOT(deleteLater()));
    lista_email->show();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura storico email"));
}

void MainWindow::gest_esistenza()
{
    es_all = new esistenza_all(this);
    es_all->setWindowModality(Qt::WindowModal);
    
    es_all->exec();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura statitica fatturato/ordinato/preventivato"));
}

void MainWindow::gest_registro_iva()
{
    iva_reg = new reg_iva(this);
    iva_reg->setWindowModality(Qt::WindowModal);
    
    iva_reg->exec();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura registro iva..."));
} 

void MainWindow::gest_agenti()
{
    prov_agent = new agenti(this);
    prov_agent->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *sub_agent = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    sub_agent->setWidget(prov_agent);
    mdiArea->addSubWindow(sub_agent);
    
    connect(prov_agent,SIGNAL(destroyed()),sub_agent,SLOT(deleteLater()));
    prov_agent->show();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura anagrafica agenti"));
}

void MainWindow::gest_riep_agenti()
{
    riepilogo_agenti = new riep_agent(this);
    riepilogo_agenti->setWindowModality(Qt::WindowModal);
    QMdiSubWindow *sub_agent = new QMdiSubWindow(this,Qt::FramelessWindowHint);
    sub_agent->setWidget(riepilogo_agenti);
    mdiArea->addSubWindow(sub_agent);
    
    connect(riepilogo_agenti,SIGNAL(destroyed()),sub_agent,SLOT(deleteLater()));
    riepilogo_agenti->show();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura anagrafica agenti"));
}

void MainWindow::gest_dropbox()
{
    DropboxFabaria *drop = new DropboxFabaria(this);
    drop->exec();
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura Backup e Ripristino con DropBox..."));
}

QString MainWindow::getLineFromCommandOutput(QString command){
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
MainWindow::~MainWindow()
{
    mdiArea->closeAllSubWindows();
    delete manager;
    delete settingsManager;
}

void MainWindow::ex(){

    QProcess *proc = new QProcess();
    proc->start("/opt/FabariaGest/FabariaGest");
    exit(0);
}

void MainWindow::installa_plugin()
{
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
    QProcess proc;
    proc.start("pkexec chmod 777 /opt/FabariaGest/plugin");
    if(proc.waitForFinished()){
      QString fileName = QFileDialog::getOpenFileName(this, qApp->translate("MainWindow","Apri file"), QString(), qApp->translate("MainWindow","Plugin FabariaGest(*.zip);;Tutti i file(*.*)"));
      if(!fileName.isNull()){
	    extractFileZip(fileName,"/opt/FabariaGest/plugin");
            QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
            LOG_FILE(QString::fromUtf8(qApp->translate("MainWindow","FabariaGest::Il plugin è stato installato correttamente").toStdString().c_str()),LOG_INFO_ALL,QIODevice::Append);
            trayIcon->showMessage("FabariaGest", QString::fromUtf8(qApp->translate("MainWindow","Il plugin è stato installato correttamente...").toStdString().c_str()),ts, 15*10000);
            ag_tray->setVisible(false);
      }
      else{
        QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
        LOG_FILE(QString::fromUtf8(qApp->translate("MainWindow","FabariaGest::Il plugin non è stato installato correttamente").toStdString().c_str()),LOG_INFO_ALL,QIODevice::Append);
            trayIcon->showMessage("FabariaGest", QString::fromUtf8(qApp->translate("MainWindow","Il plugin non è stato installato correttamente...").toStdString().c_str()),ts, 15*10000);
        ag_tray->setVisible(false);
      }
    }
#elif defined (Q_OS_WIN)
    QString fileName = QFileDialog::getOpenFileName(this, qApp->translate("MainWindow","Apri file"), QString(), qApp->translate("MainWindow","Plugin FabariaGest(*.zip);;Tutti i file(*.*)"));
    fileName = QDir::toNativeSeparators(fileName);
    if(!fileName.isNull()){
        extractFileZip(fileName,QDir::currentPath()+"/plugin");
        QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
        LOG_FILE(QString::fromUtf8(qApp->translate("MainWindow","FabariaGest::Il plugin è stato installato correttamente").toStdString().c_str()),LOG_INFO_ALL,QIODevice::Append);
        trayIcon->showMessage("FabariaGest", QString::fromUtf8(qApp->translate("MainWindow","Il plugin è stato installato correttamente...").toStdString().c_str()),ts, 15*10000);
        ag_tray->setVisible(false);
    }
    else{
        QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
        LOG_FILE(QString::fromUtf8(qApp->translate("MainWindow","FabariaGest::Il plugin non è stato installato correttamente").toStdString().c_str()),LOG_INFO_ALL,QIODevice::Append);
        trayIcon->showMessage("FabariaGest", QString::fromUtf8(qApp->translate("MainWindow","Il plugin non è stato installato correttamente...").toStdString().c_str()),ts, 15*10000);
        ag_tray->setVisible(false);
    }
#elif defined (Q_OS_MACX)
    QString fileName = QFileDialog::getOpenFileName(this, qApp->translate("MainWindow","Apri file"), QString(), qApp->translate("MainWindow","Plugin FabariaGest(*.zip);;Tutti i file(*.*)"));
    fileName = QDir::toNativeSeparators(fileName);
    if(!fileName.isNull()){
	extractFileZip(fileName,"/Applications/FabariaGest/FabariaGest.app/Content/PlugIns");
        QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
        LOG_FILE(QString::fromUtf8(qApp->translate("MainWindow","FabariaGest::Il plugin è stato installato correttamente").toStdString().c_str()),LOG_INFO_ALL,QIODevice::Append);
        trayIcon->showMessage("FabariaGest", QString::fromUtf8(qApp->translate("MainWindow","Il plugin è stato installato correttamente...").toStdString().c_str()),ts, 15*10000);
        ag_tray->setVisible(false);
    }
    else{
        QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
        LOG_FILE(QString::fromUtf8(qApp->translate("MainWindow","FabariaGest::Il plugin non è stato installato correttamente").toStdString().c_str()),LOG_INFO_ALL,QIODevice::Append);
        trayIcon->showMessage("FabariaGest", QString::fromUtf8(qApp->translate("MainWindow","Il plugin non è stato installato correttamente...").toStdString().c_str()),ts, 15*10000);
        ag_tray->setVisible(false);
    }
#endif
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::moveEvent(QMoveEvent *)
{
    scrivi_posizione();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    scrivi_posizione();
}

void MainWindow::scrivi_posizione()
{
    QStringList list;
    QString m_list;
    if(!isMaximized())
    {
        list << QString("PosizionePOS='"+QString::number(pos().x())+","+QString::number(pos().y())+"'") 
             << QString("PosizioneSize='"+QString::number(size().width())+","+QString::number(size().height())+"'");
        m_list = list.join(",");
    }
    else{
        list << QString("PosizioneGeometry='"+saveGeometry()+"'") << QString("PosizioneStato='"+saveState()+"'")
            << QString("PosizioneMassimizza='"+QString::number(isMaximized())+"'");
        m_list = list.join(",");
    }
    updateConfigurazione(m_list);
}

void MainWindow::leggi_posizione()
{
    restoreGeometry(getConfigurazioneSql("PosizioneGeometry").toByteArray());
    restoreState(getConfigurazioneSql("PosizioneStato").toByteArray());
    move(getConfigurazioneSql("PosizionePOS").toPoint());
    resize(getConfigurazioneSql("PosizioneSize").toSize());

    if(getConfigurazioneSql("PosizioneMassimizza").toBool() == true)
        showMaximized();
}

void MainWindow::aggiorna_database()
{

    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("FabariaGest");
    box->setText(qApp->translate("MainWindow","Aggiornamento database"));
    box->setInformativeText(qApp->translate("MainWindow","Prima di effettuare l'aggiornamento al\ndatabase accertati di averne salvato\n"
                            "una copia..."));
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
    case QMessageBox::Ok:
         //Open database update
         agg_db = new update_db(this);
         agg_db->exec();
         box->close();
         break;
    case QMessageBox::Cancel:
         //Close
         box->close();
         break;
    }

}

void MainWindow::createMagazzino()
{ 
    m_magazzino = new Magazzino(this);
    m_magazzino->exec();
    
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura magazzino..."));
}

void MainWindow::setMagazzino()
{
    m_mag = new SelezionaMag(this);
    m_mag->exec();
    
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Apertura seleziona magazzino..."));
}

void MainWindow::gest_update()
{
    sh1 = new pref(this);
    sh1->self_update();
    
    QMainWindow::statusBar()->showMessage(qApp->translate("MainWindow","Lancio aggiornamento..."));
}
