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
#include "ui_fornitori.h"
#include "fornitori.h"
#include "itdelegato.h"
#include "defines.h"
#include "queryexception.h"
#include "print.h"
#include "fatt_new.h"

#include <QtSql>
#include <QMessageBox>
#include <QModelIndex>
#include <QCompleter>

using namespace std;
using namespace Defines;
using namespace QXlsx;

fornitori::fornitori(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(qApp->translate("fornitori","Anagrafica fornitori"));
    
    //Esci dall'Anagrafica fornitori
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    esci->setShortcut(tr("Ctrl+E"));
    
    //Nuovo fornitore
    connect(nuovo_b,SIGNAL(clicked()),this,SLOT(nuovo()));
    nuovo_b->setShortcut(tr("Ctrl+N"));
    
    //Elimina fornitore
    connect(elimina_b,SIGNAL(clicked()),this,SLOT(elimina()));
    elimina_b->setShortcut(tr("Ctrl+D"));
    
    //Salva fornitore
    connect(salva_b,SIGNAL(clicked()),this,SLOT(salva()));
    salva_b->setShortcut(tr("Ctrl+S"));
    connect(tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    
    //Stampa lista fornitori
    connect(stampa_b,SIGNAL(clicked()),this,SLOT(stampa()));
    stampa_b->setShortcut(tr("Ctrl+P"));
    connect(cerca_forn,SIGNAL(clicked()),this,SLOT(filtro()));
    connect(cerca,SIGNAL(textEdited(const QString &)),this,SLOT(cerca_utente(const QString &)));
    
    //Esporta in excel
    connect(export_excel,SIGNAL(clicked()),this,SLOT(esporta_excel()));
    export_excel->setShortcut(tr("Ctrl+X"));
    
    connect(cap->lineEdit(),&QLineEdit::textEdited,this,&fornitori::inst_cap);
    connect(localita->lineEdit(),&QLineEdit::textEdited,this,&fornitori::inst_paese);
    connect(cod_fisc,SIGNAL(textEdited(const QString &)),this,SLOT(leggi_codFis(const QString &)));
    connect(part_iva,SIGNAL(textEdited(const QString &)),this,SLOT(leggi_codPartIva(const QString &)));
    
    //Inizializza la lista:
    mod_grid = new QSqlRelationalTableModel();
    //Disabilito id edit
    id->setEnabled(false);

    //Visualizza lista query:
    lista();
    somma();
    nuovo();
}

void fornitori::cerca_utente(const QString &)
{
    if(combo_pref->currentIndex() == 0){
    QString text;
    text = cerca->text();
    QStringList list_st;
    QSqlQuery querys("select ragione_sociale from fornitori");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    cerca->setCompleter(complete);
    if(text.length() == 0){
        lista();
    }
    }
    else if(combo_pref->currentIndex() == 1){
        QString text1;
        text1 = cerca->text();
        QStringList list_st1;
        QSqlQuery querys1("select part_iva from fornitori");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        cerca->setCompleter(complete1);
        if(text1.length() == 0){
            lista();
        }
    }
    else if(combo_pref->currentIndex() == 2){
        QString text2;
        text2 = cerca->text();
        QStringList list_st1;
        QSqlQuery querys1("select cod_fiscale from fornitori");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        cerca->setCompleter(complete1);
        if(text2.length() == 0){
            lista();
        }
    }
}

void fornitori::filtro()
{
    if(cerca->text().length() == 0){
        lista();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText(qApp->translate("fornitori","Avviso"));
        MsgBox.setInformativeText(qApp->translate("fornitori","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(combo_pref->currentIndex() == 0){
        mod_grid->setFilter(QString("ragione_sociale = '" +cerca->text()+"'").arg(cerca->text()));
    }
    else if(combo_pref->currentIndex() == 1){
        mod_grid->setFilter(QString("part_iva = '" +cerca->text()+"'").arg(cerca->text()));
    }
    else if(combo_pref->currentIndex() == 2){
        mod_grid->setFilter(QString("cod_fiscale = '" +cerca->text()+"'").arg(cerca->text()));
    }
}

void fornitori::nuovo(){

    //Pulisco la schermata
    clear_text();
    
    QSqlQuery query;
    query.prepare("SELECT max(id+1) FROM fornitori");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
	   id->setText("1");
	}
	else{
        QString text = query.value(0).toString();
        id->setText(text);
	}
    }
}

void fornitori::salva(){

    if(id->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("fornitori","Inserisci correttamente i dati"));
    }
    else{
    QSqlQuery query;
    if(!id->text().isEmpty()){
        query.prepare("SELECT * FROM fornitori where id='"+id->text()+"'");
        //Esegui query
        query.exec();
        if(query.next()){
            aggiorna();
        }
        else{
            inserisci();
        }
    }
    else{
        inserisci();
    }
    }

}

