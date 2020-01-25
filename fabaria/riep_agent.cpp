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
#include "riep_agent.h"
#include "ui_riep_agent.h"
#include "itdelegato.h"
#include "defines.h"

using namespace Defines;

riep_agent::riep_agent(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::riep_agent)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    
    //Esco dalla finestra di dialogo
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    ui->esci->setShortcut(qApp->translate("riep_agent","Ctrl+E"));
    
    //Altre funzioni
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(ui->cerca_txt,SIGNAL(textEdited(const QString &)),this,SLOT(cerca_utente(const QString &)));
    
    //Cerco all'interno della tabella
    connect(ui->find_email,SIGNAL(clicked()),this,SLOT(filtro()));
    ui->find_email->setShortcut(qApp->translate("riep_agent","Ctrl+F"));
    
    //Stampo lista riepilogo agenti
    connect(ui->stampa,SIGNAL(clicked()),this,SLOT(anteprima()));
    ui->stampa->setShortcut(qApp->translate("riep_agent","Ctrl+P"));
    
    //Esporto lista pdf riepilogo agenti
    connect(ui->print_agente,SIGNAL(clicked()),this,SLOT(anteprima_agente()));
    ui->print_agente->setShortcut(qApp->translate("riep_agent","Ctrl+Shift+P"));
    
    //Stampo un singolo agente
    connect(ui->pdf_print,SIGNAL(clicked()),this,SLOT(stampa_pdf()));
    ui->pdf_print->setShortcut(qApp->translate("riep_agent","Ctrl+D"));
    
    //Esporto in pdf un singolo agente
    connect(ui->pdf_print_agente,SIGNAL(clicked()),this,SLOT(stampa_pdf_agente()));
    ui->pdf_print_agente->setShortcut(qApp->translate("riep_agent","Ctrl+Shift+D"));
    
    //Agente pagato
    connect(ui->ag_npag,SIGNAL(clicked()),this,SLOT(non_paga_conto()));
    ui->ag_npag->setShortcut(qApp->translate("riep_agent","Ctrl+N"));
    
    //Agente non pagato
    connect(ui->ag_pag,SIGNAL(clicked()),this,SLOT(salda_conto()));
    ui->ag_pag->setShortcut(qApp->translate("riep_agent","Ctrl+C"));
    
    //Visualizza lista agenti pagati
    connect(ui->agenti_pagati,SIGNAL(clicked()),this,SLOT(lista_ag_pag()));
    
    //Visualizza lista agenti non pagati
    connect(ui->agenti_non_pagati,SIGNAL(clicked()),this,SLOT(lista_ag_npag()));
    
    //Visualizzo la lista completa
    connect(ui->list_comp,SIGNAL(clicked()),this,SLOT(lista()));
    
    //Seleziona tutte le righe
    connect(ui->sel_all,SIGNAL(clicked()),this,SLOT(seleziona_tutto()));
    ui->sel_all->setShortcut(qApp->translate("riep_agent","Ctrl+A"));
    
    //Deseleziona tutte le righe
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(deseleziona_tutto()));
    ui->toolButton->setShortcut(qApp->translate("riep_agent","Ctrl+M"));
    
    
    /**************************************************
     * Impostazione tabellare di dati sql
     ***************************************************/
    mod_grid = new QSqlRelationalTableModel;
    
    //Visualizzo lista
    lista();
}

void riep_agent::clickgrid()
{
    QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    cerca(id_new);
}

