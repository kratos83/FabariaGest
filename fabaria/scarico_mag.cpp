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
#include "scarico_mag.h"
#include "ui_scarico_mag.h"
#include "itdelegato.h"
#include "print.h"
#include "defines.h"
#include"queryexception.h"

using namespace std;
using namespace Defines;
using namespace QXlsx;

scarico_mag::scarico_mag(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(qApp->translate("scarico_mag","Scarico magazzino"));
    mod_grid = new QSqlRelationalTableModel();
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    //Esco dalla finestra di dialogo
    connect(pushButton,SIGNAL(clicked()),this,SLOT(close()));
    pushButton->setShortcut(qApp->translate("scarico_mag","Ctrl+E"));
    //Esporto scarico magazzino in csv
    connect(ex_csv,SIGNAL(clicked()),this,SLOT(esporta_csv()));
    ex_csv->setShortcut(qApp->translate("scarico_mag","Ctrl+C"));
    //Esporto scarico magazzino in pdf
    connect(ex_pdf,SIGNAL(clicked()),this,SLOT(esporta_pdf()));
    ex_pdf->setShortcut(qApp->translate("scarico_mag","Ctrl+F"));
    //Stampo la lista scarico magazzino
    connect(pr_st,SIGNAL(clicked()),this,SLOT(stampa_scarico()));
    pr_st->setShortcut(qApp->translate("scarico_mag","Ctrl+P"));
    //Elimino una riga dallo scarico magazzino
    connect(del_rec,SIGNAL(clicked()),this,SLOT(elimina()));
    del_rec->setShortcut(qApp->translate("scarico_mag","Ctrl+D"));
    //Esporta excel scarico magazzino
    connect(esportaexcel,SIGNAL(clicked()),this,SLOT(esporta_excel_scarico()));
    esportaexcel->setShortcut(qApp->translate("scarico_mag","Ctrl+S"));
    quan->setReadOnly(true);
    tot_scar->setReadOnly(true);
    dettagli->setReadOnly(true);
    quan_art();
    lista_prodotti();
    totale_venduto();
}


void scarico_mag::lista_prodotti()
{
    mod_grid->setTable("scarico_mag");
    mod_grid->select();
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("scarico_mag","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("scarico_mag","Codice a barre."));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("scarico_mag","Prodotto."));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("scarico_mag","Descrizione."));
    mod_grid->setHeaderData(4, Qt::Horizontal,QString::fromUtf8(qApp->translate("scarico_mag","Unità di misura").toStdString().c_str()));
    mod_grid->setHeaderData(5, Qt::Horizontal, qApp->translate("scarico_mag","Prezzo unitario"));
    mod_grid->setHeaderData(6, Qt::Horizontal, qApp->translate("scarico_mag","Prezzo senza iva"));
    mod_grid->setHeaderData(7, Qt::Horizontal, qApp->translate("scarico_mag","Prezzo con iva"));
    mod_grid->setHeaderData(8, Qt::Horizontal, qApp->translate("scarico_mag","Iva"));
    mod_grid->setHeaderData(9, Qt::Horizontal,QString::fromUtf8(qApp->translate("scarico_mag","Quantità").toStdString().c_str()));
    mod_grid->setHeaderData(10, Qt::Horizontal, qApp->translate("scarico_mag","Sconto"));
    mod_grid->setHeaderData(11, Qt::Horizontal, qApp->translate("scarico_mag","Totale"));
    mod_grid->setHeaderData(13, Qt::Horizontal, qApp->translate("scarico_mag","Codice listino"));
    mod_grid->setHeaderData(14, Qt::Horizontal, qApp->translate("scarico_mag","Categoria"));
    mod_grid->setHeaderData(16,Qt::Horizontal,qApp->translate("scarico_mag","Lotto"));
    mod_grid->setHeaderData(17,Qt::Horizontal,qApp->translate("scarico_mag","Scadenza lotto"));
    tab_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    tab_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab_view->setSelectionMode(QAbstractItemView::SingleSelection);
    tab_view->setSortingEnabled(true);
    tab_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tab_view->verticalHeader()->hide();
    tab_view->setModel(mod_grid);
    tab_view->setColumnHidden(12,true);
    tab_view->setColumnHidden(15,true);
    tab_view->setAlternatingRowColors(true);

    tab_view->setItemDelegateForColumn(14, new coldxdelegato(this));
}


void scarico_mag::clickgrid()
{
    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    cerca();
}

void scarico_mag::cerca()
{
	QSqlQuery query;
	QString cod_articolo = mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),tab_view->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
	query.prepare("SELECT * FROM scarico_mag where cod_articolo='"+cod_articolo+"'");
        query.exec();
	
        if (query.next())
        {

            dettagli->setText(qApp->translate("scarico_mag","Id: ")+query.value(0).toString()+"\n"+
                              qApp->translate("scarico_mag","Codice a barre: ")+query.value(1).toString()+"\n"+
                              qApp->translate("scarico_mag","Nome prodotto: ")+query.value(2).toString()+"\n"+
                              qApp->translate("scarico_mag","Descrizione: ")+query.value(3).toString()+"\n"+
                              QString::fromUtf8(qApp->translate("scarico_mag","Unità di misura: ").toStdString().c_str())+query.value(4).toString()+"\n"+
                              qApp->translate("scarico_mag","Prezzo unitario: ")+QString::fromUtf8("\u20AC")+" "+query.value(5).toString()+"\n"+
                              qApp->translate("scarico_mag","Prezzo senza iva: ")+QString::fromUtf8("\u20AC")+" "+query.value(6).toString()+"\n"+
                              qApp->translate("scarico_mag","Aliquota IVA: ")+query.value(8).toString()+"%\n"+
                              QString::fromUtf8(qApp->translate("scarico_mag","Quantità: ").toStdString().c_str())+query.value(9).toString()+"\n"+
                              qApp->translate("scarico_mag","Sconto: ")+QString::fromUtf8("\u20AC")+" "+query.value(10).toString()+"\n"+
                              qApp->translate("scarico_mag","Totale: ")+QString::fromUtf8("\u20AC")+" "+query.value(11).toString()+"\n"+
                             qApp->translate("scarico_mag","Categoria: ")+query.value(14).toString()+"\n"+
                             qApp->translate("scarico_mag","Tipo di listino: ")+query.value(13).toString());

        }
        else
        {
            // Elemento non trovato, pulizia campi di immissione
             QMessageBox::warning(this,"Fabaria Gest",qApp->translate("scarico_mag","Attenzione devi cliccare su ID articolo"));
        }
}



