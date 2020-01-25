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
#include "itdelegato.h"
#include "lista_invio_mail.h"

#include "itdelegato.h"
#include "defines.h"

using namespace std;
using namespace Defines;
using namespace QXlsx;

lista_invio_mail::lista_invio_mail(QWidget *parent) :
    QDialog(parent),ui(new Ui::lista_invio_mail)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    
    /***********************************************************
    *  @author Angelo Scarnà
    *  Immagini per il menu a tendina
    ************************************************************/
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/excel.png"));

    QIcon pdf;
    pdf.addFile(QString::fromUtf8(":/images/pdf.png"));
    /*
     * Menu bottone trasforma
     */

    esporta_pdf = new QAction(qApp->translate("lista_invio_mail","Esporta in pdf"),this);
    esporta_pdf->setIcon(pdf);
    esporta_pdf->setIconVisibleInMenu(true);
    
    esporta_excel = new QAction(qApp->translate("lista_invio_mail","Esporta in excel"),this);
    esporta_excel->setIcon(icon);
    esporta_excel->setIconVisibleInMenu(true);

    export_csv = new QAction(qApp->translate("lista_invio_mail","Esporta in csv"),this);
    export_csv->setIcon(icon);
    export_csv->setIconVisibleInMenu(true);
    /*
     * Imposto il menu
     */
    menu = new QMenu(this);
    menu->addAction(esporta_excel);
    menu->addSeparator();
    menu->addAction(export_csv);
    menu->addSeparator();
    menu->addAction(esporta_pdf);
    ui->esporta->setMenu(menu);
    
    //Esco dalla finestra delle email
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    ui->esci->setShortcut(tr("Ctrl+E"));
    
    //Stampo lista email
    connect(ui->stampa_pdf,SIGNAL(clicked()),this,SLOT(anteprima()));
    ui->stampa_pdf->setShortcut(tr("Ctrl+P"));
    
    connect(export_csv,SIGNAL(triggered()),this,SLOT(esporta_cvs()));
    connect(esporta_excel,SIGNAL(triggered()),this,SLOT(esporta_excel_email()));
    connect(esporta_pdf,SIGNAL(triggered()),this,SLOT(stampa_pdf()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(ui->cerca_txt,SIGNAL(textEdited(const QString &)),this,SLOT(cerca_utente(const QString &)));
    connect(ui->find_email,SIGNAL(clicked()),this,SLOT(filtro()));

    /**************************************************
     * Impostazione tabellare di dati sql
     ***************************************************/
    mod_grid = new QSqlRelationalTableModel;
    
    //Visualizzo lista
    lista();
}

void lista_invio_mail::lista()
{
    mod_grid->setTable("lista_invio_mail");
    mod_grid->select();
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("lista_invio_mail","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("lista_invio_mail","Nome file"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("lista_invio_mail","Fornitore-cliente"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("lista_invio_mail","Data"));
    
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(mod_grid);
    
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(4, new coldxdelegato(this));
}

void lista_invio_mail::cerca()
{
      	QSqlQuery query;
	QString id = mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),ui->tableView->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString();
        query.prepare("SELECT * FROM lista_invio_mail WHERE id = '"+id+"'");

    try{
        query.exec();
        if (query.next())
        {
            ui->editor->setText(qApp->translate("lista_invio_mail","ID: ")+query.value(0).toString()+"\n"+
                                  qApp->translate("lista_invio_mail","Nome file: ")+query.value(1).toString()+"\n"+
                                  qApp->translate("lista_invio_mail","Fornitore-cliente: ")+query.value(2).toString()+"\n"+
                                  qApp->translate("lista_invio_mail","Data spedizione: ")+query.value(3).toString()+"\n");
        }
        else
            LOG_FILE(qApp->translate("lista_invio_mail","Impossibile cercare(lista_invio_mail::cerca)"),LOG_ERROR_ALL,QIODevice::Append);
            throw QueryException(query.lastError().text().toStdString().c_str());
      }
      catch(FabariaException e){
            LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }

}

void lista_invio_mail::clickgrid()
{
    QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    cerca();
}

void lista_invio_mail::cerca_utente(const QString &)
{
    if(ui->combo_cerca->currentIndex() == 0){
    QString text;
    text = ui->cerca_txt->text();
    QStringList list_st;
    QSqlQuery querys("select nome_file from lista_invio_mail");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cerca_txt->setCompleter(complete);
    if(text.length() == 0){
        lista();
    }
    }
    else if(ui->combo_cerca->currentIndex() == 1){
        QString text1;
        text1 = ui->cerca_txt->text();
        QStringList list_st1;
        QSqlQuery querys1("select forn_cl from lista_invio_mail");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        ui->cerca_txt->setCompleter(complete1);
        if(text1.length() == 0){
            lista();
        }
    }
    else if(ui->combo_cerca->currentIndex() == 2)
    {
	QString text2;
        text2 = ui->cerca_txt->text();
        QStringList list_st2;
        QSqlQuery querys2("select data from lista_invio_mail");
        if(querys2.exec()){
        while(querys2.next()){
            list_st2 << querys2.value(0).toString().split(" ");
        }
        }
        QCompleter *complete1 = new QCompleter(list_st2,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        ui->cerca_txt->setCompleter(complete1);
        if(text2.length() == 0){
            lista();
        }
    }
}

void lista_invio_mail::filtro()
{
    if(ui->cerca_txt->text().length() == 0){
        lista();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText(qApp->translate("lista_invio_mail","Avviso"));
        MsgBox.setInformativeText(qApp->translate("lista_invio_mail","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(ui->combo_cerca->currentIndex() == 0){
    mod_grid->setFilter(QString("nome_file = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
    }
    else if(ui->combo_cerca->currentIndex() == 1){
    mod_grid->setFilter(QString("forn_cl = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
    }
    else if(ui->combo_cerca->currentIndex() == 2){
      mod_grid->setFilter(QString("data = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
    }
}

void lista_invio_mail::stampa_pdf()
{
  
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("lista_invio_mail","Esporta PDF"),
                                                    "*.pdf", qApp->translate("lista_invio_mail","Pdf Files(*.pdf);;Tutti i file(*.*)"));
  try{
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
       if(!fileName.isEmpty())
	  printer->setOutputFileName(fileName);
       else
	  throw InvalidFile("File non salvato",fileName);
       
       Stampe *ex_pdf = new Stampe();
       ex_pdf->stampa_lista_email(printer,mod_grid);
  }
  catch(FabariaException e)
  {
    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }
}

void lista_invio_mail::esporta_cvs()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("lista_invio_mail","Esporta CSV"),
                                                    "*.csv", qApp->translate("lista_invio_mail","CSV(*.csv);;Tutti i file(*.*)"));

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

    counthidden+=ui->tableView->isColumnHidden(jint);
    jint++;
    }


    int w = 1;
    while (x < mod_grid->columnCount()){

    if (!ui->tableView->isColumnHidden(x)) {


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
    if (!ui->tableView->isColumnHidden(x)) {


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

  try{
       QFile file;
     if (!fileName.isEmpty()) {
     file.setFileName(fileName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return;
    }
    else{
      throw InvalidFile("Impossibile salvare il file",fileName);
    }
    QByteArray ttext;
    ttext.append(exportdata);
    file.write(ttext);
  }
  catch(exception e)
  {
    throw InvalidFile("Impossibile salvare il file",fileName);
  }
}

void lista_invio_mail::esporta_excel_email()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("lista_invio_mail","Esporta Excel"),
                                                            "*.xlsx", qApp->translate("lista_invio_mail","XLSX(*.xlsx);;Tutti i file(*.*)"));


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
		QMessageBox::information(this,"FabariaGest",qApp->translate("lista_invio_mail","File salvato correttamente"));
            else
                throw InvalidFile(qApp->translate("lista_invio_mail","File non salvato").toStdString(),fileName);
	  }
	  catch(exception e)
	  {
	      throw InvalidFile(e.what(),fileName);
	  }
}

void lista_invio_mail::anteprima()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("lista_invio_mail","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview.setWindowIcon(icon);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_new(QPrinter*)));
    preview.exec();
}

void lista_invio_mail::preview_new(QPrinter *printer)
{
    Q_UNUSED(printer);
    Stampe *report = new Stampe();
    report->stampa_lista_email(printer,mod_grid);
}

lista_invio_mail::~lista_invio_mail()
{
    delete ui;
}
