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
#include "carico_mag.h"
#include "itdelegato.h"
#include "print.h"
#include "ui_carico_mag.h"
#include "settingsmanager.h"
#include "queryexception.h"
#include "defines.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

carico_mag::carico_mag(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(qApp->translate("carico_mag","Carico magazzino"));
    mod_grid = new QSqlRelationalTableModel();
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    //Esco dalla finestra di dialogo
    connect(pushButton,SIGNAL(clicked()),this,SLOT(close()));
    pushButton->setShortcut(qApp->translate("carico_mag","Ctrl+E"));
    //Esporto carico magazzino in csv
    connect(ex_csv,SIGNAL(clicked()),this,SLOT(esporta_csv()));
    ex_csv->setShortcut(qApp->translate("carico_mag","Ctrl+C"));
    //Esporto carico magazzino in pdf
    connect(ex_pdf,SIGNAL(clicked()),this,SLOT(esporta_pdf()));
    ex_pdf->setShortcut(qApp->translate("carico_mag","Ctrl+F"));
    //Stampo la lista carico magazzino
    connect(pr_st,SIGNAL(clicked()),this,SLOT(stampa_carico()));
    pr_st->setShortcut(qApp->translate("carico_mag","Ctrl+P"));
    //Elimino una riga dallo carico magazzino
    connect(del_rec,SIGNAL(clicked()),this,SLOT(elimina()));
    del_rec->setShortcut(qApp->translate("carico_mag","Ctrl+D"));
    //Esporta excel carico magazzino
    connect(export_excel,SIGNAL(clicked()),this,SLOT(esporta_excel()));
    export_excel->setShortcut(qApp->translate("carico_mag","Ctrl+X"));
    quan->setReadOnly(true);
    tot_car->setReadOnly(true);
    dettagli->setReadOnly(true);
    quan_art();
    lista_prodotti();
    totale_magazzino();
}


