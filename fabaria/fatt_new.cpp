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
#include "fatt_new.h"
#include "ui_fatt_new.h"
#include "fatt_acq_art.h"
#include "fatt_acq.h"
#include "fornitori.h"
#include "itdelegato.h"
#include "impo_iva_toto.h"
#include <QCompleter>
#include <QtGui>
#include <QtSql>
#include <QEvent>
#include "settingsmanager.h"
#include "defines.h"
#include "queryexception.h"
#include "LineEditDelegate.h"
#include <iostream>
#include "magazzino.h"

fatt_new *fattura_acq_insert = 0;

using namespace Defines;

fatt_new::fatt_new(QWidget *parent) :
    QDialog(parent),
    found(false),
    m_prezzos_iva(0),
    m_prezzo_c_iva(0),
    m_perc(0),
    m_totale(0)
{
    interfaccia();
}

fatt_new::fatt_new(QWidget *parent, QString title, int id_fatt) :
    QDialog(parent),
    found(false),
    m_submit(false),
    isCreateRow(false), 
    m_prezzos_iva(0),
    m_prezzo_c_iva(0),
    m_perc(0),
    m_totale(0)
{
    interfaccia();
    tipo_fattura_acq = title;
    id_fattura = id_fatt;
    id->setText(QString::number(id_fattura));
    setWindowTitle(tipo_fattura_acq);
}

void fatt_new::interfaccia()
{

    fattura_acq_insert = this;

    setupUi(this);
    //Aggiungo una fattura
    connect(n_riga,SIGNAL(clicked()),this,SLOT(fattura_riga_open()));
    n_riga->setShortcut(qApp->translate("fatt_new","Ctrl+N"));
    
    //Esco dalla fattura
    connect(annulla,SIGNAL(clicked()),this,SLOT(chiudi()));
    annulla->setShortcut(qApp->translate("fatt_new","Ctrl+A"));
    
    //Modifico la riga
    connect(md_riga,SIGNAL(clicked()),this,SLOT(modifica_riga()));
    md_riga->setShortcut(qApp->translate("fatt_new","Ctrl+M"));
    
    /*Clicco sulle righe per visualizzare le informazioni oppure 
     * per modificare una riga
     */
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(add_row,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid_add_row()));
    connect(tab_view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(modifica_riga()));
    
    //Apro la finestra di dialogo dei fornitori
    connect(fornitore_fatt,SIGNAL(clicked()),this,SLOT(open_fornitore()));
    fornitore_fatt->setShortcut(qApp->translate("fatt_new","Ctrl+F"));
    
    /*
     * Le seguenti QLineEdit servono a modificare i prezzi mediante sconto,
     * spesa di trasporto e spesa di incasso.
     */
    connect(sp_tr,SIGNAL(textEdited(QString)),this,SLOT(spesa_trasp(const QString &)));
    connect(sp_in,SIGNAL(textEdited(QString)),this,SLOT(spesa_in(const QString &)));
    connect(sconto_1,SIGNAL(textEdited(QString)),this,SLOT(sconto_1t(const QString &)));
    connect(sconto_2,SIGNAL(textEdited(QString)),this,SLOT(sconto_2t(const QString &)));
    connect(sconto_3,SIGNAL(textEdited(QString)),this,SLOT(sconto_3t(const QString &)));
    
    //Elimino una riga
    connect(el_riga,SIGNAL(clicked()),this,SLOT(elimina()));
    connect(del_riga,SIGNAL(clicked()),this,SLOT(elimina()));
    el_riga->setShortcut(qApp->translate("fatt_new","Ctrl+D"));
    
    //Salvo la fattura di acquisto
    connect(save,SIGNAL(clicked()),this,SLOT(fatt_inserisci()));
    save->setShortcut(qApp->translate("fatt_new","Ctrl+S"));
    
    //Aggiorno la fattura di acquisto
    connect(agg_fatt,SIGNAL(clicked()),this,SLOT(fatt_aggiorna()));
    agg_fatt->setShortcut(qApp->translate("fatt_new","Ctrl+U"));
    
    connect(insert_row,SIGNAL(clicked(bool)),this,SLOT(addRow()));
    connect(add_row,SIGNAL(clicked(QModelIndex)),this,SLOT(InserisciRiga()));
    connect(import_excel_file,SIGNAL(clicked()),this,SLOT(importa()));
    connect(im_ex_file,SIGNAL(clicked()),this,SLOT(importa()));
    
    //Setting data
    data_fat->setCalendarPopup(true);
    data_fat->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());

    mod_grid = new QSqlRelationalTableModel(this);
    local_settings = new QLocale(QLocale::system());

    //Avvio combox
    add_row->viewport()->installEventFilter(this);
    combo_fornitori();
    combo_pagamenti();
    lista();
    listino();
}

fatt_new::~fatt_new()
{
}

void fatt_new::fattura_riga_open(){

    fatt_acq_art *ft = new fatt_acq_art(this,tipo_fattura_acq,id_fattura);
    ft->setModal(true);
    connect(ft,SIGNAL(save_fatt()),this,SLOT(lista()));
    connect(ft,SIGNAL(save_fatt()),this,SLOT(tot_imp_iva()));
    ft->exec();
    lista();
}

double fatt_new::s_iva(double psiva){

    cl = psiva;
    return psiva;
}

double fatt_new::c_iva(double pciva){

    ts = pciva;
    return pciva;
}

double fatt_new::iva_tot(double ivatot){

    cn = ivatot;
    return ivatot;
}

void fatt_new::formatta_num(){

    QString sc1 = QString::number(sconto_1->text().toDouble(),'f',2);
    sconto_1->setText(sc1);
    QString sc2 = QString::number(sconto_2->text().toDouble(),'f',2);
    sconto_2->setText(sc2);
    QString sc3 = QString::number(sconto_3->text().toDouble(),'f',2);
    sconto_3->setText(sc3);
    QString spese_tr = QString::number(sp_tr->text().toDouble(),'f',2);
    sp_tr->setText(spese_tr);
    QString spese_in = QString::number(sp_in->text().toDouble(),'f',2);
    sp_in->setText(spese_in);
    QString tot = QString::number(totale->text().toDouble(),'f',2 );
    totale->setText(tot);
    QString iva = QString::number(iva_ft->text().toDouble(),'f',2 );
    iva_ft->setText(iva);
    QString imp = QString::number(imponibile->text().toDouble(),'f',2);
    imponibile->setText(imp);
}

