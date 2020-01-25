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
/*
 * Libreria Utente
 */
#include "user.h"
#include "ui_user.h"
#include <QtGui>
#include <QMessageBox>
#include <QDebug>
#include <QtSql/QtSql>
#include <QPainter>
#include <QCompleter>
#include "itdelegato.h"
#include "print.h"
#include "pdf_export.h"
#include "defines.h"
#include "queryexception.h"

using namespace std;
using namespace Defines;
using namespace QXlsx;

user::user(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::user)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    
    //Elimino un cliente
    connect(ui->elimina,SIGNAL(clicked()),this,SLOT(del_rec()));
    ui->elimina->setShortcut(qApp->translate("user","Ctrl+D"));
    //Salvo e aggiono un cliente
    connect(ui->salva,SIGNAL(clicked()),this,SLOT(save()));
    ui->salva->setShortcut(qApp->translate("user","Ctrl+S"));
    connect(ui->g_lista, SIGNAL(clicked(QModelIndex)), this, SLOT(clickgrid()));
    //Creo un nuovo cliente
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(nuovo()));
    ui->pushButton->setShortcut(qApp->translate("user","Ctrl+N"));
    //Stampo la lista clienti
    connect(ui->preview,SIGNAL(clicked()),this,SLOT(anteprima()));
    ui->preview->setShortcut(qApp->translate("user","Ctrl+P"));
    //Cerco un cliente
    connect(ui->cerca_user,SIGNAL(clicked()),this,SLOT(filtro()));
    ui->cerca_user->setShortcut(qApp->translate("user","Ctrl+F"));
    connect(ui->cerca,SIGNAL(textEdited(QString)),this,SLOT(cerca_utente(QString)));
    connect(ui->cap->lineEdit(),&QLineEdit::textEdited,this,&user::inst_cap);
    connect(ui->comune->lineEdit(),&QLineEdit::textEdited,this,&user::inst_paese);
    connect(ui->radioButton,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_ced(bool)));
    connect(ui->radioButton_2,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_ced(bool)));
    connect(ui->cod_f,SIGNAL(textEdited(const QString &)),this,SLOT(leggi_codFis(const QString &)));
    connect(ui->part_iva,SIGNAL(textEdited(const QString &)),this,SLOT(leggi_codPartIva(const QString &)));
    
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
    esporta_pdf = new QAction(qApp->translate("user","Esporta in pdf"),this);
    esporta_pdf->setIcon(pdf);
    esporta_pdf->setIconVisibleInMenu(true);

    esporta_excel = new QAction(qApp->translate("user","Esporta in excel"),this);
    esporta_excel->setIcon(icon);
    esporta_excel->setIconVisibleInMenu(true);

    export_csv = new QAction(qApp->translate("user","Esporta in csv"),this);
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
    ui->esporta_all->setMenu(menu);
    
    //Connessione dei menu
    connect(esporta_pdf,SIGNAL(triggered()),this,SLOT(stampa_pdf()));
    connect(export_csv,SIGNAL(triggered()),this,SLOT(esporta_cvs()));
    connect(esporta_excel,SIGNAL(triggered()),this,SLOT(esporta_excel_clienti()));
    
    ui->id->setEnabled(false);
    ui->totale_list->setReadOnly(true);
    mod_grid = new QSqlRelationalTableModel;
    lista();
    somma();
    nuovo();
}

user::~user()
{
    delete ui;
}

void user::somma(){

    QString str;
    QSqlQuery query;
    query.exec("select count(id) from clienti");
    if(query.next()){
       str = query.value(0).toString();
       ui->totale_list->setText(QString::fromUtf8("N°  ")+str);
    }
}
void user::nuovo(){

    QSqlQuery query;
    query.exec("SELECT max(id+1) from clienti");

    if(query.next()){
        if(query.value(0).toInt() == 0){
              ui->id->setText("1");
        }
        else{
           ui->id->setText(query.value(0).toString());
        }
    }
    ui->nome->clear();
    ui->denom->clear();
    ui->cognome->clear();
    ui->indirizzo->clear();
    ui->telefono->clear();
    ui->email->clear();
    ui->cod_f->clear();
    ui->part_iva->clear();
    ui->fax->clear();
    somma();
}