void carico_mag::lista_prodotti()
{
    mod_grid->setTable("carico_magazzino");
    mod_grid->select();
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("carico_mag","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("carico_mag","Codice a barre."));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("carico_mag","Prodotto."));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("carico_mag","Descrizione."));
    mod_grid->setHeaderData(4, Qt::Horizontal, qApp->translate("carico_mag","Unita' di misura"));
    mod_grid->setHeaderData(5, Qt::Horizontal, qApp->translate("carico_mag","Prezzo unitario"));
    mod_grid->setHeaderData(6, Qt::Horizontal, qApp->translate("carico_mag","Prezzo senza iva"));
    mod_grid->setHeaderData(7, Qt::Horizontal, qApp->translate("carico_mag","Prezzo con iva"));
    mod_grid->setHeaderData(8, Qt::Horizontal, qApp->translate("carico_mag","Iva"));
    mod_grid->setHeaderData(9, Qt::Horizontal, qApp->translate("carico_mag","Quantita'"));
    mod_grid->setHeaderData(10, Qt::Horizontal, qApp->translate("carico_mag","Sconto"));
    mod_grid->setHeaderData(11, Qt::Horizontal, qApp->translate("carico_mag","Totale"));
    mod_grid->setHeaderData(13, Qt::Horizontal, qApp->translate("carico_mag","Codice listino"));
    mod_grid->setHeaderData(14, Qt::Horizontal, qApp->translate("carico_mag","Categoria"));
    mod_grid->setHeaderData(16,Qt::Horizontal,qApp->translate("carico_mag","Lotto"));
    mod_grid->setHeaderData(17,Qt::Horizontal,qApp->translate("carico_mag","Scadenza lotto"));
    mod_grid->setHeaderData(18,Qt::Horizontal,qApp->translate("carico_mag","Magazzino"));
    tab_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    tab_view->setSelectionMode(QAbstractItemView::SingleSelection);
    tab_view->setSortingEnabled(true);
    tab_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab_view->verticalHeader()->hide();
    tab_view->setModel(mod_grid);
    tab_view->setColumnHidden(12,true);
    tab_view->setColumnHidden(15,true);
    tab_view->setAlternatingRowColors(true);

    tab_view->setItemDelegateForColumn(14, new coldxdelegato(this));
}


void carico_mag::clickgrid()
{
    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    cerca();
}

void carico_mag::cerca()
{
    dettagli->setText(qApp->translate("carico_mag","Codice articolo: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Codice a barre: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Nome prodotto: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),2),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Descrizione: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),3),Qt::DisplayRole).toString()+"\n"+
                              QString::fromUtf8(qApp->translate("carico_mag","Unità di misura: ").toStdString().c_str())+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),4),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Prezzo unitario: ")+QString::fromUtf8("\u20AC")+" "+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),5),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Prezzo senza iva: ")+QString::fromUtf8("\u20AC")+" "+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),6),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Aliquota IVA: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),8),Qt::DisplayRole).toString()+"%\n"+
                              QString::fromUtf8(qApp->translate("carico_mag","Quantità: ").toStdString().c_str())+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),9),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Sconto: ")+QString::fromUtf8("\u20AC")+" "+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),10),Qt::DisplayRole).toString()+"\n"+
                              qApp->translate("carico_mag","Totale: ")+QString::fromUtf8("\u20AC")+" "+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),11),Qt::DisplayRole).toString()+"\n"+
                             qApp->translate("carico_mag","Categoria: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),14),Qt::DisplayRole).toString()+"\n"+
                             qApp->translate("carico_mag","Tipo di listino: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),13),Qt::DisplayRole).toString());
}


void carico_mag::quan_art()
{
    quan->clear();
    double m_quan = 0;
    for (int i = 0; i < mod_grid->rowCount(); i++)
    {
        QModelIndex col_quant = mod_grid->index(i,9);
        QString _quan = QString(mod_grid->data(col_quant,Qt::DisplayRole).toString());
        m_quan += _quan.toDouble();
    }
    quan->setText(QString::fromUtf8("N° ")+QString::number(m_quan));
}

void carico_mag::totale_magazzino()
{
    double m_tot = 0;
    for (int i = 0; i < mod_grid->rowCount(); i++)
    {
        QModelIndex col_tot = mod_grid->index(i,9);
        QString _tot = QString(mod_grid->data(col_tot,Qt::DisplayRole).toString());
        m_tot += _tot.toDouble();
    }
    tot_car->setText(QString::fromUtf8("\u20AC")+" "+QString::number(m_tot,'f',2));
}

void carico_mag::esporta_csv()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
        esportaCSV("carico_mag",tab_view,mod_grid);     
    }
    else{
    QMessageBox::warning(this,"FabariaGest",qApp->translate("carico_mag","Selezionare una riga da esportare in csv..."));
    }
}

void carico_mag::esporta_pdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("carico_mag","Esporta PDF"),
                                                    "*.pdf", qApp->translate("carico_mag","Pdf Files(*.pdf);;Tutti i file(*.*)"));

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
       print_carico(printer);
    }
}

void carico_mag::elimina()
{
    elimina_record_prodotti();
}

void carico_mag::elimina_record_prodotti()
{
        if(tab_view->selectionModel()->currentIndex().isValid()){
            QMessageBox *box= new QMessageBox(this);
            box->setWindowTitle("Fabaria gest");
            box->setInformativeText(qApp->translate("carico_mag","Vuoi eliminare veramente \n il record selezionato?...."));
            box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box->setDefaultButton(QMessageBox::Ok);
            int ret = box->exec();
            switch(ret){
            case QMessageBox::Ok:
                 //Close
                 elimina_riga_prodotti();
                 box->close();
                 break;
            case QMessageBox::Cancel:
                //return
                box->close();
                break;
            }
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("carico_mag","Selezionare una riga da eliminare..."));
    }
}

void carico_mag::elimina_riga_prodotti()
{     
    eliminaQuery("DELETE FROM carico_magazzino where cod_articolo='"+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),tab_view->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString()+"'","carico_magazzino");
    eliminaQuery("DELETE FROM articoli where cod_articolo='"+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),tab_view->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString()+"'","carico_magazzino");
  
    lista_prodotti();
    quan_art();
    totale_magazzino();
}

