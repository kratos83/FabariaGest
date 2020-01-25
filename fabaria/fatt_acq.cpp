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
#include "ui_fatt_acq.h"
#include "fatt_acq.h"
#include "fatt_new.h"
#include <QtGui>
#include <QtSql>
#include "print.h"
#include "articoli.h"
#include "settingsmanager.h"
#include "defines.h"
#include "queryexception.h"

fatt_acq *fattura_acquisto = 0;

using namespace Defines;

fatt_acq::fatt_acq(QWidget *parent) :
    QDialog(parent)
{

    fattura_acquisto = this;

    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);



    /*********************************************
     * Connessioni ai diversi metodi
     *********************************************/
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    esci->setShortcut(qApp->translate("fatt_acq","Ctrl+E"));
    connect(elimina,SIGNAL(clicked()),this,SLOT(elimina_fatt()));
    elimina->setShortcut(qApp->translate("fatt_acq","Ctrl+D"));
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(tab_view,SIGNAL(viewportEntered()),this,SLOT(clickgrid()));
    connect(tab_view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(aggiorna_fattura()));
    connect(modifica,SIGNAL(clicked()),this,SLOT(aggiorna_fattura()));
    modifica->setShortcut(qApp->translate("fatt_acq","Ctrl+M"));
    connect(stampa,SIGNAL(clicked()),this,SLOT(stampa_fatt()));
    stampa->setShortcut(qApp->translate("fatt_acq","Ctrl+P"));
    connect(cerca1,SIGNAL(textEdited(QString)),this,SLOT(cerca_dxt(const QString &)));
    connect(Aggiungi,SIGNAL(clicked()),this,SLOT(nuova_fattura()));
    Aggiungi->setShortcut(qApp->translate("fatt_acq","Ctrl+N"));
    connect(find_cer,SIGNAL(clicked()),this,SLOT(filtro()));
    find_cer->setShortcut(qApp->translate("fatt_acq","Ctrl+F"));

    /************************************************
     * Impostazione combBox cerca_txt
     ************************************************/
    QStringList list1;
    list1 << qApp->translate("fatt_acq","Cerca per ID") << qApp->translate("fatt_acq","Cerca per fornitore") << qApp->translate("fatt_acq","Cerca per tipo fattura");
    cerca_txt->addItems(list1);

    id->setVisible(false);
    textEdit->setReadOnly(true);
    mod_grid = new QSqlRelationalTableModel;

    model = new QSortFilterProxyModel;

    tab_view->setCurrentIndex(tab_view->currentIndex());
    lista();
    salva_fattura();
}

fatt_acq::~fatt_acq()
{
}

void fatt_acq::cerca_dxt(const QString &){
    if(cerca_txt->currentIndex() == 0){
    QString text1;
    text1 = cerca1->text();
    QStringList list_st;
    QSqlQuery querys("select id from fatture_acq");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    cerca1->setCompleter(complete);
    if(text1.length() == 0){
        lista();
    }
    }
    else if(cerca_txt->currentIndex() == 1){
        QString text2;
        text2 = cerca1->text();
        QStringList list_st1;
        QSqlQuery querys1("select fornitore from fatture_acq");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        cerca1->setCompleter(complete1);
        if(text2.length() == 0){
            lista();
        }
    }
    else if(cerca_txt->currentIndex() == 2){
        QString text3;
        text3 = cerca1->text();
        QStringList list_st2;
        QSqlQuery querys2("select tipo_fattura from fatture_acq");
        if(querys2.exec()){
        while(querys2.next()){
            list_st2 << querys2.value(0).toString();
        }
        }
        QCompleter *complete2 = new QCompleter(list_st2,this);
        complete2->setCaseSensitivity(Qt::CaseInsensitive);
        cerca1->setCompleter(complete2);
        if(text3.length() == 0){
            lista();
        }
    }
}