void user::on_esci_clicked()
{
        close();
}

void user::clear_text()
{
    ui->id->clear();
    ui->nome->clear();
    ui->denom->clear();
    ui->cognome->clear();
    ui->indirizzo->clear();
    ui->cap->clear();
    ui->comune->clear();
    ui->prov->clear();
    ui->telefono->clear();
    ui->email->clear();
    ui->cod_f->clear();
    ui->part_iva->clear();
    ui->fax->clear();
}

void user::save(){

    if(ui->id->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("user","Inserisci correttamente i dati"));
    }
    else{
         QSqlQuery Qctrl;
         if(!ui->id->text().isEmpty()){
         Qctrl.prepare("SELECT * FROM clienti WHERE id = '"+ui->id->text()+"'");
         Qctrl.exec();

         if (Qctrl.next()) //Se esiste già
         {
             QSqlQuery Query;
             Query.prepare("UPDATE clienti SET nome='"+ui->nome->text()+"', cognome='"+ui->cognome->text()+"', "
			   "indirizzo='"+ui->indirizzo->text()+"',telefono='"+ui->telefono->text()+"', "
			   "email='"+ui->email->text()+"',cod_fisc='"+ui->cod_f->text()+"',part_iva='"+ui->part_iva->text()+"', "
			   "fax='"+ui->fax->text()+"',denominazione='"+ui->denom->text()+"', cap='"+ui->cap->currentText()+"', " 
			   "comune='"+ui->comune->currentText()+"', provincia='"+ui->prov->text()+"' WHERE id= '"+ui->id->text()+"'");
	    try{
             if (Query.exec())
             {
                clear_text();
                nuovo();
                 LOG_FILE(qApp->translate("user","Aggiornamento effettuato correttamente(user::save)"),LOG_INFO_ALL,QIODevice::Append);
              }
             else
             {	
		 LOG_FILE(qApp->translate("user","Impossibile aggiornare(user::save)"),LOG_ERROR_ALL,QIODevice::Append);
		 LOG_FILE("(user::save): "+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		 throw InvalidQuery(Query.lastError().text().toStdString().c_str());
             }
	    }
	    catch(FabariaException e)
	    {
	       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	    }
         }
         else //Se invece non esiste il record..
         {
             inserisci();
         }

    lista();
    somma();
    emit clienti_rel();
    }
    }
}

