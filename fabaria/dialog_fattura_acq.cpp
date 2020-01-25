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
#include "dialog_fattura_acq.h"
#include "ui_dialog_fattura_acq.h"
#include "settingsmanager.h"
#include "fatt_acq.h"
#include "defines.h"

dialog_fattura_acq::dialog_fattura_acq(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setWindowTitle(qApp->translate("dialog_fattura_acq","Scelta tipo di fattura"));
    connect(cancel,SIGNAL(clicked()),this,SLOT(close()));
    connect(radio_fatt_acq,SIGNAL(clicked()),this,SLOT(interfaccia()));
    connect(radio_nota_credito,SIGNAL(clicked()),this,SLOT(interfaccia()));
    connect(bt_ok,SIGNAL(clicked()),this,SLOT(not_selected()));
}

void dialog_fattura_acq::interfaccia()
{
    if(radio_fatt_acq->isChecked())
    {
        connect(bt_ok,SIGNAL(clicked()),this,SLOT(add_fattura_acq()));
        radio_nota_credito->setEnabled(false);
    }
    else if(radio_nota_credito->isChecked())
    {
        connect(bt_ok,SIGNAL(clicked()),this,SLOT(add_nota_credito()));
        radio_fatt_acq->setEnabled(false);
    }
}

void dialog_fattura_acq::add_nota_credito()
{
    close();
    QString ft = tipo_fattura.Nota_di_credito;
    ft = qApp->translate("dialog_fattura_acq","Nota di credito");
    fatt_new *fs; 
    QSqlQuery query("select max(id)+1 from fatture_acq where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            fs = new fatt_new(this,ft,1);
            fs->formatta_num();


    connect(fs,SIGNAL(salvafatturanew()),fattura_acquisto,SLOT(lista()));
    fs->exec();
    }
    else{
            fs = new fatt_new(this,ft,query.value(0).toInt());
            fs->formatta_num();

            connect(fs,SIGNAL(salvafatturanew()),fattura_acquisto,SLOT(lista()));
            fs->exec();
        }
    }
}

void dialog_fattura_acq::add_fattura_acq()
{
    close();
    QString ft = tipo_fattura.Fattura_acq;
    ft = qApp->translate("dialog_fattura_acq","Fattura acquisto");
    fatt_new *fs;
    QSqlQuery query("select max(id)+1 from fatture_acq where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            fs = new fatt_new(this,ft,1);
            fs->formatta_num();


    connect(fs,SIGNAL(salvafatturanew()),fattura_acquisto,SLOT(lista()));
    fs->exec();
    }
    else{
            fs = new fatt_new(this,ft,query.value(0).toInt());
            fs->formatta_num();

            connect(fs,SIGNAL(salvafatturanew()),fattura_acquisto,SLOT(lista()));
            fs->exec();
        }
    }
}

void dialog_fattura_acq::not_selected()
{
    if((radio_fatt_acq->isChecked() == false) && (radio_nota_credito->isChecked() == false))
        QMessageBox::warning(this,"FabariaGest",qApp->translate("dialog_fattura_acq","Seleziona una opzione"));
}

dialog_fattura_acq::~dialog_fattura_acq()
{
}
