/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2017  angelo <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "inputdialogcombo.h"
#include "ui_inputdialogcombo.h"
#include "defines.h"
#include "queryexception.h"
#include <QDebug>
#include <QCompleter>

using namespace Defines;

InputDialogCombo::InputDialogCombo(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::InputDialogCombo)
{
    ui->setupUi(this);
    setWindowTitle(qApp->translate("InputDialogCombo","Seleziona azienda"));
    connect(ui->button_ok,SIGNAL(clicked()),this,SLOT(accetta()));
    connect(ui->button_cancel,SIGNAL(clicked()),this,SLOT(rifiuta()));
    connect(ui->aziendaCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboString(QString)));
    setCombo();
    cerca();
}

QString InputDialogCombo::getCombo()
{
    return ui->aziendaCombo->currentText();
}

void InputDialogCombo::setCombo()
{
    QSqlQuery Qctrl;
    Qctrl.prepare("SELECT * FROM azienda");
    Qctrl.exec();
    QStringList list;
    while(Qctrl.next()) //Se esiste già
    {
         list << Qctrl.value("nome_azienda").toString();
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->aziendaCombo->setCompleter(complete);
    ui->aziendaCombo->addItems(list);
}

void InputDialogCombo::comboString(QString azienda)
{
     QSqlQuery Qctrl;
    Qctrl.prepare("SELECT * FROM azienda where nome_azienda='"+azienda.replace("'","''")+"'");
    Qctrl.exec();
    if(Qctrl.next()) //Se esiste già
    {
        setId(Qctrl.value("id").toString());
        setAzienda(azienda);
    }
}

void InputDialogCombo::setId(QString id)
{
    m_id = id;
}

QString InputDialogCombo::getId()
{
    return m_id;
}

QString InputDialogCombo::getAzienda()
{
    return m_azienda;
}

void InputDialogCombo::setAzienda(QString azienda)
{
     m_azienda = azienda;
}

void InputDialogCombo::accetta()
{
    QSqlQuery Query,m_query;
    m_query.prepare("select id from azienda where predefinito='"+qApp->translate("InputDialogCombo","Predefinito")+"'");
    if(m_query.exec()){
        if(m_query.next())
        {
                Query.prepare("UPDATE azienda SET  predefinito='' "
                            "WHERE id='"+m_query.value(0).toString()+"'; UPDATE azienda SET  predefinito='"+qApp->translate("InputDialogCombo","Predefinito")+"' "
                            "WHERE id='"+getId()+"'");
        }
        else{
        Query.prepare("UPDATE azienda SET  predefinito='"+qApp->translate("InputDialogCombo","Predefinito")+"'"
                        "WHERE id='"+getId()+"'");
        }
    }
         
    try{
        if(Query.exec()){
            settingsManager->setGeneralValue("Azienda/id",getId());
            emit SetAzienda(getAzienda());
        }
        else{
	    LOG_FILE(qApp->translate("InputDialogCombo","Impossibile rendere predifinita azienda"),LOG_ERROR_ALL,QIODevice::Append);
	    LOG_FILE(Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	    throw InvalidQuery(QString(qApp->translate("InputDialogCombo","Impossibile rendere predifinita azienda\n")+Query.lastError().text()).toStdString().c_str());
        }
        }
      catch(FabariaException e)
      {
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
    accept();
}

void InputDialogCombo::rifiuta()
{
    reject();
}

void InputDialogCombo::cerca()
{
        QSqlQuery query;
        if(settingsManager->generalValue("Azienda/id",QVariant()).toString() == "")
            query.prepare("select * from azienda where id='"+getId()+"' and predefinito='"+qApp->translate("InputDialogCombo","Predefinito")+"' ");
        else
            query.prepare("select * from azienda where id='"+settingsManager->generalValue("Azienda/id",QVariant()).toString()+"' and predefinito='"+qApp->translate("InputDialogCombo","Predefinito")+"'");
        if(query.exec())
        {
            if(query.next())
                ui->aziendaCombo->setCurrentText(query.value("nome_azienda").toString());
        }
}

InputDialogCombo::~InputDialogCombo()
{
    delete ui;
}