void riep_agent::cerca(QString id)
{
	QSqlQuery query;
        query.prepare("SELECT * FROM lista_riep_agenti WHERE id='"+id+"'");
	query.exec();
	
    try{
        if (query.next())
        {
            ui->editor->setText(qApp->translate("riep_agent","ID: ")+query.value(0).toString()+"\n"+
                                qApp->translate("riep_agent","Agente: ")+query.value(1).toString()+"\n"+
                                qApp->translate("riep_agent","Provvigione: ")+query.value(2).toString()+"\n"+
                                qApp->translate("riep_agent","Importo: ")+query.value(3).toString()+"\n"+
                                qApp->translate("riep_agent","ID fattura: ")+query.value(3).toString()+"\n"+
                                qApp->translate("riep_agent","Pagata/Non pagata: ")+query.value(7).toString()+"\n" );
        }
        else{
	  LOG_FILE(qApp->translate("riep_agent","Impossibile cercare(lista_riep_agenti::cerca)"),LOG_ERROR_ALL,QIODevice::Append);
	  throw QueryException(query.lastError().text().toStdString().c_str());
	}
      }
      catch(FabariaException e){
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
}

void riep_agent::filtro()
{
    if(ui->cerca_txt->text().length() == 0){
        lista();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText(qApp->translate("riep_agent","Avviso"));
        MsgBox.setInformativeText(qApp->translate("riep_agent","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(ui->combo_cerca->currentIndex() == 0){
    mod_grid->setFilter(QString("agente = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
    }
    else if(ui->combo_cerca->currentIndex() == 1){
      mod_grid->setFilter(QString("data = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
    }
    else if(ui->combo_cerca->currentIndex() == 1){
      mod_grid->setFilter(QString("MONTH(data) = '" +QString::number(ui->mese->currentIndex())+"'").arg(QString::number(ui->mese->currentIndex())));
    }
}

void riep_agent::cerca_utente(const QString&)
{
    if(ui->combo_cerca->currentIndex() == 0){
    QString text;
    text = ui->cerca_txt->text();
    QStringList list_st;
    QSqlQuery querys("select agente from lista_riep_agenti");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cerca_txt->setCompleter(complete);
    if(text.length() == 0){
        lista();
    }
    }
    else if(ui->combo_cerca->currentIndex() == 1){
        QString text1;
        text1 = ui->cerca_txt->text();
        QStringList list_st1;
        QSqlQuery querys1("select data from lista_riep_agenti");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        ui->cerca_txt->setCompleter(complete1);
        if(text1.length() == 0){
            lista();
        }
    }
    else if(ui->combo_cerca->currentIndex() == 2){
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText("Avviso");
        MsgBox.setInformativeText(qApp->translate("riep_agent","Seleziona un mese..."));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
}

void riep_agent::anteprima()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("riep_agent","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview.setWindowIcon(icon);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_new(QPrinter*)));
    preview.exec();
}

void riep_agent::preview_new(QPrinter* printer)
{
    Q_UNUSED(printer);
    int riga = mod_grid->rowCount();
    int colonna = mod_grid->columnCount();
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();
    QModelIndex index = mod_grid->index(riga,colonna);
    Stampe *st_agente = new Stampe();
    st_agente->stampa_riep_agenti(printer,mod_grid,indexList,&index,ui->mese->currentText());
}

void riep_agent::stampa_pdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("riep_agent","Esporta PDF"),
                                                    "*.pdf",qApp->translate("riep_agent", "Pdf Files(*.pdf);;Tutti i file(*.*)"));
  try{
    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".pdf") < 0) {
                fileName += ".pdf";
            }
        }
       QPrinter *printer = new QPrinter(QPrinter::HighResolution);
       printer->setPageSize(QPrinter::A4);
       printer->setOrientation(QPrinter::Landscape);
       Q_UNUSED(printer);
       if(!fileName.isEmpty())
	  printer->setOutputFileName(fileName);
       else{
		throw InvalidFile(qApp->translate("riep_agent","File non salvato").toStdString(),fileName);}
       preview_new(printer);
  }
  catch(FabariaException e)
  {
    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }
}

void riep_agent::anteprima_agente()
{
    if(!ui->tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",QString::fromUtf8(qApp->translate("riep_agent","Seleziona una o più righe da stampare...").toStdString().c_str()));
        }
        else{
	    QPrinter printer(QPrinter::HighResolution);
	    QPrintPreviewDialog preview(&printer);
	    preview.setWindowFlags(Qt::Window);
	    preview.setWindowTitle(qApp->translate("riep_agent","Anteprima di stampa."));
	    QIcon icon;
	    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
	    preview.setWindowIcon(icon);
	    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_new_agente(QPrinter*)));
	    preview.exec();
	}
}

void riep_agent::preview_new_agente(QPrinter* printer)
{
    int riga = mod_grid->rowCount();
    int colonna = mod_grid->columnCount();
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();
    QModelIndex index = mod_grid->index(riga,colonna);
    Stampe *st_agente = new Stampe();
    st_agente->stampa_riep_agenti(printer,mod_grid,indexList,&index,ui->mese->currentText());
}

void riep_agent::stampa_pdf_agente()
{
    if(!ui->tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",QString::fromUtf8(qApp->translate("riep_agent","Seleziona una o più righe da esportare...").toStdString().c_str()));
        }
        else{
	   QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("riep_agent","Esporta PDF"),
                                                    "*.pdf", qApp->translate("riep_agent","Pdf Files(*.pdf);;Tutti i file(*.*)"));
	    try{
		if (fileName.length() != 0) {
		  // Aggiunge estensione alla fine del file se non c'è
		  if (fileName.indexOf(".pdf") < 0) {
		      fileName += ".pdf";
		  }
		}
		QPrinter *printer = new QPrinter(QPrinter::HighResolution);
		printer->setPageSize(QPrinter::A4);
		printer->setOrientation(QPrinter::Landscape);
		Q_UNUSED(printer);
		if(!fileName.isEmpty())
		printer->setOutputFileName(fileName);
		else{
		throw InvalidFile(qApp->translate("riep_agent","File non salvato").toStdString(),fileName);}
       
		preview_new_agente(printer);
	    }
	    catch(FabariaException e)
	    {
		LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	    }
      }
}