void scarico_mag::quan_art()
{
    QSqlQuery query;
    query.prepare("SELECT SUM(quantita) from scarico_mag");
    query.exec();

    if(query.next()){
        double text = query.value(0).toInt();
        quan->setText(QString::fromUtf8("N°  ")+QString::number(text));
    }
    else{
        QMessageBox::warning(this,"Fabaria Gest",QString::fromUtf8(qApp->translate("scarico_mag","Impossibile instanziare la quantità di articoli non presenti in magazzino...").toStdString().c_str()));
    }
}

void scarico_mag::totale_venduto()
{
    QSqlQuery query;
    query.prepare("SELECT SUM(totale) from scarico_mag");
    query.exec();

    if(query.next()){
        double prezzo = query.value(0).toDouble();
        tot_scar->setText(QString::fromUtf8("\u20AC")+" "+QString::number(prezzo));
    }
    else{
        QMessageBox::warning(this,"Fabaria Gest",QString::fromUtf8(qApp->translate("scarico_mag","Impossibile instanziare il prezzo totale degli articoli non presenti in magazzino...").toStdString().c_str()));
    }
}

void scarico_mag::esporta_csv()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("scarico_mag","Esporta CSV"),
                                                    "*.csv", qApp->translate("scarico_mag","CSV(*.csv);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".csv") < 0) {
                fileName += ".csv";
            }
        }

    int x = 0;
    QString exportdata;

    int counthidden=0, jint = 0;

    while (jint < mod_grid->columnCount()) {

    counthidden+=tab_view->isColumnHidden(jint);
    jint++;
    }


    int w = 1;
    while (x < mod_grid->columnCount()){

    if (!tab_view->isColumnHidden(x)) {


    exportdata.append(mod_grid->headerData(x,Qt::Horizontal,Qt::DisplayRole).toString());


    if (mod_grid->columnCount() - w > counthidden)
    exportdata.append(";");
    else {
    exportdata.append("\n");

    }
    w++;
    }
    x++;

    }

    int z = 0;

    w = 1;
    while (z < mod_grid->rowCount()) {

    x = 0;

    w = 1;
    while (x < mod_grid->columnCount()) {
    if (!tab_view->isColumnHidden(x)) {


    exportdata.append(mod_grid->data(mod_grid->index(z,x),Qt::DisplayRole).toString());

    if (mod_grid->columnCount() - w > counthidden)
    exportdata.append(";");
    else
    exportdata.append("\n");

    w++;
    }
    x++;

    }

    z++;
    }





       QFile file;
       if (!fileName.isEmpty()) {
     file.setFileName(fileName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return;
    }
    QByteArray ttext;
    ttext.append(exportdata);
    file.write(ttext);
    }
    else{
    QMessageBox::warning(this,"Fabaria Gest",qApp->translate("scarico_mag","Selezionare una riga da esportare in csv..."));
    }
}