void user::inserisci(){

    QSqlQuery Query;
    Query.prepare("INSERT INTO clienti "
                  " VALUES ('"+ui->id->text()+"','"+ui->nome->text()+"','"+ui->cognome->text()+"','"+ui->denom->text()+"', "
		  "'"+ui->indirizzo->text()+"','"+ui->cap->currentText()+"','"+ui->comune->currentText()+"', '"+ui->prov->text()+"', "
		  "'"+ui->telefono->text()+"','"+ui->email->text()+"', "
		  "'"+ui->cod_f->text()+"','"+ui->part_iva->text()+"','"+ui->fax->text()+"')");

	try{
             if (Query.exec())
             {
		 clear_text();
                 LOG_FILE(qApp->translate("user","Inserimento effettuato correttamente(user::inserisci)"),LOG_INFO_ALL,QIODevice::Append);
              }
             else
             {	
		 LOG_FILE(qApp->translate("user","Impossibile inserire(user::inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
		 LOG_FILE("(user::inserisci): "+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		 throw InvalidQuery(Query.lastError().text().toStdString().c_str());
             }
	    }
	    catch(FabariaException e)
	    {
	       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	    }

    //Fine Tentativo di inserimento
    lista();
    somma();
    emit clienti_rel();
    nuovo();
}

void user::del_rec(){

    if(!ui->g_lista->selectionModel()->currentIndex().isValid()){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("user","Selezionare una riga da eliminare..."));
    }
     else if (!ui->id->text().isEmpty())
        {
            const char* error="";
            QSqlQuery qctrl,query;
            qctrl.prepare("SELECT * FROM clienti WHERE id = '"+ui->id->text()+"'");
            qctrl.exec();
            query.prepare("DELETE FROM clienti WHERE id = '"+ui->id->text()+"'");

            try{
		  if (query.exec() && qctrl.exec()) //Se esito OK Eliminato da DB
		  {
		      clear_text();
		      LOG_FILE(qApp->translate("user","Eliminazione avvenuta con successo..."),LOG_INFO_ALL,QIODevice::Append);
		  }
		  else
		  {
            LOG_FILE(qApp->translate("user","Impossibile eliminare cliente: ")+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(qApp->translate("user","Impossibile eliminare cliente: ")+qctrl.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw error ? InvalidQuery(qctrl.lastError().text().toStdString().c_str()) : InvalidQuery(query.lastError().text().toStdString().c_str());
		  }
		  }
		  catch(FabariaException e)
		  {
		    LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
		  }
        }

       lista();
       somma();
       emit clienti_rel();
       nuovo();
}
void user::lista(){

        mod_grid->setTable("clienti");
        mod_grid->select();
        mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("user","ID"));
        mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("user","Nome"));
        mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("user","Cognome."));
        mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("user","Denominazione."));
        mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("user","Indirizzo."));
        mod_grid->setHeaderData(5,Qt::Horizontal,qApp->translate("user","CAP."));
        mod_grid->setHeaderData(6,Qt::Horizontal,qApp->translate("user","Comune."));
        mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("user","PROV."));
        mod_grid->setHeaderData(8,Qt::Horizontal,qApp->translate("user","Telefono."));
        mod_grid->setHeaderData(9,Qt::Horizontal,qApp->translate("user","E-mail."));
        mod_grid->setHeaderData(10,Qt::Horizontal,qApp->translate("user","Codice fiscale"));
        mod_grid->setHeaderData(11,Qt::Horizontal,qApp->translate("user","Partita Iva"));
        mod_grid->setHeaderData(12,Qt::Horizontal,qApp->translate("user","Fax"));

        ui->g_lista->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->g_lista->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->g_lista->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->g_lista->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->g_lista->setSortingEnabled(true);
        ui->g_lista->verticalHeader()->hide();
        ui->g_lista->setModel(mod_grid);
        ui->g_lista->setAlternatingRowColors(true);

        ui->g_lista->setItemDelegateForColumn(12, new coldxdelegato(this));
}

void user::cerca(){

    QSqlQuery query;
        query.prepare("SELECT * FROM clienti WHERE id ='"+ui->id->text()+"'");
        query.exec();

        if (query.next())
        {
            ui->dettagli->setText(qApp->translate("user","ID: ")+query.value(0).toString()+"\n"+
                                  qApp->translate("user","Nome: ")+query.value(1).toString()+"\n"+
                                  qApp->translate("user","Cognome: ")+query.value(2).toString()+"\n"+
                                  qApp->translate("user","Indirizzo: ")+query.value(3).toString()+"\n"+
                                  qApp->translate("user","Telefono: ")+query.value(4).toString()+"\n"+
                                  qApp->translate("user","E-mail: ")+query.value(5).toString()+"\n"+
                                  qApp->translate("user","Codice fiscale: ")+query.value(6).toString()+"\n"+
                                  qApp->translate("user","Partita iva: ")+query.value(7).toString()+"\n"+
                                  qApp->translate("user","Fax: ")+query.value(8).toString()+"\n");
            ui->nome->setText(query.value(1).toString());
            ui->cognome->setText(query.value(2).toString());
            ui->indirizzo->setText(query.value(3).toString());
            ui->telefono->setText(query.value(4).toString());
            ui->email->setText(query.value(5).toString());
            ui->cod_f->setText(query.value(6).toString());
            ui->part_iva->setText(query.value(7).toString());
            ui->fax->setText(query.value(8).toString());
        }
        else
        {
	    clear_text();
        }

}

