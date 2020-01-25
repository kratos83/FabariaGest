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
#include "causali.h"
#include "ui_causali.h"
#include "itdelegato.h"
#include <QtSql>
#include <QMessageBox>
#include <QModelIndex>
#include "queryexception.h"
#include "settingsmanager.h"
#include "defines.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

causali::causali(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    //Pulsante chiusura finestra di dialogo:
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    //Pulsante nuovo tipo di causale di trasporto:
    connect(nuovo_b,SIGNAL(clicked()),this,SLOT(nuovo()));
    //Cliccando sulla tabella si cerca automaticamente:
    connect(tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    //Salvataggio dati:
    connect(salva_b,SIGNAL(clicked()),this,SLOT(salva()));
    //Eliminazione dati:
    connect(elimina_b,SIGNAL(clicked()),this,SLOT(elimina()));
    //Disabilita qlineedit id:
    id->setEnabled(false);

    //Query mod_grid per riempire la tabella:
    mod_grid = new  QSqlQueryModel;

    //Visualizza i dati nella tabella:
    lista();
    nuovo();
}

/*
 Selezione nuovo id per nuovo tipo di causale id trasporto:
 */
void causali::nuovo(){

    if(selectQuery("select count(id)+1 from causali",0).toString() == "0")
            id->setText("1");
    else {
        QString text = selectQuery("select count(id)+1 from causali",0).toString();
        id->setText(text);
    }
    tipo->clear();
}

void causali::salva(){

    if(!id->text().isEmpty())
    {
        QSqlQuery qctrl;
        qctrl.prepare("select * from causali WHERE id='"+id->text()+"'");
        qctrl.exec();

        if(qctrl.next())
        {
           aggiorna();
        }
        else{
            inserisci();
        }
    }
    else
    {
       inserisci();
    }
    lista();
}

void causali::inserisci(){
    insertQuery("INSERT INTO causali VALUES('"+id->text()+"','"+tipo->text()+"')","causali");
    clearText();
    lista();
    nuovo();
}

void causali::aggiorna(){
    
    updateQuery("UPDATE causali SET nome_causale='"+tipo->text()+"' "
                  "where id='"+id->text()+"'","causali");
    clearText();
    lista();
    nuovo();
}

void causali::elimina(){

    if (!id->text().isEmpty())
    {
        eliminaQuery("DELETE FROM causali WHERE id='"+id->text()+"' LIMIT 1","causali");
    }
    clearText();
    lista();
    nuovo();
}

void causali::clearText()
{
    id->clear();
    tipo->clear();
}

void causali::clickgrid(){

    QString id_new;
    int riga = tableView->selectionModel()->currentIndex().row();
    QModelIndex index = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(index).toString();
    id->setText(id_new);

    this->cerca();
}

void causali::cerca(){

    id->setText(mod_grid->data(mod_grid->index(tableView->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString());
    tipo->setText(mod_grid->data(mod_grid->index(tableView->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString());
}

void causali::lista(){

    mod_grid->setQuery("SELECT * FROM causali ORDER BY id");
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("causali","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("causali","Tipo di causale"));
    tableView->setModel(mod_grid);

    tableView->setColumnWidth(0,90);
    tableView->setColumnWidth(1,150);

    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(2, new coldxdelegato(this));
}

causali::~causali()
{
}
