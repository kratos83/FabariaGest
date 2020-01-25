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
#include "distinta_riba.h"
#include "ui_distinta_riba.h"
#include "itdelegato.h"
#include "mainwindow.h"
#include "print.h"
#include "settingsmanager.h"
#include "queryexception.h"
#include "defines.h"

using namespace Defines;

distinta_riba::distinta_riba(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::distinta_riba)
{

    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    //Impostazione menu
    salda_conto = new QAction(QIcon("./images/cash2.png"),qApp->translate("distinta_riba","Salda conto"),this);
    da_pagare = new QAction(QIcon("./images/cash2.png"),qApp->translate("distinta_riba","Non pagata"),this);

    //Menu
    menu = new QMenu(this);
    menu->addAction(salda_conto);
    menu->addSeparator();
    menu->addAction(da_pagare);
    ui->salda->setMenu(menu);


    connect(ui->chiudi,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->label_prec,SIGNAL(clicked()),this,SLOT(lista_mese_prec()));
    connect(ui->label_succ,SIGNAL(clicked()),this,SLOT(lista_mese_succ()));
    connect(salda_conto,SIGNAL(triggered()),this,SLOT(saldaconto()));
    connect(da_pagare,SIGNAL(triggered()),this,SLOT(nonpagaconto()));
    connect(ui->anno_sel,SIGNAL(currentIndexChanged(QString)),this,SLOT(combo_anno()));
    connect(ui->find_txt,SIGNAL(clicked()),this,SLOT(filtro()));
    connect(ui->sel_cli_forn,SIGNAL(currentIndexChanged(int)),this,SLOT(seleziona_cli_forn(int)));
    connect(ui->vis_tutti,SIGNAL(clicked()),this,SLOT(lista()));
    connect(ui->cerca_txt,SIGNAL(textEdited(const QString &)),this,SLOT(cerca_riba(const QString &)));
    connect(ui->stampa,SIGNAL(clicked()),this,SLOT(stampa_riba()));
    connect(ui->ant_stampa,SIGNAL(clicked()),this,SLOT(anteprima_di_stampa_riba()));
    model = new QSqlRelationalTableModel;

    QStringList lista_cli_forn;
    lista_cli_forn << qApp->translate("distinta_riba","Clienti") << qApp->translate("distinta_riba","Fornitori");
    ui->sel_cli_forn->addItems(lista_cli_forn);
    lista();
    add_combo_anno();
    add_combo_cli();

    ui->anno_sel->setCurrentText(settingsManager->generalValue("Esercizio/anno",QVariant()).toString());
}

