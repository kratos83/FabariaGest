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
#include "chiudi_anno_contabile.h"
#include "ui_chiudi_anno_contabile.h"
#include "settingsmanager.h"
#include <QDebug>
#include "mainwindow.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

chiudi_anno_contabile::chiudi_anno_contabile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chiudi_anno_contabile)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->chiudi,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->avvia,SIGNAL(clicked()),this,SLOT(avvia_chiusura()));
    ui->anno_con->setText(getConfigurazioneSql("Esercizioconfigurazione").toString()+" ?");
    ui->anno_con->setAlignment(Qt::AlignCenter);
}

void chiudi_anno_contabile::avvia_chiusura()
{
    if(getConfigurazioneSql("Esercizioconfigurazione").toInt() == QDate::currentDate().year()){
         QMessageBox::warning(this,"FabariaGest",qApp->translate("chiudi_anno_contabile","Impossibile chiudere anno contabile ")+getConfigurazioneSql("Esercizioconfigurazione").toString());
    }
    else{
        insertQuery("INSERT INTO anno(anno_ins) VALUES('"+QString::number(QDate::currentDate().year())+"')","anno");

        updateConfigurazione("Esercizioconfigurazione='"+QString::number(QDate::currentDate().year())+"'");
        QSystemTrayIcon::MessageIcon ts = QSystemTrayIcon::Information;
        form->trayIcon->showMessage("FabariaGest", qApp->translate("chiudi_anno_contabile","Chiusura anno contabile effettuata correttamente..."),ts, 15*10000);
    }
}

chiudi_anno_contabile::~chiudi_anno_contabile()
{
    delete ui;
}
