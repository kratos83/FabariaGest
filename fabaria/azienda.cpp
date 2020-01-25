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
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QDebug>
#include "azienda.h"
#include "ui_azienda.h"
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <QCloseEvent>
#include "itdelegato.h"
#include "mainwindow.h"
#include "queryexception.h"
#include "defines.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

azienda::azienda(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(licenza,SIGNAL(clicked()),this,SLOT(inserisci()));
    //Salvo i dati dell'azienda
    licenza->setShortcut(tr("Ctrl+S"));
    connect(update_azienda,SIGNAL(clicked()),this,SLOT(aggiorna()));
    //Esco dalla finestra dell'azienda
    connect(pushButton,SIGNAL(clicked()),this,SLOT(esci()));
    pushButton->setShortcut(tr("Ctrl+E"));
    //Elimino i dati dell'azienda
    connect(pushButton_2,SIGNAL(clicked()),this,SLOT(elimina()));
    pushButton_2->setShortcut(tr("Ctrl+D"));
    //Leggo il cap
    m_cap = cap->addAction(QIcon(":/images/edit-find.png"),QLineEdit::TrailingPosition);
    m_cap->setToolTip(qApp->translate("azienda","Seleziona cap"));
    m_cap->setCheckable(true);
    m_cap->setChecked(false);
    connect(m_cap,&QAction::toggled,this,&azienda::inst_cap);

    //Leggo la città
    m_comuni = localita->addAction(QIcon(":/images/edit-find.png"),QLineEdit::TrailingPosition);
    m_comuni->setToolTip(qApp->translate("azienda","Seleziona località"));
    m_comuni->setCheckable(true);
    m_comuni->setChecked(false);
    connect(m_comuni,&QAction::toggled,this,&azienda::instPaese);
    connect(tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(cod_f,SIGNAL(textEdited(const QString &)),this,SLOT(leggi_codFis(const QString &)));
    connect(p_iva,SIGNAL(textEdited(const QString &)),this,SLOT(leggi_codPartIva(const QString &)));
    connect(nuova_azienda,SIGNAL(clicked()),this,SLOT(nuovo()));
    connect(predefinita,SIGNAL(clicked()),this,SLOT(messaggioAzienda()));
    mod_grid = new QSqlQueryModel(this);
    manager = new SettingsManager;
    cerca();
    lista();
}

void azienda::clearText()
{
    rag_soc->clear();
    indirizzo->clear();
    cap->clear();
    localita->clear();
    prov->clear();
    telef->clear();
    fax->clear();
    web->clear();
    email->clear();
    p_iva->clear();
    cod_f->clear();
}

void azienda::nuovo()
{
    clearText();
    id->setText(selectQuery("SELECT count(id)+1 FROM azienda",0).toString());
}

void azienda::lista()
{
    mod_grid->setQuery("SELECT * FROM azienda ORDER BY id");
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("azienda","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("azienda","Ragione sociale"));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("azienda","Partita Iva"));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("azienda","Cod. fiscale"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("azienda","Indirizzo"));
    mod_grid->setHeaderData(5,Qt::Horizontal,qApp->translate("azienda","CAP"));
    mod_grid->setHeaderData(6,Qt::Horizontal,QString::fromUtf8(qApp->translate("azienda","Località").toStdString().c_str()));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("azienda","PROV."));
    mod_grid->setHeaderData(8,Qt::Horizontal,qApp->translate("azienda","Telefono"));
    mod_grid->setHeaderData(9,Qt::Horizontal,qApp->translate("azienda","FAX"));
    mod_grid->setHeaderData(10,Qt::Horizontal,qApp->translate("azienda","Sito web"));
    mod_grid->setHeaderData(11,Qt::Horizontal,qApp->translate("azienda","Email"));
    mod_grid->setHeaderData(12,Qt::Horizontal,qApp->translate("azienda","Predefinito"));

    tableView->setModel(mod_grid);

    tableView->setColumnWidth(0, 95);
    tableView->setColumnWidth(1, 100);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 120);
    tableView->setColumnWidth(4, 100);
    tableView->setColumnWidth(5, 100);
    tableView->setColumnWidth(6, 100);
    tableView->setColumnWidth(7, 100);
    tableView->setColumnWidth(8, 100);
    tableView->setColumnWidth(9, 100);
    tableView->setColumnWidth(10, 100);

    tableView->setAlternatingRowColors(true);
    tableView->setColumnHidden(0,true);

    tableView->setItemDelegateForColumn(11, new coldxdelegato(this));
}

void azienda::inserisci() {

    insertQuery("INSERT INTO azienda VALUES ('"+id->text()+"','"+rag_soc->text().replace("'","''")+"','"+p_iva->text()+"','"+cod_f->text()+"','"+indirizzo->text().replace("'","''")+"','"+cap->text()+"','"
                  +localita->text().replace("'","''")+"','"+prov->text()+"','"+telef->text()+"','"+fax->text()+"','"+web->text()+"','"+email->text()+"','')","azienda");
    messaggioAzienda();
    emit readReady(rag_soc->text());
    lista();
}

void azienda::aggiorna()
{
    updateQuery("UPDATE azienda SET nome_azienda='"+rag_soc->text().replace("'","''")+"', partita_iva='"+p_iva->text()+"', cod_fisc='"+cod_f->text()+"', indirizzo='"+indirizzo->text().replace("'","''")+"', "
                  "cap='"+cap->text()+"', localita='"+localita->text().replace("'","''")+"', prov='"+prov->text()+"', telefono='"+telef->text()+"', fax='"+fax->text()+"', sito='"+web->text()+"', email='"+email->text()+"', predefinito='' "
                  "WHERE id='"+id->text()+"'","azienda");
    messaggioAzienda();
    emit readReady(rag_soc->text());
    lista();
}

