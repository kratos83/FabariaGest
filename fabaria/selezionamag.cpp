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

#include "selezionamag.h"
#include "ui_selezionamag.h"
#include "defines.h"
#include "queryexception.h"
#include <QDebug>
#include <QCompleter>

using namespace Defines;

SelezionaMag::SelezionaMag(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SelezionaMag)
{
    ui->setupUi(this);
    setWindowTitle(qApp->translate("SelezionaMag","Seleziona magazzino"));
    connect(ui->button_ok,SIGNAL(clicked()),this,SLOT(accetta()));
    connect(ui->button_cancel,SIGNAL(clicked()),this,SLOT(rifiuta()));
    connect(ui->magCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboString(QString)));
    setCombo();
    cerca();
}

void SelezionaMag::accetta()
{
    QSqlQuery Query,m_query;
    m_query.prepare("select id from magazzino where predefinito='"+qApp->translate("SelezionaMag","Predefinito")+"'");
    if(m_query.exec()){
        if(m_query.next())
        {
                Query.prepare("UPDATE magazzino SET  predefinito='' "
                            "WHERE id='"+m_query.value(0).toString()+"'; UPDATE magazzino SET  predefinito='"+qApp->translate("SelezionaMag","Predefinito")+"' "
                            "WHERE id='"+getId()+"'");
        }
        else{
        Query.prepare("UPDATE magazzino SET  predefinito='"+qApp->translate("SelezionaMag","Predefinito")+"' "
                        "WHERE id='"+getId()+"'");
        }
    }
         
    try{
        if(Query.exec()){
            settingsManager->setGeneralValue("Magazzino/id",getId());
        }
        else{
	    LOG_FILE(qApp->translate("SelezionaMag","Impossibile rendere predifinito magazzino"),LOG_ERROR_ALL,QIODevice::Append);
	    LOG_FILE(Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	    throw InvalidQuery(QString(qApp->translate("SelezionaMag","Impossibile rendere predifinito magazzino\n")+Query.lastError().text()).toStdString().c_str());
        }
        }
      catch(FabariaException e)
      {
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
    accept();
}

void SelezionaMag::cerca()
{
    QSqlQuery query;
        if(settingsManager->generalValue("Magazzino/id",QVariant()).toString() == "")
            query.prepare("select * from magazzino where id='"+getId()+"' and predefinito='"+qApp->translate("SelezionaMag","Predefinito")+"'");
        else
            query.prepare("select * from magazzino where id='"+settingsManager->generalValue("Magazzino/id",QVariant()).toString()+"' and predefinito='"+qApp->translate("SelezionaMag","Predefinito")+"' ");
        if(query.exec())
        {
            if(query.next())
                ui->magCombo->setCurrentText(query.value("descrizione").toString());
        }
}

void SelezionaMag::comboString(QString mag)
{
    QSqlQuery Qctrl;
    Qctrl.prepare("SELECT * FROM magazzino where descrizione='"+mag.replace("'","''")+"'");
    Qctrl.exec();
    if(Qctrl.next()) //Se esiste già
    {
        setId(Qctrl.value("id").toString());
    }
}

QString SelezionaMag::getCombo()
{
    return ui->magCombo->currentText();
}

QString SelezionaMag::getId()
{
    return m_id;
}

void SelezionaMag::rifiuta()
{
    reject();
}

void SelezionaMag::setCombo()
{
    QSqlQuery Qctrl;
    Qctrl.prepare("SELECT * FROM magazzino");
    Qctrl.exec();
    QStringList list;
    while(Qctrl.next()) //Se esiste già
    {
         list << Qctrl.value("descrizione").toString();
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->magCombo->setCompleter(complete);
    ui->magCombo->addItems(list);
}

void SelezionaMag::setId(QString id)
{
     m_id = id;
}

SelezionaMag::~SelezionaMag()
{
    delete ui;
}