void carico_mag::stampa_carico()
{
    //Aggiornamento query
   QPrinter printer(QPrinter::HighResolution);
   QPrintPreviewDialog preview(&printer);
   preview.setWindowFlags(Qt::Window);
   preview.setWindowTitle(qApp->translate("carico_mag","Anteprima di stampa."));
   QIcon icon;
   icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
   preview.setWindowIcon(icon);
   connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(print_carico(QPrinter*)));
   preview.exec();
}

void carico_mag::print_carico(QPrinter *printer)
{
    Q_UNUSED(printer);
    printer->setPageSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);
    printer->setResolution(90);
    QString codice_art,codice_barre,nome_articolo,descrizione_print,prezzo_unit,m_iva,m_totale,tabella;
        QFile file(":/stampa/carico_mag.html");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        while(!file.atEnd())
        {
            tabella = file.readAll();
            for(int i = 0; i < mod_grid->rowCount(); ++i)
            {
                QSqlRecord record = mod_grid->record(i);
                if(!record.isEmpty())
                {
                    codice_art = record.value("cod_articolo").toString();
                    codice_barre = record.value("codbarre").toString();
                    nome_articolo = record.value("nome_articolo").toString();
                    descrizione_print = record.value("descrizione").toString();
                    prezzo_unit = record.value("prezzo_unitario").toString();
                    m_iva = record.value("iva").toString();
                    m_totale = record.value("totale").toString();
                    tabella.replace("LISTA CARICO_MAGAZZINO",qApp->translate("carico_mag","LISTA CARICO MAGAZZINO"));
                    tabella.replace("CODICE_ARTICOLO",qApp->translate("carico_mag","Codice articolo"));
                    tabella.replace("CODBARRE",qApp->translate("carico_mag","Codice a barre"));
                    tabella.replace("NOME_ARTICOLO",qApp->translate("carico_mag","Prodotto"));
                    tabella.replace("DESCRIZIONE",qApp->translate("carico_mag","Descrizione"));
                    tabella.replace("PREZZO_UNITARIO",qApp->translate("carico_mag","Prezzo unitario"));
                    tabella.replace("IVA",qApp->translate("carico_mag","IVA"));
                    tabella.replace("TOTALE",qApp->translate("carico_mag","Totale"));
                    tabella.replace("<td id=\"codice_articolo\" valign=\"top\" align=\"center\">","<td id=\"codice_articolo\" valign=\"top\" align=\"center\">"+codice_art+"<br>");
                    tabella.replace("<td id=\"codbarre\" valign=\"top\" align=\"center\">","<td id=\"codbarre\" valign=\"top\" align=\"center\">"+codice_barre+"<br>");
                    tabella.replace("<td id=\"nome_articolo\" valign=\"top\" align=\"center\">","<td id=\"nome_articolo\" valign=\"top\" align=\"center\">"+nome_articolo+"<br>");
                    tabella.replace("<td id=\"descrizione\" valign=\"top\" align=\"center\">","<td id=\"descrizione\" valign=\"top\" align=\"center\">"+descrizione_print+"<br>");
                    tabella.replace("<td id=\"prezzo_unitario\" valign=\"top\" align=\"center\">","<td id=\"prezzo_unitario\" valign=\"top\" align=\"center\">€ "+prezzo_unit+"<br>");
                    tabella.replace("<td id=\"iva\" valign=\"top\" align=\"center\">","<td id=\"iva\" valign=\"top\" align=\"center\">"+m_iva+"%<br>");
                    tabella.replace("<td id=\"totale\" valign=\"top\" align=\"center\">","<td id=\"totale\" valign=\"top\" align=\"center\">€ "+m_totale+"<br>");
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

void carico_mag::esporta_excel()
{
    esportaExcelDefine("carico_mag",mod_grid);
}

carico_mag::~carico_mag()
{
    //delete ui;
}
