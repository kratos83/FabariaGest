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
#include "listino.h"
#include "ui_listino.h"
#include "itdelegato.h"
#include "defines.h"
#include "queryexception.h"

using namespace Defines;

listino::listino(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::listino)
{
    ui->setupUi(this);
    setWindowTitle(qApp->translate("listino","Listino"));
    //Esco dalla finestra di dialogo
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    ui->esci->setShortcut(qApp->translate("listino","Ctrl+E"));
    //Aggiungo nuovo listino
    connect(ui->nuovo,SIGNAL(clicked()),this,SLOT(nuovo()));
    ui->nuovo->setShortcut(qApp->translate("listino","Ctrl+N"));
    //Salvo il listino
    connect(ui->salva,SIGNAL(clicked()),this,SLOT(inserisci()));
    ui->salva->setShortcut(qApp->translate("listino","Ctrl+S"));
    //Aggiorno il listino
    connect(ui->aggiorna,SIGNAL(clicked()),this,SLOT(aggiorna()));
    ui->aggiorna->setShortcut(qApp->translate("listino","Ctrl+U"));
    //Elimino il listino
    connect(ui->elimina,SIGNAL(clicked()),this,SLOT(elimina()));
    ui->elimina->setShortcut(qApp->translate("listino","Ctrl+D"));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(ui->checkBoxIVA,SIGNAL(clicked(bool)),this,SLOT(vis_label(bool)));
    //Imposto i parametri sql
    mod_grid = new QSqlQueryModel(this);
    //Visualizza listini
    lista();
}

bool listino::vis_label(bool click)
{
    if(click == true){
        QString txt =QString::fromUtf8(qApp->translate("listino","NOTA BENE: selezionando questa casella, quando andrai a compilare\n"
                     "un documento per un cliente o fornitore il quale ha questo listino\n"
                     "impostato, il documento verrà calcolato utilizzando i prezzi ivati e facendo\n"
                     "lo scorporo dell'iva a fine documento").toStdString().c_str());

        frame_popup = new QFrame(this,Qt::Popup | Qt::FramelessWindowHint);
        frame_popup->resize(433,54);
        frame_popup->setStyleSheet("QFrame {border: 1px solid gray; border-radius: 10px; background-color: silver } QLabel { color: black; border: 1px solid gray; border-radius: 10px;}");
        cl_popup = new classe(frame_popup,txt);
        frame_popup->setFocus();
        frame_popup->move(mapToGlobal(cl_popup->geometry().bottomRight()));
        frame_popup->show();
    }
    else{
        QString txt =QString::fromUtf8(qApp->translate("listino","NOTA BENE: selezionando questa casella, quando andrai a compilare\n"
                     "un documento per un cliente o fornitore il quale ha questo listino\n"
                     "impostato, il documento verrà calcolato utilizzando i prezzi non ivati").toStdString().c_str());

        frame_popup = new QFrame(this,Qt::Popup | Qt::FramelessWindowHint);
        frame_popup->resize(404,41);
        frame_popup->setStyleSheet("QFrame {border: 1px solid gray; border-radius: 10px; background-color: silver } QLabel { color: black; border: 1px solid gray; border-radius: 10px;}");
        cl_popup = new classe(frame_popup,txt);
        frame_popup->setFocus();
        frame_popup->move(mapToGlobal(cl_popup->geometry().bottomRight()));
        frame_popup->show();
    }

    return click;
}

void listino::lista()
{
    mod_grid->setQuery("select codice,descrizione,tipo from tipo_listino");
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("listino","Codice"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("listino","Descrizione"));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("listino","Tipo"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(mod_grid);

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setItemDelegateForColumn(2,new coldxdelegato(this));
}

void listino::nuovo()
{
    ui->checkBoxIVA->setChecked(false);
    ui->codelistinoEdit->clear();
    ui->comboBoxListino->setCurrentIndex(0);
    ui->desclineEdit->clear();
}

void listino::inserisci()
{
    QSqlQuery query;
    query.prepare("INSERT INTO tipo_listino "
                  "VALUES('"+ui->codelistinoEdit->text()+"','"+ui->desclineEdit->text()+"',"
		  "'"+ui->comboBoxListino->currentText()+"','"+QString::number(ui->checkBoxIVA->isChecked())+"')");

    try{
    if(query.exec()){
        LOG_FILE(qApp->translate("listino","Inserimento effettuato con successo(listino::inserisci)"),LOG_INFO_ALL,QIODevice::Append);
        nuovo();
    }
    else{
        LOG_FILE(qApp->translate("listino","Impossibile inserire(listino::inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(listino::inserisci): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    lista();
    emit add_listino();
}

void listino::aggiorna()
{
        QSqlQuery query;
        query.prepare("UPDATE tipo_listino SET descrizione ='"+ui->desclineEdit->text()+"', "
                      "tipo='"+ui->comboBoxListino->currentText()+"',ricarico_flag='"+QString::number(ui->checkBoxIVA->isChecked())+"' "
                      " where codice='"+ui->codelistinoEdit->text()+"'");
	try{
        if(query.exec()){
            LOG_FILE(qApp->translate("listino","Aggiornamento effettuato con successo(listino::aggiorna)"),LOG_INFO_ALL,QIODevice::Append);
            nuovo();
        }
        else{
            LOG_FILE(qApp->translate("listino","Impossibile aggiornare(listino::aggiorna)"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE("(listino::aggiorna): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(query.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
        lista();

}

void listino::elimina()
{
    if (ui->tableView->selectionModel()->currentIndex().isValid())
    {
        QSqlQuery qctrl;
        qctrl.prepare("SELECT * FROM tipo_listino WHERE codice ='"+ui->codelistinoEdit->text()+"'");
        qctrl.exec();

        QSqlQuery query;
        query.prepare("DELETE FROM tipo_listino WHERE codice ='"+ui->codelistinoEdit->text()+"'");

        try{
        if(query.exec()){
            LOG_FILE(qApp->translate("listino","Eliminazione effettuata con successo(listino::elimina)"),LOG_INFO_ALL,QIODevice::Append);
            nuovo();
        }
        else{
            LOG_FILE(qApp->translate("listino","Impossibile eliminare(listino::elimina)"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE("(listino::elimina): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(query.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
    }
    else{
        QMessageBox MsgBox;
        MsgBox.setText(qApp->translate("listino","Errore"));
        MsgBox.setInformativeText(qApp->translate("listino","Seleziona una riga per eliminarla..."));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }

   lista();
   emit add_listino();
}

void listino::clickgrid()
{
    QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex index = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(index).toString();
    ui->codelistinoEdit->setText(id_new);

    cerca();
}

void listino::cerca()
{
    QSqlQuery query;
    query.prepare("select * from tipo_listino where codice ='"+ui->codelistinoEdit->text()+"'");
    query.exec();

    if(query.next()){
        ui->codelistinoEdit->setText(query.value(0).toString());
        ui->desclineEdit->setText(query.value(1).toString());
        ui->comboBoxListino->setCurrentText(query.value(2).toString());
        ui->checkBoxIVA->setChecked(query.value(3).toBool());
    }
    else{
        nuovo();
    }
}

listino::~listino()
{
    delete ui;
}