void fatt_acq::filtro(){
    if(cerca1->text().length() == 0){
        lista();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText("Avviso");
        MsgBox.setInformativeText(qApp->translate("fatt_acq","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(cerca_txt->currentIndex() == 0){
        mod_grid->setFilter(QString("id = '" +cerca1->text()+"'").arg(cerca1->text()));
    }
    else if(cerca_txt->currentIndex() == 1){
        mod_grid->setFilter(QString("fornitore = '" +cerca1->text()+"'").arg(cerca1->text()));
    }
    else if(cerca_txt->currentIndex() == 2){
        mod_grid->setFilter(QString("tipo_fattura = '" +cerca1->text()+"'").arg(cerca1->text()));
    }
    clickgrid();
}

void fatt_acq::nuova_fattura(){

    dg_acq = new dialog_fattura_acq(this);
    dg_acq->exec();
}


void fatt_acq::salva_fattura(){

    QSqlQuery query,query1;
    query.prepare("select count(id) from fatture_acq where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        QString text = QString::fromUtf8(qApp->translate("fatt_acq","Le fatture d'acquisto sono: ").toStdString().c_str());
	QString txt = query.value(0).toString();
	QString testo = text+txt;
        tot_fatt->setText(testo);
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_acq","Inizializzazione fallita... ")+query.lastError().text());
    }

    query1.prepare("select sum(totale) from fatture_acq where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query1.exec();
    if(query1.next()){
        QString txt = QString::fromUtf8(qApp->translate("fatt_acq","Il totale delle fatture d'acquisto è: ").toStdString().c_str());
	QString euro = QString::fromUtf8("\u20AC");
	QString text =query1.value(0).toString().toStdString().c_str();
	QString testo = txt+euro+" "+text;
        tot_eur->setText(testo);
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_acq","Inizializzazione fallita... ")+query1.lastError().text());
    }
}

void fatt_acq::aggiorna_fattura(){

    if(tab_view->selectionModel()->currentIndex().isValid()){
    QModelIndex modelIndex = tab_view->selectionModel()->currentIndex();
    int riga = modelIndex.row();
    QModelIndex newindex = modelIndex.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    agg_fatt(newindex);
    }
    else
    {
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_acq","Seleziona una riga da modificare..."));
    }
}

void fatt_acq::agg_fatt(QModelIndex index){

        if(index.row() < mod_grid->rowCount()){
        fatt_new *ft;
        QSqlQuery query;
        query.prepare("select * from fatture_acq_righe where id='"+id->text()+"'");
        query.exec();
        QSqlRecord rec = mod_grid->record(index.row());
        ft = new fatt_new(this,rec.value("tipo_fattura").toString(),id->text().toInt());
        if(query.next()){
        ft->data_fat->setDate(rec.value("data").toDate());
        ft->forn->setCurrentText(query.value(2).toString());
        ft->sconto_1->setText(query.value(3).toString());
        ft->sconto_2->setText(query.value(4).toString());
        ft->sconto_3->setText(query.value(5).toString());
        ft->sp_tr->setText(query.value(6).toString());
        ft->sp_in->setText(query.value(7).toString());
        ft->t_pagam->setCurrentText(query.value(8).toString());
        ft->n_fatt->setText(query.value(9).toString());
        ft->dateEdit->setDate(query.value(10).toDate());
        ft->textEdit->setText(query.value(11).toString());
	ft->tipo_listino->setCurrentText(query.value(16).toString());
        }
        ft->lista();
        ft->totale_impo_iva();
        lista();
        ft->formatta_num();
        ft->exec();
    }
    salva_fattura();

    lista();
}

void fatt_acq::elimina_fatt(){

    if(tab_view->selectionModel()->currentIndex().isValid()){
        if (!id->text().isEmpty())
        {
            // Si controlla che il cd_voce non sia usato in Spese o Budget

            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM fatture_acq WHERE id ='"+id->text()+"' LIMIT 1");
            qctrl.exec();


                    QMessageBox *box= new QMessageBox(this);
                    box->setWindowTitle("FabariaGest");
                    box->setInformativeText(qApp->translate("fatt_acq","Vuoi eliminare veramente \n il record selezionato?...."));
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
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_acq","Selezionare una riga da eliminare"));
    }

}

void fatt_acq::elimina_riga(){
    // Se si passano i controlli di cui sopra si procede all'eliminazione

    QSqlQuery query,query1,query2;

    query.prepare("DELETE FROM fatture_acq WHERE id = '"+id->text()+"' ");
    query1.prepare("DELETE FROM fatture_righe_acq_art WHERE id = '"+id->text()+"'");
    query2.prepare("DELETE FROM fatture_acq_righe where id='"+id->text()+"'");

    try{
    if (query.exec() & query1.exec() & query2.exec()) //Se esito OK Eliminato da DB
    {
        id->setText("");
    }
    else
    {
        LOG_FILE(qApp->translate("fatt_acq","Impossibile eliminare"),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(query.lastError().text().toStdString().c_str());
     }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    lista();
    salva_fattura();
    textEdit->setText("");
}



void fatt_acq::cerca(){

    QSqlQuery query;
    query.prepare("select * from fatture_acq where id='"+id->text()+"'");
    query.exec();
    if(query.next()){
        textEdit->setText(QString::fromUtf8(qApp->translate("fatt_acq","Informazioni della fattura d'acquisto: ").toStdString().c_str())+"\n"
                          +qApp->translate("fatt_acq","ID:")+query.value(0).toString()+"\n"
                          +qApp->translate("fatt_acq","Data: ")+query.value(1).toString()+"\n"
                          +qApp->translate("fatt_acq","Fornitore: ")+query.value(2).toString()+"\n"
                          +qApp->translate("fatt_acq","Tipo fattura: ")+query.value(3).toString()+"\n"
                          +qApp->translate("fatt_acq","Totale: ")+QString::fromUtf8("\u20AC")+" "+query.value(4).toString()+"\n");
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fatt_acq","Impossibile cercare...\n")+query.lastError().text());
    }
}

void fatt_acq::clickgrid(){

    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());
    id_new = mod_grid->data(elemento).toString();
    id->setText(id_new);
    cerca();
}

void fatt_acq::lista(){

    mod_grid->setTable("fatture_acq");
    mod_grid->setFilter("anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    model->setSourceModel(mod_grid);
    model->sort(0,Qt::AscendingOrder);

    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("fatt_acq","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("fatt_acq","Data"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("fatt_acq","Fornitore"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("fatt_acq","Tipo fattura"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("fatt_acq","Totale"));
    tab_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    tab_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab_view->setSelectionMode(QAbstractItemView::SingleSelection);
    tab_view->setSortingEnabled(true);
    tab_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab_view->verticalHeader()->hide();
    tab_view->setModel(model);
    mod_grid->select();
    tab_view->setColumnHidden(5,true);
    tab_view->setColumnHidden(6,true);
    salva_fattura();
}

void fatt_acq::stampa_fatt(){

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("fatt_acq","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview.setWindowIcon(icon);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_view(QPrinter*)));
    preview.exec();
}

void fatt_acq::preview_view(QPrinter *printer){

    Q_UNUSED(printer);
    Stampe *pr = new Stampe();
    pr->stampafattacq(printer,mod_grid);
}
