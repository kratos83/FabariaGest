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
/*************************************
 *Intestazione Qt4
 ************************************/
#include <QCompleter>


/*************************************
 *Intestazione fattura_rg
 ************************************/
#include "fattura_rg.h"
#include "ui_fattura_rg.h"

#include "mainwindow.h"
#include "user.h"
#include "agenti.h"
#include "banche.h"
#include "fornitori.h"
#include "itdelegato.h"
#include "fattura_rg_art.h"
#include "fattura.h"
#include "print.h"
#include "defines.h"
#include "IconFileProvider.h"
#include "queryexception.h"
#include "LineEditDelegate.h"
#include <iostream>
#include "magazzino.h"

using namespace Defines;
using namespace std;

fattura_rg::fattura_rg(QWidget *parent) :
    QDialog(parent),
    prezzos_iva(0),
    prezzo_c_iva(0),
    perc(0),
    totale(0)
{
    interfaccia();
}

fattura_rg::fattura_rg(QWidget *wig, QString fatt, int id_ft) : 
    QDialog(wig),
    prezzos_iva(0),
    prezzo_c_iva(0),
    perc(0),
    totale(0)
{
    interfaccia();
    tipo_fattura = fatt;
    idfatt_new = id_ft;
    id1->setText(QString::number(idfatt_new));
    setWindowTitle(tipo_fattura);
}

void fattura_rg::interfaccia(){
    setupUi(this);
    connect(annulla_2,SIGNAL(clicked()),this,SLOT(chiudi()));
    connect(cliente,SIGNAL(clicked()),this,SLOT(open_cliente()));
    connect(fornit,SIGNAL(clicked()),this,SLOT(open_fornitore()));
    connect(tipo_cliente,SIGNAL(currentIndexChanged(int)),this,SLOT(filtro()));
    connect(banca,SIGNAL(clicked()),this,SLOT(bank()));
    connect(vis_banca,SIGNAL(currentIndexChanged(int)),this,SLOT(open_banca(int)));
    connect(n_riga,SIGNAL(clicked()),this,SLOT(fattura_riga_open()));
    connect(md_riga,SIGNAL(clicked()),this,SLOT(modifica_riga()));
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(add_row,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid_add_row()));
    connect(tab_view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(modifica_riga()));
    connect(fatt_insert_row,SIGNAL(clicked()),this,SLOT(addRow()));
    connect(add_row,SIGNAL(clicked(QModelIndex)),this,SLOT(InserisciRiga()));
    connect(spes_tr,SIGNAL(textEdited(const QString &)),this,SLOT(spesa_trasp(const QString &)));
    connect(spes_in,SIGNAL(textEdited(const QString &)),this,SLOT(spesa_in(const QString &)));
    connect(sconto1,SIGNAL(textEdited(const QString &)),this,SLOT(sconto_1t(const QString &)));
    connect(sconto2,SIGNAL(textEdited(const QString &)),this,SLOT(sconto_2t(const QString &)));
    connect(sconto3,SIGNAL(textEdited(const QString &)),this,SLOT(sconto_3t(const QString &)));
    connect(el_riga,SIGNAL(clicked()),this,SLOT(elimina()));
    connect(del_riga,SIGNAL(clicked()),this,SLOT(elimina()));
    connect(save_2,SIGNAL(clicked()),this,SLOT(fatt_inserisci()));
    connect(agg_fatt_2,SIGNAL(clicked()),this,SLOT(fatt_aggiorna()));
    connect(pagam,SIGNAL(currentIndexChanged(int)),this,SLOT(tipo_pagamento()));
    connect(file_allegato_2,SIGNAL(clicked()),this,SLOT(open_file()));
    connect(del_file_2,SIGNAL(clicked()),this,SLOT(erase_file()));
    connect(agg_tot,SIGNAL(clicked()),this,SLOT(tot_imp_iva()));
    connect(agente,SIGNAL(clicked()),this,SLOT(open_agenti()));
    connect(lineEdit_2,SIGNAL(currentIndexChanged(int)),this,SLOT(open_agenti(int)));
    //Importo righe da file excel 
    excel->setShortcut(qApp->translate("fattura_rg","Ctrl+I"));
    connect(excel,SIGNAL(clicked()),this,SLOT(importa()));
    connect(im_ex_file,SIGNAL(clicked()),this,SLOT(importa()));
    cliente->setToolTip(qApp->translate("fattura_rg","Inserisci cliente"));
    fornit->setToolTip(qApp->translate("fattura_rg","Inserisci fornitore"));
    QStringList lis;
    lis << qApp->translate("fattura_rg","Cliente") << qApp->translate("fattura_rg","Fornitore");
    tipo_cliente->addItems(lis);
    //Setting data
    data_fatt->setCalendarPopup(true);
    data_fatt->setDate(QDate::currentDate());

    data_ora->setCalendarPopup(true);
    data_ora->setDateTime(QDateTime::currentDateTime());
    /************************
     * Avviare le istanze da lavorare
     ************************/
    mod_grid = new QSqlRelationalTableModel(this);
    local_settings = new QLocale(QLocale::system());
    
    lista_file_2->setViewMode(QListWidget::IconMode);
    lista_file_2->sortItems(Qt::AscendingOrder);
    add_row->viewport()->installEventFilter(this);
    //Enable combobox text
    add_banca();
    filtro_banca();
    pagamento_fatt();
    lista_prodotti();
    visual_scadenze();
    trasp_causali();
    porto();
    aspetto_beni();
    consegna_trasp();
    vis_allegati();
    listino();
    combo_agenti();
}

void fattura_rg::formatta_num(){

    QString sc1 = QString::number(sconto1->text().toDouble(),'f',2);
    sconto1->setText(sc1);
    QString sc2 = QString::number(sconto2->text().toDouble(),'f',2);
    sconto2->setText(sc2);
    QString sc3 = QString::number(sconto3->text().toDouble(),'f',2);
    sconto3->setText(sc3);
    QString spese_tr = QString::number(spes_tr->text().toDouble(),'f',2);
    spes_tr->setText(spese_tr);
    QString spese_in = QString::number(spes_in->text().toDouble(),'f',2);
    spes_in->setText(spese_in);
    QString tot = QString::number(totale_2->text().toDouble(),'f',2 );
    totale_2->setText(tot);
    QString iva = QString::number(iva_ft_2->text().toDouble(),'f',2 );
    iva_ft_2->setText(iva);
    QString imp = QString::number(imponibile_2->text().toDouble(),'f',2);
    imponibile_2->setText(imp);
    QString pr_ag = QString::number(lineEdit_3->text().toDouble(),'f',2);
    lineEdit_3->setText(pr_ag);
}

void fattura_rg::filtro(){
    if(tipo_cliente->currentIndex() == 0){
        connect(pers,SIGNAL(currentIndexChanged(int)),this,SLOT(open_cl(int)));
        cliente_add();
        fornit->setEnabled(false);
        cliente->setEnabled(true);
    }
    else if(tipo_cliente->currentIndex() == 1){
        connect(pers,SIGNAL(currentIndexChanged(int)),this,SLOT(open_fl(int)));
        combo_fornitore();
        cliente->setEnabled(false);
        fornit->setEnabled(true);
    }
}

void fattura_rg::filtro_banca(){
        connect(vis_banca,SIGNAL(currentIndexChanged(int)),this,SLOT(open_banca(int)));
        add_banca();
}

