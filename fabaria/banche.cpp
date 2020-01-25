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
#include "banche.h"
#include "ui_banche.h"
#include "itdelegato.h"
#include "pdf_export.h"
#include <QMessageBox>
#include <QtGui>
#include "settingsmanager.h"
#include "defines.h"
#include "queryexception.h"
#include "Debug.h"
#include <QCompleter>
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

banche::banche(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    connect(nuovo_b,SIGNAL(clicked()),this,SLOT(nuovo()));
    connect(modifica,SIGNAL(clicked()),this,SLOT(clicca()));
    connect(salva,SIGNAL(clicked()),this,SLOT(inserisci()));
    connect(elimina_b,SIGNAL(clicked()),this,SLOT(elimina()));
    connect(tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(pdf_ex,SIGNAL(clicked()),this,SLOT(esporta_pdf()));
    //Leggo il cap
    m_cap = cap->addAction(QIcon(":/images/edit-find.png"),QLineEdit::TrailingPosition);
    m_cap->setToolTip(qApp->translate("azienda","Seleziona cap"));
    m_cap->setCheckable(true);
    m_cap->setChecked(false);
    connect(m_cap,&QAction::toggled,this,&banche::inst_cap);

    //Leggo la città
    m_comuni = citta->addAction(QIcon(":/images/edit-find.png"),QLineEdit::TrailingPosition);
    m_comuni->setToolTip(qApp->translate("azienda","Seleziona località"));
    m_comuni->setCheckable(true);
    m_comuni->setChecked(false);
    connect(m_comuni,&QAction::toggled,this,&banche::instPaese);
    model = new QSqlRelationalTableModel();

    lista();
    nuovo();
}

void banche::nuovo(){
    if(selectQuery("SELECT max(id+1) from banca",0).toString() == "0"){
              id->setText("1");
    }
    else{
        id->setText(selectQuery("SELECT max(id+1) from banca",0).toString());
    }
    iban->clear();
    banca->clear();
    cap->clear();
    citta->clear();
    indirizzo->clear();
}

void banche::clearText()
{
    id->clear();
    iban->clear();
    banca->clear();
    cap->clear();
    citta->clear();
    indirizzo->clear();
}

void banche::clicca(){

    if(id->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("banche","Inserisci correttamente i dati"));
    }
    else{
        updateQuery("UPDATE banca SET iban='"+iban->text()+"', nome_banca ='"+banca->text()+"', cap='"+cap->text()+"', comune='"+citta->text()+"', indirizzo='"+indirizzo->text()+"' "
                    " where id='"+id->text()+"'","banca");
        
        lista();
        nuovo();
        emit banca_add();
    }
}

void banche::inserisci(){

    if(id->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("banche","Inserisci correttamente i dati"));
    }
    else{
        insertQuery("INSERT INTO banca VALUES('"+id->text()+"','"+iban->text()+"','"+banca->text()+"', '"+cap->text()+"','"+indirizzo->text()+"','"+citta->text()+"')","banca");
        
        lista();
        emit banca_add();
        nuovo();
    }
}

void banche::elimina(){

    if (!iban->text().isEmpty())
    {   
        eliminaQuery("DELETE FROM banca WHERE id = '"+id->text()+"' LIMIT 1","banca");
    }
    lista();
    emit banca_add();
    nuovo();
}

void banche::clickgrid(){

    QString id_new;
    int riga = tableView->selectionModel()->currentIndex().row();
    QModelIndex index = model->index(riga,0,QModelIndex());

    id_new = model->data(index).toString();
    id->setText(id_new);

    cerca();
}

void banche::lista(){

    model->setTable("banca");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, qApp->translate("banche","ID"));
    model->setHeaderData(1, Qt::Horizontal, qApp->translate("banche","IBAN"));
    model->setHeaderData(2, Qt::Horizontal, qApp->translate("banche","Banca"));
    model->setHeaderData(3, Qt::Horizontal, qApp->translate("banche","CAP"));
    model->setHeaderData(4, Qt::Horizontal, qApp->translate("banche","Indirizzo"));
    model->setHeaderData(5, Qt::Horizontal, QString::fromUtf8(qApp->translate("banche","Città").toStdString().c_str()));

    tableView->setModel(model);

    tableView->setColumnWidth(0, 200);
    tableView->setColumnWidth(1, 200);
    tableView->setColumnWidth(2, 50);
    tableView->setColumnWidth(3, 100);
    tableView->setColumnWidth(4, 100);
    tableView->setColumnWidth(5,200);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSortingEnabled(true);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->verticalHeader()->hide();
    tableView->setModel(model);
    tableView->setAlternatingRowColors(true);

    tableView->setItemDelegateForColumn(6, new coldxdelegato(this));

}

