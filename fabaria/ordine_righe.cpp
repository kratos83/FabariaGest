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
#include "ordine_righe.h"
#include "ui_ordine_righe.h"

#include "user.h"
#include "fornitori.h"
#include "itdelegato.h"
#include "ordine.h"
#include "defines.h"
#include "ordine_righe_art.h"
#include "print.h"
#include "mainwindow.h"
#include "LineEditDelegate.h"
#include <iostream>
#include <QLoggingCategory>
#include <QtCore>
#include "magazzino.h"

using namespace std;
using namespace Defines;

ordine_righe::ordine_righe(QWidget *parent) :
    QDialog(parent), 
    prezzos_iva(0),
    prezzo_c_iva(0),
    perc(0),
    totale(0)
{
    setupUi(this);
    setWindowTitle(qApp->translate("ordine_righe","Crea ordine"));

    //Chiudi la finestra di dialogo
    annulla_2->setShortcut(qApp->translate("ordine_righe","Ctrl+A"));
    connect(annulla_2,SIGNAL(clicked()),this,SLOT(chiudi()));
    //Apri la finestra per l'inserimento dei clienti
    cliente->setShortcut(qApp->translate("ordine_righe","Ctrl+C"));
    connect(cliente,SIGNAL(clicked()),this,SLOT(open_cliente()));
    //Apri la finestra per l'inserimento dei fornitori
    fornit->setShortcut(qApp->translate("ordine_righe","Ctrl+F"));
    connect(fornit,SIGNAL(clicked()),this,SLOT(open_fornitore()));
    connect(tipo_cliente,SIGNAL(currentIndexChanged(int)),this,SLOT(filtro()));
    //Apri la finestra per inserire le righe
    n_riga->setShortcut(qApp->translate("ordine_righe","Ctrl+R"));
    connect(n_riga,SIGNAL(clicked()),this,SLOT(ord_riga_open()));
    //Apri la finestra per modificare la riga
    md_riga->setShortcut(qApp->translate("ordine_righe","Ctrl+M"));
    connect(md_riga,SIGNAL(clicked()),this,SLOT(modifica_riga()));
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(tab_view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(modifica_riga()));
    connect(ord_insert_row,SIGNAL(clicked()),this,SLOT(addRow()));
    connect(add_row,SIGNAL(clicked(QModelIndex)),this,SLOT(InserisciRiga()));
    connect(spes_tr,SIGNAL(textEdited(const QString &)),this,SLOT(spesa_trasp(const QString &)));
    connect(spes_in,SIGNAL(textEdited(const QString &)),this,SLOT(spesa_in(const QString &)));
    connect(sconto1,SIGNAL(textEdited(const QString &)),this,SLOT(sconto_1t(const QString &)));
    connect(sconto2,SIGNAL(textEdited(const QString &)),this,SLOT(sconto_2t(const QString &)));
    connect(sconto3,SIGNAL(textEdited(const QString &)),this,SLOT(sconto_3t(const QString &)));
    //Elimina la riga selezionata
    el_riga->setShortcut(qApp->translate("ordine_righe","Ctrl+E"));
    connect(el_riga,SIGNAL(clicked()),this,SLOT(elimina()));
    connect(del_riga,SIGNAL(clicked()),this,SLOT(elimina()));
    //Importo righe da file excel 
    excel->setShortcut(qApp->translate("ordine_righe","Ctrl+I"));
    connect(excel,SIGNAL(clicked()),this,SLOT(importa()));
    connect(im_ex_file,SIGNAL(clicked()),this,SLOT(importa()));
    //Salva le informazioni relative al ddt
    save_2->setShortcut(qApp->translate("ordine_righe","Ctrl+S"));
    connect(save_2,SIGNAL(clicked()),this,SLOT(ord_inserisci()));
    //Aggiorna le informazioni relative al ddt
    agg_fatt_2->setShortcut(qApp->translate("ordine_righe","Ctrl+U"));
    connect(agg_fatt_2,SIGNAL(clicked()),this,SLOT(ord_aggiorna()));
    cliente->setToolTip(qApp->translate("ordine_righe","Inserisci cliente"));
    fornit->setToolTip(qApp->translate("ordine_righe","Inserisci fornitore"));
    QStringList lis;
    lis << qApp->translate("ordine_righe","Cliente ") << qApp->translate("ordine_righe","Fornitore");
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
    //Enable combobox text
    add_row->viewport()->installEventFilter(this);
    pagamento_ord();
    lista_prodotti();
    trasp_causali();
    porto();
    aspetto_beni();
    consegna_trasp();
    listino();
}