void fornitori::inserisci(){

    QSqlQuery query;
    query.prepare("INSERT INTO fornitori "
                  "VALUES ('"+id->text()+"','"+rag_soc->text()+"','"+part_iva->text()+"','"+cod_fisc->text()+"',"
		  "'"+indirizzo->text()+"','"+cap->currentText()+"','"+localita->currentText()+"','"+prov->text()+"',"
		  "'"+telefono->text()+"','"+fax->text()+"','"+email->text()+"','"+sito->text()+"')");
  try{
    if(query.exec()){
        LOG_FILE(qApp->translate("fornitori","Inserimento effettuato con successo(fornitori::inserisci)"),LOG_INFO_ALL,QIODevice::Append);
	//Pulisco schermata
	clear_text();
    }
    else{
        LOG_FILE(qApp->translate("fornitori","Impossibile inserire(fornitori::inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(fornitori::inserisci): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
  }
  catch(FabariaException e)
  {
     LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }
    emit vis_forn();
    lista();
    somma();
    nuovo();
}

void fornitori::clear_text()
{
    id->setText("");
    rag_soc->setText("");
    part_iva->setText("");
    cod_fisc->setText("");
    indirizzo->setText("");
    cap->setCurrentText("");
    localita->setCurrentText("");
    prov->setText("");
    telefono->setText("");
    email->setText("");
    sito->setText("");
}

void fornitori::aggiorna(){

    QSqlQuery query;
    query.prepare("UPDATE fornitori SET ragione_sociale='"+rag_soc->text()+"', part_iva='"+part_iva->text()+"', cod_fiscale='"+cod_fisc->text()+"', "
                  " indirizzo='"+indirizzo->text()+"', cap='"+cap->currentText()+"', localita='"+localita->currentText()+"', prov='"+prov->text()+"', "
		  " telefono='"+telefono->text()+"', fax='"+fax->text()+"', email='"+email->text()+"', sito='"+sito->text()+"' "
                  "WHERE id='"+id->text()+"'");

    try{
    if(query.exec()){
        LOG_FILE(qApp->translate("fornitori","Aggiornamento effettuato con successo(fornitori::aggiorna)"),LOG_INFO_ALL,QIODevice::Append);
	//Pulisco schermata
	clear_text();
    }
    else{
        LOG_FILE(qApp->translate("fornitori","Impossibile aggiornare(fornitori::aggiorna)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(fornitori::aggiorna): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
  }
  catch(FabariaException e)
  {
     LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }
    emit vis_forn();
    lista();
    somma();
    nuovo();
}

void fornitori::elimina(){

    QString flag_controllo = "NO";

    if(!tableView->selectionModel()->currentIndex().isValid()){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("fornitori","Selezionare una riga da eliminare..."));
    }
     else if (!id->text().isEmpty())
        {

            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM fornitori WHERE id='"+id->text()+"'");
            qctrl.exec();

            if (qctrl.next()) //Se esito OK inserimento DB
            {
                flag_controllo = "SI";
             }

            if (qctrl.next()) //Se esito OK inserimento DB
            {
                flag_controllo = "SI";
             }

            // Se si passano i controlli di cui sopra si procede all'eliminazione

            QSqlQuery query;

            query.prepare("DELETE FROM fornitori WHERE id='"+id->text()+"'");

            try{
	      if(query.exec()){
            LOG_FILE(qApp->translate("fornitori","Eliminazione effettuata con successo(fornitori::elimina)"),LOG_INFO_ALL,QIODevice::Append);
            //Pulisco schermata
            clear_text();
	      }
	      else{
            LOG_FILE(qApp->translate("fornitori","Impossibile eliminare(fornitori::elimina)"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE("(fornitori::elimina): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(query.lastError().text().toStdString().c_str());
	      }
	    }
	    catch(FabariaException e)
	    {
		  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	    }
        }
       emit vis_forn();
       lista();
       somma();
       nuovo();
}

void fornitori::clickgrid(){

    QString id_new;
    int riga = tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    id->setText(id_new);

    cerca_query();

}

void fornitori::cerca_query(){

    QSqlQuery query;
    query.prepare("SELECT * FROM fornitori WHERE id='"+id->text()+"'");
    query.exec();
    
    if(query.next()){

            dettagli->setText(qApp->translate("fornitori","ID: ")+query.value(0).toString()+"\n"+
                              qApp->translate("fornitori","Ragione sociale: ")+query.value(1).toString()+"\n"+
                              qApp->translate("fornitori","Partita iva: ")+query.value(2).toString()+"\n"+
                              qApp->translate("fornitori","Codice fiscale.: ")+query.value(3).toString()+"\n"+
                              qApp->translate("fornitori","Indirizzo: ")+query.value(4).toString()+"\n"+
                              qApp->translate("fornitori","CAP: ")+query.value(5).toString()+"\n"+
                              QString::fromUtf8(qApp->translate("fornitori","Località: ").toStdString().c_str())+query.value(6).toString()+"\n"+
                              qApp->translate("fornitori","Provincia: ")+query.value(7).toString()+"\n"+
                              qApp->translate("fornitori","Telefono: ")+query.value(8).toString()+"\n"+
                              qApp->translate("fornitori","Fax: ")+query.value(9).toString()+"\n"+
                              qApp->translate("fornitori","E-mail: ")+query.value(10).toString()+"\n"+
                              qApp->translate("fornitori","Sito web: ")+query.value(11).toString());

        rag_soc->setText(query.value(1).toString());
        part_iva->setText(query.value(2).toString());
        cod_fisc->setText(query.value(3).toString());
        indirizzo->setText(query.value(4).toString());
        cap->setCurrentText(query.value(5).toString());
        localita->setCurrentText(query.value(6).toString());
        prov->setText(query.value(7).toString());
        telefono->setText(query.value(8).toString());
        fax->setText(query.value(9).toString());
        email->setText(query.value(10).toString());
        sito->setText(query.value(11).toString());
    }
    else{
        clear_text();
    }

}

void fornitori::stampa(){

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(anteprima(QPrinter*)));
    preview.exec();
}

void fornitori::anteprima(QPrinter *printer){

    Q_UNUSED(printer);
    Stampe *report = new Stampe();
    report->stampaforn(printer,mod_grid);
}

void fornitori::lista(){

    mod_grid->setTable("fornitori");
    mod_grid->select();
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("fornitori","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("fornitori","Ragione sociale"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("fornitori","Partita IVA"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("fornitori","Codice Fiscale"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("fornitori","Indirizzo"));
    mod_grid->setHeaderData(5,Qt::Horizontal,qApp->translate("fornitori","CAP"));
    mod_grid->setHeaderData(6,Qt::Horizontal,QString::fromUtf8(qApp->translate("fornitori","Località").toStdString().c_str()));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("fornitori","PROV"));
    mod_grid->setHeaderData(8,Qt::Horizontal,qApp->translate("fornitori","Telefono"));
    mod_grid->setHeaderData(9,Qt::Horizontal,qApp->translate("fornitori","FAX"));
    mod_grid->setHeaderData(10,Qt::Horizontal,qApp->translate("fornitori","E-mail"));
    mod_grid->setHeaderData(11,Qt::Horizontal,qApp->translate("fornitori","Sito web"));


    tableView->setModel(mod_grid);
    tableView->setColumnWidth(0,50);
    tableView->setColumnWidth(1,150);
    tableView->setColumnWidth(2,50);
    tableView->setColumnWidth(3,50);
    tableView->setColumnWidth(4,100);
    tableView->setColumnWidth(5,50);
    tableView->setColumnWidth(6,80);
    tableView->setColumnWidth(7,50);
    tableView->setColumnWidth(8,100);
    tableView->setColumnWidth(9,100);
    tableView->setColumnWidth(10,100);
    tableView->setColumnWidth(11,150);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    tableView->setSortingEnabled(true);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->verticalHeader()->hide();
    tableView->setModel(mod_grid);

    tableView->setItemDelegateForColumn(12,new coldxdelegato(this));

}

void fornitori::somma(){

    QString str;
    QSqlQuery query;
    query.exec("select count(id) from fornitori");
    if(query.next()){
       str = query.value(0).toString();
       totale_list->setText(QString::fromUtf8("N°  ")+str);
    }
}

void fornitori::esporta_excel()
{
  QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("fornitori","Esporta Excel"),
                                                            "*.xlsx", qApp->translate("fornitori","XLSX(*.xlsx);;Tutti i file(*.*)"));


            if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".xlsx") < 0) {
                    fileName += ".xlsx";
                }
            }

            Document excel;
            for(int i= 0; i < mod_grid->columnCount();++i){
                    //Imposto header label per la descrizione
		    Format fm;
		    fm.setFontName("OpenSans");
		    fm.setFontBold(true);
		    fm.setFontSize(14);
		    excel.setRowHeight(1,30.0);
                    excel.write(1,1+i,mod_grid->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString(),fm);
                for(int row = 0; row < mod_grid->rowCount(); row++){
                    //Imposto le righe
                    excel.write(2+row,1+i,mod_grid->data(mod_grid->index(row,i),Qt::DisplayRole).toString());
                }
            }

         try{
            int ris = excel.saveAs(fileName);
            if(ris )
		QMessageBox::information(this,"FabariaGest",qApp->translate("fornitori","File salvato correttamente"));
            else
                throw InvalidFile(qApp->translate("fornitori","File non salvato").toStdString(),fileName);
	  }
	  catch(exception e)
	  {
	      throw InvalidFile(e.what(),fileName);
	  }
}

void fornitori::inst_cap()
{
    QSqlQuery query;
    query.prepare("select * from comuni where codice='"+cap->currentText()+"'");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
            list << query.value(2).toString();
	    
            prov->setText(query.value(1).toString());
	        localita->setCurrentText(query.value(0).toString());
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    cap->setCompleter(complete);
}

void fornitori::inst_paese()
{
    QSqlQuery query;
    query.prepare("select * from comuni where comune='"+localita->currentText()+"'");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
            list << query.value(1).toString();
	    
            prov->setText(list.at(0));
	        cap->setCurrentText(query.value(2).toString());
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    localita->setCompleter(complete);
}

void fornitori::leggi_codFis(const QString & )
{
  getVerificaCodiceFiscale(cod_fisc->text(),codfis_image);
}

void fornitori::leggi_codPartIva(const QString & )
{
  getPartitaIva(part_iva->text(),codiva_image);
}

fornitori::~fornitori()
{
}