void distinta_riba::lista(){

    model->setTable("scadenze");
    model->setFilter("doc_anno='"+ui->anno_sel->currentText()+"'");
    //Avvia selezione tabella
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("distinta_riba","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("distinta_riba","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("distinta_riba","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("distinta_riba","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("distinta_riba","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("distinta_riba","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("distinta_riba","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("distinta_riba","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("distinta_riba","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("distinta_riba","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("distinta_riba","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("distinta_riba","IBAN"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(10,true);
    ui->tableView->setColumnHidden(11,true);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void distinta_riba::lista_mese_prec(){

    model->setTable("scadenze");
    model->setFilter("MONTH(data_scadenza)=month(now())-1 and flag_sc='N' and doc_anno='"+ui->anno_sel->currentText()+"'");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("distinta_riba","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("distinta_riba","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("distinta_riba","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("distinta_riba","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("distinta_riba","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("distinta_riba","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("distinta_riba","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("distinta_riba","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("distinta_riba","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("distinta_riba","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("distinta_riba","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("distinta_riba","IBAN"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(10,true);
    ui->tableView->setColumnHidden(11,true);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void distinta_riba::lista_mese_succ()
{
    model->setTable("scadenze");
    model->setFilter("MONTH(data_scadenza)=month(now())+1 and flag_sc='N' and doc_anno='"+ui->anno_sel->currentText()+"'");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("distinta_riba","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("distinta_riba","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("distinta_riba","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("distinta_riba","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("distinta_riba","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("distinta_riba","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("distinta_riba","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("distinta_riba","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("distinta_riba","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("distinta_riba","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("distinta_riba","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("distinta_riba","IBAN"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(10,true);
    ui->tableView->setColumnHidden(11,true);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void distinta_riba::saldaconto()
{
    if(!ui->tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",qApp->translate("distinta_riba","Seleziona una riga da modificare..."));
        }
        else{
        QSqlQuery querys;
        QString sql = "P";
	QString id = model->data(model->index(ui->tableView->selectionModel()->currentIndex().row(),ui->tableView->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
        querys.prepare("select id from scadenze where id='"+id+"'");
        querys.exec();

        if(querys.next()){
            QSqlQuery query;
            query.prepare("UPDATE scadenze SET flag_sc='"+sql+"' "
                          " where id='"+id+"'");

	  try{
            if(query.exec()){
		LOG_FILE(qApp->translate("distinta_riba","Aggiornamento effettuato con successo(distinta riba)"),LOG_INFO_ALL,QIODevice::Append);
            }//fine if(query.exec())
            else{
		LOG_FILE(qApp->translate("distinta_riba","Impossibile aggiornare il record(distinta riba): ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		throw InvalidQuery(qApp->translate("distinta_riba","Impossibile aggiornare il record(distinta riba)").toStdString());
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

void distinta_riba::nonpagaconto()
{
    if(!ui->tableView->selectionModel()->currentIndex().isValid()){
            QMessageBox::warning(this,"Fabaria Gest",qApp->translate("distinta_riba","Seleziona una riga da modificare..."));
        }
        else{
        QSqlQuery querys;
        QString sql = "N";
	QString id = model->data(model->index(ui->tableView->selectionModel()->currentIndex().row(),ui->tableView->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
        querys.prepare("select id from scadenze where id='"+id+"'");
        querys.exec();

        if(querys.next()){
            QSqlQuery query;
            query.prepare("UPDATE scadenze SET flag_sc='"+sql+"' "
                          " where id='"+id+"'");

	  try{
            if(query.exec()){
		LOG_FILE(qApp->translate("distinta_riba","Aggiornamento effettuato con successo(distinta riba)"),LOG_INFO_ALL,QIODevice::Append);
            }//fine if(query.exec())
            else{
		LOG_FILE(qApp->translate("distinta_riba","Impossibile aggiornare il record(distinta riba): ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		throw InvalidQuery(qApp->translate("distinta_riba","Impossibile aggiornare il record(distinta riba)").toStdString());
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

void distinta_riba::agg_riavvia_count()
{
    form->cl1->clear();
    form->trayIcon->setVisible(false);
    form->reload_data();
    emit riavvia();
}

void distinta_riba::add_combo_anno()
{
        ui->anno_sel->clear();
        QStringList list;
        QSqlQuery query("select anno_ins from anno");
        if(query.exec()){
        while(query.next()){
            list << query.value(0).toString();
        }
        QCompleter *complete = new QCompleter(list,this);
        complete->setCaseSensitivity(Qt::CaseInsensitive);
        complete->setCompletionMode(QCompleter::PopupCompletion);
        ui->anno_sel->setCompleter(complete);
        ui->anno_sel->addItems(list);
        }
}

void distinta_riba::combo_anno(){
    model->setTable("scadenze");
    model->setFilter("doc_anno='"+ui->anno_sel->currentText()+"' order by id");
    //Avvia selezione tabella
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("distinta_riba","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("distinta_riba","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("distinta_riba","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("distinta_riba","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("distinta_riba","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("distinta_riba","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("distinta_riba","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("distinta_riba","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("distinta_riba","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("distinta_riba","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("distinta_riba","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("distinta_riba","IBAN"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(10,true);
    ui->tableView->setColumnHidden(11,true);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void distinta_riba::seleziona_cli_forn(int index)
{
    if(index == 0)
    {
        connect(ui->cli_forn,SIGNAL(currentIndexChanged(QString)),this,SLOT(lista_forn_cli()));
        add_combo_cli();
    }
    else{
        connect(ui->cli_forn,SIGNAL(currentIndexChanged(QString)),this,SLOT(lista_forn_cli()));
        add_combo_forn();
    }
}

void distinta_riba::add_combo_forn()
{
    ui->cli_forn->clear();
    QStringList list;
    QSqlQuery query("select DISTINCT s.cliente from scadenze AS s INNER JOIN fornitori as f ON s.idfornitore=f.id");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString();
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    ui->cli_forn->setCompleter(complete);
    ui->cli_forn->addItems(list);
    }
}

void distinta_riba::add_combo_cli()
{
    ui->cli_forn->clear();
    QStringList list;
    QSqlQuery query("select DISTINCT c.nome,c.cognome from clienti AS c INNER JOIN scadenze as s ON c.id=s.idcliente");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString()+ " "+query.value(1).toString();
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    ui->cli_forn->setCompleter(complete);
    ui->cli_forn->addItems(list);
    }
}

void distinta_riba::lista_forn_cli()
{
    model->setTable("scadenze");
    model->setFilter("doc_anno='"+ui->anno_sel->currentText()+"' and cliente='"+ui->cli_forn->currentText()+"' and flag_sc='N' order by id");
    //Avvia selezione tabella
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("distinta_riba","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("distinta_riba","Cliente o fornitore"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("distinta_riba","Tipo documento"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("distinta_riba","Numero documento"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("distinta_riba","Anno documento"));
    model->setHeaderData(5, Qt::Horizontal, qApp->translate("distinta_riba","Data di scadenza"));
    model->setHeaderData(6, Qt::Horizontal, qApp->translate("distinta_riba","Pagamento fattura"));
    model->setHeaderData(7, Qt::Horizontal, qApp->translate("distinta_riba","Importo"));
    model->setHeaderData(8, Qt::Horizontal, qApp->translate("distinta_riba","Note pagamento"));
    model->setHeaderData(9, Qt::Horizontal, qApp->translate("distinta_riba","Pagata o non pagata"));
    model->setHeaderData(12, Qt::Horizontal, qApp->translate("distinta_riba","Banca"));
    model->setHeaderData(13,Qt::Horizontal,qApp->translate("distinta_riba","IBAN"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(10,true);
    ui->tableView->setColumnHidden(11,true);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void distinta_riba::filtro()
{
    if(ui->cerca_txt->text().length() == 0){
            lista();
            QMessageBox MsgBox;
            MsgBox.setWindowTitle("Fabaria Gest");
            MsgBox.setText("Avviso");
            MsgBox.setInformativeText(qApp->translate("distinta_riba","Inserisci il testo nella casella cerca"));
            MsgBox.setIcon(QMessageBox::Warning);
            MsgBox.exec();
        }
        else if(ui->combo_find->currentIndex() == 0){
            model->setFilter(QString("cliente = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
        }
        else if(ui->combo_find->currentIndex() == 1){
            model->setFilter(QString("tipo_doc = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
        }
}

void distinta_riba::cerca_riba(const QString &)
{
    if(ui->combo_find->currentIndex() == 0){
            QString text;
            text = ui->cerca_txt->text();
            QStringList list_st;
            QSqlQuery querys("select distinct cliente from scadenze");
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
        else if(ui->combo_find->currentIndex() == 1){
            QString text1;
            text1 = ui->cerca_txt->text();
            QStringList list_st1;
            QSqlQuery querys1("select distinct tipo_doc from scadenze");
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
}

void distinta_riba::stampa_riba()
{
    if(ui->cli_forn->currentText() == "")
    {
      QMessageBox::warning(this,"FabariaGest",qApp->translate("distinta_riba","Seleziona o cliente o fornitore \nin base alla distinta da stampare"));
    }
    else{
        QPrinter print(QPrinter::HighResolution);
        QPrintDialog *p_diag = new QPrintDialog(&print,this);
        p_diag->show();
        int riga = model->rowCount();
        int colonna = model->columnCount();
        QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();
        QModelIndex index = model->index(riga,colonna);
        int sel_forn_li = ui->sel_cli_forn->currentIndex();
        Stampe *st_riba = new Stampe;
        st_riba->stampa_distinta(&print,model,indexList,&index,sel_forn_li);
    }
}

void distinta_riba::anteprima_di_stampa_riba()
{
    if(ui->cli_forn->currentText() == "")
    {
      QMessageBox::warning(this,"FabariaGest",qApp->translate("distinta_riba","Seleziona o cliente o fornitore \nin base alla distinta da stampare"));
    }
    else{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog *preview = new QPrintPreviewDialog(&printer,this);
    preview->setWindowFlags(Qt::Window);
    preview->setWindowTitle(qApp->translate("distinta_riba","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview->setWindowIcon(icon);
    connect(preview,SIGNAL(paintRequested(QPrinter*)),this,SLOT(anteprima(QPrinter*)));
    preview->exec();
    }
}

void distinta_riba::anteprima(QPrinter *printer)
{
    
    Q_UNUSED(printer)
    int riga = model->rowCount();
    int colonna = model->columnCount();
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();
    QModelIndex index = model->index(riga,colonna);
    int sel_forn_li = ui->sel_cli_forn->currentIndex();
    Stampe *st_riba = new Stampe();
    st_riba->stampa_distinta(printer,model,indexList,&index,sel_forn_li);
}

distinta_riba::~distinta_riba()
{
    delete ui;
}