void ordine_righe::formatta_num()
{
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
}

void ordine_righe::filtro()
{
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

void ordine_righe::trasp_causali()
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

void ordine_righe::consegna_trasp()
{
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

void ordine_righe::porto()
{
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

void ordine_righe::listino()
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

void ordine_righe::aspetto_beni()
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

void ordine_righe::open_cliente()
{
    user *utente = new user(this);
    utente->setModal(true);
    utente->nuovo();
    connect(utente,SIGNAL(clienti_rel()),this,SLOT(cliente_add()));
    utente->exec();
}

void ordine_righe::cliente_add()
{
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

int ordine_righe::open_cl(int index)
{
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

void ordine_righe::open_fornitore()
{
    fornitori *fo = new fornitori(this);
    fo->setModal(true);
    fo->nuovo();
    connect(fo,SIGNAL(vis_forn()),this,SLOT(combo_fornitore()));
    fo->exec();
}

void ordine_righe::combo_fornitore()
{
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

int ordine_righe::open_fl(int index)
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

void ordine_righe::pagamento_ord()
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


void ordine_righe::lista_prodotti()
{
    mod_grid->setTable("ordine_righe_art");
    mod_grid->setFilter("id='"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    mod_grid->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mod_grid->select();
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("ordine_righe","Codice"));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("ordine_righe","Cod. Barre"));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("ordine_righe","Prodotto"));
    mod_grid->setHeaderData(4, Qt::Horizontal, qApp->translate("ordine_righe","Descrizione"));
    mod_grid->setHeaderData(5, Qt::Horizontal, QString::fromUtf8(qApp->translate("ordine_righe","Unità").toStdString().c_str()));
    mod_grid->setHeaderData(6, Qt::Horizontal, qApp->translate("ordine_righe","Prezzo unitario"));
    mod_grid->setHeaderData(7, Qt::Horizontal, qApp->translate("ordine_righe","Prezzo S. IVA"));
    mod_grid->setHeaderData(8, Qt::Horizontal, qApp->translate("ordine_righe","Prezzo C. IVA"));
    mod_grid->setHeaderData(9, Qt::Horizontal, qApp->translate("ordine_righe","IVA"));
    mod_grid->setHeaderData(10, Qt::Horizontal, QString::fromUtf8(qApp->translate("ordine_righe","Quantità").toStdString().c_str()));
    mod_grid->setHeaderData(11, Qt::Horizontal, qApp->translate("ordine_righe","Sconto"));
    mod_grid->setHeaderData(12,Qt::Horizontal,qApp->translate("ordine_righe","Totale"));
    mod_grid->setHeaderData(17,Qt::Horizontal,qApp->translate("ordine_righe","Lotto"));
    mod_grid->setHeaderData(18,Qt::Horizontal,qApp->translate("ordine_righe","Scadenza lotto"));
    mod_grid->setHeaderData(19,Qt::Horizontal,qApp->translate("ordine_righe","Magazzino"));
    
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

void ordine_righe::clickgrid()
{
    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
}

void ordine_righe::ord_riga_open()
{
    ordine_righe_art *ft = new ordine_righe_art(this,id1->text().toInt());
    ft->setModal(true);
    ft->setWindowTitle(qApp->translate("ordine_righe","Inserisci articoli"));
    connect(ft,SIGNAL(save_ord()),this,SLOT(lista_prodotti()));
    connect(ft,SIGNAL(save_ord()),this,SLOT(tot_imp_iva()));
    ft->exec();
    lista_prodotti();
}

void ordine_righe::ord_aggiorna()
{
    if(pers->currentText()==""){
            QMessageBox MsgBox;
            MsgBox.setWindowTitle("FabariaGest");
            MsgBox.setText(qApp->translate("ordine_righe","Errore"));
            MsgBox.setInformativeText(QString::fromUtf8(qApp->translate("ordine_righe","Impossibile aggiornare l'ordine,\n poichè non hai inserito i dati correttamente...").toStdString().c_str()));
            MsgBox.setIcon(QMessageBox::Warning);
            MsgBox.exec();
            pers->setStyleSheet("background-color: rgb(249, 22, 5)");
        }
        else{
            QSqlQuery query;
            query.prepare("UPDATE ordine_righe SET data='"+data_fatt->date().toString("yyyy-MM-dd")+"',fornitore='"+pers->currentText()+"',tipo_cliente='"+QString::number(tipo_cliente->currentIndex())+"',sconto_1='"+sconto1->text()+"',"
                          " sconto_2='"+sconto2->text()+"',sconto_3='"+sconto3->text()+"',spese_trasporto='"+spes_tr->text()+"',spese_incasso='"+spes_in->text()+"', "
                          " annotazioni='"+ann->toPlainText()+"',vs_preventivo='"+ordine->text()+"',tipo_pagamento='"+pagam->currentText()+"',"
                          " note_pagamento='"+lineEdit->text()+"',rag_soc='"+rag_soc->text()+"',indirizzo='"+indirizzo->text()+"',cap='"+cap->text()+"',localita='"+local->text()+"', "
                          " prov='"+prov->text()+"',telefono='"+tel->text()+"',"
                          " fax='"+cell->text()+"', piva_cfis='"+paese->text()+"',prezzo_s_iva='"+imponibile_2->text()+"',"
                          " iva='"+iva_ft_2->text()+"',totale='"+totale_2->text()+"', "
                          " causale_trasporto='"+causali->currentText()+"', asp_est='"+aspetto_est->currentText()+"', vettore='"+vettore->currentText()+"',cons_trasp='"+consegna->currentText()+"', "
                          " porto='"+porto_ass->currentText()+"', n_colli='"+QString::number(num_colli->text().toDouble())+"', peso_lordo='"+QString::number(peso_lordo->text().toDouble())+"', data_ora='"+data_ora->dateTime().toString("dd/MM/yyyy hh:mm:ss")+"', "
                          " peso_netto='"+QString::number(peso_netto->text().toDouble())+"',anno_ins='"+QString::number(QDate::currentDate().year())+"', codice_listino='"+tipo_listino->currentText()+"' "
                          " WHERE id ='"+id1->text()+"'");

            try{
            if( query.exec()){
                update_f_prodotti();
            }
            else{
		LOG_FILE(qApp->translate("ordine_righe","L'ordine non si puo aggiornare"),LOG_WARNING_ALL,QIODevice::Append);
		LOG_FILE(qApp->translate("ordine_righe","Impossibile aggiornare l'ordine ")+query.lastError().text(),LOG_WARNING_ALL,QIODevice::Append);
                throw InvalidQuery(query.lastError().text().toStdString().c_str());
            }
	    }
	    catch(FabariaException e)
	    {
	      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	    }

            lista_prodotti();
            emit salva_ord();
            ordine_prodotti->lista_ord();
            close();
            }
}

void ordine_righe::ord_inserisci()
{
    if(pers->currentText()==""){
            QMessageBox MsgBox;
            MsgBox.setWindowTitle("FabariaGest");
            MsgBox.setText(qApp->translate("ordine_righe","Errore"));
            MsgBox.setInformativeText(QString::fromUtf8(qApp->translate("ordine_righe","Impossibile inserire l'ordine',\n poichè non hai inserito i dati correttamente...").toStdString().c_str()));
            MsgBox.setIcon(QMessageBox::Warning);
            MsgBox.exec();
            pers->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else{
    QSqlQuery query;
    query.prepare("INSERT INTO ordine_righe VALUES ('"+id1->text()+"','"+data_fatt->date().toString("yyyy-MM-dd")+"','"+pers->currentText()+"',"
		   "'"+QString::number(tipo_cliente->currentIndex())+"','"+sconto1->text()+"','"+sconto2->text()+"','"+sconto3->text()+"',"
		   "'"+spes_tr->text()+"','"+spes_in->text()+"','"+ann->toPlainText()+"','"+ordine->text()+"','"+pagam->currentText()+"',"
		   "'"+lineEdit->text()+"','"+rag_soc->text()+"','"+indirizzo->text()+"','"+cap->text()+"','"+local->text()+"','"+prov->text()+"',"
		   "'"+tel->text()+"','"+cell->text()+"','"+paese->text()+"','"+imponibile_2->text()+"','"+iva_ft_2->text()+"','"+totale_2->text()+"',"
		   "'"+causali->currentText()+"','"+aspetto_est->currentText()+"','"+vettore->currentText()+"','"+consegna->currentText()+"',"
		   "'"+porto_ass->currentText()+"','"+num_colli->text()+"','"+peso_lordo->text()+"','"+data_ora->dateTime().toString("yyyy-MM-dd hh:mm:ss")+"',"
		   "'"+peso_netto->text()+"','"+QString::number(QDate::currentDate().year())+"','"+tipo_listino->currentText()+"') ");
  try{
    if(query.exec()){
        insert_f_prodotti();
    }
    else{
	LOG_FILE(qApp->translate("ordine_righe","Impossibile inserire l'Ordine: ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(qApp->translate("ordine_righe","Impossibile inserire l'ordine...").toStdString());
    }
   }
   catch(FabariaException e)
   {
     LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
   }
    emit salva_ord();
    tot_imp_iva();
    close();
    ordine_prodotti->lista_ord();
    }
}

void ordine_righe::insert_f_prodotti()
{
    QSqlQuery query1;
    query1.prepare("INSERT INTO ordine  VALUES('"+id1->text()+"','"+data_fatt->date().toString("yyyy-MM-dd")+"',"
		   "'"+pers->currentText()+"','Ordine','"+totale_2->text()+"','"+QString::number(QDate::currentDate().year())+"','"+tipo_listino->currentText()+"','')");
    
    try{
    if(query1.exec()){
        LOG_FILE(qApp->translate("ordine_righe","Inserimento effettuato con successo(Ordine)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
	LOG_FILE(qApp->translate("ordine_righe","Impossibile inserire l'ordine: ")+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(query1.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void ordine_righe::update_f_prodotti()
{
    QSqlQuery query1;
    query1.prepare("UPDATE ordine SET data='"+data_fatt->date().toString("yyyy-MM-dd")+"',fornitore='"+pers->currentText()+"',totale='"+totale_2->text()+"',tipo_ordine='Ordine',"
		   "anno_ins='"+QString::number(QDate::currentDate().year())+"',evaso='Ordine',codice_listino='"+tipo_listino->currentText()+"' WHERE id='"+id1->text()+"'");

    try{
    if(query1.exec()){
        LOG_FILE(qApp->translate("ordine_righe","Aggiornamento effettuato con successo(Ordine)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
	LOG_FILE(qApp->translate("ordine_righe","Impossibile aggiornare l'ordine: ")+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(query1.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void ordine_righe::modifica_riga()
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
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Selezionare una riga da modificare..."));
    }
}

void ordine_righe::modifica_riga_one(QModelIndex index)
{
    //Tentativo di aggiornamento del record perché trovato
    if(index.row() < mod_grid->rowCount()){
        ordine_righe_art *gt = new ordine_righe_art(this,id1->text().toInt());
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
                 connect(gt,SIGNAL(save_ord()),this,SLOT(tot_imp_iva()));
                 connect(gt,SIGNAL(save_ord()),this,SLOT(lista_prodotti()));
                 lista_prodotti();

        }
        connect(gt,SIGNAL(save_ord()),this,SLOT(tot_imp_iva()));
        gt->exec();
    }
}

void ordine_righe::elimina()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
        if (!id1->text().isEmpty())
        {

            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM ordine_righe_art WHERE id = '"+id1->text()+"' LIMIT 1");
            qctrl.exec();

                QMessageBox *box= new QMessageBox(this);
                box->setWindowTitle("FabariaGest");
                box->setInformativeText(qApp->translate("ordine_righe","Vuoi eliminare veramente \n il record selezionato?...."));
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

void ordine_righe::elimina_riga()
{
    QSqlQuery query,query1;
    const char* error = "";
    query.prepare("DELETE FROM ordine_righe_art WHERE id = id ='"+id1->text()+"' LIMIT 1");
    query1.prepare("DELETE FROM ordine_righe WHERE id = id ='"+id1->text()+"' LIMIT 1");
   
    try{
    if (query.exec() && query1.exec()) //Se esito OK Eliminato da DB
    {
        lista_prodotti();
	LOG_FILE("Eliminazione avvenuta con successo...",LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
        LOG_FILE(qApp->translate("ordine_righe","Impossibile eliminare l'ordine: ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE(qApp->translate("ordine_righe","Impossibile eliminare l'ordine delle righe: ")+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw error ? InvalidQuery(query.lastError().text().toStdString().c_str()) : InvalidQuery(query1.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    emit salva_ord();
    lista_prodotti();
    tot_imp_iva();
}

void ordine_righe::sconto_1t(const QString &)
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

void ordine_righe::sconto_2t(const QString &)
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

void ordine_righe::sconto_3t(const QString &)
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

void ordine_righe::spesa_trasp(const QString &)
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

void ordine_righe::spesa_in(const QString &)
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

void ordine_righe::tot_imp_iva()
{
    QSqlQuery Query1,Query2,Query3;
    QSqlQuery query;
    query.prepare("select id from ordine_righe_art where id= '"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){

        Query1.prepare("select sum(prezzo_s_iva) from ordine_righe_art where id='"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query1.exec();
    if(Query1.next()){
        imponibile_2->setText(QString::number(Query1.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
    }

    Query2.prepare("select sum(totale-prezzo_s_iva) from ordine_righe_art where id='"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query2.exec();
    if(Query2.next()){
        iva_ft_2->setText(QString::number(Query2.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Impossibile instanziare l'iva... ")+Query2.lastError().text());
    }


    Query3.prepare("select sum(totale) from ordine_righe_art where id ='"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query3.exec();
    if(Query3.next()){
        totale_2->setText(QString::number(Query3.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Impossibile instanziare il totale... ")+Query3.lastError().text());
    }
    }
    emit salva_ord();
}

void ordine_righe::totale_impo_iva()
{
    QSqlQuery Query1,Query2,Query3;
    QSqlQuery query;
    query.prepare("select id from ordine_righe where id= '"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){

        Query1.prepare("select sum(prezzo_s_iva) from ordine_righe where id='"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query1.exec();
    if(Query1.next()){
        imponibile_2->setText(QString::number(Query1.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Impossibile instanziare il prezzo senza iva... ")+Query1.lastError().text());
    }

    Query2.prepare("select sum(totale-prezzo_s_iva) from ordine_righe where id='"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query2.exec();
    if(Query2.next()){
        iva_ft_2->setText(QString::number(Query2.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Impossibile instanziare l'iva... ")+Query2.lastError().text());
    }


    Query3.prepare("select sum(totale) from ordine_righe where id ='"+id1->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Query3.exec();
    if(Query3.next()){
        totale_2->setText(QString::number(Query3.value(0).toDouble()));
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Impossibile instanziare il totale... ")+Query3.lastError().text());
    }
    }
    emit salva_ord();
}

void ordine_righe::chiudi()
{
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("FabariaGest");
    box->setInformativeText(qApp->translate("ordine_righe","Sei sicuro di voler uscire dall'ordine?\n I dati non verranno salvati...."));
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

void ordine_righe::importa()
{ 
  QString testo = qApp->translate("ordine_righe","ID\nCodice articolo\nCodice a barre\nNome prodotto\nDescrizione\nUnità di misura\n"
      "Prezzo unitario\nPrezzo senza iva\nPrezzo con iva\nAliquota IVA\n"
      "Quantità\nSconto\nTotale\nAnno inserimento\nListino\nCategoria\n"
      "Data inserimento,\nNumero lotto,\nScadenza lotto,\nMagazzino\n\n\n\n\nAttenzione tutte le celle\ndevono essere compilate.");
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle(qApp->translate("ordine_righe","FabariaGest"));
    box->setText(qApp->translate("ordine_righe","Il file excel\ndeve essere composto:\n"));
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

void ordine_righe::import_file_excel()
{
    QString filePath = QFileDialog::getOpenFileName(0, qApp->translate("ordine_righe","Apri file excel"), QString(), "*.xlsx");
    if(filePath.isEmpty())
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Non hai aperto nesssun file"));
    else{
    Document xlsx(filePath);
        
    for(int i=2; i <= xlsx.dimension().rowCount(); i++)
    {
      QProgressDialog progressDialog(this);
      progressDialog.setRange(0, xlsx.dimension().rowCount());
      progressDialog.setWindowModality(Qt::WindowModal);
      progressDialog.setWindowTitle(qApp->translate("ordine_righe","Inserimento dati da excel"));
      progressDialog.setValue(i);
      progressDialog.setLabelText(qApp->translate("ordine_righe","Inserimento dati %1 di %2...")
                      .arg(i).arg(xlsx.dimension().rowCount()));
      qApp->processEvents();
      QSqlQuery query,query_id,query_up;
      QString req = "INSERT INTO ordine_righe_art(id,cod_art,codbarre,nome_articolo,descrizione,unita, "
		    "prezzo_unit,prezzo_s_iva,prezzo_c_iva,iva,quantita,sconto,totale, "
		    "anno_ins,codice_listino,categoria,data_insert,numero_lotto,data_lotto,addMagazzino ) VALUES('";
      Cell *code_art = xlsx.cellAt(i,2);
      query_up.prepare("select * from ordine_righe_art where cod_art='"+code_art->value().toString()+"'");
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
           qDebug() <<  "Inserimento effettuato";
      else
           QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Errore Inserimento:\n ")+query.lastError().text());
    }
    }
    }
    lista_prodotti();
}

void ordine_righe::aggiorna_file_excel(QString doc)
{
    Document xlsx(doc);
        
    for(int i=2; i <= xlsx.dimension().rowCount(); i++)
    {
      QProgressDialog progressDialog(this);
      progressDialog.setRange(0, xlsx.dimension().rowCount());
      progressDialog.setWindowModality(Qt::WindowModal);
      progressDialog.setWindowTitle(qApp->translate("ordine_righe","Aggionamento dati da excel"));
      progressDialog.setValue(i);
      progressDialog.setLabelText(qApp->translate("ordine_righe","Aggionamento dati %1 di %2...")
                      .arg(i).arg(xlsx.dimension().rowCount()));
      qApp->processEvents();
      QSqlQuery query,query_up;
      query_up.prepare("select * from ordine_righe_art where id='"+id1->text()+"'");
      query_up.exec();
      if(query_up.next())
      {
      QString req = "UPDATE ordine_righe_art SET cod_art='";
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
           qDebug() <<  "Aggionamento effettuato";
      else
           QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Errore aggiornamento:\n ")+query.lastError().text());
    }
    }
    lista_prodotti();
}

void ordine_righe::addRow()
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
        QMessageBox::warning(this,"FabariaGest",qApp->translate("ordine_righe","Devi creare almeno un deposito magazzino"));
        Magazzino *mg = new Magazzino(this);
        mg->exec();
    }
}

bool ordine_righe::InserisciRiga()
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

bool ordine_righe::eventFilter(QObject *o, QEvent *j)
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

void ordine_righe::submit()
{
      mod_grid->database().transaction();
       if (mod_grid->submitAll()) {
           mod_grid->database().commit();
       } else {
           mod_grid->database().rollback();
           QMessageBox::warning(this, qApp->translate("ordine_righe","FabariaGest"),
                                qApp->translate("ordine_righe","Errore nel database: %1")
                                .arg(mod_grid->lastError().text()));
       }
       tot_imp_iva();
}

void ordine_righe::update_articoli(double qua, double _p_siva, double _p_civa, double _tot)
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
              "numero_lotto='"+getNumeroLotto()+"', data_lotto='"+getDataLotto()+"' , addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+getCodArt()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("ordine_righe","Aggiornamento articoli da ORDINE effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("ordine_righe","Impossibile aggiornare articoli da ORDINE: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

void ordine_righe::update_carico_prod(double ca, double _p_siva, double _p_civa, double _tot)
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
              "numero_lotto='"+getNumeroLotto()+"', data_lotto='"+getDataLotto()+"' , addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+getCodArt()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("ordine_righe","Aggiornamento carico magazzino da ORDINE effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("ordine_righe","Impossibile aggiornare carico magazzino da ORDINE: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}


void ordine_righe::update_scarico_prod(double ca, double _p_siva, double _p_civa, double _tot)
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
              "numero_lotto='"+getNumeroLotto()+"', data_lotto='"+getDataLotto()+"' , addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+getCodArt()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("ordine_righe","Aggiornamento scarico magazzino da ORDINE effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("ordine_righe","Impossibile aggiornare scarico magazzino da ORDINE: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

void ordine_righe::insert_scarico_prod()
{
    QSqlQuery Query;
    Query.prepare("INSERT INTO scarico_mag VALUES ('"+getCodArt()+"','"+getCodBarre()+"','"+getNomeArticolo()+"','"+getDesc()+"','"+getUnita()+"',"
			"'"+getPrezzoUnit()+"','"+getPrezzoSIVA()+"','"+getPrezzoCIVA()+"',"
			"'"+QString::number(getIVA().toDouble())+"','"+getQuantita()+"','"+getSconto()+"','"+getTotale()+"','"+QString::number(QDate::currentDate().year())+"',"
			"'"+getListino()+"','"+getCateg()+"','"+QDate::currentDate().toString("yyyy-MM-dd")+"','"+getNumeroLotto()+"','"+getDataLotto()+", '"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')");
    
    try{
      if(Query.exec()){
           LOG_FILE(qApp->translate("ordine_righe","Inserimento scarico magazzino da ORDINE effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
      }
      else{
	   LOG_FILE(qApp->translate("ordine_righe","Impossibile inserire scarico magazzino da ORDINE: ")+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	   throw InvalidQuery(Query.lastError().text().toStdString().c_str());
      }
      }
      catch(FabariaException e)
      {
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
}

QString ordine_righe::getORDID()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colID = mod_grid->index(riga,0,QModelIndex());
    QString IDcol = mod_grid->data(colID).toString();
    return IDcol;
}

QString ordine_righe::getCodArt()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCodArt = mod_grid->index(riga,1,QModelIndex());
    QString CodArtCol = mod_grid->data(colCodArt).toString();
    return CodArtCol;
}

QString ordine_righe::getCodBarre()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCodBarre = mod_grid->index(riga,2,QModelIndex());
    QString CodBarreCol = mod_grid->data(colCodBarre).toString();
    return CodBarreCol;
}

QString ordine_righe::getNomeArticolo()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colNomeArticolo = mod_grid->index(riga,3,QModelIndex());
    QString NomeArticoloCol = mod_grid->data(colNomeArticolo).toString();
    return NomeArticoloCol;
}

QString ordine_righe::getDesc()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDesc = mod_grid->index(riga,4,QModelIndex());
    QString DescCol = mod_grid->data(colDesc).toString();
    return DescCol;
}

QString ordine_righe::getUnita()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colUnita = mod_grid->index(riga,5,QModelIndex());
    QString UnitaCol = mod_grid->data(colUnita).toString();
    return UnitaCol;
}

QString ordine_righe::getPrezzoUnit()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoUnit = mod_grid->index(riga,6,QModelIndex());
    QString PrezzoUnitCol = mod_grid->data(colPrezzoUnit).toString();
    return PrezzoUnitCol;
}

QString ordine_righe::getPrezzoSIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoSIVA = mod_grid->index(riga,7,QModelIndex());
    QString PrezzoSIVACol = mod_grid->data(colPrezzoSIVA).toString();
    return PrezzoSIVACol;
}

QString ordine_righe::getPrezzoCIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colPrezzoCIVA = mod_grid->index(riga,8,QModelIndex());
    QString PrezzoCIVACol = mod_grid->data(colPrezzoCIVA).toString();
    return PrezzoCIVACol;
}

QString ordine_righe::getIVA()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colIVA = mod_grid->index(riga,9,QModelIndex());
    QString IVACol = mod_grid->data(colIVA).toString();
    return IVACol;
}

QString ordine_righe::getQuantita()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colQuantita = mod_grid->index(riga,10,QModelIndex());
    QString QuantitaCol = mod_grid->data(colQuantita).toString();
    return QuantitaCol;
}

QString ordine_righe::getSconto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colSconto = mod_grid->index(riga,11,QModelIndex());
    QString ScontoCol = mod_grid->data(colSconto).toString();
    return ScontoCol;
}

QString ordine_righe::getTotale()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colTotale = mod_grid->index(riga,12,QModelIndex());
    QString TotaleCol = mod_grid->data(colTotale).toString();
    return TotaleCol;
}

QString ordine_righe::getAnnoIns()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colAnnoIns = mod_grid->index(riga,13,QModelIndex());
    QString AnnoInsCol = mod_grid->data(colAnnoIns).toString();
    return AnnoInsCol;
}

QString ordine_righe::getListino()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colListino = mod_grid->index(riga,14,QModelIndex());
    QString ListinoCol = mod_grid->data(colListino).toString();
    return ListinoCol;
}

QString ordine_righe::getCateg()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colCateg = mod_grid->index(riga,15,QModelIndex());
    QString CategCol = mod_grid->data(colCateg).toString();
    return CategCol;
}

QString ordine_righe::getDataInsert()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,16,QModelIndex());
    QString DataInsertCol = mod_grid->data(colDataInsert).toString();
    return DataInsertCol;
}

QString ordine_righe::getDataLotto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,18,QModelIndex());
    QString DataInsertLotto = mod_grid->data(colDataInsert).toString();
    return DataInsertLotto;
}

QString ordine_righe::getNumeroLotto()
{
    int riga = add_row->selectionModel()->currentIndex().row();
    QModelIndex colDataInsert = mod_grid->index(riga,17,QModelIndex());
    QString numeroLotto = mod_grid->data(colDataInsert).toString();
    return numeroLotto;
}

ordine_righe::~ordine_righe()
{

}