void banche::cerca(){
    id->setText(model->data(model->index(tableView->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString());
    iban->setText(model->data(model->index(tableView->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString());
    banca->setText(model->data(model->index(tableView->selectionModel()->currentIndex().row(),2),Qt::DisplayRole).toString());
    cap->setText(model->data(model->index(tableView->selectionModel()->currentIndex().row(),3),Qt::DisplayRole).toString());
    citta->setText(model->data(model->index(tableView->selectionModel()->currentIndex().row(),5),Qt::DisplayRole).toString());
    indirizzo->setText(model->data(model->index(tableView->selectionModel()->currentIndex().row(),4),Qt::DisplayRole).toString());
}

void banche::esporta_pdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("banche","Esporta PDF"),
                                                    "*.pdf", qApp->translate("banche","Pdf Files(*.pdf);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".pdf") < 0) {
                fileName += ".pdf";
            }
        }
    if(!fileName.isEmpty()){
       QPrinter *printer = new QPrinter(QPrinter::HighResolution);
       printer->setPageSize(QPrinter::A4);
       printer->setOrientation(QPrinter::Landscape);
       Q_UNUSED(printer);
        printer->setResolution(90);
        printer->setOutputFileName(fileName);
        printer->setPageMargins(10.0,10.0,10.0,10.0,QPrinter::Millimeter);
        QString id,nome_banca,iban,indirizzo,localita,cap,tabella;
        QFile file(":/stampa/banche.html");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        while(!file.atEnd())
        {
            tabella = file.readAll();
            for(int i = 0; i < model->rowCount(); ++i)
            {
                QSqlRecord record = model->record(i);
                if(!record.isEmpty())
                {
                    id = record.value("id").toString();
                    nome_banca = record.value("nome_banca").toString();
                    iban = record.value("iban").toString();
                    indirizzo = record.value("indirizzo").toString();
                    localita = record.value("localita").toString();
                    cap = record.value("cap").toString();
                    tabella.replace("LISTA BANCHE",qApp->translate("agenti","LISTA BANCHE"));
                    tabella.replace("ID",qApp->translate("agenti","Id"));
                    tabella.replace("NAME_BANCA",qApp->translate("agenti","Banca"));
                    tabella.replace("IBAN",qApp->translate("agenti","Iban"));
                    tabella.replace("ADDRESS",qApp->translate("agenti","Indirizzo"));
                    tabella.replace("CAP",qApp->translate("agenti","C.A.P."));
                    tabella.replace("CITY",qApp->translate("agenti","Città"));
                    tabella.replace("<td id=\"id\" valign=\"top\" align=\"center\">","<td id=\"id\" valign=\"top\" align=\"center\">"+id+"<br>");
                    tabella.replace("<td id=\"name_banca\" valign=\"top\" align=\"center\">","<td id=\"name_banca\" valign=\"top\" align=\"center\">"+nome_banca+"<br>");
                    tabella.replace("<td id=\"iban\" valign=\"top\" align=\"center\">","<td id=\"iban\" valign=\"top\" align=\"center\">"+iban+"<br>");
                    tabella.replace("<td id=\"address\" valign=\"top\" align=\"center\">","<td id=\"address\" valign=\"top\" align=\"center\">"+indirizzo+"<br>");
                    tabella.replace("<td id=\"cap\" valign=\"top\" align=\"center\">","<td id=\"cap\" valign=\"top\" align=\"center\">"+cap+"<br>");
                    tabella.replace("<td id=\"city\" valign=\"top\" align=\"center\">","<td id=\"city\" valign=\"top\" align=\"center\">"+localita+"<br>");
                }
            }
        }
        QTextDocument *doc = new QTextDocument;
        QSizeF paperSize;
        paperSize.setWidth(printer->width());
        paperSize.setHeight(printer->height());
        doc->setHtml(tabella);
        doc->setPageSize(paperSize);
        doc->print(printer);
        file.close();
    }
}

void banche::inst_cap()
{
    _comune = new Comuni("cap",this);
    connect(_comune,&Comuni::addComuni,this,&banche::inst_paese);
    _comune->exec();
}

void banche::instPaese()
{
    _comune = new Comuni("comune",this);
    connect(_comune,&Comuni::addComuni,this,&banche::inst_paese);
    _comune->exec();
}

void banche::inst_paese(QString m_com, QString m_pr, QString m_cp)
{
    Q_UNUSED(m_pr)
    citta->setText(m_com);
    cap->setText(m_cp);
}

banche::~banche()
{
}
