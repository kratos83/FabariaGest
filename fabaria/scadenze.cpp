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
#include "scadenze.h"
#include "ui_scadenze.h"
#include "itdelegato.h"
#include "mainwindow.h"
#include "print.h"
#include "settingsmanager.h"
#include "defines.h"
#include "queryexception.h"

using namespace std;
using namespace Defines;
using namespace QXlsx;

scadenze::scadenze(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    
    //Esci dalla finestra di dialogo
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    esci->setShortcut(qApp->translate("scadenze","Ctrl+E"));
    connect(tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(checkBox,SIGNAL(clicked()),this,SLOT(check_list()));
    //Aggiorno le scadenze
    connect(aggiorna_db,SIGNAL(clicked()),this,SLOT(aggiorna()));
    aggiorna_db->setShortcut(qApp->translate("scadenze","Ctrl+U"));
    connect(aggiorna_db,SIGNAL(clicked(bool)),this,SLOT(agg_riavvia_count()));
    connect(lib_rientro,SIGNAL(clicked()),this,SLOT(aggiorna()));
    connect(list_r,SIGNAL(clicked()),this,SLOT(clicca_fatture_pagate()));
    connect(cerca_txt,SIGNAL(textEdited(QString)),this,SLOT(cerca_fatture(QString)));
    //Cerco una scadenza
    connect(_search,SIGNAL(clicked()),this,SLOT(filtro()));
    _search->setShortcut(qApp->translate("scadenze","Ctrl+F"));
    //Cancello una scadenza
    connect(cancella,SIGNAL(clicked()),this,SLOT(elimina()));
    cancella->setShortcut(qApp->translate("scadenze","Ctrl+D"));
    connect(cancella,SIGNAL(clicked(bool)),this,SLOT(el_riavvia_count()));
    connect(scad_m_p,SIGNAL(clicked()),this,SLOT(scadenza_m_precedente()));
    connect(scad_m_s,SIGNAL(clicked()),this,SLOT(scadenza_m_successivo()));
    connect(scad_a_c,SIGNAL(clicked()),this,SLOT(scadenza_a_corrente()));
    connect(sit_fornitori,SIGNAL(clicked()),this,SLOT(situazione_fornitori()));
    //Anteprima di stampa scadenze
    connect(ant_scadenze,SIGNAL(clicked()),this,SLOT(anteprima_di_stampa()));
    ant_scadenze->setShortcut(qApp->translate("scadenze","Ctrl+A"));
    //Stampo lista scadenze
    connect(dist_riba,SIGNAL(clicked()),this,SLOT(stampa_scadenze()));
    dist_riba->setShortcut(qApp->translate("scadenze","Ctrl+P"));
    //Esporta in pdf le scadenze
    connect(esportapdf,SIGNAL(clicked()),this,SLOT(esporta_pdf_scadenze()));
    esportapdf->setShortcut(qApp->translate("scadenze","Ctrl+G"));
    //Esporto la lista scadenze
    connect(esportaexcel,SIGNAL(clicked()),this,SLOT(esporta_excel_scadenze()));
    esportaexcel->setShortcut(qApp->translate("scadenze","Ctrl+L"));
    model = new QSqlRelationalTableModel(this);
    local_settings = new QLocale(QLocale::system());

    QDate data;
    data.setDate(QDate::currentDate().year(),QDate::currentDate().month(),QDate::currentDate().day());
    data_prestito->setCalendarPopup(true);
    data_prestito->setDate(data);

    /*************************************************************
     Inizializzazione dati
     **************************************************************/
     list_r->setChecked(true);
     lista();
}

void scadenze::clickgrid()
{
    QString id_new;
    int riga = tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = model->index(riga,0,QModelIndex());

    id_new = model->data(elemento).toString();
    id->setText(id_new);
    cerca();
}

void scadenze::check_list()
{
    if(checkBox->isChecked() == false){
            lista();
            list_r->setChecked(true);
    }
    else if(checkBox->isChecked() == true){
            lista_comp();
            list_r->setChecked(false);
    }
}

void scadenze::lista(){
    model->setTable("scadenze");
    model->setFilter("data_scadenza=CURDATE() and flag_sc='N' and doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);

    tableView->setAlternatingRowColors(true);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::clicca_fatture_pagate()
{
    if(list_r->isChecked() == true){
            lista();
            checkBox->setChecked(false);
     }
    else{
            lista_fatture_pagate();
            checkBox->setChecked(true);
    }
}

void scadenze::lista_fatture_pagate(){

    model->setTable("scadenze");
    model->setFilter("flag_sc='P' and doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);

    tableView->setAlternatingRowColors(true);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::lista_comp()
{
    model->setTable("scadenze");
    model->setFilter("doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::scadenza_m_precedente(){
    model->setTable("scadenze");
    model->setFilter("MONTH(data_scadenza)=month(now())-1 and flag_sc='N' and doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::scadenza_m_successivo(){
    model->setTable("scadenze");
    model->setFilter("MONTH(data_scadenza)=month(now())+1 and flag_sc='N' and doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::scadenza_a_corrente(){

    model->setTable("scadenze");
    model->setFilter("YEAR(data_scadenza)=YEAR(CURDATE()) and flag_sc='N' and doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::situazione_fornitori(){

    model->setTable("scadenze");
    model->setFilter("doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->setRelation(11,QSqlRelation("fornitori","id","ragione_sociale"));
    //Avvia selezione tabella
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::situazione_clienti(){
    model->setTable("scadenze");
    model->setFilter("doc_anno='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"' order by id");
    model->setRelation(11,QSqlRelation("clienti","id","nome"));
    //Avvia selezione tabella
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("scadenze","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("scadenze","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("scadenze","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("scadenze","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("scadenze","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("scadenze","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("scadenze","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("scadenze","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("scadenze","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("scadenze","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("scadenze","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("scadenze","IBAN"));

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->verticalHeader()->hide();
    tableView->setModel(model);
    tableView->setColumnHidden(10,true);
    tableView->setColumnHidden(11,true);
    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void scadenze::aggiorna(){
    if(!tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",qApp->translate("scadenze","Seleziona una riga da modificare..."));
        }
        else{
        QSqlQuery querys;
        QString sql = "N"; QString sql1 = "P";
        querys.prepare("select id from scadenze where id='"+id->text()+"'");
        querys.exec();

        if(querys.next()){
            QSqlQuery query;
            if(lib_rientro->isChecked() == true){
                query.prepare("UPDATE scadenze SET cliente='"+cliente->text()+"', "
                          "num_doc='"+libro->text()+"', flag_sc='"+sql+"' "
                          " where id='"+id->text()+"'");
            }
            else{
                query.prepare("UPDATE scadenze SET cliente='"+cliente->text()+"', "
                          "num_doc='"+libro->text()+"', flag_sc='"+sql+"' "
                          " where id='"+id->text()+"'");
            }
	  
	  try{
            if(query.exec()){
                LOG_FILE(qApp->translate("scadenze","Aggionamento scadenze effettuato con successo"),LOG_INFO_ALL,QIODevice::Append);
            }//fine if(query.exec())
            else{
                LOG_FILE(qApp->translate("scadenze","Impossibile aggionare le scadenze"),LOG_ERROR_ALL,QIODevice::Append);
		throw InvalidQuery(query.lastError().text().toStdString().c_str());
            }//fine else
	  }
	  catch(FabariaException e)
	  {
	     LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	  }
        }//fine if(query.next())
        lista();
        agg_riavvia_count();
        }
}

void scadenze::cerca()
{
	QSqlQuery query;
        query.prepare("select * from scadenze where id='"+id->text()+"'");
        query.exec();

        if(query.next()){
            id->setText(query.value(0).toString());
            cliente->setText(query.value(1).toString());
            libro->setText(query.value(3).toString());
            data_prestito->setDate(query.value(5).toDate());
            if(query.value(9).toString().indexOf("N")){
                lib_rientro->setChecked(true);
            }
            else{
                lib_rientro->setChecked(false);
            }
        }
        else{
            id->setText("");
            cliente->setText("");
            libro->setText("");
        }
}

void scadenze::cerca_fatture(const QString &){
    if(comboBox->currentIndex() == 0){
            QString text;
            text = cerca_txt->text();
            QStringList list_st;
            QSqlQuery querys("select cliente from scadenze");
            if(querys.exec()){
            while(querys.next()){
                list_st << querys.value(0).toString();
            }
            }
            QCompleter *complete = new QCompleter(list_st,this);
            complete->setCaseSensitivity(Qt::CaseInsensitive);
            cerca_txt->setCompleter(complete);
            if(text.length() == 0){
                lista_comp();
            }
        }
        else if(comboBox->currentIndex() == 1){
            QString text1;
            text1 = cerca_txt->text();
            QStringList list_st1;
            QSqlQuery querys1("select tipo_doc from scadenze");
            if(querys1.exec()){
            while(querys1.next()){
                list_st1 << querys1.value(0).toString();
            }
            }
            QCompleter *complete1 = new QCompleter(list_st1,this);
            complete1->setCaseSensitivity(Qt::CaseInsensitive);
            cerca_txt->setCompleter(complete1);
            if(text1.length() == 0){
                lista_comp();
            }
        }
}

void scadenze::elimina(){

	const char* error = "";
        if(!tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",qApp->translate("scadenze","Selezionare una riga da eliminare..."));
        }
         else if (!id->text().isEmpty())
            {
                QSqlQuery qctrl,query;
                qctrl.prepare("SELECT * FROM scadenze WHERE id = '"+id->text()+"'");
                qctrl.exec();
                query.prepare("DELETE FROM scadenze WHERE id = '"+id->text()+"' LIMIT 1");

                try{
		  if (query.exec() && qctrl.exec()) //Se esito OK Eliminato da DB
		  {
		      LOG_FILE(qApp->translate("scadenze","Eliminazione scadenza avvenuta con successo..."),LOG_INFO_ALL,QIODevice::Append);
		  }
		  else
		  {
		  LOG_FILE(qApp->translate("scadenze","Impossibile eliminare la scadenza: ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		  LOG_FILE(qApp->translate("scadenze","Impossibile eliminare la scadenza: ")+qctrl.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		  throw error ? InvalidQuery(qctrl.lastError().text().toStdString().c_str()) : InvalidQuery(query.lastError().text().toStdString().c_str());
		  }
		  }
		  catch(FabariaException e)
		  {
		    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
		  }

            }
            el_riavvia_count();
            lista_comp();
}

void scadenze::filtro(){
    if(cerca_txt->text().length() == 0){
            lista_comp();
            QMessageBox MsgBox;
            MsgBox.setWindowTitle("Fabaria Gest");
            MsgBox.setText(qApp->translate("scadenze","Avviso"));
            MsgBox.setInformativeText(qApp->translate("scadenze","Inserisci il testo nella casella cerca"));
            MsgBox.setIcon(QMessageBox::Warning);
            MsgBox.exec();
        }
        else if(comboBox->currentIndex() == 0){
            model->setFilter(QString("cliente = '" +cerca_txt->text()+"'").arg(cerca_txt->text()));
        }
        else if(comboBox->currentIndex() == 1){
            model->setFilter(QString("tipo_doc = '" +cerca_txt->text()+"'").arg(cerca_txt->text()));
        }
}

void scadenze::agg_riavvia_count(){
    form->cl1->clear();
    form->trayIcon->setVisible(false);
    form->reload_all();
    emit riavvia();
}

void scadenze::el_riavvia_count(){
    form->cl1->clear();
    form->trayIcon->setVisible(false);
    form->reload_all();
    emit riavvia();
}

void scadenze::anteprima_di_stampa(){
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer,this);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("scadenze","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview.setWindowIcon(icon);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(stampa(QPrinter*)));
    preview.exec();
}

void scadenze::stampa(QPrinter *print){
    Q_UNUSED(print);
    Stampe *pr = new Stampe();
    pr->stampa_lista_scadenze(print,model);
}

void scadenze::stampa_scadenze(){
    QPrinter printer(QPrinter::HighResolution);
    Q_UNUSED(printer);
    Stampe *pr = new Stampe();
    pr->stampa_lista_scadenze(&printer,model);
}

void scadenze::esporta_pdf_scadenze()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("scadenze","Esporta PDF"),
                                                        "*.pdf", qApp->translate("scadenze","Pdf Files(*.pdf);;Tutti i file(*.*)"));

        if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".pdf") < 0) {
                    fileName += ".pdf";
                }
            }

        if(!fileName.isEmpty()){
           QPrinter *printer = new QPrinter(QPrinter::HighResolution);
           printer->setPageSize(QPrinter::A4);
           printer->setOrientation(QPrinter::Landscape);
	   Q_UNUSED(printer);
           printer->setOutputFileName(fileName);
           Stampe *ex_pdf = new Stampe();
           ex_pdf->stampa_lista_scadenze(printer,model);
        }
}

void scadenze::esporta_excel_scadenze()
{
      QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("scadenze","Esporta Excel"),
                                                            "*.xlsx", qApp->translate("scadenze","XLSX(*.xlsx);;Tutti i file(*.*)"));


            if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".xlsx") < 0) {
                    fileName += ".xlsx";
                }
            }

            Document excel;
            for(int i= 0; i < model->columnCount();++i){
                    //Imposto header label per la descrizione
		    Format fm;
		    fm.setFontName("OpenSans");
		    fm.setFontBold(true);
		    fm.setFontSize(14);
		    excel.setRowHeight(1,30.0);
                    excel.write(1,1+i,model->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString(),fm);
                for(int row = 0; row < model->rowCount(); row++){
                    //Imposto le righe
                    excel.write(2+row,1+i,model->data(model->index(row,i),Qt::DisplayRole).toString());
                }
            }

         try{
            int ris = excel.saveAs(fileName);
            if(ris )
		QMessageBox::information(this,"FabariaGest",qApp->translate("scadenze","File salvato correttamente"));
            else
                throw InvalidFile(qApp->translate("scadenze","File non salvato").toStdString(),fileName);
	  }
	  catch(exception e)
	  {
	      throw InvalidFile(e.what(),fileName);
	  }
}

scadenze::~scadenze()
{
    //delete ui;
}
