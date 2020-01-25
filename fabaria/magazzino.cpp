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
#include "magazzino.h"
#include "ui_magazzino.h"
#include "itdelegato.h"
#include "queryexception.h"
#include "defines.h"

using namespace Defines;

Magazzino::Magazzino(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Magazzino)
{
    ui->setupUi(this);
    setWindowTitle(qApp->translate("Magazzino","Gestisci magazzino"));
    connect(ui->nuovo,SIGNAL(clicked()),this,SLOT(nuovo()));
    connect(ui->save_magazzino,SIGNAL(clicked()),this,SLOT(clicca()));
    connect(ui->predefinito,SIGNAL(clicked()),this,SLOT(Magazzinopredefinito()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(ui->elimina,SIGNAL(clicked()),this,SLOT(elimina()));
    mod_grid = new QSqlQueryModel(this);
    manager = new SettingsManager;
    cerca();
    lista();
}

Magazzino::~Magazzino()
{
    delete ui;
}

void Magazzino::aggiorna()
{
     QSqlQuery Query;
        Query.prepare("UPDATE magazzino SET descrizione='"+ui->lineMagazzino->text()+"', predefinito='' "
                      "WHERE id='"+ui->id->text()+"'");
     
    try{
        if(Query.exec()){
            messaggioMagazzino();
        }
        else{
            LOG_FILE(qApp->translate("Magazzino","Impossibile aggiornare dati magazzino"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(Query.lastError().text().toStdString().c_str());
        }
        cerca();
        }
      catch(FabariaException e)
      {
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
      lista();
}

void Magazzino::cerca()
{
     QSqlQuery query;
        if(manager->generalValue("Magazzino/id",QVariant()).toString() == "")
            query.prepare("select * from magazzino where id='"+ui->id->text()+"' and predefinito='Predefinito' ");
        else
            query.prepare("select * from magazzino where id='"+manager->generalValue("Magazzino/id",QVariant()).toString()+"' and predefinito='Predefinito' ");
        query.exec();

        if(query.next()){
            ui->id->setText(query.value(0).toString());
            ui->lineMagazzino->setText(query.value(1).toString());
        }
}

void Magazzino::cerca_query()
{
     QSqlQuery query;
        query.prepare("select * from magazzino where id='"+ui->id->text()+"'");
        query.exec();

        if(query.next()){
            ui->id->setText(query.value(0).toString());
            ui->lineMagazzino->setText(query.value(1).toString());
        }
}

 void Magazzino::clearText()
{
    ui->id->clear();
    ui->lineMagazzino->clear();
}

void Magazzino::clicca()
{
     if(ui->id->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("Magazzino","Inserisci correttamente i dati"));
    }
    else{
        QSqlQuery querys;
        if(!ui->id->text().isEmpty()){

        querys.prepare("select id from magazzino where id='"+ui->id->text()+"'");
        querys.exec();
        try{
        if(querys.next()){
            aggiorna();
        }
        else{
            inserisci();
        }
        }
        catch(FabariaException e)
        {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
        }
        }
    }
}

void Magazzino::clickgrid()
{
     QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    ui->id->setText(id_new);

    cerca_query();
}

void Magazzino::elimina()
{
     QString flag_controllo = "NO";

        if (!ui->lineMagazzino->text().isEmpty())
        {

            QSqlQuery qctrl;
            qctrl.prepare("SELECT * FROM magazzino where id='"+ui->id->text()+"'");
            qctrl.exec();
	  try{
            if (qctrl.next()) //Se esito OK inserimento DB
            {
                flag_controllo = "SI";
             }

            if (qctrl.next()) //Se esito OK inserimento DB
            {
                flag_controllo = "SI";
                LOG_FILE(qApp->translate("Magazzino","Impossibile eliminare dato magazzino"),LOG_WARNING_ALL,QIODevice::Append);
                LOG_FILE(qApp->translate("Magazzino","Impossibile eliminare dato magazzino poichè predefinita"),LOG_WARNING_ALL,QIODevice::Append);
                throw InvalidQuery(qApp->translate("Magazzino","Impossibile eliminare dato magazzino poichè predefinita").toStdString());
                    return;
             }

            // Se si passano i controlli di cui sopra si procede all'eliminazione

            QSqlQuery query;

            query.prepare("DELETE FROM magazzino WHERE id ='"+ui->id->text()+"' LIMIT 1");

            if (query.exec()) //Se esito OK Eliminato da DB
            {
                clearText();
            }
            else
            {
                //scrivere codice per gestione Errore cancellazione
                LOG_FILE(qApp->translate("Magazzino","Impossibile eliminare dati magazzino"),LOG_ERROR_ALL,QIODevice::Append);
                LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
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

void Magazzino::inserisci()
{
    QSqlQuery query;
    query.prepare("INSERT INTO magazzino VALUES ('"+ui->id->text()+"','"+ui->lineMagazzino->text()+"','')");

    try{
        if(query.exec()){
            messaggioMagazzino();
        }
        else{
	    LOG_FILE(qApp->translate("Magazzino","Impossibile inserire dati magazzino"),LOG_ERROR_ALL,QIODevice::Append);
	    LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	    throw InvalidQuery(query.lastError().text().toStdString().c_str());
        }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    lista();
}

void Magazzino::lista()
{
    mod_grid->setQuery("SELECT * FROM magazzino ORDER BY id");
     mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("Magazzino","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("Magazzino","Magazzino"));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("Magazzino","Predefinito"));

    ui->tableView->setModel(mod_grid);

    ui->tableView->setColumnWidth(0, 95);
    ui->tableView->setColumnWidth(1, 100);
    ui->tableView->setColumnWidth(2, 150);
    
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(3, new coldxdelegato(this));
}

void Magazzino::Magazzinopredefinito()
{
     QSqlQuery Query,m_query;
    m_query.prepare("select id from magazzino where predefinito='"+qApp->translate("Magazzino","Predefinito")+"'");
    if(m_query.exec()){
        if(m_query.next())
        {
                Query.prepare("UPDATE magazzino SET  predefinito='' "
                            "WHERE id='"+m_query.value(0).toString()+"'; UPDATE magazzino SET  predefinito='"+qApp->translate("Magazzino","Predefinito")+"' "
                            "WHERE id='"+ui->id->text()+"'");
        }
        else{
        Query.prepare("UPDATE magazzino SET  predefinito='"+qApp->translate("Magazzino","Predefinito")+"'"
                        "WHERE id='"+ui->id->text()+"'");
        }
    }
         
    try{
        if(Query.exec()){
            manager->setGeneralValue("Magazzino/id",ui->id->text());
            manager->setGeneralValue("Magazzino/nome",ui->lineMagazzino->text());
        }
        else{
            LOG_FILE(qApp->translate("Magazzino","Impossibile rendere predifinito il magazzino"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(QString(qApp->translate("Magazzino","Impossibile rendere predifinito il magazzino\n")+Query.lastError().text()).toStdString().c_str());
        }
        }
      catch(FabariaException e)
      {
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
    lista();
    cerca();
}

void Magazzino::messaggioMagazzino()
{
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("FabariaGest");
    box->setText(qApp->translate("Magazzino","Magazzino predefinito"));
    box->setInformativeText(QString(qApp->translate("Magazzino","Vuoi rendere predefinito il magazzino ")+ui->lineMagazzino->text()+qApp->translate("Magazzino"," inserito?").toStdString().c_str()));
    box->setIcon(QMessageBox::Information);
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret){
        case QMessageBox::Ok:{
                Magazzinopredefinito();
                cerca();
                break;
        }
        case QMessageBox::Cancel:
                return;
                break;
    }
}

void Magazzino::nuovo()
{
    clearText();
     QSqlQuery Qctrl;
    Qctrl.prepare("SELECT count(id)+1 FROM magazzino");

    Qctrl.exec();

    if (Qctrl.next()) //Se esiste già
    {
        ui->id->setText(Qctrl.value(0).toString());
    }
}