void azienda::elimina() {

    if (!rag_soc->text().isEmpty())
    {
        eliminaQuery("DELETE FROM azienda WHERE id ='"+id->text()+"' LIMIT 1","azienda");
        clearText();
    }
    lista();
}
void azienda::clickgrid()
{
    QString id_new;
    int riga = tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    id->setText(id_new);

    cerca_query();
}

void azienda::cerca_query() {

    id->setText(selectQuery("select * from azienda where id='"+id->text()+"'",0).toString());
    rag_soc->setText(selectQuery("select * from azienda where id='"+id->text()+"'",1).toString());
    indirizzo->setText(selectQuery("select * from azienda where id='"+id->text()+"'",4).toString());
    cap->setText(selectQuery("select * from azienda where id='"+id->text()+"'",5).toString());
    localita->setText(selectQuery("select * from azienda where id='"+id->text()+"'",6).toString());
    prov->setText(selectQuery("select * from azienda where id='"+id->text()+"'",7).toString());
    telef->setText(selectQuery("select * from azienda where id='"+id->text()+"'",8).toString());
    fax->setText(selectQuery("select * from azienda where id='"+id->text()+"'",9).toString());
    web->setText(selectQuery("select * from azienda where id='"+id->text()+"'",10).toString());
    email->setText(selectQuery("select * from azienda where id='"+id->text()+"'",11).toString());
    p_iva->setText(selectQuery("select * from azienda where id='"+id->text()+"'",2).toString());
    cod_f->setText(selectQuery("select * from azienda where id='"+id->text()+"'",3).toString());
}

void azienda::cerca() {

    QSqlRecord query;
    if(getConfigurazioneSql("AziendaID").toString() == "")
        query = selectQueryWhileList("select * from azienda where id='"+id->text()+"' and predefinito='Predefinito' ");
    else
        query = selectQueryWhileList("select * from azienda where id='"+getConfigurazioneSql("AziendaID").toString()+"' and predefinito='Predefinito' ");

    id->setText(query.value(0).toString());
    rag_soc->setText(query.value(1).toString());
    indirizzo->setText(query.value(4).toString());
    cap->setText(query.value(5).toString());
    localita->setText(query.value(6).toString());
    prov->setText(query.value(7).toString());
    telef->setText(query.value(8).toString());
    fax->setText(query.value(9).toString());
    web->setText(query.value(10).toString());
    email->setText(query.value(11).toString());
    p_iva->setText(query.value(2).toString());
    cod_f->setText(query.value(3).toString());
}

void azienda::esci() {

    close();
}

bool azienda::eventFilter(QObject *o, QEvent *e)
{
    QKeyEvent* ke = (QKeyEvent*)e;
    //Add event to menu key
    if (e->type() == QEvent::KeyPress) {

        if (ke->key() == Qt::Key_Delete) {
            // ESEGUO IL CODICE DI ELIMINAZIONE
            elimina();
        }
        if(ke->key() == Qt::Key_S) {
            //ESEGUO IL CODICE PER INSERIRE
            inserisci();
        }
        if(ke->key() == Qt::Key_M) {
            //ESEGUO IL CODICE PER INSERIRE
            aggiorna();
        }
        if(ke->key() == Qt::Key_E) {
            //ESEGUO IL CODICE PER AGGIORNARE
            esci();
        }

    }

    //Installazione eventi Qt::Key_Tab
    return QDialog::eventFilter(o, e);
}

void azienda::inst_cap()
{
    _comune = new Comuni("cap",this);
    connect(_comune,&Comuni::addComuni,this,&azienda::inst_paese);
    _comune->exec();
}

void azienda::instPaese()
{
    _comune = new Comuni("comune",this);
    connect(_comune,&Comuni::addComuni,this,&azienda::inst_paese);
    _comune->exec();
}

void azienda::inst_paese(QString m_com, QString m_pr, QString m_cp)
{
    localita->setText(m_com);
    prov->setText(m_pr);
    cap->setText(m_cp);
}

void azienda::leggi_codFis(const QString &)
{
    getVerificaCodiceFiscale(cod_f->text(),codfis_image);
}

void azienda::leggi_codPartIva(const QString &)
{
    getPartitaIva(p_iva->text(),codiva_image);
}

void azienda::azienda_predefinita()
{
    if(selectQuery("select id from azienda where predefinito='Predefinito'",0).toString() == getConfigurazioneSql("AziendaID").toString()) 
    {
        updateQuery("UPDATE azienda SET  predefinito='' "
                          "WHERE id='"+getConfigurazioneSql("AziendaID").toString()+"'; UPDATE azienda SET  predefinito='Predefinito' "
                          "WHERE id='"+id->text()+"'","azienda");
    }
    else {
        updateQuery("UPDATE azienda SET  predefinito='Predefinito' "
                    "WHERE id='"+id->text()+"'","azienda");
    }

    emit readReady(rag_soc->text());
    updateQuery("UPDATE configurazione set AziendaID='"+id->text()+"' "
                "WHERE id='1'","azienda");
    lista();
    cerca();
}

void azienda::messaggioAzienda()
{
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle("FabariaGest");
    box->setText(qApp->translate("azienda","Azienda predefinita"));
    box->setInformativeText(QString(qApp->translate("azienda","Vuoi rendere predefinita l'azienda ")+rag_soc->text()+qApp->translate("azienda"," inserita?")));
    box->setIcon(QMessageBox::Information);
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret) {
    case QMessageBox::Ok: {
        azienda_predefinita();
        cerca();
        emit readReady(rag_soc->text());
        break;
    }
    case QMessageBox::Cancel:
        return;
        break;
    }
}

azienda::~azienda()
{
}