void scarico_mag::esporta_pdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("scarico_mag","Esporta PDF"),
                                                    "*.pdf", qApp->translate("scarico_mag","Pdf Files(*.pdf);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".pdf") < 0) {
                fileName += ".pdf";
            }
        }
       QPrinter *printer = new QPrinter(QPrinter::HighResolution);
       printer->setPageSize(QPrinter::A4);
       printer->setOrientation(QPrinter::Landscape);
       Q_UNUSED(printer);
       printer->setOutputFileName(fileName);
       Stampe *st = new Stampe();
       st->esporta_scarico(printer,mod_grid);
}

void scarico_mag::elimina()
{
    elimina_record_prodotti();
}

void scarico_mag::elimina_record_prodotti()
{

    if(tab_view->selectionModel()->currentIndex().isValid()){
        QSqlQuery qctrl;
	QString cod_articolo = mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),tab_view->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
        qctrl.prepare("SELECT * FROM scarico_mag where cod_articolo='"+cod_articolo+"'");
        qctrl.exec();

            QMessageBox *box= new QMessageBox(this);
            box->setWindowTitle("Fabaria gest");
            box->setInformativeText(qApp->translate("scarico_mag","Vuoi eliminare veramente \n il record selezionato?...."));
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
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("scarico_mag","Selezionare una riga da eliminare..."));
    }
}

void scarico_mag::elimina_riga_prodotti()
{
    QSqlQuery query;
    QString cod_articolo = mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),tab_view->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
    query.prepare("DELETE FROM scarico_mag where cod_articolo='"+cod_articolo+"' LIMIT 1");
    
    try{
    if (query.exec()) //Se esito OK Eliminato da DB
    {
        lista_prodotti();
	LOG_FILE(qApp->translate("scarico_mag","Eliminazione avvenuta con successo..."),LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
	LOG_FILE(qApp->translate("scarico_mag","Impossibile eliminare: ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    quan_art();
    totale_venduto();
}



void scarico_mag::stampa_scarico()
{
    //Aggiornamento query
   QPrinter printer(QPrinter::HighResolution);
   QPrintPreviewDialog preview(&printer);
   preview.setWindowFlags(Qt::Window);
   preview.setWindowTitle(qApp->translate("scarico_mag","Anteprima di stampa."));
   QIcon icon;
   icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
   preview.setWindowIcon(icon);
   connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(print_carico(QPrinter*)));
   preview.exec();
}

void scarico_mag::print_carico(QPrinter *printer)
{
    Q_UNUSED(printer);
    Stampe *st = new Stampe();
    st->esporta_scarico(printer,mod_grid);
}

void scarico_mag::esporta_excel_scarico()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("scarico_mag","Esporta Excel"),
                                                            "*.xlsx", qApp->translate("scarico_mag","XLSX(*.xlsx);;Tutti i file(*.*)"));


            if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".xlsx") < 0) {
                    fileName += ".xlsx";
                }
            }

            Document excel;
            for(int i= 0; i < mod_grid->columnCount();++i){
                    //Imposto header label per la descrizione
		    Format fm;
		    fm.setFontName("OpenSans");
		    fm.setFontBold(true);
		    fm.setFontSize(14);
		    excel.setRowHeight(1,30.0);
                    excel.write(1,1+i,mod_grid->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString(),fm);
                for(int row = 0; row < mod_grid->rowCount(); row++){
                    //Imposto le righe
                    excel.write(2+row,1+i,mod_grid->data(mod_grid->index(row,i),Qt::DisplayRole).toString());
                }
            }

         try{
            int ris = excel.saveAs(fileName);
            if(ris )
		QMessageBox::information(this,"FabariaGest",qApp->translate("scarico_mag","File salvato correttamente"));
            else
                throw InvalidFile(qApp->translate("scarico_mag","File non salvato").toStdString(),fileName);
	  }
	  catch(exception e)
	  {
	      throw InvalidFile(e.what(),fileName);
	  }
}

scarico_mag::~scarico_mag()
{
    //delete ui;
}
