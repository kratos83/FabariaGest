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
#include "iva.h"
#include "ui_iva.h"
#include <QMessageBox>
#include <QDebug>
#include <QtSql/QtSql>
#include "itdelegato.h"
#include "queryexception.h"
#include "defines.h"

using namespace Defines;

iva::iva(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    
    //Esco dalla finestra di dialogo dell'iva
    connect(esci_iva,SIGNAL(clicked()),this,SLOT(close()));
    esci_iva->setShortcut(qApp->translate("iva","Ctrl+E"));
    
    connect(list_view, SIGNAL(clicked(QModelIndex)), this, SLOT(clickgrid()));
    connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(ctrlkey()));
    
    //Salvo e aggiorno l'iva
    connect(agg_iva,SIGNAL(clicked()),this,SLOT(aggiungi_iva()));
    agg_iva->setShortcut(qApp->translate("iva","Ctrl+S"));
    
    //Elimino l'iva
    connect(del_iva,SIGNAL(clicked()),this,SLOT(delete_iva()));
    del_iva->setShortcut(qApp->translate("iva","Ctrl+D"));
    
    //Nuova ova
    connect(nuovo_b,SIGNAL(clicked()),this,SLOT(nuovo()));
    nuovo_b->setShortcut(qApp->translate("iva","Ctrl+N"));
    
    lineEdit->setEnabled(false);
    mod_grid = new QSqlQueryModel;
    lista();
    nuovo();
}

iva::~iva()
{
}

void iva::nuovo(){

    clear_text();
    QString id;
    QSqlQuery query;
    query.prepare("select count(id)+1 from aliquota");
    query.exec();
    if(query.next()){
         id = query.value(0).toString();
    }
    lineEdit->setText(id);
}

void iva::lista(){

    mod_grid->setQuery("select * from aliquota order by id");
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("iva","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("iva","Iva"));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("iva","Descizione."));

    list_view->setModel(mod_grid);

    list_view->setColumnWidth(0, 60);
    list_view->setColumnWidth(1, 100);
    list_view->setColumnWidth(2, 150);

    list_view->setAlternatingRowColors(true);

     list_view->setItemDelegateForColumn(2, new coldxdelegato(this));

}

void iva::clickgrid(){
    QString id_new;
    int riga = list_view->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    lineEdit->setText(id_new);

    cerca();
}

void iva::aggiungi_iva(){

    if(lineEdit->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("iva","Inserisci correttamente i dati"));
    }
    else{
         QSqlQuery Qctrl;
         if(!lineEdit->text().isEmpty()){
         Qctrl.prepare("SELECT * FROM aliquota WHERE id = '"+lineEdit->text()+"'");
         Qctrl.exec();

         if (Qctrl.next()) //Se esiste già
         {
            aggiorna();
         }
         else //Se invece non esiste il record..
         {
            inserisci();
         }
         }
         else{
             inserisci();
         }
    }
    lista();

}

void iva::aggiorna(){

    QSqlQuery Query;

    Query.prepare("UPDATE aliquota SET descrizione ='"+textEdit->toPlainText()+"', iva = '"+lineEdit_2->text()+"' "
                  "WHERE id = '"+lineEdit->text()+"'");
    
    try{
    if (Query.exec())
    {
       LOG_FILE(qApp->translate("iva","Aggiornamento effettuato correttamente(iva::aggiorna)"),LOG_INFO_ALL,QIODevice::Append);
       clear_text();
    }
    else
    {
       LOG_FILE(qApp->translate("iva","Impossibile aggiornare(iva::aggiorna)"),LOG_ERROR_ALL,QIODevice::Append);
       LOG_FILE("(iva::aggiorna): "+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
       throw InvalidQuery(Query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    lista();
    nuovo();
    emit add_iva();
}

void iva::inserisci(){

    QSqlQuery Query;
    Query.prepare("INSERT INTO aliquota "
                  " VALUES ('"+lineEdit->text()+"','"+lineEdit_2->text()+"','"+textEdit->toPlainText()+"')");

    try{
    if (Query.exec()) //Se esito OK inserimento DB
    {
	LOG_FILE(qApp->translate("iva","Inserimento effettuato con successo(iva::inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
	clear_text();
    }
    else
    {
        LOG_FILE(qApp->translate("iva","Impossibile inserire(iva::inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(iva::inserisci): ",LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(Query.lastError().text().toStdString().c_str());
     }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
     lista();
     nuovo();
     emit add_iva();
    //Fine Tentativo di inserimento
}

void iva::delete_iva(){

    if(!list_view->selectionModel()->currentIndex().isValid()){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("iva","Selezionare una riga da eliminare..."));
    }
     else if (!lineEdit->text().isEmpty())
        {
            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM aliquota WHERE id = '"+lineEdit->text()+"'");
            qctrl.exec();

            QSqlQuery query;

            query.prepare("DELETE FROM aliquota WHERE id = '"+lineEdit->text()+"'");

            try{
	      if (query.exec()) //Se esito OK inserimento DB
	      {
            LOG_FILE(qApp->translate("iva","Eliminazione effettuata con successo(iva::delete_iva)"),LOG_ERROR_ALL,QIODevice::Append);
            clear_text();
	      }
	      else
	      {
                LOG_FILE(qApp->translate("iva","Impossibile eliminare(iva::delete_iva)"),LOG_ERROR_ALL,QIODevice::Append);
                LOG_FILE("(iva::delete_iva): ",LOG_ERROR_ALL,QIODevice::Append);
                throw InvalidQuery(query.lastError().text().toStdString().c_str());
	      }
	      }
	      catch(FabariaException e)
	      {
                LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	      }
        }
	lista();
    nuovo();
    emit add_iva();
}

void iva::cerca(){

	QSqlQuery query;
        query.prepare("SELECT * FROM aliquota WHERE id = '"+lineEdit->text()+"'");
        query.exec();

        if (query.next())
        {
            lineEdit_2->setText(query.value(1).toString());
            textEdit->setText(query.value(2).toString());
        }
        else
        {
            lineEdit_2->setText("");
            textEdit->setText("");

        }
}

void iva::ctrlkey(){

    if (lineEdit->text().length()==3)
        {
           iva::cerca();

        } else
        {
            lineEdit_2->setText("");
            textEdit->setText("");

        }
}

void iva::clear_text()
{
    lineEdit->clear();
    lineEdit_2->clear();
    textEdit->clear();
}