void fatt_new::fatt_inserisci(){

    if((n_fatt->text() =="") & (forn->currentText()=="")){
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText("Errore");
        MsgBox.setInformativeText(QString::fromUtf8(qApp->translate("fatt_new","Impossibile inserire la fattura,\n poichè non hai inserito i dati correttamente...").toStdString().c_str()));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
        n_fatt->setStyleSheet("background-color: rgb(249, 22, 5)");
        forn->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else{
    QSqlQuery query;
    query.prepare("INSERT INTO fatture_acq_righe "
                  " VALUES('"+QString::number(id_fattura)+"','"+data_fat->date().toString("yyyy-MM-dd")+"','"+forn->currentText()+"',"
		  "'"+sconto_1->text()+"','"+sconto_2->text()+"','"+sconto_3->text()+"','"+sp_tr->text()+"','"+sp_in->text()+"',"
		  "'"+t_pagam->currentText()+"','"+n_fatt->text()+"','"+dateEdit->date().toString("yyyy-MM-dd")+"','"+textEdit->toPlainText()+"',"
		  "'"+imponibile->text()+"','"+iva_ft->text()+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"','"+tipo_listino->currentText()+"')");

        insert_f_prodotti();

  try{
    if(query.exec()){
        LOG_FILE(qApp->translate("fatt_new","Inserimento effettuato con successo(fatt_inserisci)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
        LOG_FILE(qApp->translate("fatt_new","Impossibile inserire(fatt_inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
  }
  catch(FabariaException e)
  {
    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }

    emit salvafatturanew();
    tot_imp_iva();
    close();
    }
    fattura_acquisto->lista();
}

void fatt_new::insert_f_prodotti(){
    QSqlQuery query1;
    query1.prepare("INSERT INTO fatture_acq "
                   " VALUES('"+QString::number(id_fattura)+"','"+data_fat->date().toString("yyyy-MM-dd")+"','"+forn->currentText()+"',"
		   "'"+tipo_fattura_acq+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"','"+tipo_listino->currentText()+"')");

    try{
    if(query1.exec()){
        LOG_FILE(qApp->translate("fatt_new","Inserimento effettuato con successo(insert_f_prodotti)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
	LOG_FILE(qApp->translate("fatt_new","Impossibile inserire(insert_f_prodotti)"),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE(query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(query1.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    fattura_acquisto->lista();
}

void fatt_new::fatt_aggiorna(){

    if((n_fatt->text() =="") & (forn->currentText()=="")){
        QMessageBox MsgBox;
        MsgBox.setText(qApp->translate("fatt_new","Errore"));
        MsgBox.setInformativeText(QString::fromUtf8(qApp->translate("fatt_new","Impossibile aggiornare la fattura,\n poichè non hai inserito i dati correttamente...").toStdString().c_str()));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
        n_fatt->setStyleSheet("background-color: rgb(249, 22, 5)");
        forn->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else{
    QSqlQuery query;
    query.prepare("UPDATE fatture_acq_righe SET data='"+data_fat->date().toString("yyyy-MM-dd")+"',fornitore='"+forn->currentText()+"',sconto_1='"+sconto_1->text()+"', "
               " sconto_2='"+sconto_2->text()+"',sconto_3='"+sconto_3->text()+"',spese_trasporto='"+sp_tr->text()+"',spese_incasso='"+sp_in->text()+"', "
	       "tipo_pagamento='"+t_pagam->currentText()+"',numero_fattura='"+n_fatt->text()+"',data_fattura='"+dateEdit->date().toString("yyyy-M-dd")+"', "
	       "note_fattura='"+textEdit->toPlainText()+"',prezzo_s_iva='"+imponibile->text()+"',iva='"+iva_ft->text()+"',totale='"+totale->text()+"', "
	       "anno_ins='"+QString::number(QDate::currentDate().year())+"', codice_listino='"+tipo_listino->currentText()+"' "
               " WHERE id ='"+QString::number(id_fattura)+"'");

        update_f_prodotti();

  try{
    if( query.exec()){
        LOG_FILE(qApp->translate("fatt_new","Aggiornamento effettuato con successo(fatt_aggiorna)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
        LOG_FILE(qApp->translate("fatt_new","Impossibile aggiornare(fatt_aggiorna)"),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
  }
  catch(FabariaException e)
  {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }

    emit salvafatturanew();
    close();
    }
    fattura_acquisto->lista();
}

void fatt_new::update_f_prodotti(){

    QSqlQuery query1;
    QString txt = qApp->translate("fatt_new",tipo_fattura_acq.toStdString().c_str());
    query1.prepare("UPDATE fatture_acq SET data='"+data_fat->date().toString("yyyy-MM-dd")+"',fornitore='"+forn->currentText()+"',tipo_fattura='"+txt+"',"
		   "totale='"+totale->text()+"',anno_ins='"+QString::number(QDate::currentDate().year())+"',codice_listino='"+tipo_listino->currentText()+"' WHERE id='"+QString::number(id_fattura)+"'");
    try{
    if(query1.exec()){
	LOG_FILE(qApp->translate("fatt_new","Aggiornamento effettuato correttamente(update_f_prodotti)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
        LOG_FILE(qApp->translate("fatt_new","Impossibile inserire(update_f_prodotti)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE(query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query1.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    fattura_acquisto->lista();
}

void fatt_new::open_fornitore(){

    fornitori *fo = new fornitori(this);
    fo->setModal(true);
    fo->nuovo();
    connect(fo,SIGNAL(vis_forn()),this,SLOT(combo_fornitori()));
    fo->exec();
}

void fatt_new::elimina(){
    if(tab_view->selectionModel()->currentIndex().isValid() || 
        add_row->selectionModel()->currentIndex().isValid()
    ){
        if (id_fattura != 0)
        {
            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM fatture_righe_acq_art WHERE id = '"+QString::number(id_fattura)+"' LIMIT 1");
            qctrl.exec();

                QMessageBox *box= new QMessageBox(this);
                box->setWindowTitle("FabariaGest");
                box->setInformativeText(qApp->translate("fatt_new","Vuoi eliminare veramente \n il record selezionato?...."));
                box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                box->setDefaultButton(QMessageBox::Ok);
                int ret = box->exec();
                switch(ret){
                case QMessageBox::Ok:
                     //Close
                     elimina_riga();
                     box->close();
                     break;
                case QMessageBox::Cancel:
                    //return
                    box->close();
                    break;
                }
            }
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Selezionare una riga da eliminare"));
    }
}

void fatt_new::elimina_riga(){

    QSqlQuery query,query1;
    const char* error= "";
    query.prepare("DELETE FROM fatture_righe_acq_art WHERE id = id = '"+QString::number(id_fattura)+"' LIMIT 1");
    query1.prepare("DELETE FROM fatture_acq_righe WHERE id = id = '"+QString::number(id_fattura)+"' LIMIT 1");

    try{
    if (query.exec() && query1.exec()) //Se esito OK Eliminato da DB
    {
        lista();
    }
    else
    {
       LOG_FILE(qApp->translate("fatt_new","Impossibile eliminare(elimina_riga)"),LOG_ERROR_ALL,QIODevice::Append);
       LOG_FILE(qApp->translate("fatt_new","(elimina_riga)----query.exec(): ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
       LOG_FILE(qApp->translate("fatt_new","(elimina_riga)----query1.exec(): ")+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
       throw error ? InvalidQuery(query.lastError().text().toStdString().c_str()) : InvalidQuery(query.lastError().text().toStdString().c_str());
     }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    emit salvafatturanew();
    lista();
    tot_imp_iva();
}

void fatt_new::clickgrid(){

    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
}

void fatt_new::clickgrid_add_row()
{
    QString id_new;
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
}

void fatt_new::salva_fattura_ric(){

    emit salvafatturanew();
    fattura_acquisto->lista();
    lista();
}

void fatt_new::listino()
{
	QStringList lista;
        QSqlQuery query("select descrizione from tipo_listino");
        if(query.exec()){
            while(query.next()){
               lista << query.value(0).toString();
            }
        }
        QCompleter *complete = new QCompleter(lista,this);
        complete->setCaseSensitivity(Qt::CaseInsensitive);
        complete->setCompletionMode(QCompleter::PopupCompletion);
        complete->setModel(tipo_listino->model());
        tipo_listino->setCompleter(complete);
        tipo_listino->addItems(lista);
}


void fatt_new::modifica_riga(){

    if(tab_view->selectionModel()->currentIndex().isValid()){
    QModelIndex modelIndex = tab_view->selectionModel()->currentIndex();
    int riga = modelIndex.row();
    QModelIndex newindex = modelIndex.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    modifica_riga_one(newindex);
    }
    else
    {
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Selezionare una riga da modificare..."));
    }
}

void fatt_new::modifica_riga_one(QModelIndex index){
    //Tentativo di aggiornamento del record perché trovato
    if(index.row() < mod_grid->rowCount()){
        fatt_acq_art *gt = new fatt_acq_art(this,tipo_fattura_acq,id_fattura);
        gt->setModal(true);
        QSqlRecord record = mod_grid->record(index.row());
        if(!record.isEmpty()){
			gt->cod_art->setText(record.value("cod_art").toString());
			gt->cod_barre->setText(record.value("codbarre").toString());
			gt->art_nom->setText(record.value("nome_articolo").toString());
			gt->descrizione->setText(record.value("descrizione").toString());
			gt->um->setCurrentText(record.value("unita").toString());
			gt->prezzo_unitario->setText(record.value("prezzo_unit").toString());
			gt->p_s_iva->setText(record.value("prezzo_s_iva").toString());
			gt->p_c_iva->setText(record.value("prezzo_c_iva").toString());
			gt->comboBox->setCurrentText(record.value("iva").toString());
			gt->quant->setText(record.value("quantita").toString());
			gt->sconto->setText(record.value("sconto").toString());
			gt->totale->setText(record.value("totale").toString());
			gt->categ->setCurrentText(record.value("categoria").toString());
			gt->tipo_listino->setCurrentText(record.value("codice_listino").toString());
            gt->numero_lotto->setText(record.value("numero_lotto").toString());
            gt->data_lotto->setText(record.value("data_lotto").toString());
                     connect(gt,SIGNAL(save_fatt()),this,SLOT(tot_imp_iva()));
        }
        connect(gt,SIGNAL(save_fatt()),this,SLOT(lista()));
        connect(gt,SIGNAL(save_fatt()),this,SLOT(tot_imp_iva()));
        gt->exec();
    }
    lista();
}


void fatt_new::sconto_1t(const QString &){

    double sptr = local_settings->toDouble(sconto_1->text());
    double sp =0,sp1=0,sp2 =0;

    sp = imponibile->text().toDouble()-((imponibile->text().toDouble()*sptr)/100);
    imponibile->setText(QString::number(sp));
    sp1 = iva_ft->text().toDouble()-((iva_ft->text().toDouble()*sptr)/100);
    iva_ft->setText(QString::number(sp1));
    sp2 = totale->text().toDouble()-((totale->text().toDouble()*sptr)/100);
    totale->setText(QString::number(sp2));

    if(sconto_1->text().length() == 0){
        sconto_1->setText("0.00");
        tot_imp_iva();
    }
}

void fatt_new::sconto_2t(const QString &){

    double sptr = local_settings->toDouble(sconto_2->text());
    double sp =0,sp1=0,sp2 =0;

    sp = imponibile->text().toDouble()-((imponibile->text().toDouble()*sptr)/100);
    imponibile->setText(QString::number(sp));
    sp1 = iva_ft->text().toDouble()-((iva_ft->text().toDouble()*sptr)/100);
    iva_ft->setText(QString::number(sp1));
    sp2 = totale->text().toDouble()-((totale->text().toDouble()*sptr)/100);
    totale->setText(QString::number(sp2));

    if(sconto_2->text().length() == 0){
        sconto_2->setText("0.00");
        tot_imp_iva();
    }
}

void fatt_new::sconto_3t(const QString &){

    double sptr = local_settings->toDouble(sconto_3->text());
    double sp =0,sp1=0,sp2 =0;

    sp = imponibile->text().toDouble()-((imponibile->text().toDouble()*sptr)/100);
    imponibile->setText(QString::number(sp));
    sp1 = iva_ft->text().toDouble()-((iva_ft->text().toDouble()*sptr)/100);
    iva_ft->setText(QString::number(sp1));
    sp2 = totale->text().toDouble()-((totale->text().toDouble()*sptr)/100);
    totale->setText(QString::number(sp2));

    if(sconto_3->text().length() == 0){
        sconto_3->setText("0.00");
        tot_imp_iva();
    }
}

void fatt_new::combo_fornitori(){

    forn->clear();
    QStringList list;
    QSqlQuery query("select ragione_sociale from fornitori");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(forn->model());
    forn->setCompleter(complete);
    forn->addItems(list);
}

void fatt_new::combo_pagamenti(){

    t_pagam->clear();
    QStringList list;
    QSqlQuery query("select codice from pagam");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(t_pagam->model());
    t_pagam->setCompleter(complete);
    t_pagam->addItems(list);
}



void fatt_new::spesa_trasp(const QString &){

    double sptr = local_settings->toDouble(sp_tr->text());
    double sp = 0,sp1 = 0,sp2=0; //(sptr*20)/100;

    sp = imponibile->text().toDouble()+sptr;
    imponibile->setText(QString::number(sp));
    sp1 = iva_ft->text().toDouble()+((sptr*20)/100);
    iva_ft->setText(QString::number(sp1));
    sp2 = totale->text().toDouble()+sptr-((sptr*20)/100);
    totale->setText(QString::number(sp2));

    if(sp_tr->text().length()==0){
        tot_imp_iva();
        sp_tr->setText("0.00");
    }
}

void fatt_new::spesa_in(const QString &){

    double sptr = local_settings->toDouble(sp_in->text());
    double sp = 0,sp1 = 0,sp2=0; //(sptr*20)/100;

    sp = imponibile->text().toDouble()+sptr;
    imponibile->setText(QString::number(sp));
    sp1 = iva_ft->text().toDouble()+((sptr*20)/100);
    iva_ft->setText(QString::number(sp1));
    sp2 = totale->text().toDouble()+sptr-((sptr*20)/100);
    totale->setText(QString::number(sp2));

    if(sp_in->text().length()==0){
        tot_imp_iva();
        sp_in->setText("0.00");
    }
}

void fatt_new::lista(){

    mod_grid->setTable("fatture_righe_acq_art");
    mod_grid->setFilter("id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    mod_grid->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mod_grid->select();
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("fatt_new","Codice"));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("fatt_new","Cod. Barre"));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("fatt_new","Prodotto"));
    mod_grid->setHeaderData(4, Qt::Horizontal, qApp->translate("fatt_new","Descrizione"));
    mod_grid->setHeaderData(5, Qt::Horizontal, QString::fromUtf8(qApp->translate("fatt_new","Unità").toStdString().c_str()));
    mod_grid->setHeaderData(6, Qt::Horizontal, qApp->translate("fatt_new","Prezzo unitario"));
    mod_grid->setHeaderData(7, Qt::Horizontal, qApp->translate("fatt_new","Prezzo S. IVA"));
    mod_grid->setHeaderData(8, Qt::Horizontal, qApp->translate("fatt_new","Prezzo C. IVA"));
    mod_grid->setHeaderData(9, Qt::Horizontal, qApp->translate("fatt_new","IVA"));
    mod_grid->setHeaderData(10, Qt::Horizontal, QString::fromUtf8(qApp->translate("fatt_new","Quantità").toStdString().c_str()));
    mod_grid->setHeaderData(11, Qt::Horizontal, qApp->translate("fatt_new","Sconto"));
    mod_grid->setHeaderData(12,Qt::Horizontal,qApp->translate("fatt_new","Totale"));
    mod_grid->setHeaderData(17,Qt::Horizontal,qApp->translate("fatt_new","Lotto"));
    mod_grid->setHeaderData(18,Qt::Horizontal,qApp->translate("fatt_new","Scadenza lotto"));
     mod_grid->setHeaderData(19,Qt::Horizontal,qApp->translate("fatt_new","Magazzino"));
    tab_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    tab_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab_view->setSelectionMode(QAbstractItemView::SingleSelection);
    tab_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab_view->verticalHeader()->hide();
    tab_view->setModel(mod_grid);
    tab_view->setColumnHidden(0,true);
    tab_view->setColumnHidden(13,true);
    tab_view->setColumnHidden(14,true);
    tab_view->setColumnHidden(15,true);
    tab_view->setColumnHidden(16,true);
    tab_view->setAlternatingRowColors(true);
    
    //Seconda tabella
    add_row->setSelectionBehavior(QAbstractItemView::SelectRows);
    add_row->setSelectionMode(QAbstractItemView::SingleSelection);
    add_row->setEditTriggers(QAbstractItemView::DoubleClicked);
    add_row->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    add_row->verticalHeader()->hide();
    add_row->setItemDelegateForColumn(1,new LineEditDelegate(add_row,this));
    add_row->setItemDelegateForColumn(5,new combo_um(this));
    add_row->setItemDelegateForColumn(3,new LineEditDelegate(add_row,this));
    add_row->setItemDelegateForColumn(6,new LineEditDelegate(add_row,this));
    add_row->setItemDelegateForColumn(9,new Iva(this));
    add_row->setItemDelegateForColumn(7,new LineEditDelegate(add_row,this));
    add_row->setItemDelegateForColumn(8,new LineEditDelegate(add_row,this));
    add_row->setItemDelegateForColumn(10,new LineEditDelegate(add_row,this));
    add_row->setItemDelegateForColumn(11,new LineEditDelegate(add_row,this));
    add_row->setModel(mod_grid);
    add_row->setAlternatingRowColors(true);
    add_row->setColumnHidden(0,true);
    add_row->setColumnHidden(13,true);
    add_row->setColumnHidden(14,true);
    add_row->setColumnHidden(15,true);
    add_row->setColumnHidden(16,true);
}

void fatt_new::tot_imp_iva(){

    if(tipo_fattura_acq == qApp->translate("fatt_new","Nota di credito"))
    {
        QSqlQuery Query1,Query2,Query3;
        QSqlQuery query;
        query.prepare("select cod_art from fatture_righe_acq_art where id= '"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        query.exec();
        if(query.next()){

            Query1.prepare("select sum(-prezzo_s_iva) from fatture_righe_acq_art where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query1.exec();
        if(Query1.next()){
            imponibile->setText(QString::number(Query1.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
        }

        Query2.prepare("select sum((-totale+prezzo_s_iva)) from fatture_righe_acq_art where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query2.exec();
        if(Query2.next()){
            iva_ft->setText(QString::number(Query2.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare l'iva... ")+Query2.lastError().text());
        }
        Query3.prepare("select sum(-totale) from fatture_righe_acq_art where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query3.exec();
        if(Query3.next()){
            totale->setText(QString::number(Query3.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il totale... ")+Query3.lastError().text());
        }
        }
    }
    else{
    QSqlQuery Query1,Query2,Query3;
    QSqlQuery query;
    query.prepare("select cod_art from fatture_righe_acq_art where id= '"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){

        Query1.prepare("select sum(prezzo_s_iva) from fatture_righe_acq_art where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query1.exec();
    if(Query1.next()){
        imponibile->setText(QString::number(Query1.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
    }

    Query2.prepare("select sum(totale-prezzo_s_iva) from fatture_righe_acq_art where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query2.exec();
    if(Query2.next()){
        iva_ft->setText(QString::number(Query2.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare l'iva... ")+Query2.lastError().text());
    }
    Query3.prepare("select sum(totale) from fatture_righe_acq_art where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query3.exec();
    if(Query3.next()){
        totale->setText(QString::number(Query3.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il totale... ")+Query3.lastError().text());
    }
    }
    }
    emit salvafatturanew();
}

void fatt_new::totale_impo_iva()
{
  if(tipo_fattura_acq == qApp->translate("fatt_new","Nota di credito"))
    {
        QSqlQuery Query1,Query2,Query3;
        QSqlQuery query;
        query.prepare("select cod_art from fatture_acq_righe where id= '"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        query.exec();
        if(query.next()){

            Query1.prepare("select sum(-prezzo_s_iva) from fatture_acq_righe where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query1.exec();
        if(Query1.next()){
            imponibile->setText(QString::number(Query1.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
        }

        Query2.prepare("select sum((-totale+prezzo_s_iva)) from fatture_acq_righe where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query2.exec();
        if(Query2.next()){
            iva_ft->setText(QString::number(Query2.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare l'iva... ")+Query2.lastError().text());
        }
        Query3.prepare("select sum(-totale) from fatture_acq_righe where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query3.exec();
        if(Query3.next()){
            totale->setText(QString::number(Query3.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il totale... ")+Query3.lastError().text());
        }
        }
    }
    else{
    QSqlQuery Query1,Query2,Query3;
    QSqlQuery query;
    query.prepare("select cod_art from fatture_righe_acq_art where id= '"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){

        Query1.prepare("select sum(prezzo_s_iva) from fatture_acq_righe where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query1.exec();
    if(Query1.next()){
        imponibile->setText(QString::number(Query1.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
    }

    Query2.prepare("select sum(totale-prezzo_s_iva) from fatture_acq_righe where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query2.exec();
    if(Query2.next()){
        iva_ft->setText(QString::number(Query2.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare l'iva... ")+Query2.lastError().text());
    }
    Query3.prepare("select sum(totale) from fatture_acq_righe where id='"+QString::number(id_fattura)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query3.exec();
    if(Query3.next()){
        totale->setText(QString::number(Query3.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Impossibile instanziare il totale... ")+Query3.lastError().text());
    }
    }
    }
    emit salvafatturanew();
}

void fatt_new::chiudi()
{
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("FabariaGest");
    box->setInformativeText(qApp->translate("fatt_new","Sei sicuro di voler uscire dalla fattura?\n I dati non verranno salvati...."));
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Close);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
    case QMessageBox::Ok:
         //Close
         save->setChecked(true);
         save->setStyleSheet("background-color: rgb(249, 22, 5)");
         elimina_riga();
         box->close();
         close();
         break;
    case QMessageBox::Cancel:
        //return
        box->close();
        break;
    case QMessageBox::Close:
        box->close();
        close();
    }
}

void fatt_new::importa()
{
  QString testo = qApp->translate("fatt_new","ID\nCodice articolo\nCodice a barre\nNome prodotto\nDescrizione\nUnità di misura\n"
      "Prezzo unitario\nPrezzo senza iva\nPrezzo con iva\nAliquota IVA\n"
      "Quantità\nSconto\nTotale\nAnno inserimento\nListino\nCategoria\n"
      "Data inserimento\nNumero lotto\nScadenza lotto\nMagazzino\n\n\n\nAttenzione tutte le celle\ndevono essere compilate o lasciate vuote.");
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle(qApp->translate("fatt_new","FabariaGest"));
    box->setText(qApp->translate("fatt_new","Il file excel\ndeve essere composto:\n"));
    box->setInformativeText(testo);
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
    case QMessageBox::Ok:
         //Open file excel
         import_file_excel();
         box->close();
         break;
    case QMessageBox::Cancel:
         //Close
         box->close();
         break;
    }
}

void fatt_new::import_file_excel()
{
    QString filePath = QFileDialog::getOpenFileName(0, qApp->translate("fatt_new","Apri file excel"), QString(), "*.xlsx");
    if(filePath.isEmpty())
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Non hai aperto nesssun file"));
    else{
    Document xlsx(filePath);
        
    for(int i=2; i <= xlsx.dimension().rowCount(); i++)
    {
      QProgressDialog progressDialog(this);
      progressDialog.setRange(0, xlsx.dimension().rowCount());
      progressDialog.setWindowModality(Qt::WindowModal);
      progressDialog.setWindowTitle(qApp->translate("fatt_new","Inserimento dati da excel"));
      progressDialog.setValue(i);
      progressDialog.setLabelText(qApp->translate("fatt_new","Inserimento dati %1 di %2...")
                      .arg(i).arg(xlsx.dimension().rowCount()));
      qApp->processEvents();
      QSqlQuery query,query_id,query_up;
      QString req = "INSERT INTO fatture_righe_acq_art(id,cod_art,codbarre,nome_articolo,descrizione,unita, "
		    "prezzo_unit,prezzo_s_iva,prezzo_c_iva,iva,quantita,sconto,totale, "
		    "anno_ins,codice_listino,categoria,data_insert, numero_lotto, data_lotto,addMagazzino) VALUES('";
      Cell *code_art = xlsx.cellAt(i,2);
      query_up.prepare("select * from fatture_righe_acq_art where cod_art='"+code_art->value().toString()+"'");
      query_up.exec();
      if(query_up.next())
      {
	  aggiorna_file_excel(filePath);
      }
      else{
	//Inserimento
	req.append(QString::number(id_fattura));
	req.append("','");
      if(Cell *code_articolo = xlsx.cellAt(i,2)){//cod_articolo
        req.append(QString(code_articolo->value().toString().toStdString().c_str()).replace("'","''"));
        req.append("','");
      }
      if(Cell *codbarre = xlsx.cellAt(i,3)){//codbarre
        req.append(QString(codbarre->value().toString().toStdString().c_str()).replace("'","''"));
        req.append("','");
      }
      if(Cell *name_articolo = xlsx.cellAt(i,4)){//nome_articolo
        req.append(name_articolo->value().toString());
        req.append("','");
      }
      if(Cell *desc = xlsx.cellAt(i,5)){//descrizione
        req.append(desc->value().toString());
        req.append("','");
      }
      if(Cell *un = xlsx.cellAt(i,6)){//unità
        req.append(un->value().toString());
        req.append("','");
      }
      if(Cell *pr_unit = xlsx.cellAt(i,7)){//prezzo_unitario
        req.append(QString(pr_unit->value().toString().toStdString().c_str()).replace("'","''"));
        req.append("','");
      }
      if(Cell *pr_s_iva = xlsx.cellAt(i,8)){//prezzo_s_iva
        req.append(pr_s_iva->value().toString());
        req.append("','");
      }
      if(Cell *pr_c_iva = xlsx.cellAt(i,9)){//prezzo_c_iva
        req.append(pr_c_iva->value().toString());
        req.append("','");
      }
      if(Cell *m_iva = xlsx.cellAt(i,10)){//iva
        req.append(m_iva->value().toString());
        req.append("','");
      }
      if(Cell *quan = xlsx.cellAt(i,11)){//quantita
        req.append(quan->value().toString());
        req.append("','");
      }
      if(Cell *sc = xlsx.cellAt(i,12)){//sconto
        req.append(sc->value().toString());
        req.append("','");
      }
      if(Cell *tot = xlsx.cellAt(i,13)){//totale
        req.append(tot->value().toString());
        req.append("','");
      }
      if(Cell *anno = xlsx.cellAt(i,14)){//anno_ins
        req.append(anno->value().toString());
        req.append("','");
      }
      if(Cell *cod_list = xlsx.cellAt(i,15)){//codice_listino
        req.append(cod_list->value().toString());
        req.append("','");
      }
      if(Cell *cat = xlsx.cellAt(i,16)){//categoria
        req.append(cat->value().toString());
        req.append("','");
      }
      if(Cell *date_insert = xlsx.cellAt(i,17)){//data_insert
        req.append(date_insert->value().toString());
        req.append("','");
      }
       if(Cell *num_lotto= xlsx.cellAt(i,18)){//numero_lotto
        req.append(num_lotto->value().toString());
        req.append("','");
      }
       if(Cell *date_lotto = xlsx.cellAt(i,19)){//data_lotto
        req.append(date_lotto->value().toString());
        req.append("','");
      }
      if(Cell *mMag = xlsx.cellAt(i,20)){//addMagazzino
        req.append(mMag->value().toString());
        req.append("','");
      }
      req.chop(2);
      req.append(");");
      query.prepare(req);
      progressDialog.show();
      progressDialog.update();
      QSqlQuery query("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+code_art->value().toString()+"'");
       query.exec();
       if(query.next()){
            found = true;
            double qa = query.value(0).toDouble();
            double pr_s = query.value(1).toDouble();
            double pr_c = query.value(2).toDouble();
            double tot_ = query.value(3).toDouble();
            update_articoli(qa,pr_s,pr_c,tot_);
            update_carico_prod(qa,pr_s,pr_c,tot_);
         }
         else{
            insert_articoli();
            insert_carico_magazzino();
         }
      if(query.exec())
      {
          LOG_FILE(qApp->translate("fatt_new","Inserimento effettuato con successo(fatt_new::import_file_excel"),LOG_INFO_ALL,QIODevice::Append);
      }
      else
           QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Errore Inserimento:\n ")+query.lastError().text());
    }
    }
    }
    lista();
}

void fatt_new::aggiorna_file_excel(QString doc)
{
    Document xlsx(doc);
        
    for(int i=2; i <= xlsx.dimension().rowCount(); i++)
    {
      QProgressDialog progressDialog(this);
      progressDialog.setRange(0, xlsx.dimension().rowCount());
      progressDialog.setWindowModality(Qt::WindowModal);
      progressDialog.setWindowTitle(qApp->translate("fatt_new","Aggionamento dati da excel"));
      progressDialog.setValue(i);
      progressDialog.setLabelText(qApp->translate("fatt_new","Aggionamento dati %1 di %2...")
                      .arg(i).arg(xlsx.dimension().rowCount()));
      qApp->processEvents();
      QSqlQuery query,query_up;
      query_up.prepare("select * from fatture_righe_acq_art where id='"+QString::number(id_fattura)+"'");
      query_up.exec();
      if(query_up.next())
      {
      QString req = "UPDATE fatture_righe_acq_art SET cod_art='";
      if(Cell *codarticolo = xlsx.cellAt(i,2))
      {
	req.append(QString(codarticolo->value().toString().toStdString().c_str()).replace("'","''"));
        req.append("',cod_art='");
      }
      if(Cell *codbarre = xlsx.cellAt(i,3)){//codbarre
        req.append(QString(codbarre->value().toString().toStdString().c_str()).replace("'","''"));
        req.append("',nome_articolo='");
      }
      if(Cell *name_articolo = xlsx.cellAt(i,4)){//nome_articolo
        req.append(name_articolo->value().toString());
        req.append("',descrizione='");
      }
      if(Cell *desc = xlsx.cellAt(i,5)){//descrizione
        req.append(desc->value().toString());
        req.append("',unita='");
      }
      if(Cell *un = xlsx.cellAt(i,6)){//unità
        req.append(un->value().toString());
        req.append("',prezzo_unitario='");
      }
      if(Cell *pr_unit = xlsx.cellAt(i,7)){//prezzo_unitario
        req.append(pr_unit->value().toString());
        req.append("',prezzo_s_iva='");
      }
      if(Cell *pr_s_iva = xlsx.cellAt(i,8)){//prezzo_s_iva
        double prezzo = pr_s_iva->value().toDouble()+query_up.value(6).toDouble();
        req.append(QString::number(prezzo));
        req.append("',prezzo_c_iva='");
      }
      if(Cell *pr_c_iva = xlsx.cellAt(i,9)){//prezzo_c_iva
        double prezzo = pr_c_iva->value().toDouble()+query_up.value(7).toDouble();
        req.append(QString::number(prezzo));
        req.append("',iva='");
      }
      if(Cell *m_iva = xlsx.cellAt(i,10)){//iva
        req.append(m_iva->value().toString());
        req.append("',quantita='");
      }
      if(Cell *quan = xlsx.cellAt(i,11)){//quantita
        double prezzo = quan->value().toDouble()+query_up.value(9).toDouble();
        req.append(QString::number(prezzo));
        req.append("',sconto='");
      }
      if(Cell *sc = xlsx.cellAt(i,12)){//sconto
        req.append(sc->value().toString());
        req.append("',totale='");
      }
      if(Cell *tot = xlsx.cellAt(i,13)){//totale
        double prezzo = tot->value().toDouble()+query_up.value(11).toDouble();
        req.append(QString::number(prezzo));
        req.append("',anno_ins='");
      }
      if(Cell *anno = xlsx.cellAt(i,14)){//anno_ins
        req.append(anno->value().toString());
        req.append("',codice_listino='");
      }
      if(Cell *cod_list = xlsx.cellAt(i,15)){//codice_listino
        req.append(cod_list->value().toString());
        req.append("',categoria='");
      }
      if(Cell *cat = xlsx.cellAt(i,16)){//categoria
        req.append(cat->value().toString());
        req.append("',data_insert='");
      }
     if(Cell *date_insert = xlsx.cellAt(i,17)){//data_insert
        req.append(date_insert->value().toString());
        req.append("'numero_lotto='");
      }
       if(Cell *num_lotto = xlsx.cellAt(i,18)){//numero_lotto
        req.append(num_lotto->value().toString());
        req.append("',data_lotto='");
      }
       if(Cell *date_lotto= xlsx.cellAt(i,19)){//data_lotto
        req.append(date_lotto->value().toString());
        req.append("',addMagazzino='");
      }
      if(Cell *date_lotto= xlsx.cellAt(i,20)){//addMagazzino
        req.append(date_lotto->value().toString());
        req.append("',where id='");
      }
      if(Cell *code_id = xlsx.cellAt(i,1)){//codice id
        req.append(QString(code_id->value().toString()));
        req.append("','");
      }
      req.chop(2);
      req.append(";");
      query.prepare(req);
      progressDialog.show();
      progressDialog.update();
      Cell *code_art = xlsx.cellAt(i,2);
      QSqlQuery query("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+code_art->value().toString()+"'");
        query.exec();
        if(query.next()){
            found = true;
            double qa = query.value(0).toDouble();
            double pr_s = query.value(1).toDouble();
            double pr_c = query.value(2).toDouble();
            double tot_ = query.value(3).toDouble();
                update_articoli(qa,pr_s,pr_c,tot_);
                update_carico_prod(qa,pr_s,pr_c,tot_);
        }
      if(query.exec())
           qDebug() <<  qApp->translate("fatt_new","Aggionamento effettuato");
      else
           QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Errore aggiornamento:\n ")+query.lastError().text());
    }
    }
    lista();
}

void fatt_new::insert_articoli(){

                    QSqlQuery Query;
		    double quan = getQuantita().toDouble();
		    if(tipo_fattura_acq == qApp->translate("fatt_new","Nota di credito")){
		    Query.prepare("INSERT INTO articoli"
                              " VALUES ('"+getCodArt()+"','"+getCodBarre()+"',"
			       "'"+getNomeArticolo()+"','"+getDesc()+"','"+getUnita()
                               +"','"+getPrezzoUnit()+"',"
			       "'"+getPrezzoSIVA()+"','"+getPrezzoCIVA()+"','"+QString::number(getIVA().toDouble())+"',"
			       "'"+QString::number(+quan)+"','"+getSconto()+"','"+getTotale()+"','"+QString::number(QDate::currentDate().year())+"',"
			       "'"+getCateg()+"','"+getListino()
                               +"','"+QDate::currentDate().toString("yyyy/MM/dd")+"','"+getNumeroLotto()+"','"+getDataLotto()+"', '"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')"
                               );
		    }
		    else{
		      Query.prepare("INSERT INTO articoli"
                               " VALUES ('"+getCodArt()+"','"+getCodBarre()+"',"
			       "'"+getNomeArticolo()+"','"+getDesc()+"','"+getUnita()
                               +"','"+getPrezzoUnit()+"',"
			       "'"+getPrezzoSIVA()+"','"+getPrezzoCIVA()+"','"+QString::number(getIVA().toDouble())+"',"
			       "'"+QString::number(+quan)+"','"+getSconto()+"','"+getTotale()+"','"+QString::number(QDate::currentDate().year())+"',"
			       "'"+getCateg()+"','"+getListino()
                               +"','"+QDate::currentDate().toString("yyyy/MM/dd")+"','"+getNumeroLotto()+"','"+getDataLotto()+"', '"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')"
                               );
		    }

    try{
    if(Query.exec()){
	LOG_FILE(qApp->translate("fatt_new","Inserimento effettuato con successo(fatt_new::insert_articoli)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
	LOG_FILE(qApp->translate("fatt_new","Errore inserimento(fatt_new::insert_articoli)"),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE(qApp->translate("fatt_new","(fatt_new::insert_articoli)---")+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(Query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}


void fatt_new::insert_carico_magazzino()
{
                    QSqlQuery Query;
		    double quan = getQuantita().toDouble();
		    if(tipo_fattura_acq == qApp->translate("fatt_new","Nota di credito")){
		    Query.prepare("INSERT INTO carico_magazzino"
                               " VALUES ('"+getCodArt()+"','"+getCodBarre()+"',"
			       "'"+getNomeArticolo()+"','"+getDesc()+"','"+getUnita()
                               +"','"+getPrezzoUnit()+"',"
			       "'"+getPrezzoSIVA()+"','"+getPrezzoCIVA()+"','"+QString::number(getIVA().toDouble())+"',"
			       "'"+QString::number(+quan)+"','"+getSconto()+"','"+getTotale()+"','"+QString::number(QDate::currentDate().year())+"',"
			       "'"+getCateg()+"','"+getListino()
                               +"','"+QDate::currentDate().toString("yyyy/MM/dd")+"','"+getNumeroLotto()+"','"+getDataLotto()+"', '"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')"
                               );
		    }
		    else{
		      Query.prepare("INSERT INTO carico_magazzino"
                               " VALUES ('"+getCodArt()+"','"+getCodBarre()+"',"
			       "'"+getNomeArticolo()+"','"+getDesc()+"','"+getUnita()
                               +"','"+getPrezzoUnit()+"',"
			       "'"+getPrezzoSIVA()+"','"+getPrezzoCIVA()+"','"+QString::number(getIVA().toDouble())+"',"
			       "'"+QString::number(+quan)+"','"+getSconto()+"','"+getTotale()+"','"+QString::number(QDate::currentDate().year())+"',"
			       "'"+getCateg()+"','"+getListino()
                               +"','"+QDate::currentDate().toString("yyyy/MM/dd")+"','"+getNumeroLotto()+"','"+getDataLotto()+"', '"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')"
                               );
		    }

    try{
    if(Query.exec()){
	LOG_FILE(qApp->translate("fatt_new","Inserimento effettuato con successo(fatt_new::insert_carico_magazzino)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
	LOG_FILE(qApp->translate("fatt_new","Errore inserimento(fatt_new::insert_carico_magazzino)"),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE(qApp->translate("fatt_new","(fatt_new::insert_carico_magazzino)---")+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(Query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

bool fatt_new::eventFilter(QObject *o, QEvent *j)
{
   QMouseEvent *mouseEvent = (QMouseEvent*)j ;
    if ((j->type() == QEvent::KeyPress)) {
        if(o==add_row->viewport()){
            QKeyEvent* ke = (QKeyEvent*)j;
            if (ke->key() == Qt::Key_Enter) {
                if(InserisciRiga()){
                    QSqlQuery query;
                    query.exec("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+getCodArt()+"'");
                    if(query.next())
                    {
                        m_submit = true;
                        double qa = query.value(0).toDouble();
                        double pr_s = query.value(1).toDouble();
                        double pr_c = query.value(2).toDouble();
                        double tot_ = query.value(3).toDouble();
                        update_articoli(qa,pr_s,pr_c,tot_);
                        update_carico_prod(qa,pr_s,pr_c,tot_);
                        submit();
                    }
                    else{
                        insert_articoli();
                        insert_carico_magazzino();
                        submit();
                    }
            }
        }
    }
    }
    else if ((QEvent::MouseButtonPress==j->type()) && (mouseEvent->button() == Qt::RightButton) && o==add_row->viewport())
    {
            if(InserisciRiga()){
                QSqlQuery query;
                query.exec("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+getCodArt()+"'");
                if(query.next())
                {
                    m_submit = true;
                    double qa = query.value(0).toDouble();
                    double pr_s = query.value(1).toDouble();
                    double pr_c = query.value(2).toDouble();
                    double tot_ = query.value(3).toDouble();
                    update_articoli(qa,pr_s,pr_c,tot_);
                    update_carico_prod(qa,pr_s,pr_c,tot_);
                    submit();
                }
                else{
                    m_submit = false;
                    insert_articoli();
                    insert_carico_magazzino();
                    submit();
                }
            }
    }

    return QObject::eventFilter(o,j);
}

void fatt_new::submit()
{
      mod_grid->database().transaction();
       if (mod_grid->submitAll()) {
           mod_grid->database().commit();
       } else {
           mod_grid->database().rollback();
           QMessageBox::warning(this, qApp->translate("fatt_new","FabariaGest"),
                                qApp->translate("fatt_new","Errore nel database: %1")
                                .arg(mod_grid->lastError().text()));
       }
       tot_imp_iva();
}

void fatt_new::update_articoli(double qua, double _p_siva, double _p_civa, double _tot)
{
	  
	//Imposto la quantità
	double quan = getQuantita().toDouble();
    double somma = qua+quan;
	//Aggiorno il prezzo senza iva 
	double ps_iva = getPrezzoSIVA().toDouble();
    double sum_pr_siva = _p_siva+ps_iva;
	//Aggiorno il prezzo con iva
	double pc_iva = getPrezzoCIVA().toDouble();
    double sum_pr_civa = _p_civa+pc_iva;
	//Aggiorno il totale
	double tot = getTotale().toDouble();
    double sum_tot = _tot+tot;
  
        QSqlQuery Query2;
        Query2.prepare("UPDATE articoli SET  codbarre='"+getCodBarre()+"',nome_articolo='"+getNomeArticolo()+"', "
                          "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+getDesc()+"', unita='"+getUnita()+"', " 
			  "prezzo_unitario='"+getPrezzoUnit()+"',prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(getIVA().toDouble())+"', quantita='"+QString::number(somma)+"',  "
                          "sconto='"+getSconto()+"', totale='"+QString::number(sum_tot)+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+getListino()+"', categoria='"+getCateg()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto='"+getNumeroLotto()+"', data_lotto='"+getDataLotto()+"', addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+getCodArt()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("fatt_new","Aggiornamento articoli da FATT_ACQ effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("fatt_new","Impossibile aggiornare articoli da FATT_ACQ: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

void fatt_new::update_carico_prod(double ca, double _p_siva, double _p_civa, double _tot)
{
	//Imposto la quantità
	double quan = getQuantita().toDouble();
        double somma = ca+quan;
	//Aggiorno il prezzo senza iva 
	double ps_iva = getPrezzoSIVA().toDouble();
        double sum_pr_siva = _p_siva+ps_iva;
	//Aggiorno il prezzo con iva
	double pc_iva = getPrezzoCIVA().toDouble();
        double sum_pr_civa = _p_civa+pc_iva;
	//Aggiorno il totale
	double tot = getTotale().toDouble();
        double sum_tot = _tot+tot;
  
        QSqlQuery Query2;
        Query2.prepare("UPDATE carico_magazzino SET  codbarre='"+getCodBarre()+"',nome_articolo='"+getNomeArticolo()+"', "
                          "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+getDesc()+"', unita='"+getUnita()+"', " 
			  "prezzo_unitario='"+getPrezzoUnit()+"',prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(getIVA().toDouble())+"', quantita='"+QString::number(somma)+"',  "
                          "sconto='"+getSconto()+"', totale='"+QString::number(sum_tot)+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+getListino()+"', categoria='"+getCateg()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"', "
              "numero_lotto='"+getNumeroLotto()+"', data_lotto='"+getDataLotto()+"', addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+getCodArt()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("fatt_new","Aggiornamento carico magazzino da FATT_ACQ effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("fatt_new","Impossibile aggiornare carico magazzino da FATT_ACQ: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

QString fatt_new::getFATT_ACQID()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colID = mod_grid->index(riga,0,QModelIndex());
    QString IDcol = mod_grid->data(colID).toString();
    return IDcol;
}

QString fatt_new::getCodArt()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCodArt = mod_grid->index(riga,1,QModelIndex());
    QString CodArtCol = mod_grid->data(colCodArt).toString();
    return CodArtCol;
}

QString fatt_new::getCodBarre()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCodBarre = mod_grid->index(riga,2,QModelIndex());
    QString CodBarreCol = mod_grid->data(colCodBarre).toString();
    return CodBarreCol;
}

QString fatt_new::getNomeArticolo()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colNomeArticolo = mod_grid->index(riga,3,QModelIndex());
    QString NomeArticoloCol = mod_grid->data(colNomeArticolo).toString();
    return NomeArticoloCol;
}

QString fatt_new::getDesc()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDesc = mod_grid->index(riga,4,QModelIndex());
    QString DescCol = mod_grid->data(colDesc).toString();
    return DescCol;
}

QString fatt_new::getUnita()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colUnita = mod_grid->index(riga,5,QModelIndex());
    QString UnitaCol = mod_grid->data(colUnita).toString();
    return UnitaCol;
}

QString fatt_new::getPrezzoUnit()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoUnit = mod_grid->index(riga,6,QModelIndex());
    QString PrezzoUnitCol = mod_grid->data(colPrezzoUnit).toString();
    return PrezzoUnitCol;
}

QString fatt_new::getPrezzoSIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoSIVA = mod_grid->index(riga,7,QModelIndex());
    QString PrezzoSIVACol = mod_grid->data(colPrezzoSIVA).toString();
    return PrezzoSIVACol;
}

QString fatt_new::getPrezzoCIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoCIVA = mod_grid->index(riga,8,QModelIndex());
    QString PrezzoCIVACol = mod_grid->data(colPrezzoCIVA).toString();
    return PrezzoCIVACol;
}

QString fatt_new::getIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colIVA = mod_grid->index(riga,9,QModelIndex());
    QString IVACol = mod_grid->data(colIVA).toString();
    return IVACol;
}

QString fatt_new::getQuantita()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colQuantita = mod_grid->index(riga,10,QModelIndex());
    QString QuantitaCol = mod_grid->data(colQuantita).toString();
    return QuantitaCol;
}

QString fatt_new::getSconto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colSconto = mod_grid->index(riga,11,QModelIndex());
    QString ScontoCol = mod_grid->data(colSconto).toString();
    return ScontoCol;
}

QString fatt_new::getTotale()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colTotale = mod_grid->index(riga,12,QModelIndex());
    QString TotaleCol = mod_grid->data(colTotale).toString();
    return TotaleCol;
}

QString fatt_new::getAnnoIns()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colAnnoIns = mod_grid->index(riga,13,QModelIndex());
    QString AnnoInsCol = mod_grid->data(colAnnoIns).toString();
    return AnnoInsCol;
}

QString fatt_new::getListino()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colListino = mod_grid->index(riga,14,QModelIndex());
    QString ListinoCol = mod_grid->data(colListino).toString();
    return ListinoCol;
}

QString fatt_new::getCateg()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCateg = mod_grid->index(riga,15,QModelIndex());
    QString CategCol = mod_grid->data(colCateg).toString();
    return CategCol;
}

QString fatt_new::getDataInsert()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,16,QModelIndex());
    QString DataInsertCol = mod_grid->data(colDataInsert).toString();
    return DataInsertCol;
}

QString fatt_new::getDataLotto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,18,QModelIndex());
    QString DataInsertLotto = mod_grid->data(colDataInsert).toString();
    return DataInsertLotto;
}

QString fatt_new::getNumeroLotto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,17,QModelIndex());
    QString numeroLotto = mod_grid->data(colDataInsert).toString();
    return numeroLotto;
}

void fatt_new::addRow()
{
    if(settingsManager->generalValue("Magazzino/nome",QVariant()).toString() != ""){
            QSqlQuery query_um,query_iva;
            query_um.exec("select unita from misura");
            query_iva.exec("select iva from aliquota");
            if(query_um.next() && query_iva.next()){
                int row = mod_grid->rowCount();
                QSqlRecord rec = mod_grid->record();
            
                rec.setValue(0,id->text());
                rec.setValue(5,query_um.value(0).toString());
                rec.setValue(9,query_iva.value(0).toString());
                rec.setValue(13,QString::number(QDate::currentDate().year()));
                rec.setValue(16,QDate::currentDate().toString("yyyy/MM/dd"));
                rec.setValue(18,QDate::currentDate().toString("yyyy/MM/dd"));
                rec.setValue(19,settingsManager->generalValue("Magazzino/nome",QVariant()).toString());
            mod_grid->insertRecord(row,rec);
            submit();
            }
             lista();
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_new","Devi creare almeno un deposito magazzino"));
        Magazzino *mg = new Magazzino(this);
        mg->exec();
    }
}

bool fatt_new::InserisciRiga()
{
    QString m_prezzo_s_iva,m_quantita,m_iva,m_sconto;
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex col_pr_unit = mod_grid->index(riga,6,QModelIndex());
    QModelIndex col_psiva = mod_grid->index(riga,7,QModelIndex());
    QModelIndex col_pciva = mod_grid->index(riga,8,QModelIndex());
    QModelIndex col_iva = mod_grid->index(riga,9,QModelIndex());
    QModelIndex col_quan = mod_grid->index(riga,10,QModelIndex());
    QModelIndex col_sconto = mod_grid->index(riga,11,QModelIndex());
    QModelIndex col_totale = mod_grid->index(riga,12,QModelIndex());
    m_prezzo_s_iva = mod_grid->data(col_psiva).toString();
    m_iva = mod_grid->data(col_iva).toString();
    m_quantita = mod_grid->data(col_quan).toString();
    m_sconto = mod_grid->data(col_sconto).toString();
    //Prezzo con iva comprese quantita
    if(m_iva.toDouble() == 0 )
    {
        mod_grid->setData(col_pciva,0);
        mod_grid->setData(col_totale,0);
        mod_grid->setData(col_sconto,0);
    }
    else if(m_quantita.toDouble() == 0){
        m_prezzo_c_iva = ((m_prezzo_s_iva.toDouble()*m_iva.toDouble()/100)+m_prezzo_s_iva.toDouble());
        mod_grid->setData(col_pciva,QString::number(m_prezzo_c_iva,'f',2));
        mod_grid->setData(col_totale,QString::number(m_prezzo_c_iva,'f',2));
        mod_grid->setData(col_psiva,mod_grid->data(col_pr_unit).toString());
    }
    else if(m_sconto.toDouble() == 0){
        m_prezzo_c_iva = (((m_prezzo_s_iva.toDouble()*m_iva.toDouble())/100)+m_prezzo_s_iva.toDouble())*m_quantita.toDouble();
        mod_grid->setData(col_pciva,QString::number(m_prezzo_c_iva,'f',2));
        mod_grid->setData(col_totale,QString::number(m_prezzo_c_iva,'f',2));
        mod_grid->setData(col_psiva,mod_grid->data(col_pr_unit).toString());
    }
    else
    {   //Calcolo lo sconto
        m_prezzo_c_iva = (((m_prezzo_s_iva.toDouble()*m_iva.toDouble())/100)+m_prezzo_s_iva.toDouble())*m_quantita.toDouble();
        m_prezzos_iva = (m_prezzo_s_iva.toDouble()*m_quantita.toDouble());
        m_perc = m_prezzo_c_iva-((m_prezzo_c_iva*m_sconto.toDouble())/100);
        m_totale = m_perc;
        mod_grid->setData(col_psiva,QString::number(m_prezzos_iva,'f',2));
        mod_grid->setData(col_pciva,QString::number(m_prezzo_c_iva,'f',2));
        mod_grid->setData(col_totale,QString::number(m_totale,'f',2));
    }
    tot_imp_iva();
    
    return true;
}
