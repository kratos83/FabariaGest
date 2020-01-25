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
#include "pag.h"
#include "ui_pag.h"
#include "itdelegato.h"
#include <QtSql>
#include <QMessageBox>
#include "defines.h"
#include "queryexception.h"

using namespace Defines;

pag::pag(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    //Esci dalla finestra di dialogo
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    esci->setShortcut(tr("Ctrl+E"));
    connect(tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    //Elimino i pagamenti
    connect(del,SIGNAL(clicked()),this,SLOT(elimina()));
    del->setShortcut(tr("Ctrl+D"));
    //Nuovo pagamento
    connect(nuovo,SIGNAL(clicked()),this,SLOT(nuovo_id()));
    nuovo->setShortcut(tr("Ctrl+N"));
    //Salvo il pagamento
    connect(save,SIGNAL(clicked()),this,SLOT(clicca()));
    save->setShortcut(tr("Ctrl+S"));
    
    mod_grid = new QSqlQueryModel(this);
    lista();
    nuovo_id();
}

void pag::nuovo_id(){

    codice->setText("");
    textEdit->setPlainText("");
    textEdit_2->setPlainText("");

}

void pag::clicca(){

    if(codice->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("pag","Inserisci correttamente i dati"));
    }
    else{
    QSqlQuery Qctrl;
    Qctrl.prepare("SELECT * FROM pagam WHERE codice = '"+codice->text()+"'");
    Qctrl.exec();

    if (Qctrl.next()) //Se esiste già
    {
        QSqlQuery Query;

        Query.prepare("UPDATE pagam SET descrizione='"+textEdit->toPlainText()+"', nota_fattura='"+textEdit_2->toPlainText()+"' "
                      "WHERE codice = '"+codice->text()+"'");

	try{
        if(Query.exec()){
	    nuovo_id();
            LOG_FILE(qApp->translate("pag","Aggiornamento effettuato correttamente(pag::clicca)"),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE(qApp->translate("pag","Impossibile aggiornare(pag::clicca)"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE("(pag::clicca): "+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(Query.lastError().text().toStdString().c_str());
        }
        }
        catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
    }	
    else
        {
         inserisci();
        }
    }
        lista();
}

void pag::clickgrid(){

    QString id_new;
    int riga = tableView->selectionModel()->currentIndex().row();
    QModelIndex index = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(index).toString();
    codice->setText(id_new);

    cerca();
}

void pag::lista(){

    mod_grid->setQuery("select * from pagam order by codice");
    mod_grid->setHeaderData(0,Qt::Horizontal, qApp->translate("pag","Codice"));
    mod_grid->setHeaderData(1,Qt::Horizontal, qApp->translate("pag","Descrizione"));
    mod_grid->setHeaderData(2,Qt::Horizontal, qApp->translate("pag","Nota su Fattura"));

    tableView->setModel(mod_grid);

    tableView->setColumnWidth(0,150);
    tableView->setColumnWidth(1,150);
    tableView->setColumnWidth(2,400);

    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(3,new coldxdelegato(this));

}

void pag::cerca(){

    QSqlQuery query;
    query.prepare("select * from pagam where codice='"+codice->text()+"'");
    query.exec();

    if(query.next()){
        codice->setText(query.value(0).toString());
        textEdit->setText(query.value(1).toString());
        textEdit_2->setText(query.value(2).toString());
    }
    else{
        nuovo_id();
    }
}

void pag::elimina(){

    if(!tableView->selectionModel()->currentIndex().isValid()){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("pag","Selezionare una riga da eliminare..."));
    }
     else if (!codice->text().isEmpty())
        {
            QSqlQuery qctrl,query;
            qctrl.prepare("SELECT * FROM pagam WHERE codice = '"+codice->text()+"'");
            qctrl.exec();
	    
            query.prepare("DELETE FROM pagam WHERE codice = '"+codice->text()+"'");

	  try{
            if (query.exec()) //Se esito OK Eliminato da DB
            {
                LOG_FILE(qApp->translate("pag","Eliminazione avvenuta con successo(pag::elimina)"),LOG_INFO_ALL,QIODevice::Append);
                nuovo_id();
            }
            else
            {
                LOG_FILE(qApp->translate("pag","Impossibile eliminare(pag::elimina)"),LOG_ERROR_ALL,QIODevice::Append);
                LOG_FILE("(pag::elimina): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                throw InvalidQuery(query.lastError().text().toStdString().c_str());
            }
	    }
	    catch(FabariaException e)
	    {
	       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	    }
        }

       lista();

}

void pag::inserisci(){

    QSqlQuery query;
    query.prepare("INSERT INTO pagam "
                  "VALUES('"+codice->text()+"','"+textEdit->toPlainText()+"','"+textEdit_2->toPlainText()+"')");

    try{
    if(query.exec()){
        LOG_FILE(qApp->translate("pag","Inserimento avvenuto con successo(pag::inserisci)"),LOG_INFO_ALL,QIODevice::Append);
        nuovo_id();
    }
    else{
        LOG_FILE(qApp->translate("pag","Impossibile inserire(pag::inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(pag::inserisci): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    lista();
    nuovo_id();
}

pag::~pag()
{
}