void fattura_rg::trasp_causali()
{
    causali->clear();
    QStringList list;
    QSqlQuery query("select * from causali");
    if(query.exec()){
        while(query.next()){
            list << query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(causali->model());
    causali->setCompleter(complete);
    causali->addItems(list);
}

void fattura_rg::listino()
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

void fattura_rg::consegna_trasp(){

    consegna->clear();
    QStringList list;
    QSqlQuery query("select * from consegna");
    if(query.exec()){
        while(query.next()){
            list << query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(consegna->model());
    consegna->setCompleter(complete);
    consegna->addItems(list);
}

void fattura_rg::porto(){
    porto_ass->clear();
    QStringList list;
    QSqlQuery query("select * from porto");
    if(query.exec()){
        while(query.next()){
            list << query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(porto_ass->model());
    porto_ass->setCompleter(complete);
    porto_ass->addItems(list);
}

void fattura_rg::aspetto_beni()
{
    aspetto_est->clear();
    QStringList list;
    QSqlQuery query("select * from aspetto_esteriore");
    if(query.exec()){
        while(query.next()){
            list << query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(aspetto_est->model());
    aspetto_est->setCompleter(complete);
    aspetto_est->addItems(list);
}

void fattura_rg::open_cliente(){
    user *utente = new user(this);
    utente->setModal(true);
    utente->nuovo();
    connect(utente,SIGNAL(clienti_rel()),this,SLOT(cliente_add()));
    utente->exec();
}

void fattura_rg::bank(){
    banche *bc = new banche(this);
    bc->setModal(true);
    bc->nuovo();
    connect(bc,SIGNAL(banca_add()),this,SLOT(add_banca()));
    bc->exec();
}

void fattura_rg::add_banca(){


    vis_banca->clear();
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
    complete->setModel(vis_banca->model());
    vis_banca->setCompleter(complete);
    vis_banca->addItems(list);

}

int fattura_rg::open_banca(int index)
{
    /**********************************************************
    * Visualizza i dati nella regione
    * antistante a destra.
    **********************************************************/
    QSqlQuery query("select * from banca where id='"+QString::number(1+index)+"'");
    if(query.exec()){
    while(query.next()){
        iban->setText(query.value(1).toString());
    }
    }
    return index;
}

void fattura_rg::cliente_add(){

    pers->clear();
    QStringList list;
    QSqlQuery query("select * from clienti");
    if(query.exec()){
    while(query.next()){
        list << query.value(1).toString() + " " + query.value(2).toString();
    }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(pers->model());
    pers->setCompleter(complete);
    pers->addItems(list);

}

int fattura_rg::open_cl(int index){

    /**********************************************************
     * Visualizza i dati nella regione
     * antistante a destra.
     **********************************************************/
    QSqlQuery query("select * from clienti where id='"+QString::number(1+index)+"'");
    if(query.exec()){
    while(query.next()){

        QString ragione = query.value(1).toString() +" "+query.value(2).toString();
        rag_soc->setText(ragione);
        indirizzo->setText(query.value(3).toString());
        tel->setText(query.value(4).toString());
        paese->setText(query.value(6).toString()+" "+query.value(7).toString());
        cell->setText(query.value(8).toString());
    }
    }
    return index;
}

void fattura_rg::open_fornitore(){
    fornitori *fo = new fornitori(this);
    fo->setModal(true);
    fo->nuovo();

    connect(fo,SIGNAL(vis_forn()),this,SLOT(combo_fornitore()));
    fo->exec();
}

void fattura_rg::combo_fornitore(){

    pers->clear();
    QStringList list;
    QSqlQuery query("select * from fornitori");
    if(query.exec()){
    while(query.next()){
        list << query.value(1).toString();
    }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(pers->model());
    pers->setCompleter(complete);
    pers->addItems(list);
}

int fattura_rg::open_fl(int index)
{

    /**********************************************************
     * Visualizza i dati nella regione
     * antistante a destra.
     **********************************************************/

    QSqlQuery query("select * from fornitori where id='"+QString::number(1+index)+"'");
    if(query.exec()){
    while(query.next()){
        rag_soc->setText(query.value(1).toString());
        indirizzo->setText(query.value(4).toString());
        cap->setText(query.value(5).toString());
        local->setText(query.value(6).toString());
        prov->setText(query.value(7).toString());
        tel->setText(query.value(8).toString());
        cell->setText(query.value(9).toString());
        paese->setText(query.value(2).toString());
        }
    }
    return index;
}

void fattura_rg::open_agenti()
{
    agenti *ins_agenti = new agenti(this);
    ins_agenti->setModal(true);
    ins_agenti->nuovo();
    
    connect(ins_agenti,SIGNAL(save_agent()),this,SLOT(combo_agenti()));
    ins_agenti->exec();
}

int fattura_rg::open_agenti(int index)
{
    /**********************************************************
    * Visualizza i dati nella regione
    * antistante a destra.
    **********************************************************/
    QSqlQuery query("select * from lista_agenti where id='"+QString::number(index)+"'");
    if(query.exec()){
    while(query.next()){
        lineEdit_3->setText(query.value(10).toString());
    }
    }
    return index;
}

void fattura_rg::combo_agenti()
{
    lineEdit_2->clear();
    QStringList list;
    QSqlQuery query("select * from lista_agenti ");
    if(query.exec()){
        list << " ";
    while(query.next()){
        list << query.value(1).toString() + " " + query.value(2).toString();
    }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(lineEdit_2->model());
    lineEdit_2->setCompleter(complete);
    lineEdit_2->addItems(list);
}

void fattura_rg::pagamento_fatt()
{
    pagam->clear();
    QStringList list;
    QSqlQuery query("select * from pagam");
    if(query.exec()){
    while(query.next()){
        list << query.value(1).toString();
    }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(pagam->model());
    pagam->setCompleter(complete);
    pagam->addItems(list);
}

void fattura_rg::lista_prodotti()
{
    mod_grid->setTable("fatture_righe_vendita_art");
    mod_grid->setFilter("id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    mod_grid->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mod_grid->select();
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("fattura_rg","Codice"));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("fattura_rg","Cod. Barre"));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("fattura_rg","Prodotto"));
    mod_grid->setHeaderData(4, Qt::Horizontal, qApp->translate("fattura_rg","Descrizione"));
    mod_grid->setHeaderData(5, Qt::Horizontal, QString::fromUtf8(qApp->translate("fattura_rg","Unità").toStdString().c_str()));
    mod_grid->setHeaderData(6, Qt::Horizontal, qApp->translate("fattura_rg","Prezzo unitario"));
    mod_grid->setHeaderData(7, Qt::Horizontal, qApp->translate("fattura_rg","Prezzo S. IVA"));
    mod_grid->setHeaderData(8, Qt::Horizontal, qApp->translate("fattura_rg","Prezzo C. IVA"));
    mod_grid->setHeaderData(9, Qt::Horizontal, qApp->translate("fattura_rg","IVA"));
    mod_grid->setHeaderData(10, Qt::Horizontal, QString::fromUtf8(qApp->translate("fattura_rg","Quantità").toStdString().c_str()));
    mod_grid->setHeaderData(11, Qt::Horizontal, qApp->translate("fattura_rg","Sconto"));
    mod_grid->setHeaderData(12,Qt::Horizontal,qApp->translate("fattura_rg","Totale"));
    mod_grid->setHeaderData(17,Qt::Horizontal,qApp->translate("fattura_rg","Lotto"));
    mod_grid->setHeaderData(18,Qt::Horizontal,qApp->translate("fattura_rg","Scadenza lotto"));
    mod_grid->setHeaderData(19,Qt::Horizontal,qApp->translate("fattura_rg","Magazzino"));
    tab_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    tab_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab_view->setSelectionMode(QAbstractItemView::SingleSelection);
    tab_view->setSortingEnabled(true);
    tab_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab_view->verticalHeader()->hide();
    tab_view->setModel(mod_grid);
    tab_view->setAlternatingRowColors(true);
    tab_view->setColumnHidden(0,true);
    tab_view->setColumnHidden(13,true);
    tab_view->setColumnHidden(14,true);
    tab_view->setColumnHidden(15,true);
    tab_view->setColumnHidden(16,true);
    tab_view->setItemDelegateForColumn(16, new coldxdelegato(this));
    
    //Seconda tabella
    add_row->setSelectionBehavior(QAbstractItemView::SelectRows);
    add_row->setSelectionMode(QAbstractItemView::SingleSelection);
    add_row->setEditTriggers(QAbstractItemView::DoubleClicked);
    add_row->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    add_row->setSortingEnabled(true);
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
    
    tot_imp_iva();
}


void fattura_rg::clickgrid()
{
    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
}

void fattura_rg::clickgrid_add_row()
{
    QString id_new;
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
}

void fattura_rg::fattura_riga_open()
{

    fattura_rg_art *ft = new fattura_rg_art(this,tipo_fattura,idfatt_new);
    ft->setModal(true);
    ft->setWindowTitle(qApp->translate("fattura_rg","Inserisci articoli"));
    connect(ft,SIGNAL(save_fatt()),this,SLOT(lista_prodotti()));
    connect(ft,SIGNAL(save_fatt()),this,SLOT(tot_imp_iva()));
    ft->exec();
    lista_prodotti();
}

void fattura_rg::fatt_aggiorna()
{
    if(pers->currentText()=="" && iban->text()=="" && vis_banca->currentText()=="" ){
            QMessageBox MsgBox;
            MsgBox.setWindowTitle("FabariaGest");
            MsgBox.setText(qApp->translate("fattura_rg","Errore"));
            MsgBox.setInformativeText(QString::fromUtf8(qApp->translate("fattura_rg","Impossibile aggiornare la fattura,\n poichè non hai inserito i dati correttamente...").toStdString().c_str()));
            MsgBox.setIcon(QMessageBox::Warning);
            MsgBox.exec();
            pers->setStyleSheet("background-color: rgb(249, 22, 5)");
            iban->setStyleSheet("background-color: rgb(249, 22, 5)");
            vis_banca->setStyleSheet("background-color: rgb(249, 22, 5)");
        }
        else{
            QSqlQuery query;
            query.prepare("UPDATE fatture_vendita_righe SET data=:data,fornitore=:fornitore,tipo_cliente=:tipo_cliente,sconto_1=:sconto_1,"
                          " sconto_2=:sconto_2,sconto_3=:sconto_3,spese_trasporto=:spese_trasporto,spese_incasso=:spese_incasso,annotazioni=:annotazioni,vs_ordine=:vs_ordine,tipo_pagamento=:tipo_pagamento,"
                          " note_pagamento=:note_pagamento,rag_soc=:rag_soc,indirizzo=:indirizzo,cap=:cap,localita=:localita,prov=:prov,telefono=:telefono,"
                          " fax=:fax, piva_cfis=:piva_cfis,banca=:banca,iban=:iban,agente=:agente,provvigione=:provvigione,prezzo_s_iva=:prezzo_s_iva,"
                          " iva=:iva,totale=:totale,scadenza_1=:scadenza_1,scadenza_2=:scadenza_2,scadenza_3=:scadenza_3,importo_1=:importo_1,importo_2=:importo_2,importo_3=:importo_3, "
                          " causale_trasporto=:causale_trasporto, asp_est=:asp_est, vettore=:vettore,cons_trasp=:cons_trasp, porto=:porto, n_colli=:n_colli, peso_lordo=:peso_lordo, data_ora=:data_ora, "
			  " peso_netto=:peso_netto,anno_ins=:anno_ins,codice_listino=:codice_listino "
                          " WHERE id =:id");

            query.bindValue(":id",idfatt_new);
            query.bindValue(":data",data_fatt->date());
            query.bindValue(":fornitore",pers->currentText());
            query.bindValue(":tipo_cliente",tipo_cliente->currentIndex());
            query.bindValue(":sconto_1",sconto1->text());
            query.bindValue(":sconto_2",sconto2->text());
            query.bindValue(":sconto_3",sconto3->text());
            query.bindValue(":spese_trasporto",spes_tr->text());
            query.bindValue(":spese_incasso",spes_in->text());
            query.bindValue(":annotazioni",ann->toPlainText());
            query.bindValue(":vs_ordine",ordine->text());
            query.bindValue(":tipo_pagamento",pagam->currentText());
            query.bindValue(":note_pagamento",lineEdit->text());
            query.bindValue(":rag_soc",rag_soc->text());
            query.bindValue(":indirizzo",indirizzo->text());
            query.bindValue(":cap",cap->text());
            query.bindValue(":localita",local->text());
            query.bindValue(":prov",prov->text());
            query.bindValue(":telefono",tel->text());
            query.bindValue(":fax",cell->text());
            query.bindValue(":piva_cfis",paese->text());
            query.bindValue(":banca",vis_banca->currentText());
            query.bindValue(":iban",iban->text());
            query.bindValue(":agente",lineEdit_2->currentText());
            query.bindValue(":provvigione",lineEdit_3->text());
            query.bindValue(":prezzo_s_iva",imponibile_2->text());
            query.bindValue(":iva",iva_ft_2->text());
            query.bindValue(":totale",totale_2->text());
            query.bindValue(":anno_ins",QDate::currentDate().year());
	    query.bindValue(":codice_listino",tipo_listino->currentText());

            if(scadenza_1->isVisible() && !scadenza_2->isVisible() && !scadenza_3->isVisible()){
            query.bindValue(":scadenza_1",scadenza_1->date());
            query.bindValue(":importo_1",totale_2->text());
            }
            else if(scadenza_1->isVisible() && scadenza_2->isVisible() && !scadenza_3->isVisible()){
                query.bindValue(":scadenza_1",scadenza_1->date());
                query.bindValue(":scadenza_2",scadenza_2->date());
                double div = totale_2->text().toDouble()/2;
                query.bindValue(":importo_1",div);
                query.bindValue(":importo_2",div);
            }
            else if(scadenza_1->isVisible() && scadenza_2->isVisible() && scadenza_3->isVisible()){
                query.bindValue(":scadenza_1",scadenza_1->date());
                query.bindValue(":scadenza_2",scadenza_2->date());
                query.bindValue(":scadenza_3",scadenza_3->date());
                double div = totale_2->text().toDouble()/3;
                query.bindValue(":importo_1",div);
                query.bindValue(":importo_2",div);
                query.bindValue(":importo_3",div);
            }
            else if(!scadenza_1->isVisible() && !scadenza_2->isVisible() && !scadenza_3->isVisible()){
                query.bindValue(":scadenza_1","0000-00-00");
                query.bindValue(":scadenza_2","0000-00-00");
                query.bindValue(":scadenza_3","0000-00-00");
                query.bindValue(":importo_1","0.00");
                query.bindValue(":importo_2","0.00");
                query.bindValue(":importo_3","0.00");
            }

            if(tipo_fattura == qApp->translate("fattura_rg","Fattura immediata") || tipo_fattura == qApp->translate("fattura_rg","Fattura proforma") || tipo_fattura == qApp->translate("fattura_rg","Nota di credito")  || tipo_fattura == qApp->translate("fattura_rg","Scontrino")){
                query.bindValue(":causale_trasporto","");
                query.bindValue(":asp_est","");
                query.bindValue(":vettore","");
                query.bindValue(":cons_trasp","");
                query.bindValue(":porto","");
                query.bindValue(":n_colli","0.00");
		query.bindValue(":peso_lordo","0.00");
		query.bindValue(":data_ora","0000-00-00 00:00:00");
		query.bindValue(":peso_netto","0.00");
            }
            else if(tipo_fattura == qApp->translate("fattura_rg","Fattura accompagnatoria")){
                query.bindValue(":causale_trasporto",causali->currentText());
                query.bindValue(":asp_est",aspetto_est->currentText());
                query.bindValue(":vettore",vettore->currentText());
                query.bindValue(":cons_trasp",consegna->currentText());
                query.bindValue(":porto",porto_ass->currentText());
                query.bindValue(":n_colli",num_colli->text());
                query.bindValue(":peso_lordo",peso_lordo->text());
                query.bindValue(":data_ora",data_ora->dateTime());
                query.bindValue(":peso_netto",peso_netto->text());
            }
	  try{
            if( query.exec()){
                update_f_prodotti();
		aggiorna_agenti(lineEdit_2->currentText(),data_fatt->date());
		LOG_FILE(qApp->translate("fattura_rg","Aggiornamento effettuato correttamente(fattura_rg::fatt_aggiorna)"),LOG_INFO_ALL,QIODevice::Append);
            }
            else{
                LOG_FILE(qApp->translate("fattura_rg","Impossibile aggiornare(fattura_rg::fatt_aggiorna)"),LOG_ERROR_ALL,QIODevice::Append);
                LOG_FILE("(fattura_rg::fatt_aggiorna): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                throw InvalidQuery(query.lastError().text().toStdString().c_str());
            }
	  }
	  catch(FabariaException e)
	  {
	     LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	  }
            lista_prodotti();
            emit salvafattura();
            fatt_vendita->lista_prodotti();
            close();
            }
}

void fattura_rg::fatt_inserisci()
{
    if(pers->currentText()=="" || iban->text()=="" || vis_banca->currentText()=="" ){
            QMessageBox MsgBox;
            MsgBox.setWindowTitle("FabariaGest");
            MsgBox.setText(qApp->translate("fattura_rg","Errore"));
            MsgBox.setInformativeText(QString::fromUtf8(qApp->translate("fattura_rg","Impossibile inserire la fattura,\n poichè non hai inserito i dati correttamente...").toStdString().c_str()));
            MsgBox.setIcon(QMessageBox::Warning);
            MsgBox.exec();
            pers->setStyleSheet("background-color: rgb(249, 22, 5)");
            iban->setStyleSheet("background-color: rgb(249, 22, 5)");
            vis_banca->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else{
    QSqlQuery query;
    query.prepare("INSERT INTO fatture_vendita_righe (id,data,fornitore,tipo_cliente,sconto_1,sconto_2,sconto_3,spese_trasporto,spese_incasso,annotazioni,vs_ordine,tipo_pagamento,note_pagamento,rag_soc,indirizzo,cap,localita,prov,telefono,fax,piva_cfis,banca,iban,agente,provvigione,prezzo_s_iva,iva,totale,scadenza_1,scadenza_2,scadenza_3,importo_1,importo_2,importo_3,causale_trasporto,asp_est,vettore,cons_trasp,porto,n_colli,peso_lordo,data_ora,peso_netto,anno_ins,codice_listino)"
                  " VALUES (:id,:data,:fornitore,:tipo_cliente,:sconto_1,:sconto_2,:sconto_3,:spese_trasporto,:spese_incasso,:annotazioni,:vs_ordine,:tipo_pagamento,:note_pagamento,:rag_soc,:indirizzo,:cap,:localita,:prov,:telefono,:fax,:piva_cfis,:banca,:iban,:agente,:provvigione,:prezzo_s_iva,:iva,:totale,:scadenza_1,:scadenza_2,:scadenza_3,:importo_1,:importo_2,:importo_3,:causale_trasporto,:asp_est,:vettore,:cons_trasp,:porto,:n_colli,:peso_lordo,:data_ora,:peso_netto,:anno_ins,:codice_listino) ");

    query.bindValue(":id",idfatt_new);
    query.bindValue(":data",data_fatt->date());
    query.bindValue(":fornitore",pers->currentText());
    query.bindValue(":tipo_cliente",tipo_cliente->currentIndex());
    query.bindValue(":sconto_1",sconto1->text());
    query.bindValue(":sconto_2",sconto2->text());
    query.bindValue(":sconto_3",sconto3->text());
    query.bindValue(":spese_trasporto",spes_tr->text());
    query.bindValue(":spese_incasso",spes_in->text());
    query.bindValue(":annotazioni",ann->toPlainText());
    query.bindValue(":vs_ordine",ordine->text());
    query.bindValue(":tipo_pagamento",pagam->currentText());
    query.bindValue(":note_pagamento",lineEdit->text());
    query.bindValue(":rag_soc",rag_soc->text());
    query.bindValue(":indirizzo",indirizzo->text());
    query.bindValue(":cap",cap->text());
    query.bindValue(":localita",local->text());
    query.bindValue(":prov",prov->text());
    query.bindValue(":telefono",tel->text());
    query.bindValue(":fax",cell->text());
    query.bindValue(":piva_cfis",paese->text());
    query.bindValue(":banca",vis_banca->currentText());
    query.bindValue(":iban",iban->text());
    query.bindValue(":agente",lineEdit_2->currentText());
    query.bindValue(":provvigione",lineEdit_3->text());
    query.bindValue(":prezzo_s_iva",imponibile_2->text());
    query.bindValue(":iva",iva_ft_2->text());
    query.bindValue(":totale",totale_2->text());
    query.bindValue(":anno_ins",QDate::currentDate().year());
    query.bindValue(":codice_listino",tipo_listino->currentText());

    if(scadenza_1->isVisible() && !scadenza_2->isVisible() && !scadenza_3->isVisible()){
    query.bindValue(":scadenza_1",scadenza_1->date());
    query.bindValue(":importo_1",totale_2->text());
        scadenza1();
        form->cl1->clear();
        form->trayIcon->setVisible(false);
        form->reload_data();
    }
    else if(scadenza_1->isVisible() && scadenza_2->isVisible() && !scadenza_3->isVisible()){
        query.bindValue(":scadenza_1",scadenza_1->date());
        query.bindValue(":scadenza_2",scadenza_2->date());
        double div = totale_2->text().toDouble()/2;
        query.bindValue(":importo_1",div);
        query.bindValue(":importo_2",div);
        scadenza2();
        form->cl1->clear();
        form->trayIcon->setVisible(false);
        form->reload_data();
    }
    else if(scadenza_1->isVisible() && scadenza_2->isVisible() && scadenza_3->isVisible()){
        query.bindValue(":scadenza_1",scadenza_1->date());
        query.bindValue(":scadenza_2",scadenza_2->date());
        query.bindValue(":scadenza_3",scadenza_3->date());
        double div = totale_2->text().toDouble()/3;
        query.bindValue(":importo_1",div);
        query.bindValue(":importo_2",div);
        query.bindValue(":importo_3",div);
        scadenza3();
        form->cl1->clear();
        form->trayIcon->setVisible(false);
        form->reload_data();
    }
    else if(!scadenza_1->isVisible() && !scadenza_2->isVisible() && !scadenza_3->isVisible()){
        query.bindValue(":scadenza_1","0000-00-00");
        query.bindValue(":scadenza_2","0000-00-00");
        query.bindValue(":scadenza_3","0000-00-00");
        query.bindValue(":importo_1","0.00");
        query.bindValue(":importo_2","0.00");
        query.bindValue(":importo_3","0.00");
    }

    if(tipo_fattura == qApp->translate("fattura_rg","Fattura immediata") || tipo_fattura == qApp->translate("fattura_rg","Fattura proforma") ||
            tipo_fattura == qApp->translate("fattura_rg","Nota di credito") || tipo_fattura == qApp->translate("fattura_rg","Scontrino")){
        query.bindValue(":causale_trasporto","");
        query.bindValue(":asp_est","");
        query.bindValue(":vettore","");
        query.bindValue(":cons_trasp","");
        query.bindValue(":porto","");
        query.bindValue(":n_colli","0.00");
        query.bindValue(":peso_lordo","0.00");
        query.bindValue(":data_ora","0000-00-00 00:00:00");
        query.bindValue(":peso_netto","0.00");
    }
    else if(tipo_fattura == qApp->translate("fattura_rg","Fattura accompagnatoria")){
        query.bindValue(":causale_trasporto",causali->currentText());
        query.bindValue(":asp_est",aspetto_est->currentText());
        query.bindValue(":vettore",vettore->currentText());
        query.bindValue(":cons_trasp",consegna->currentText());
        query.bindValue(":porto",porto_ass->currentText());
        query.bindValue(":n_colli",num_colli->text());
        query.bindValue(":peso_lordo",peso_lordo->text());
        query.bindValue(":data_ora",data_ora->dateTime());
        query.bindValue(":peso_netto",peso_netto->text());
    }

    try{
            if( query.exec()){
               	insert_f_prodotti();
		inserisci_agenti(lineEdit_2->currentText(),data_fatt->date());
		LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::fatt_inserisci)"),LOG_INFO_ALL,QIODevice::Append);
            }
            else{
                LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::fatt_inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
                LOG_FILE("(fattura_rg::fatt_inserisci): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                throw InvalidQuery(query.lastError().text().toStdString().c_str());
            }
	  }
	  catch(FabariaException e)
	  {
	     LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	  }
    emit salvafattura();
    tot_imp_iva();
    close();
    fatt_vendita->lista_prodotti();
    }

}

void fattura_rg::inserisci_allegati(QString m_name, int riga)
{
  QSqlQuery qquery_allegati;
  qquery_allegati.prepare("INSERT INTO allegati_fattura"
				 " VALUES('"+QString::number(idfatt_new)+"','"+m_name+"','"+QString(m_name.toLatin1().toBase64())+"','"+QString::number(riga)+"')");

  try{
  if(qquery_allegati.exec())
  {
    LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::inserisci_allegati)"),LOG_INFO_ALL,QIODevice::Append);
  }
  else{
    LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::inserisci_allegati)"),LOG_ERROR_ALL,QIODevice::Append);
    LOG_FILE("(fattura_rg::inserisci_allegati): "+qquery_allegati.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
    throw InvalidQuery(qquery_allegati.lastError().text().toStdString().c_str());
  }
  }
  catch(FabariaException e)
  {
    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }
  
}

void fattura_rg::insert_f_prodotti()
{
    QSqlQuery query1;
    query1.prepare("INSERT INTO fattura_vendita "
                   " VALUES('"+QString::number(idfatt_new)+"','"+data_fatt->date().toString("yyyy-MM-dd")+"',"
		   "'"+pers->currentText()+"','"+tipo_fattura+"','"+totale_2->text()+"',"
		   "'"+QString::number(QDate::currentDate().year())+"','"+tipo_listino->currentText()+"')");
  try{
    if(query1.exec()){
        LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::insert_f_prodotti)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
        LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::insert_f_prodotti)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(fattura_rg::insert_f_prodotti): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query1.lastError().text().toStdString().c_str());
    }
  }
  catch(FabariaException e)
  {
    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }
}


////////////////////////////////////////////////////////////////////////
void fattura_rg::update_f_prodotti()
{
    QSqlQuery query1;
    query1.prepare("UPDATE fattura_vendita SET data='"+data_fatt->date().toString("yyyy-MM-dd")+"',fornitore='"+pers->currentText()+"',totale='"+totale_2->text()+"',"
		   "tipo_fattura='"+tipo_fattura+"',anno_ins='"+QString::number(QDate::currentDate().year())+"',codice_listino='"+tipo_listino->currentText()+"' WHERE id='"+QString::number(idfatt_new)+"'");
    
    try{
    if(query1.exec()){
        LOG_FILE(qApp->translate("fattura_rg","Aggiornamento effettuato correttamente(fattura_rg::update_f_prodotti)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
        LOG_FILE(qApp->translate("fattura_rg","Impossibile aggiornare(fattura_rg::update_f_prodotti)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(fattura_rg::update_f_prodotti): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query1.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}
/////////////////////////////////////////////////////////////////////////

void fattura_rg::inserisci_agenti(QString m_agente, QDate date)
{
    if(m_agente == "")
        return;
    
    //Imposto totale provvigione
    double perc = lineEdit_3->text().toDouble();
    double importo = totale_2->text().toDouble();
    double provvigione = (importo*perc)/100;
    
    QSqlQuery query_agente,query_ins_agente;
    query_agente.prepare("select id from lista_agenti where concat(cognome,' ',nome)='"+m_agente+"'");
    if(query_agente.exec())
    {
       if(query_agente.next())
       {
	  query_ins_agente.prepare("INSERT INTO lista_riep_agenti VALUES("
				   "'','"+m_agente+"','"+lineEdit_3->text()+"','"+QString::number(provvigione)+"', "
				  "'"+query_agente.value(0).toString()+"','"+QString::number(idfatt_new)+"','"+date.toString("yyyy/MM/dd")+"','N')");
	  try{
	    if(query_ins_agente.exec())
	    {
	      LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::inserisci_agenti)"),LOG_INFO_ALL,QIODevice::Append);
	    }
	    else{
	      LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::inserisci_agenti)"),LOG_ERROR_ALL,QIODevice::Append);
	      LOG_FILE("(fattura_rg::inserisci_agenti): "+query_ins_agente.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	      throw InvalidQuery(query_ins_agente.lastError().text().toStdString().c_str());
	    }
	  }
	  catch(FabariaException e)
	  {
	    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	  }
       }
       else{
	    LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire l'agente(fattura_rg::inserisci_agenti)"),LOG_ERROR_ALL,QIODevice::Append);
       }
    }
}

////////////////////////////////////////////////////////////////////////

void fattura_rg::aggiorna_agenti(QString m_agente, QDate date)
{
    
    if(m_agente == "")
        return;
    //Imposto totale provvigione
    double perc = lineEdit_3->text().toDouble();
    double importo = totale_2->text().toDouble();
    double provvigione = (importo*perc)/100;
    
    QSqlQuery query_agente,query_ins_agente;
    query_agente.prepare("select id from lista_agenti where concat(cognome,' ',nome)='"+m_agente+"'");
    if(query_agente.exec())
    {
       if(query_agente.next())
       {
	  query_ins_agente.prepare("UPDATE lista_riep_agenti SET "
				   "agente='"+m_agente+"',provvigione='"+lineEdit_3->text()+"',importo='"+QString::number(provvigione)+"', "
				  "id_agente='"+query_agente.value(0).toString()+"',id_fattura='"+QString::number(idfatt_new)+"',data='"+date.toString("yyyy/MM/dd")+"', "
				  "'N' where id_fattura='"+QString::number(idfatt_new)+"'");
	  try{
	    if(query_ins_agente.exec())
	    {
	      LOG_FILE(qApp->translate("fattura_rg","Aggiornamento effettuato correttamente(fattura_rg::aggiorna_agenti)"),LOG_INFO_ALL,QIODevice::Append);
	    }
	    else{
	      LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::aggiorna_agenti)"),LOG_ERROR_ALL,QIODevice::Append);
	      LOG_FILE("(fattura_rg::aggiorna_agenti): "+query_ins_agente.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	      throw InvalidQuery(query_ins_agente.lastError().text().toStdString().c_str());
	    }
	  }
	  catch(FabariaException e)
	  {
	    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	  }
       }
       else{
	    LOG_FILE(qApp->translate("fattura_rg","Impossibile aggiornare l'agente(fattura_rg::aggiorna_agenti)"),LOG_ERROR_ALL,QIODevice::Append);
       }
    }
}

////////////////////////////////////////////////////////////////////////

void fattura_rg::modifica_riga()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QModelIndex modelIndex = tab_view->selectionModel()->currentIndex();
    int riga = modelIndex.row();
    QModelIndex newindex = modelIndex.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    modifica_riga_one(newindex);
    }
    else
    {
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Selezionare una riga da modificare..."));
    }
}

void fattura_rg::modifica_riga_one(QModelIndex index)
{
    //Tentativo di aggiornamento del record perché trovato
    if(index.row() < mod_grid->rowCount()){
        fattura_rg_art *gt = new fattura_rg_art(this,tipo_fattura,idfatt_new);
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
                 connect(gt,SIGNAL(save_fatt()),this,SLOT(lista_prodotti()));
                 lista_prodotti();

        }
        connect(gt,SIGNAL(save_fatt()),this,SLOT(tot_imp_iva()));
        gt->exec();
    }
}

void fattura_rg::elimina()
{
    if(tab_view->selectionModel()->currentIndex().isValid() || add_row->selectionModel()->currentIndex().isValid() ){
        if (!id1->text().isEmpty())
        {
            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM fatture_righe_vendita_art WHERE id = '"+QString::number(idfatt_new)+"' LIMIT 1");
            qctrl.exec();

                QMessageBox *box= new QMessageBox(this);
                box->setWindowTitle("FabariaGest");
                box->setInformativeText(qApp->translate("fattura_rg","Vuoi eliminare veramente \n il record selezionato?...."));
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

}

void fattura_rg::elimina_riga()
{
        QSqlQuery query,query1;
	const char* error;
        query.prepare("DELETE FROM fatture_righe_vendita_art WHERE id = '"+QString::number(idfatt_new)+"' LIMIT 1");
        query1.prepare("DELETE FROM fatture_vendita_righe WHERE id = '"+QString::number(idfatt_new)+"' LIMIT 1");

	try{
        if (query.exec() && query1.exec()) //Se esito OK Eliminato da DB
        {
            LOG_FILE(qApp->translate("fattura_rg","Eliminazione effettuata correttamente(fattura_rg::elimina_riga)"),LOG_INFO_ALL,QIODevice::Append);
        }
        else
        {
	    LOG_FILE(qApp->translate("fattura_rg","Impossibile eliminare(fattura_rg::elimina_riga)"),LOG_ERROR_ALL,QIODevice::Append);
	    LOG_FILE("(fattura_rg::elimina_riga): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	    LOG_FILE("(fattura_rg::elimina_riga): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	    throw error ? InvalidQuery(query.lastError().text().toStdString().c_str()) : InvalidQuery(query1.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	}
        emit salvafattura();
        lista_prodotti();
        tot_imp_iva();

}

void fattura_rg::sconto_1t(const QString &)
{
    double sptr = local_settings->toDouble(sconto1->text());
    double sp =0,sp1=0,sp2 =0;

    sp = imponibile_2->text().toDouble()-((imponibile_2->text().toDouble()*sptr)/100);
    imponibile_2->setText(QString::number(sp));
    sp1 = iva_ft_2->text().toDouble()-((iva_ft_2->text().toDouble()*sptr)/100);
    iva_ft_2->setText(QString::number(sp1));
    sp2 = totale_2->text().toDouble()-((totale_2->text().toDouble()*sptr)/100);
    totale_2->setText(QString::number(sp2));

    if(sconto1->text().length() == 0){
        sconto1->setText("0.00");
        tot_imp_iva();
    }
}

void fattura_rg::sconto_2t(const QString &)
{
    double sptr = local_settings->toDouble(sconto2->text());
    double sp =0,sp1=0,sp2 =0;

    sp = imponibile_2->text().toDouble()-((imponibile_2->text().toDouble()*sptr)/100);
    imponibile_2->setText(QString::number(sp));
    sp1 = iva_ft_2->text().toDouble()-((iva_ft_2->text().toDouble()*sptr)/100);
    iva_ft_2->setText(QString::number(sp1));
    sp2 = totale_2->text().toDouble()-((totale_2->text().toDouble()*sptr)/100);
    totale_2->setText(QString::number(sp2));

    if(sconto2->text().length() == 0){
        sconto2->setText("0.00");
        tot_imp_iva();
    }
}

void fattura_rg::sconto_3t(const QString &)
{
    double sptr = local_settings->toDouble(sconto3->text());
    double sp =0,sp1=0,sp2 =0;

    sp = imponibile_2->text().toDouble()-((imponibile_2->text().toDouble()*sptr)/100);
    imponibile_2->setText(QString::number(sp));
    sp1 = iva_ft_2->text().toDouble()-((iva_ft_2->text().toDouble()*sptr)/100);
    iva_ft_2->setText(QString::number(sp1));
    sp2 = totale_2->text().toDouble()-((totale_2->text().toDouble()*sptr)/100);
    totale_2->setText(QString::number(sp2));

    if(sconto3->text().length() == 0){
        sconto3->setText("0.00");
        tot_imp_iva();
    }
}

void fattura_rg::spesa_trasp(const QString &)
{
    double sptr = local_settings->toDouble(spes_tr->text());
    double sp = 0,sp1 = 0,sp2=0; //(sptr*20)/100;

    sp = imponibile_2->text().toDouble()+sptr;
    imponibile_2->setText(QString::number(sp));
    sp1 = iva_ft_2->text().toDouble()+((sptr*20)/100);
    iva_ft_2->setText(QString::number(sp1));
    sp2 = totale_2->text().toDouble()+sptr-((sptr*20)/100);
    totale_2->setText(QString::number(sp2));

    if(spes_tr->text().length()==0){
        tot_imp_iva();
        spes_tr->setText("0.00");
    }
}

void fattura_rg::spesa_in(const QString &)
{
    double sptr = local_settings->toDouble(spes_in->text());
    double sp = 0,sp1 = 0,sp2=0; //(sptr*20)/100;

    sp = imponibile_2->text().toDouble()+sptr;
    imponibile_2->setText(QString::number(sp));
    sp1 = iva_ft_2->text().toDouble()+((sptr*20)/100);
    iva_ft_2->setText(QString::number(sp1));
    sp2 = totale_2->text().toDouble()+sptr-((sptr*20)/100);
    totale_2->setText(QString::number(sp2));

    if(spes_in->text().length()==0){
        tot_imp_iva();
        spes_in->setText("0.00");
    }
}

void fattura_rg::tot_imp_iva()
{
    if(tipo_fattura == qApp->translate("fattura_rg","Nota di credito")){
        QSqlQuery Query1,Query2,Query3;
        QSqlQuery query;
        query.prepare("select id from fatture_righe_vendita_art where id= '"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        query.exec();
        if(query.next()){

            Query1.prepare("select sum(-prezzo_s_iva) from fatture_righe_vendita_art where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query1.exec();
        if(Query1.next()){
            imponibile_2->setText(QString::number(Query1.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
        }

        Query2.prepare("select sum((-totale+prezzo_s_iva)) from fatture_righe_vendita_art where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query2.exec();
        if(Query2.next()){
            iva_ft_2->setText(QString::number(Query2.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare l'iva... ")+Query2.lastError().text());
        }


        Query3.prepare("select sum(-totale) from fatture_righe_vendita_art where id ='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query3.exec();
        if(Query3.next()){
            totale_2->setText(QString::number(Query3.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il totale... ")+Query3.lastError().text());
        }
        }
    }
    else{
    QSqlQuery Query1,Query2,Query3;
    QSqlQuery query;
    query.prepare("select id from fatture_righe_vendita_art where id= '"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){

        Query1.prepare("select sum(prezzo_s_iva) from fatture_righe_vendita_art where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query1.exec();
    if(Query1.next()){
        imponibile_2->setText(QString::number(Query1.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
    }

    Query2.prepare("select sum(totale-prezzo_s_iva) from fatture_righe_vendita_art where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query2.exec();
    if(Query2.next()){
        iva_ft_2->setText(QString::number(Query2.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare l'iva... ")+Query2.lastError().text());
    }


    Query3.prepare("select sum(totale) from fatture_righe_vendita_art where id ='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query3.exec();
    if(Query3.next()){
        totale_2->setText(QString::number(Query3.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il totale... ")+Query3.lastError().text());
    }
    }
    }
    emit salvafattura();
}

void fattura_rg::totale_impo_iva()
{
    if(tipo_fattura == qApp->translate("fattura_rg","Nota di credito")){
        QSqlQuery Query1,Query2,Query3;
        QSqlQuery query;
        query.prepare("select id from fatture_vendita_righe where id= '"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        query.exec();
        if(query.next()){

            Query1.prepare("select sum(-prezzo_s_iva) from fatture_vendita_righe where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query1.exec();
        if(Query1.next()){
            imponibile_2->setText(QString::number(Query1.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
        }

        Query2.prepare("select sum((-totale+prezzo_s_iva)) from fatture_vendita_righe where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query2.exec();
        if(Query2.next()){
            iva_ft_2->setText(QString::number(Query2.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare l'iva... ")+Query2.lastError().text());
        }


        Query3.prepare("select sum(-totale) from fatture_vendita_righe where id ='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
        Query3.exec();
        if(Query3.next()){
            totale_2->setText(QString::number(Query3.value(0).toDouble()));
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il totale... ")+Query3.lastError().text());
        }
        }
    }
    else{
    QSqlQuery Query1,Query2,Query3;
    QSqlQuery query;
    query.prepare("select id from fatture_vendita_righe where id= '"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){

        Query1.prepare("select sum(prezzo_s_iva) from fatture_vendita_righe where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query1.exec();
    if(Query1.next()){
        imponibile_2->setText(QString::number(Query1.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
    }

    Query2.prepare("select sum(totale-prezzo_s_iva) from fatture_vendita_righe where id='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query2.exec();
    if(Query2.next()){
        iva_ft_2->setText(QString::number(Query2.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare l'iva... ")+Query2.lastError().text());
    }


    Query3.prepare("select sum(totale) from fatture_vendita_righe where id ='"+QString::number(idfatt_new)+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query3.exec();
    if(Query3.next()){
        totale_2->setText(QString::number(Query3.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile instanziare il totale... ")+Query3.lastError().text());
    }
    }
    }
    emit salvafattura();
}


void fattura_rg::chiudi(){
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("FabariaGest");
    box->setInformativeText(qApp->translate("fattura_rg","Sei sicuro di voler uscire dalla fattura?\n I dati non verranno salvati...."));
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Close);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
    case QMessageBox::Ok:
         //Close
         save_2->setChecked(true);
         save_2->setStyleSheet("background-color: rgb(249, 22, 5)");
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

void fattura_rg::visual_scadenze()
{
    //Disalibilito visualizzazione scadenze
    title_scadenze->setVisible(false);
    scadenza_1->setVisible(false);
    scadenza_2->setVisible(false);
    scadenza_3->setVisible(false);
}

void fattura_rg::tipo_pagamento()
{
    for(int i=0;i < 3;++i){
        visual_scadenze();
    }

    if(pagam->currentIndex()==4 || pagam->currentIndex()==5)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        QDate data;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(120);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
    }
    else if(pagam->currentIndex()==6 || pagam->currentIndex()==7)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        QDate data;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(150);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);

    }
    else if(pagam->currentIndex()==8 || pagam->currentIndex()==9)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        QDate data;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(180);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
    }
    else if(pagam->currentIndex()==10 || pagam->currentIndex()==11)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        QDate data;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(30);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
    }
    else if(pagam->currentIndex()==12 || pagam->currentIndex()==13)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        scadenza_2->setVisible(true);
        QDate data,data1;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(30);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
        data1.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data1 = data1.addDays(60);
        scadenza_2->setCalendarPopup(true);
        scadenza_2->setDate(data1);
    }
    else if(pagam->currentIndex()==14 || pagam->currentIndex()==15)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        scadenza_2->setVisible(true);
        scadenza_3->setVisible(true);
        QDate data,data1,data2;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(30);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
        data1.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data1 = data1.addDays(60);
        scadenza_2->setCalendarPopup(true);
        scadenza_2->setDate(data1);
        data2.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data2 = data2.addDays(90);
        scadenza_3->setCalendarPopup(true);
        scadenza_3->setDate(data2);
    }
    else if(pagam->currentIndex()==16 || pagam->currentIndex()==17)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        QDate data;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(60);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
    }
    else if(pagam->currentIndex()==18 || pagam->currentIndex()==19)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        scadenza_2->setVisible(true);
        QDate data,data1;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(60);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
        data1.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data1 = data1.addDays(90);
        scadenza_2->setCalendarPopup(true);
        scadenza_2->setDate(data1);
    }
    else if(pagam->currentIndex()==20 || pagam->currentIndex()==21)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        QDate data;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(90);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
    }
    else if(pagam->currentIndex()==22 || pagam->currentIndex()==23)
    {
        title_scadenze->setVisible(true);
        scadenza_1->setVisible(true);
        QDate data;
        data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
        data = data.addDays(210);
        scadenza_1->setCalendarPopup(true);
        scadenza_1->setDate(data);
    }
}

void fattura_rg::scadenza1(){
    if(tipo_cliente->currentIndex() == 0){
               QString query_clienti ="select * from clienti where concat(nome,' ',cognome)='"+pers->currentText()+"'";
               QSqlQuery query(query_clienti);
               query.exec();
               if(query.next()){
               QSqlQuery query1;
                         query1.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','"+QString::number(query.value(0).toString().toInt())+"','0','"+vis_banca->currentText()+"','"+iban->text()+"')");
			try{
                         if(query1.exec()){
                             LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::scadenza1())"),LOG_ERROR_ALL,QIODevice::Append);
                         }
                         else{
                             LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::scadenza1())"),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza1()): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            throw InvalidQuery(query1.lastError().text().toStdString().c_str());
                         }
			}
			catch(FabariaException e)
			{
			   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
			}
               }
               else{
                   QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile creare istanza di inserimento...\n")+query.lastError().text());
               }
       }       
    else if(tipo_cliente->currentIndex() == 1){
        QString query_fornitori ="SELECT * from fornitori where ragione_sociale='"+pers->currentText()+"' order by id";
        QSqlQuery query(query_fornitori);
        query.exec();
        if(query.next()){
        QSqlQuery query1;
                  query1.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','0','"+QString::number(query.value(0).toString().toInt())+"','"+vis_banca->currentText()+"','"+iban->text()+"')");
                  try{
                         if(query1.exec()){
                             LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::scadenza1())"),LOG_ERROR_ALL,QIODevice::Append);
                         }
                         else{
                             LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::scadenza1())"),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza1()): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            throw InvalidQuery(query1.lastError().text().toStdString().c_str());
                         }
			}
			catch(FabariaException e)
			{
			   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
			}
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile creare istanza di inserimento...\n")+query.lastError().text());
        }
     }
     form->reload_all();
}

void fattura_rg::scadenza2(){
    //double div = totale_2->text().toDouble()/2;
    if(tipo_cliente->currentIndex() == 0){
       if(pers->currentIndex() == -1){
       }
       else{
	       const char* error = "";
               QString query_clienti ="select * from clienti where concat(nome,' ',cognome)='"+pers->currentText()+"'";
               QSqlQuery query(query_clienti);
               query.exec();
               if(query.next()){
                         QSqlQuery query1,query2;
                         query1.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','"+QString::number(query.value(0).toString().toInt())+"','0','"+vis_banca->currentText()+"','"+iban->text()+"')");
			 
			 query2.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','"+QString::number(query.value(0).toString().toInt())+"','0','"+vis_banca->currentText()+"','"+iban->text()+"')");
			try{
                         if(query1.exec() && query2.exec()){
                             LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::scadenza2())"),LOG_ERROR_ALL,QIODevice::Append);
                         }
                         else{
                             LOG_FILE("Impossibile inserire(fattura_rg::scadenza2())",LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza2()): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza2()): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            throw error ? InvalidQuery(query1.lastError().text().toStdString().c_str()) : InvalidQuery(query2.lastError().text().toStdString().c_str());
                         }
			}
			catch(FabariaException e)
			{
			   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
			}
               }
               else{
                   QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile creare istanza di inserimento...\n")+query.lastError().text());
               }
        }
     }
    else if(tipo_cliente->currentIndex() == 1){
	const char* error = "";
        QString query_fornitori ="SELECT * from fornitori where ragione_sociale='"+pers->currentText()+"' order by id";
        QSqlQuery query(query_fornitori);
        query.exec();
        if(query.next()){
                  QSqlQuery query1,query2;
                  query1.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','0','"+QString::number(query.value(0).toString().toInt())+"','"+vis_banca->currentText()+"','"+iban->text()+"')");
		  
		  query2.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','0','"+QString::number(query.value(0).toString().toInt())+"','"+vis_banca->currentText()+"','"+iban->text()+"')");
                  try{
                         if(query1.exec() && query2.exec()){
                             LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::scadenza2())"),LOG_ERROR_ALL,QIODevice::Append);
                         }
                         else{
                             LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::scadenza2())"),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza2()): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza2()): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            throw error ? InvalidQuery(query1.lastError().text().toStdString().c_str()) : InvalidQuery(query2.lastError().text().toStdString().c_str());
                         }
			}
			catch(FabariaException e)
			{
			   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
			}
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile creare istanza di inserimento scadenze...\n")+query.lastError().text());
        }
     }
     form->reload_all();
}

void fattura_rg::scadenza3(){
    
    if(tipo_cliente->currentIndex() == 0){
       if(pers->currentIndex() == -1){
       }
       else{
	       const char* error = "";
              QString query_clienti ="select * from clienti where concat(nome,' ',cognome)='"+pers->currentText()+"'";
               QSqlQuery query(query_clienti);
               query.exec();
               if(query.next()){
                         QSqlQuery query1,query2,query3;
                         query1.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','"+QString::number(query.value(0).toString().toInt())+"','0','"+vis_banca->currentText()+"','"+iban->text()+"')");
			 
			 query2.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','"+QString::number(query.value(0).toString().toInt())+"','0','"+vis_banca->currentText()+"','"+iban->text()+"')");
			 
			 query3.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','"+QString::number(query.value(0).toString().toInt())+"','0','"+vis_banca->currentText()+"','"+iban->text()+"')");
			try{
                         if(query1.exec() && query2.exec() && query3.exec()){
                             LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::scadenza3())"),LOG_ERROR_ALL,QIODevice::Append);
                         }
                         else{
                             LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::scadenza3())"),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza3()): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza3()): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza3()): "+query3.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            throw error ? InvalidQuery(query1.lastError().text().toStdString().c_str()) : InvalidQuery(query2.lastError().text().toStdString().c_str());
                            throw InvalidQuery(query3.lastError().text().toStdString().c_str());
                         }
			}
			catch(FabariaException e)
			{
			   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
			}
               }
               else{
                   QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile creare istanza di inserimento...\n")+query.lastError().text());
               }
        }
     }
    else if(tipo_cliente->currentIndex() == 1){
        //int in = pers->currentIndex();
	const char* error = "";
        QString query_fornitori ="SELECT * from fornitori where ragione_sociale='"+pers->currentText()+"' order by id";
        QSqlQuery query(query_fornitori);
        query.exec();
        if(query.next()){
                  QSqlQuery query1,query2,query3;
                  query1.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','0','"+QString::number(query.value(0).toString().toInt())+"','"+vis_banca->currentText()+"','"+iban->text()+"')");
		  
		  query2.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','0','"+QString::number(query.value(0).toString().toInt())+"','"+vis_banca->currentText()+"','"+iban->text()+"')");
		  
		  query3.prepare("INSERT INTO scadenze "
                                " VALUES('','"+pers->currentText()+"','"+tipo_fattura+"','"+QString::number(idfatt_new)+"','"+QString::number(data_fatt->date().year())+"','"+scadenza_1->date().toString("yyyy-MM-dd")+"',"
				"'"+pagam->currentText()+"','"+totale_2->text()+"','"+lineEdit->text()+"','N','0','"+QString::number(query.value(0).toString().toInt())+"','"+vis_banca->currentText()+"','"+iban->text()+"')");
                  try{
                         if(query1.exec() && query2.exec() && query.exec()){
                             LOG_FILE(qApp->translate("fattura_rg","Inserimento effettuato correttamente(fattura_rg::scadenza3())"),LOG_ERROR_ALL,QIODevice::Append);
                         }
                         else{
                             LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire(fattura_rg::scadenza3())"),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza3()): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza3()): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg::scadenza3()): "+query3.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            throw error ? InvalidQuery(query1.lastError().text().toStdString().c_str()) : InvalidQuery(query2.lastError().text().toStdString().c_str());
                            throw InvalidQuery(query3.lastError().text().toStdString().c_str());
                         }
			}
			catch(FabariaException e)
			{
			   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
			}
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Impossibile creare istanza di inserimento...\n")+query.lastError().text());
        }
     }
     form->reload_all();
}

void fattura_rg::enable_fatt_acc()
{
   dati_fattura_accompagnatoria->setVisible(true);
   setMaximumSize(1200,600);
}

void fattura_rg::disable_fatt_acc()
{
    dati_fattura_accompagnatoria->setVisible(false);
    setMaximumSize(800,600);
}

void fattura_rg::dis_allegati()
{
    tabWidget->removeTab(2);
}

void fattura_rg::open_file()
{
    QString fileName;
    QStringList fileNames;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    
    if(dialog.exec())
      fileNames = dialog.selectedFiles();
    foreach(fileName,fileNames){
	QListWidgetItem *newItem = new QListWidgetItem(lista_file_2);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	QFileInfo fi(fileName);
	//Imposto nome file file allegato
	name = fi.fileName();
	newItem->setText(name);
	//Inserimento allegato
	inserisci_allegati(fileName,(lista_file_2->count()-1));
	//Imposto icona file allegato
	IconFileProvider provider;
	QIcon icon = provider.icon(fi);
	newItem->setIcon(icon);
	newItem->setFont(f);
    }
    lista_file_2->update();
}

void fattura_rg::erase_file()
{
    lista_file_2->takeItem(lista_file_2->row(lista_file_2->currentItem()));
    
    QSqlQuery query;
    query.prepare("DELETE from allegati_fattura where id='"+QString::number(idfatt_new)+"' and riga='"+QString::number(lista_file_2->count())+"'");
    try{
    if(query.exec())
    {
      LOG_FILE(qApp->translate("fattura_rg","Eliminazione eseguita con successo(allegati_fattura)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
      LOG_FILE(qApp->translate("fattura_rg","Impossibile eliminare(allegati_fattura)"),LOG_ERROR_ALL,QIODevice::Append);
      LOG_FILE("(allegati_fattura): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
      throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void fattura_rg::vis_allegati()
{
    QStringList list;
    QString m_fileName;
    QSqlQuery qvis_allegati("select * from allegati_fattura where id='"+QString::number(idfatt_new)+"'");
    if(qvis_allegati.exec()){
    while(qvis_allegati.next())
    {	
	list = qvis_allegati.value(1).toStringList();
	foreach(m_fileName,list){
	QListWidgetItem *item = new QListWidgetItem(lista_file_2);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	QFileInfo fi(m_fileName);
	//Imposto nome file file allegato
	QString m_name = fi.fileName();
	item->setText(m_name);
	//Imposto icona file allegato
	IconFileProvider provider;
	QIcon icon = provider.icon(fi);
	item->setIcon(icon);
	item->setFont(f);
	}
    }
    }
}

void fattura_rg::importa()
{
  QString testo = qApp->translate("fattura_rg","ID\nCodice articolo\nCodice a barre\nNome prodotto\nDescrizione\nUnità di misura\n"
      "Prezzo unitario\nPrezzo senza iva\nPrezzo con iva\nAliquota IVA\n"
      "Quantità\nSconto\nTotale\nAnno inserimento\nListino\nCategoria\n"
      "Data inserimento\nNumero lotto\nScadenza lotto\nMagazzino\n\n\n\nAttenzione tutte le celle\ndevono essere compilate.");
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle(qApp->translate("fattura_rg","FabariaGest"));
    box->setText(qApp->translate("fattura_rg","Il file excel\ndeve essere composto:\n"));
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

void fattura_rg::import_file_excel()
{
    QString filePath = QFileDialog::getOpenFileName(0, qApp->translate("fattura_rg","Apri file excel"), QString(), "*.xlsx");
    if(filePath.isEmpty())
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Non hai aperto nesssun file"));
    else{
    Document xlsx(filePath);
        
    for(int i=2; i <= xlsx.dimension().rowCount(); i++)
    {
      QProgressDialog progressDialog(this);
      progressDialog.setRange(0, xlsx.dimension().rowCount());
      progressDialog.setWindowModality(Qt::WindowModal);
      progressDialog.setWindowTitle(qApp->translate("fattura_rg","Inserimento dati da excel"));
      progressDialog.setValue(i);
      progressDialog.setLabelText(qApp->translate("fattura_rg","Inserimento dati %1 di %2...")
                      .arg(i).arg(xlsx.dimension().rowCount()));
      qApp->processEvents();
      QSqlQuery query,query_id,query_up;
      QString req = "INSERT INTO fatture_righe_vendita_art(id,cod_art,codbarre,nome_articolo,descrizione,unita, "
		    "prezzo_unit,prezzo_s_iva,prezzo_c_iva,iva,quantita,sconto,totale, "
		    "anno_ins,codice_listino,categoria,data_insert, numero_lotto, data_lotto,addMagazzino) VALUES('";
      Cell *code_art = xlsx.cellAt(i,2);
      query_up.prepare("select * from fatture_righe_vendita_art where cod_art='"+code_art->value().toString()+"'");
      query_up.exec();
      if(query_up.next())
      {
	  aggiorna_file_excel(filePath);
      }
      else{
	//Inserimento
	req.append(id1->text());
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
      if(Cell *addMagazzino = xlsx.cellAt(i,20)){//addMagazzino
        req.append(addMagazzino->value().toString());
        req.append("','");
      }
      req.chop(2);
      req.append(");");
      query.prepare(req);
      progressDialog.show();
      progressDialog.update();
      QSqlQuery query("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+code_art->value().toString()+"'");
       QSqlQuery query1("select quantita,prezzo_s_iva,prezzo_c_iva,totale from scarico_mag where cod_articolo='"+code_art->value().toString()+"'");
       query.exec();
       query1.exec();
       if(query.next()){
            found = true;
            double qa = query.value(0).toDouble();
            double pr_s = query.value(1).toDouble();
            double pr_c = query.value(2).toDouble();
            double tot_ = query.value(3).toDouble();
            update_articoli(qa,pr_s,pr_c,tot_);
            update_carico_prod(qa,pr_s,pr_c,tot_);
            if(query1.next()){
                 found = true;
                 double qa = query1.value(0).toDouble();
                 double pr_s = query1.value(1).toDouble();
                 double pr_c = query1.value(2).toDouble();
                 double tot_ = query1.value(3).toDouble();
                 update_scarico_prod(qa,pr_s,pr_c,tot_);
                 }
                 else{
                      found = false;
                      insert_scarico_prod();
                 }
         }
      if(query.exec())
           qDebug() <<  qApp->translate("fattura_rg","Inserimento effettuato");
      else
           QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Errore Inserimento:\n ")+query.lastError().text());
    }
    }
    }
    lista_prodotti();
}

void fattura_rg::aggiorna_file_excel(QString doc)
{
    Document xlsx(doc);
        
    for(int i=2; i <= xlsx.dimension().rowCount(); i++)
    {
      QProgressDialog progressDialog(this);
      progressDialog.setRange(0, xlsx.dimension().rowCount());
      progressDialog.setWindowModality(Qt::WindowModal);
      progressDialog.setWindowTitle(qApp->translate("fattura_rg","Aggionamento dati da excel"));
      progressDialog.setValue(i);
      progressDialog.setLabelText(qApp->translate("fattura_rg","Aggionamento dati %1 di %2...")
                      .arg(i).arg(xlsx.dimension().rowCount()));
      qApp->processEvents();
      QSqlQuery query,query_up;
      query_up.prepare("select * from fatture_righe_vendita_art where id='"+id1->text()+"'");
      query_up.exec();
      if(query_up.next())
      {
      QString req = "UPDATE fatture_righe_vendita_art SET cod_art='";
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
                update_scarico_prod(qa,pr_s,pr_c,tot_);
                update_carico_prod(qa,pr_s,pr_c,tot_);
        }
      if(query.exec())
           qDebug() <<  qApp->translate("fattura_rg","Aggionamento effettuato");
      else
           QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Errore aggiornamento:\n ")+query.lastError().text());
    }
    }
    lista_prodotti();
}

void fattura_rg::addRow()
{
    if(settingsManager->generalValue("Magazzino/nome",QVariant()).toString() != ""){
            QSqlQuery query_um,query_iva;
            query_um.exec("select unita from misura");
            query_iva.exec("select iva from aliquota");
            if(query_um.next() && query_iva.next()){
                int row = mod_grid->rowCount();
                QSqlRecord rec = mod_grid->record();
            
                rec.setValue(0,id1->text());
                rec.setValue(5,query_um.value(0).toString());
                rec.setValue(9,query_iva.value(0).toString());
                rec.setValue(13,QString::number(QDate::currentDate().year()));
                rec.setValue(16,QDate::currentDate().toString("yyyy/MM/dd"));
                rec.setValue(18,QDate::currentDate().toString("yyyy/MM/dd"));
                rec.setValue(19,settingsManager->generalValue("Magazzino/nome",QVariant()).toString());
            mod_grid->insertRecord(row,rec);
            submit();
            }
             lista_prodotti();
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_rg","Devi creare almeno un deposito magazzino"));
        Magazzino *mg = new Magazzino(this);
        mg->exec();
    }
}

bool fattura_rg::InserisciRiga()
{
    QString prezzo_s_iva,m_quantita,m_iva,m_sconto;
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex col_pr_unit = mod_grid->index(riga,6,QModelIndex());
    QModelIndex col_psiva = mod_grid->index(riga,7,QModelIndex());
    QModelIndex col_pciva = mod_grid->index(riga,8,QModelIndex());
    QModelIndex col_iva = mod_grid->index(riga,9,QModelIndex());
    QModelIndex col_quan = mod_grid->index(riga,10,QModelIndex());
    QModelIndex col_sconto = mod_grid->index(riga,11,QModelIndex());
    QModelIndex col_totale = mod_grid->index(riga,12,QModelIndex());
    prezzo_s_iva = mod_grid->data(col_psiva).toString();
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
        prezzo_c_iva = ((prezzo_s_iva.toDouble()*m_iva.toDouble()/100)+prezzo_s_iva.toDouble());
        mod_grid->setData(col_pciva,QString::number(prezzo_c_iva,'f',2));
        mod_grid->setData(col_totale,QString::number(prezzo_c_iva,'f',2));
	mod_grid->setData(col_psiva,mod_grid->data(col_pr_unit).toString());
    }
    else if(m_sconto.toDouble() == 0){
        prezzo_c_iva = (((prezzo_s_iva.toDouble()*m_iva.toDouble())/100)+prezzo_s_iva.toDouble())*m_quantita.toDouble();
        mod_grid->setData(col_pciva,QString::number(prezzo_c_iva,'f',2));
        mod_grid->setData(col_totale,QString::number(prezzo_c_iva,'f',2));
	mod_grid->setData(col_psiva,mod_grid->data(col_pr_unit).toString());
    }
    else
    {   //Calcolo lo sconto
        prezzo_c_iva = (((prezzo_s_iva.toDouble()*m_iva.toDouble())/100)+prezzo_s_iva.toDouble())*m_quantita.toDouble();
	prezzos_iva = (prezzo_s_iva.toDouble()*m_quantita.toDouble());
        perc = prezzo_c_iva-((prezzo_c_iva*m_sconto.toDouble())/100);
        totale = perc;
	mod_grid->setData(col_psiva,QString::number(prezzos_iva,'f',2));
        mod_grid->setData(col_pciva,QString::number(prezzo_c_iva,'f',2));
        mod_grid->setData(col_totale,QString::number(totale,'f',2));
    }
    tot_imp_iva();
    return true;
}

bool fattura_rg::eventFilter(QObject *o, QEvent *j)
{
    QMouseEvent *mouseEvent = (QMouseEvent*)j ;
    if ((j->type() == QEvent::KeyPress)) {
        if(o==add_row->viewport()){
            QKeyEvent* ke = (QKeyEvent*)j;
            if (ke->key() == Qt::Key_Enter) {
                if(InserisciRiga()){
                    QSqlQuery query("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+getCodArt()+"'");
                    QSqlQuery query1("select quantita,prezzo_s_iva,prezzo_c_iva,totale from scarico_mag where cod_articolo='"+getCodArt()+"'");
                    query.exec();
                    query1.exec();
                    if(query.next()){
                            found = true;
                            double qa = query.value(0).toDouble();
                            double pr_s = query.value(1).toDouble();
                            double pr_c = query.value(2).toDouble();
                            double tot_ = query.value(3).toDouble();
                            update_articoli(qa,pr_s,pr_c,tot_);
                            update_carico_prod(qa,pr_s,pr_c,tot_);
                            if(query1.next()){
                                found = true;
                                double qa = query1.value(0).toDouble();
                                double pr_s = query1.value(1).toDouble();
                                double pr_c = query1.value(2).toDouble();
                                double tot_ = query1.value(3).toDouble();
                                update_scarico_prod(qa,pr_s,pr_c,tot_);
                                }
                                else{
                                    found = false;
                                    insert_scarico_prod();
                                }
                        }
                    submit();
                }
            }
        }
    }
    else if ((QEvent::MouseButtonPress==j->type()) && (mouseEvent->button() == Qt::RightButton) && o==add_row->viewport())
    {
            if(InserisciRiga()){
                    QSqlQuery query("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+getCodArt()+"'");
                    QSqlQuery query1("select quantita,prezzo_s_iva,prezzo_c_iva,totale from scarico_mag where cod_articolo='"+getCodArt()+"'");
                    query.exec();
                    query1.exec();
                    if(query.next()){
                            found = true;
                            double qa = query.value(0).toDouble();
                            double pr_s = query.value(1).toDouble();
                            double pr_c = query.value(2).toDouble();
                            double tot_ = query.value(3).toDouble();
                            update_articoli(qa,pr_s,pr_c,tot_);
                            update_carico_prod(qa,pr_s,pr_c,tot_);
                            if(query1.next()){
                                found = true;
                                double qa = query1.value(0).toDouble();
                                double pr_s = query1.value(1).toDouble();
                                double pr_c = query1.value(2).toDouble();
                                double tot_ = query1.value(3).toDouble();
                                update_scarico_prod(qa,pr_s,pr_c,tot_);
                                }
                                else{
                                    found = false;
                                    insert_scarico_prod();
                                }
                        }
                    submit();
                }
    }

     return QObject::eventFilter(o,j);
}

void fattura_rg::submit()
{
      mod_grid->database().transaction();
       if (mod_grid->submitAll()) {
           mod_grid->database().commit();
       } else {
           mod_grid->database().rollback();
           QMessageBox::warning(this, qApp->translate("fattura_rg","FabariaGest"),
                                qApp->translate("fattura_rg","Errore nel database: %1")
                                .arg(mod_grid->lastError().text()));
       }
       tot_imp_iva();
}

void fattura_rg::update_articoli(double qua, double _p_siva, double _p_civa, double _tot)
{
	  
	//Imposto la quantità
	double quan = getQuantita().toDouble();
        double somma = qua-quan;
	//Aggiorno il prezzo senza iva 
	double ps_iva = getPrezzoSIVA().toDouble();
        double sum_pr_siva = _p_siva-ps_iva;
	//Aggiorno il prezzo con iva
	double pc_iva = getPrezzoCIVA().toDouble();
        double sum_pr_civa = _p_civa-pc_iva;
	//Aggiorno il totale
	double tot = getTotale().toDouble();
        double sum_tot = _tot-tot;
  
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
              LOG_FILE(qApp->translate("fattura_rg","Aggiornamento articoli da FATTURA VENDITA effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("fattura_rg","Impossibile aggiornare articoli da FATTURA VENDITA: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

void fattura_rg::update_carico_prod(double ca, double _p_siva, double _p_civa, double _tot)
{
	//Imposto la quantità
	double quan = getQuantita().toDouble();
        double somma = ca-quan;
	//Aggiorno il prezzo senza iva 
	double ps_iva = getPrezzoSIVA().toDouble();
        double sum_pr_siva = _p_siva-ps_iva;
	//Aggiorno il prezzo con iva
	double pc_iva = getPrezzoCIVA().toDouble();
        double sum_pr_civa = _p_civa-pc_iva;
	//Aggiorno il totale
	double tot = getTotale().toDouble();
        double sum_tot = _tot-tot;
  
        QSqlQuery Query2;
        Query2.prepare("UPDATE carico_magazzino SET  codbarre='"+getCodBarre()+"',nome_articolo='"+getNomeArticolo()+"', "
                          "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+getDesc()+"', unita='"+getUnita()+"', " 
			  "prezzo_unitario='"+getPrezzoUnit()+"',prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(getIVA().toDouble())+"', quantita='"+QString::number(somma)+"',  "
                          "sconto='"+getSconto()+"', totale='"+QString::number(sum_tot)+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+getListino()+"', categoria='"+getCateg()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto='"+getNumeroLotto()+"', data_lotto='"+getDataLotto()+"', addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+getCodArt()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("fattura_rg","Aggiornamento carico magazzino da FATTURA VENDITA effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("fattura_rg","Impossibile aggiornare carico magazzino da FATTURA VENDITA: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}


void fattura_rg::update_scarico_prod(double ca, double _p_siva, double _p_civa, double _tot)
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
        Query2.prepare("UPDATE scarico_mag SET  codbarre='"+getCodBarre()+"',nome_articolo='"+getNomeArticolo()+"', "
                          "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+getDesc()+"', unita='"+getUnita()+"', " 
			  "prezzo_unitario='"+getPrezzoUnit()+"',prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(getIVA().toDouble())+"', quantita='"+QString::number(somma)+"',  "
                          "sconto='"+getSconto()+"', totale='"+QString::number(sum_tot)+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+getListino()+"', categoria='"+getCateg()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto='"+getNumeroLotto()+"', data_lotto='"+getDataLotto()+"', addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+getCodArt()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("fattura_rg","Aggiornamento scarico magazzino da FATTURA VENDITA effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("fattura_rg","Impossibile aggiornare scarico magazzino da FATTURA VENDITA: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

void fattura_rg::insert_scarico_prod()
{
    QSqlQuery Query;
    Query.prepare("INSERT INTO scarico_mag VALUES ('"+getCodArt()+"','"+getCodBarre()+"','"+getNomeArticolo()+"','"+getDesc()+"','"+getUnita()+"',"
			"'"+getPrezzoUnit()+"','"+getPrezzoSIVA()+"','"+getPrezzoCIVA()+"',"
			"'"+QString::number(getIVA().toDouble())+"','"+getQuantita()+"','"+getSconto()+"','"+getTotale()+"','"+QString::number(QDate::currentDate().year())+"',"
			"'"+getListino()+"','"+getCateg()+"','"+QDate::currentDate().toString("yyyy-MM-dd")+"','"+getNumeroLotto()+"','"+getDataLotto()+"'', '"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')");
    
    try{
      if(Query.exec()){
           LOG_FILE(qApp->translate("fattura_rg","Inserimento scarico magazzino da FATTURA VENDITA effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
      }
      else{
	   LOG_FILE(qApp->translate("fattura_rg","Impossibile inserire scarico magazzino da FATTURA VENDITA: ")+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	   throw InvalidQuery(Query.lastError().text().toStdString().c_str());
      }
      }
      catch(FabariaException e)
      {
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
}

QString fattura_rg::getFATTURAID()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colID = mod_grid->index(riga,0,QModelIndex());
    QString IDcol = mod_grid->data(colID).toString();
    return IDcol;
}

QString fattura_rg::getCodArt()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCodArt = mod_grid->index(riga,1,QModelIndex());
    QString CodArtCol = mod_grid->data(colCodArt).toString();
    return CodArtCol;
}

QString fattura_rg::getCodBarre()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCodBarre = mod_grid->index(riga,2,QModelIndex());
    QString CodBarreCol = mod_grid->data(colCodBarre).toString();
    return CodBarreCol;
}

QString fattura_rg::getNomeArticolo()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colNomeArticolo = mod_grid->index(riga,3,QModelIndex());
    QString NomeArticoloCol = mod_grid->data(colNomeArticolo).toString();
    return NomeArticoloCol;
}

QString fattura_rg::getDesc()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDesc = mod_grid->index(riga,4,QModelIndex());
    QString DescCol = mod_grid->data(colDesc).toString();
    return DescCol;
}

QString fattura_rg::getUnita()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colUnita = mod_grid->index(riga,5,QModelIndex());
    QString UnitaCol = mod_grid->data(colUnita).toString();
    return UnitaCol;
}

QString fattura_rg::getPrezzoUnit()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoUnit = mod_grid->index(riga,6,QModelIndex());
    QString PrezzoUnitCol = mod_grid->data(colPrezzoUnit).toString();
    return PrezzoUnitCol;
}

QString fattura_rg::getPrezzoSIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoSIVA = mod_grid->index(riga,7,QModelIndex());
    QString PrezzoSIVACol = mod_grid->data(colPrezzoSIVA).toString();
    return PrezzoSIVACol;
}

QString fattura_rg::getPrezzoCIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoCIVA = mod_grid->index(riga,8,QModelIndex());
    QString PrezzoCIVACol = mod_grid->data(colPrezzoCIVA).toString();
    return PrezzoCIVACol;
}

QString fattura_rg::getIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colIVA = mod_grid->index(riga,9,QModelIndex());
    QString IVACol = mod_grid->data(colIVA).toString();
    return IVACol;
}

QString fattura_rg::getQuantita()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colQuantita = mod_grid->index(riga,10,QModelIndex());
    QString QuantitaCol = mod_grid->data(colQuantita).toString();
    return QuantitaCol;
}

QString fattura_rg::getSconto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colSconto = mod_grid->index(riga,11,QModelIndex());
    QString ScontoCol = mod_grid->data(colSconto).toString();
    return ScontoCol;
}

QString fattura_rg::getTotale()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colTotale = mod_grid->index(riga,12,QModelIndex());
    QString TotaleCol = mod_grid->data(colTotale).toString();
    return TotaleCol;
}

QString fattura_rg::getAnnoIns()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colAnnoIns = mod_grid->index(riga,13,QModelIndex());
    QString AnnoInsCol = mod_grid->data(colAnnoIns).toString();
    return AnnoInsCol;
}

QString fattura_rg::getListino()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colListino = mod_grid->index(riga,14,QModelIndex());
    QString ListinoCol = mod_grid->data(colListino).toString();
    return ListinoCol;
}

QString fattura_rg::getCateg()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCateg = mod_grid->index(riga,15,QModelIndex());
    QString CategCol = mod_grid->data(colCateg).toString();
    return CategCol;
}

QString fattura_rg::getDataInsert()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,16,QModelIndex());
    QString DataInsertCol = mod_grid->data(colDataInsert).toString();
    return DataInsertCol;
}

QString fattura_rg::getDataLotto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,18,QModelIndex());
    QString DataInsertLotto = mod_grid->data(colDataInsert).toString();
    return DataInsertLotto;
}

QString fattura_rg::getNumeroLotto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,17,QModelIndex());
    QString numeroLotto = mod_grid->data(colDataInsert).toString();
    return numeroLotto;
}

fattura_rg::~fattura_rg()
{

}