void user::clickgrid(){

        QString id_new;
        int riga = ui->g_lista->currentIndex().row();
        QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

        id_new = mod_grid->data(elemento).toString();
        ui->id->setText(id_new);

        cerca();
}

void user::cerca_utente(const QString &){

    if(ui->combo_pref->currentIndex() == 0){
    QString text;
    text = ui->cerca->text();
    QStringList list_st;
    QSqlQuery querys("select nome from clienti");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cerca->setCompleter(complete);
    if(text.length() == 0){
        lista();
    }
    }
    else if(ui->combo_pref->currentIndex() == 1){
        QString text1;
        text1 = ui->cerca->text();
        QStringList list_st1;
        QSqlQuery querys1("select cognome from clienti");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        ui->cerca->setCompleter(complete1);
        if(text1.length() == 0){
            lista();
        }
    }
    else if(ui->combo_pref->currentIndex() == 2){
        QString text1;
        text1 = ui->cerca->text();
        QStringList list_st1;
        QSqlQuery querys1("select cognome from denominazione");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        ui->cerca->setCompleter(complete1);
        if(text1.length() == 0){
            lista();
        }
    }
}

void user::filtro(){

    if(ui->cerca->text().length() == 0){
        lista();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText(qApp->translate("user","Avviso"));
        MsgBox.setInformativeText(qApp->translate("user","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(ui->combo_pref->currentIndex() == 0){
    mod_grid->setFilter(QString("nome = '" +ui->cerca->text()+"'").arg(ui->cerca->text()));
    }
    else if(ui->combo_pref->currentIndex() == 1){
    mod_grid->setFilter(QString("cognome = '" +ui->cerca->text()+"'").arg(ui->cerca->text()));
    }
    else if(ui->combo_pref->currentIndex() == 2){
    mod_grid->setFilter(QString("denominazione = '" +ui->cerca->text()+"'").arg(ui->cerca->text()));
    }
}

void user::anteprima(){
     //Aggiornamento query
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("user","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview.setWindowIcon(icon);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),this,SLOT(preview_new(QPrinter*)));
    preview.exec();
}

void user::preview_new(QPrinter *printer){

        Q_UNUSED(printer);
        Stampe *report = new Stampe();
        report->StampaVoci1(printer,mod_grid);
}

void user::stampa_pdf(){

    QString fileName = QFileDialog::getSaveFileName(this,qApp->translate("user","Esporta PDF"),
                                                    "*.pdf", qApp->translate("user","Pdf Files(*.pdf);;Tutti i file(*.*)"));

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
       pdf_export *ex_pdf = new pdf_export();
       ex_pdf->StampaVoci1(printer,mod_grid);
}

void user::esporta_cvs()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("user","Esporta CSV"),
                                                    "*.csv", qApp->translate("user","CSV(*.csv);;Tutti i file(*.*)"));

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

    counthidden+=ui->g_lista->isColumnHidden(jint);
    jint++;
    }


    int w = 1;
    while (x < mod_grid->columnCount()){

    if (!ui->g_lista->isColumnHidden(x)) {


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
    if (!ui->g_lista->isColumnHidden(x)) {


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

bool user::eventFilter(QObject *o, QEvent *e){

    //Add event to menu key
    if (o==ui->g_lista && e->type() == QEvent::KeyPress) {
                    QKeyEvent* ke = (QKeyEvent*)e;
                    if (ke->key() == Qt::Key_Delete) {
                        // ESEGUO IL CODICE DI ELIMINAZIONE
                        del_rec();
                    }
                    if(ke->key() == Qt::Key_S){
                        //ESEGUO IL CODICE PER INSERIRE
                        inserisci();
                    }
                    if(ke->key() == Qt::Key_U){
                        //ESEGUO IL CODICE PER AGGIORNARE
                        save();
                    }
                    if(ke->key() == Qt::Key_N){
                        //ESEGUO IL CODICE PER UN NUOVO ARTICOLO
                        nuovo();
                    }
                    if(ke->key() == Qt::Key_P){
                        //ESEGUO IL CODICE DI STAMPA
                        anteprima();
                    }
                    if(ke->key() == Qt::Key_C){
                        //ESEGUO IL CODICE DI ESPORTAZIONE PDF
                        stampa_pdf();
                    }
                    if(ke->key() == Qt::Key_S){
                        esporta_cvs();
                    }
                }

        if( o==ui->g_lista && e->type() == QEvent::ContextMenu)
                {
                    QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (e);
                    this ->Popup(mouseEvent->pos());
                    return false;
        }

        return QDialog::eventFilter(o,e);

}

void user::mousePressEvent(QMouseEvent *event){

    /******************************************
      * Imposta il tasto destro per il mouse
      * riproducendo un menu contestuale
      *****************************************/
    if (event->button() == Qt::RightButton) {

                    this->Popup(event ->pos());
                    event->accept();
                }
}

void  user::Popup(const QPoint &pt){

                /**************************************************
                  * Imposta la variabile pt per abilitare il
                  * menu.
                  * @param global = mapToGlobal(pt)
                  * @author Angelo Scarnà
                  *************************************************/
                QPoint global = ui->g_lista ->mapToGlobal(pt);
                QMenu* pPopup = new QMenu(this);

                /***********************************************************
                *  @author Angelo Scarnà
                *  Immagini per il menu a tendina
                ************************************************************/
                QIcon icon;
                icon.addFile(QString::fromUtf8(":/images/dialog-close.png"));
                QIcon icon1;
                icon1.addFile(QString::fromUtf8(":/images/edit-delete.png"));
                QIcon icon2;
                icon2.addFile(QString::fromUtf8(":/images/filenew.png"));
                QIcon icon3;
                icon3.addFile(QString::fromUtf8(":/images/document-save.png"));
                QIcon icon4;
                icon4.addFile(QString::fromUtf8(":/images/view-refresh.png"));
                QIcon icon5;
                icon5.addFile(QString::fromUtf8(":/images/document-print.png"));
                QIcon icon6;
                icon6.addFile(QString::fromUtf8(":/images/excel.png"));
                QIcon icon7;
                icon7.addFile(QString::fromUtf8(":/images/pdf.png"));


                /***********************************************************
                *  @author Angelo Scarnà
                *  Menu a tendina
                ************************************************************/
                QAction* pAction1 = new QAction(qApp->translate("user","Chiudi"), this);
                pAction1->setIcon(icon);
                pAction1->setIconVisibleInMenu(true);
                QAction* pAction2 = new QAction(qApp->translate("user","Elimina"), this);
                pAction2->setIcon(icon1);
                pAction2->setIconVisibleInMenu(true);
                QAction* nw = new QAction(qApp->translate("user","Nuovo"),this);
                nw->setIcon(icon2);
                nw->setIconVisibleInMenu(true);
                QAction* sa = new QAction(qApp->translate("user","Salva"),this);
                sa->setIcon(icon3);
                sa->setIconVisibleInMenu(true);
                QAction* md = new QAction(qApp->translate("user","Aggiorna"),this);
                md->setIcon(icon4);
                md->setIconVisibleInMenu(true);
                QAction* pr = new QAction(qApp->translate("user","Stampa"), this);
                pr->setIcon(icon5);
                pr->setIconVisibleInMenu(true);
                QAction *expdf = new QAction(qApp->translate("user","Esporta pdf"),this);
                expdf->setIcon(icon7);
                expdf->setIconVisibleInMenu(true);
                QAction *excsv = new QAction(qApp->translate("user","Esporta csv"),this);
                excsv->setIcon(icon6);
                excsv->setIconVisibleInMenu(true);

                /***********************************************************
                *  @author Angelo Scarnà
                *  Instanzio il menu
                ************************************************************/
                pPopup->addAction(nw);
                pPopup->addSeparator();
                pPopup->addAction(sa);
                pPopup->addSeparator();
                pPopup->addAction(md);
                pPopup->addSeparator();
                pPopup->addAction(pAction2);
                pPopup->addSeparator();
                pPopup->addAction(expdf);
                pPopup->addSeparator();
                pPopup->addAction(excsv);
                pPopup->addSeparator();
                pPopup->addAction(pr);
                pPopup->addSeparator();
                pPopup->addAction(pAction1);

                /***********************************************************
                *  @author Angelo Scarnà
                *  Imposto la connessione ai moduli
                ************************************************************/
                QAction* pItem = pPopup ->exec(global);

                if(pItem == pAction1)
                {
                    close();
                }
                else if(pItem == pAction2)
                {
                    del_rec();
                }
                else if(pItem == nw){
                    nuovo();
                }
                else if(pItem == sa)
                {
                    inserisci();
                }
                else if(pItem == md)
                {
                    save();
                }
                else if(pItem == pr)
                {
                    anteprima();
                }
                else if(pItem == expdf)
                {
                    stampa_pdf();
                }
                else if(pItem == excsv)
                {
                    esporta_cvs();
                }
}

void user::esporta_excel_clienti()
{
QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("user","Esporta Excel"),
                                                            "*.xlsx", qApp->translate("user","XLSX(*.xlsx);;Tutti i file(*.*)"));


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
		QMessageBox::information(this,"FabariaGest",qApp->translate("user","File salvato correttamente"));
            else
                throw InvalidFile(qApp->translate("user","File non salvato").toStdString(),fileName);
	  }
	  catch(exception e)
	  {
	      throw InvalidFile(e.what(),fileName);
	  }
}


void user::inst_cap()
{
   QSqlQuery query;
    query.prepare("select * from comuni where codice='"+ui->cap->currentText()+"'");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
            list << query.value(2).toString();
	    
            ui->prov->setText(query.value(1).toString());
	        ui->comune->setCurrentText(query.value(0).toString());
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cap->setCompleter(complete);
}


void user::inst_paese()
{
    QSqlQuery query;
    query.prepare("select * from comuni where comune='"+ui->comune->currentText()+"'");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
            list << query.value(1).toString();
	    
            ui->prov->setText(list.at(0));
	        ui->cap->setCurrentText(query.value(2).toString());
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comune->setCompleter(complete);
}

bool user::per_giud_fis_ced(bool vero)
{
    if(ui->radioButton->isChecked() == vero)
    {	
      ui->nome->setEnabled(false);
      ui->cognome->setEnabled(false);
      ui->denom->setEnabled(true);
      ui->part_iva->setEnabled(true);
      ui->cod_f->setEnabled(true);
    }
    else if(ui->radioButton_2->isChecked() == vero)
    {
      ui->denom->setEnabled(false);
      ui->nome->setEnabled(true);
      ui->cognome->setEnabled(true);
      message();
    }
    
    return vero;
}

void user::message()
{
	    QMessageBox *box= new QMessageBox(this);
            box->setWindowTitle("Fabaria Gest");
            box->setInformativeText(qApp->translate("user","Hai la partita iva?"));
            box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box->setDefaultButton(QMessageBox::Ok);
            int ret = box->exec();
            switch(ret){
            case QMessageBox::Ok:
                 //Abilito partita iva
		 ui->part_iva->setEnabled(true);
		 ui->cod_f->setEnabled(false);
                 box->close();
                 break;
            case QMessageBox::Cancel:
                 //Disabilito partita iva
                 ui->part_iva->setEnabled(false);
		 ui->cod_f->setEnabled(false);
                 box->close();
                 break;
            }
}

void user::leggi_codFis(const QString &)
{
  getVerificaCodiceFiscale(ui->cod_f->text(),ui->codfis_image);
}

void user::leggi_codPartIva(const QString &)
{
  getPartitaIva(ui->part_iva->text(),ui->codiva_image);
}
