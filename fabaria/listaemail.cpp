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
#include "listaemail.h"
#include "ui_listaemail.h"
#include "itdelegato.h"
#include "queryexception.h"
#include "settingsmanager.h"
#include "defines.h"
#include "Debug.h"
#include "email_send.h"

using namespace Defines;

ListaEmail::ListaEmail(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ListaEmail)
{
    ui->setupUi(this);
    ui->label_id->setVisible(false);
    ui->id->setVisible(false);
    //Pulsante chiusura finestra di dialogo:
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    //Cliccando sulla tabella si cerca automaticamente:
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    //Salvataggio dati:
    connect(ui->salva,SIGNAL(clicked()),this,SLOT(salva()));
    //Eliminazione dati:
    connect(ui->elimina,SIGNAL(clicked()),this,SLOT(elimina()));
    //Inserisco le email
    connect(ui->inserisci,SIGNAL(clicked()),this,SLOT(insertEmailInto()));
    //Query mod_grid per riempire la tabella:
    mod_grid = new  QSqlQueryModel;
    nuovo();
    lista();
}

void ListaEmail::aggiorna()
{
    QSqlQuery query;
    query.prepare("UPDATE email SET descrizione='"+ui->email->toPlainText()+"' "
                  "where id='"+ui->id->text()+"'");

    try{
    if(query.exec()){
        ui->id->setText("");;
        ui->email->setText("");
    }
    else{
        LOG_FILE(qApp->translate("ListaEmail","Impossibile aggiornare i dati in email"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    emit insertEmail();
    lista();
    nuovo();
}

void ListaEmail::cerca()
{
    QSqlQuery query;
    query.prepare("select * from email where id='"+ui->id->text()+"'");
    query.exec();

    if(query.next()){
        ui->id->setText(query.value(0).toString());
        ui->email->append(QString(query.value(1).toString()+';').replace("\n"," "));
    }
    else{
        ui->id->setText("");
        ui->email->setPlainText("");
    }
}

void ListaEmail::clickgrid()
{
    QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex index = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(index).toString();
    ui->id->setText(id_new);

    cerca();
}

void ListaEmail::elimina()
{
    QString flag_controllo = "NO";

        if (!ui->id->text().isEmpty())
        {
            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM email WHERE id='"+ui->id->text()+"'");
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

            query.prepare("DELETE FROM email WHERE id='"+ui->id->text()+"' LIMIT 1");
	    
	    try{
            if (query.exec()) //Se esito OK Eliminato da DB
            {
                ui->id->setText("");
                ui->email->setText("");
            }
            else
            {
		LOG_FILE(qApp->translate("ListaEmail","Impossibile eliminare i dati in email"),LOG_ERROR_ALL,QIODevice::Append);
		LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		throw InvalidQuery(query.lastError().text().toStdString().c_str());
            }
	    }
	    catch(FabariaException e)
	    {
	      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	    }
        }
    emit insertEmail();
    lista();
    nuovo();
}

void ListaEmail::inserisci()
{
    QSqlQuery query;
    query.prepare("INSERT INTO email VALUES('"+ui->id->text()+"','"+ui->email->toPlainText()+"')");

    try{
    if(query.exec()){
        ui->id->setText("");;
        ui->email->setPlainText("");
    }
    else{
        LOG_FILE(qApp->translate("ListaEmail","Impossibile inserire i dati in email"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    emit insertEmail();
    lista();
    nuovo();
}

void ListaEmail::insertEmailInto()
{
    QString m_mail = ui->email->toPlainText();
    if(m_mail.endsWith(";"))
    {
        m_mail.chop(1);
    }
    m_emailSend->cc_email->setText(m_mail.replace("\n"," "));
    close();
}

void ListaEmail::lista()
{
    mod_grid->setQuery("SELECT * FROM email ORDER BY id");
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("ListaEmail","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("ListaEmail","Email"));
    ui->tableView->setModel(mod_grid);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(2, new coldxdelegato(this));
}

void ListaEmail::nuovo()
{
    QSqlQuery query;
    query.exec("select count(id)+1 from email");
    QString id_cl;
    if(query.next()){
        if(query.value(0).toInt() == 0){
            ui->id->setText("1");
        }
        else{
           id_cl = (query.value(0).toString());
           ui->id->setText(id_cl);
        }
    }
    ui->email->setText("");
}

void ListaEmail::salva()
{
    if(!ui->id->text().isEmpty())
    {
        QSqlQuery qctrl;
        qctrl.prepare("select * from email WHERE id='"+ui->id->text()+"'");
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


ListaEmail::~ListaEmail()
{
    delete ui;
}
