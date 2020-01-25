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
#include "imposta_anno.h"
#include "ui_imposta_anno.h"
#include "databasesql.h"
#include "defines.h"

using namespace Defines;
using namespace DatabaseSql;

imposta_anno::imposta_anno(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::imposta_anno)
{
    ui->setupUi(this);
    connect(ui->chiudi,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->ok,SIGNAL(clicked()),this,SLOT(imposta_configurazione()));
    read_configurazione();
    ui->anno_sel->setCurrentText(getConfigurazioneSql("Esercizioconfigurazione").toString());
}

void imposta_anno::read_configurazione()
{
    ui->anno_sel->clear();
    QStringList list;
    list = selectQueryWhile("select anno_ins from anno",0);
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    ui->anno_sel->setCompleter(complete);
    ui->anno_sel->addItems(list);
}

void imposta_anno::imposta_configurazione()
{
    updateConfigurazione("Esercizioconfigurazione='"+ui->anno_sel->currentText()+"'");
    close();
}

imposta_anno::~imposta_anno()
{
    delete ui;
}
