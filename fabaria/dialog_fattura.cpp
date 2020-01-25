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
#include "dialog_fattura.h"
#include "ui_dialog_fattura.h"
#include "fattura.h"
#include "fattura_rg.h"
#include "settingsmanager.h"
#include "defines.h"

dialog_fattura::dialog_fattura(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    setWindowTitle(qApp->translate("dialog_fattura","Scelta tipo di fattura"));
    connect(cancel,SIGNAL(clicked()),this,SLOT(close()));
    connect(radio_fatt_imm,SIGNAL(clicked()),this,SLOT(interfaccia()));
    connect(radio_fatt_acc,SIGNAL(clicked()),this,SLOT(interfaccia()));
    connect(radio_pro_forma,SIGNAL(clicked()),this,SLOT(interfaccia()));
    connect(radio_nota_credito,SIGNAL(clicked()),this,SLOT(interfaccia()));
    connect(radio_scontrino,SIGNAL(clicked()),this,SLOT(interfaccia()));
    connect(bt_ok,SIGNAL(clicked()),this,SLOT(not_selected()));

}

void dialog_fattura::interfaccia()
{
    if(radio_fatt_imm->isChecked()){
        connect(bt_ok,SIGNAL(clicked()),this,SLOT(fa_imm()));
        radio_fatt_acc->setEnabled(false);
        radio_pro_forma->setEnabled(false);
        radio_nota_credito->setEnabled(false);
        radio_scontrino->setEnabled(false);
    }
    else if(radio_fatt_acc->isChecked()){
        connect(bt_ok,SIGNAL(clicked()),this,SLOT(fa_acc()));
        radio_fatt_imm->setEnabled(false);
        radio_pro_forma->setEnabled(false);
        radio_nota_credito->setEnabled(false);
        radio_scontrino->setEnabled(false);
    }
    else if(radio_pro_forma->isChecked()){
        connect(bt_ok,SIGNAL(clicked()),this,SLOT(fa_proforma()));
        radio_fatt_imm->setEnabled(false);
        radio_fatt_acc->setEnabled(false);
        radio_nota_credito->setEnabled(false);
        radio_scontrino->setEnabled(false);
    }
    else if(radio_nota_credito->isChecked())
    {
        connect(bt_ok,SIGNAL(clicked()),this,SLOT(nota_credito()));
        radio_fatt_imm->setEnabled(false);
        radio_fatt_acc->setEnabled(false);
        radio_scontrino->setEnabled(false);
        radio_pro_forma->setEnabled(false);
    }
    else if(radio_scontrino->isChecked())
    {
        connect(bt_ok,SIGNAL(clicked()),this,SLOT(scontrino()));
        radio_fatt_imm->setEnabled(false);
        radio_fatt_acc->setEnabled(false);
        radio_nota_credito->setEnabled(false);
        radio_pro_forma->setEnabled(false);
    }
}

void dialog_fattura::fa_imm()
{
    close();
    QString ft = fatt_tipo.Fattura_immediata;
    ft = qApp->translate("dialog_fattura","Fattura immediata");

    QSqlQuery query("select max(id)+1 from fattura_vendita where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            fs = new fattura_rg(this,ft,1);
            fs->formatta_num();
            fs->disable_fatt_acc();

    connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
    fs->exec();
    }
    else{
            fs = new fattura_rg(this,ft,query.value(0).toInt());
            fs->formatta_num();
            fs->disable_fatt_acc();
            connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
            fs->exec();
        }
    }
}

void dialog_fattura::fa_acc()
{
    close();
    QString ft = fatt_tipo.Fattura_accompagnatoria;
    ft = qApp->translate("dialog_fattura","Fattura accompagnatoria");

    QSqlQuery query("select max(id)+1 from fattura_vendita where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            fs = new fattura_rg(this,ft,1);
            fs->formatta_num();
            fs->enable_fatt_acc();   //Abilita la finestra di dialogo per
                                       // la fattura accompagnatoria
    connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
    fs->exec();
    }
    else{
            fs = new fattura_rg(this,ft,query.value(0).toInt());
            fs->formatta_num();
            fs->enable_fatt_acc();  //Abilita la finestra di dialogo per
                                       // la fattura accompagnatoria
            connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
            fs->exec();
        }
    }
}

void dialog_fattura::fa_proforma()
{
    close();
    QString ft = fatt_tipo.Fattura_proforma;
    ft = qApp->translate("dialog_fattura","Fattura proforma");

    QSqlQuery query("select max(id)+1 from fattura_vendita where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            fs = new fattura_rg(this,ft,1);
            fs->formatta_num();
            fs->disable_fatt_acc();

    connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
    fs->exec();
    }
    else{
            fs = new fattura_rg(this,ft,query.value(0).toInt());
            fs->formatta_num();
            fs->disable_fatt_acc();
            connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
            fs->exec();
        }
    }
}

void dialog_fattura::nota_credito()
{
    close();
    QString ft = fatt_tipo.Nota_di_credito;
    ft = qApp->translate("dialog_fattura","Nota di credito");

    QSqlQuery query("select max(id)+1 from fattura_vendita where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            fs = new fattura_rg(this,ft,1);
            fs->formatta_num();
            fs->disable_fatt_acc();
	    fs->dis_allegati();
    connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
    fs->exec();
    }
    else{
            fs = new fattura_rg(this,ft,query.value(0).toInt());
            fs->formatta_num();
            fs->disable_fatt_acc();
	    fs->dis_allegati();
            connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
            fs->exec();
        }
    }
}

void dialog_fattura::scontrino()
{
    close();
    QString ft = fatt_tipo.Scontrino;
    ft = qApp->translate("dialog_fattura","Scontrino");

    QSqlQuery query("select max(id)+1 from fattura_vendita where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            fs = new fattura_rg(this,ft,1);
            fs->formatta_num();
            fs->disable_fatt_acc();
	    fs->dis_allegati();

    connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
    fs->exec();
    }
    else{
            fs = new fattura_rg(this,ft,query.value(0).toInt());
            fs->formatta_num();
            fs->disable_fatt_acc();
	    fs->dis_allegati();
            connect(fs,SIGNAL(salvafattura()),fatt_vendita,SLOT(lista_prodotti()));
            fs->exec();
        }
    }
    fs->exec();
}

void dialog_fattura::not_selected()
{
    if((radio_fatt_acc->isChecked() == false) && (radio_fatt_imm->isChecked() == false) && (radio_nota_credito->isChecked() == false)
            && (radio_pro_forma->isChecked() == false) && (radio_scontrino->isChecked() == false) )
        QMessageBox::warning(this,"FabariaGest",qApp->translate("dialog_fattura","Seleziona una opzione"));
}

dialog_fattura::~dialog_fattura()
{

}