void riep_agent::salda_conto()
{
    if(!ui->tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",QString::fromUtf8(qApp->translate("riep_agent","Seleziona una o più righe da modificare...").toStdString().c_str()));
        }
        else{
        QSqlQuery querys;
        QString sql = "P";
	QString id = mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),ui->tableView->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
        querys.prepare("select id from lista_riep_agenti where id='"+id+"'");
        querys.exec();

        if(querys.next()){
            QSqlQuery query;
            query.prepare("UPDATE lista_riep_agenti SET flag_agenti='"+sql+"' "
                          " where id='"+id+"'");

	  try{
            if(query.exec()){
		LOG_FILE(qApp->translate("riep_agent","Aggiornamento effettuato con successo(riep_agent)"),LOG_INFO_ALL,QIODevice::Append);
            }//fine if(query.exec())
            else{
                LOG_FILE(qApp->translate("riep_agent","Impossibile aggiornare il record(riep_agent): ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                throw InvalidQuery(qApp->translate("riep_agent","Impossibile aggiornare il record(riep_agent)").toStdString());
            }//fine else
	  }
	  catch(FabariaException e)
	  {
	      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	  }
        }//fine if(query.next())
        lista();
        }
}

void riep_agent::non_paga_conto()
{
    if(!ui->tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",QString::fromUtf8(qApp->translate("riep_agent","Seleziona una o più righe da modificare...").toStdString().c_str()));
        }
        else{
        QSqlQuery querys;
        QString sql = "N";
	QString id = mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),ui->tableView->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
        querys.prepare("select id from lista_riep_agenti where id='"+id+"'");
        querys.exec();

        if(querys.next()){
            QSqlQuery query;
            query.prepare("UPDATE lista_riep_agenti SET flag_agenti='"+sql+"' "
                          " where id='"+id+"'");

	  try{
            if(query.exec()){
                LOG_FILE(qApp->translate("riep_agent","Aggiornamento effettuato con successo(riep_agent)"),LOG_INFO_ALL,QIODevice::Append);
            }//fine if(query.exec())
            else{
                LOG_FILE(qApp->translate("riep_agent","Impossibile aggiornare il record(riep_agent): ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                throw InvalidQuery(qApp->translate("riep_agent","Impossibile aggiornare il record(riep_agent)").toStdString());
            }//fine else
	  }
	  catch(FabariaException e)
	  {
	      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	  }
        }//fine if(query.next())
        lista();
        }
}

void riep_agent::lista()
{
    mod_grid->setTable("lista_riep_agenti");
    mod_grid->select();
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("riep_agent","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("riep_agent","Agente"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("riep_agent","Provvigione"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("riep_agent","Importo"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("riep_agent","ID fattura"));
    mod_grid->setHeaderData(6,Qt::Horizontal,qApp->translate("riep_agent","Data"));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("riep_agent","Pagata/Non pagata"));
    
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(mod_grid);
    ui->tableView->setColumnHidden(5,true);
    
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(5, new coldxdelegato(this));
}

void riep_agent::lista_ag_npag()
{
    mod_grid->setTable("lista_riep_agenti");
    mod_grid->setFilter("flag_agenti='N'");
    mod_grid->select();
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("riep_agent","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("riep_agent","Agente"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("riep_agent","Provvigione"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("riep_agent","Importo"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("riep_agent","ID fattura"));
    mod_grid->setHeaderData(6,Qt::Horizontal,qApp->translate("riep_agent","Data"));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("riep_agent","Pagata/Non pagata"));
    
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(mod_grid);
    ui->tableView->setColumnHidden(5,true);
    
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(5, new coldxdelegato(this));
}

void riep_agent::lista_ag_pag()
{
    mod_grid->setTable("lista_riep_agenti");
    mod_grid->setFilter("flag_agenti='P'");
    mod_grid->select();
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("riep_agent","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("riep_agent","Agente"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("riep_agent","Provvigione"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("riep_agent","Importo"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("riep_agent","ID fattura"));
    mod_grid->setHeaderData(6,Qt::Horizontal,qApp->translate("riep_agent","Data"));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("riep_agent","Pagata/Non pagata"));
    
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(mod_grid);
    ui->tableView->setColumnHidden(5,true);
    
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(5, new coldxdelegato(this));
}

void riep_agent::lista_mese()
{
    mod_grid->setTable("lista_riep_agenti");
    mod_grid->setFilter("MONTH(data)='"+QString::number(ui->mese->currentIndex())+"' AND agente='"+ui->cerca_txt->text()+"'");
    mod_grid->select();
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("riep_agent","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("riep_agent","Agente"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("riep_agent","Provvigione"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("riep_agent","Importo"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("riep_agent","ID fattura"));
    mod_grid->setHeaderData(6,Qt::Horizontal,qApp->translate("riep_agent","Data"));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("riep_agent","Pagata/Non pagata"));
    
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(mod_grid);
    ui->tableView->setColumnHidden(5,true);
    
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(5, new coldxdelegato(this));
}

void riep_agent::seleziona_tutto()
{
    ui->tableView->selectAll();
}

void riep_agent::deseleziona_tutto()
{
  ui->tableView->clearSelection();
}

riep_agent::~riep_agent()
{
    delete ui;
}
