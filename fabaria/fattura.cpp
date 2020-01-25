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
#include "fattura.h"
#include "ui_fattura.h"
#include "fattura_rg.h"
#include "print.h"
#include "defines.h"
#include "queryexception.h"
#include "pref.h"

#include <QtSql>
#include <QtGui>
#include <QtXml>

fattura *fatt_vendita =0;

using namespace Defines;

fattura::fattura(QWidget *parent) :
    QWidget(parent)
{    
    fatt_vendita = this;

    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    
    id->setVisible(false);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Immagini per il menu a tendina
    ************************************************************/
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/transform.png"));
    
    QIcon icon_email;
    icon_email.addFile(QString::fromUtf8(":/images/invio_mail.png"));
    /*
     * Menu bottone trasforma
     */
    esporta_fattura = new QAction(qApp->translate("fattura","Esporta fattura elettronica PA"),this);
    esporta_fattura->setIcon(icon);
    esporta_fattura->setIconVisibleInMenu(true);

    esporta_pdf = new QAction(qApp->translate("fattura","Esporta fattura singola in pdf"),this);
    esporta_pdf->setIcon(icon);
    esporta_pdf->setIconVisibleInMenu(true);
    
    send_mail = new QAction(qApp->translate("fattura","Invia fattura tramite email"),this);
    send_mail->setIcon(icon_email);
    send_mail->setIconVisibleInMenu(true); 
    /*
     * Imposto il menu
     */
    menu = new QMenu(this);
    menu->addAction(esporta_fattura);
    menu->addSeparator();
    menu->addAction(esporta_pdf);
    menu->addSeparator();
    menu->addAction(send_mail);
    pushButton->setMenu(menu);

    
    /*********************************************
     * Connessioni ai diversi metodi
     *********************************************/
    //Esci dalla finestra di dialogo
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    esci->setShortcut(qApp->translate("fattura","Ctrl+E"));
    
    //Stampa la fattura
    connect(stampa,SIGNAL(clicked()),this,SLOT(stampa_fat()));
    stampa->setShortcut(qApp->translate("fattura","Ctrl+P"));
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid_prodotti()));
    connect(tab_view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(aggiorna_fattura_prodotti()));
    tab_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tab_view,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(Popup(const QPoint &)));
    
    //Cerca all'interno della tabella fattura
    connect(find_cer,SIGNAL(clicked()),this,SLOT(filtro()));
    find_cer->setShortcut(qApp->translate("fattura","Ctrl+F"));
    
    //Crea nuova fattura
    connect(Aggiungi,SIGNAL(clicked()),this,SLOT(nuova_fattura_vendita_prodotti()));
    Aggiungi->setShortcut(qApp->translate("fattura","Ctrl+N"));
    
    //Modifica la fattura
    connect(modifica,SIGNAL(clicked()),this,SLOT(aggiorna_fattura_prodotti()));
    modifica->setShortcut(qApp->translate("fattura","Ctrl+M"));
    
    //Stampa lista fatture
    connect(p_fatt_s,SIGNAL(clicked()),this,SLOT(stampa_prodotti()));
    p_fatt_s->setShortcut(qApp->translate("fattura","Ctrl+L"));
    
    //Trasforma una fattura
    connect(trasform,SIGNAL(clicked()),this,SLOT(trasforma()));
    trasform->setShortcut(qApp->translate("fattura","Ctrl+T"));
    
    //Elimina una fattura
    connect(elimina,SIGNAL(clicked()),this,SLOT(elimina_fattura()));
    elimina->setShortcut(qApp->translate("fattura","Ctrl+D"));
    
    
    /*
     * Imposto i paramentri della connessione dei pulsanti
     */
    //Esporta fattura elettronica
    connect(esporta_fattura,SIGNAL(triggered()),this,SLOT(esporta_fatt_elett_forn()));
    esporta_fattura->setShortcut(qApp->translate("fattura","Ctrl+Shift+F"));
    
    //Esporta la fattura in pdf
    connect(esporta_pdf,SIGNAL(triggered()),this,SLOT(esporta_pdf_prodotti()));
    esporta_pdf->setShortcut(qApp->translate("fattura","Ctrl+Shift+P"));
    
    //Invio la fattura tramite email
    connect(send_mail,SIGNAL(triggered()),this,SLOT(invia_email()));
    send_mail->setShortcut(qApp->translate("fattura","Ctrl+Shift+M"));

    /**************************************************
     * Impostazione tabellare di dati sql
     ***************************************************/
    mod_grid = new QSqlRelationalTableModel(this);
    model = new QSortFilterProxyModel;
    textEdit->setReadOnly(true);

    /************************************************
     * Impostazione combBox cerca_txt
     ************************************************/
    QStringList list1;
    list1 << qApp->translate("fattura","Cerca per ID") << qApp->translate("fattura","Cerca per fornitore o cliente") << qApp->translate("fattura","Cerca per tipo fattura");
    cerca_txt->addItems(list1);
    lista_prodotti();
    vis_dati_fattura_prodotti();
}



void fattura::cerca_dxt_prodotti(const QString &){

    if(cerca_txt->currentIndex() == 0){
    QString text1;
    text1 = cerca1->text();
    QStringList list_st;
    QSqlQuery querys("select id from fattura_vendita");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    cerca1->setCompleter(complete);
    if(text1.length() == 0){
        lista_prodotti();
    }
    }
    else if(cerca_txt->currentIndex() == 1){
        QString text2;
        text2 = cerca1->text();
        QStringList list_st1;
        QSqlQuery querys1("select fornitore from fattura_vendita");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        cerca1->setCompleter(complete1);
        if(text2.length() == 0){
            lista_prodotti();
        }
    }
    else if(cerca_txt->currentIndex() == 2){
        QString text3;
        text3 = cerca1->text();
        QStringList list_st2;
        QSqlQuery querys2("select tipo_fattura from fattura_vendita");
        if(querys2.exec()){
        while(querys2.next()){
            list_st2 << querys2.value(0).toString();
        }
        }
        QCompleter *complete2 = new QCompleter(list_st2,this);
        complete2->setCaseSensitivity(Qt::CaseInsensitive);
        cerca1->setCompleter(complete2);
        if(text3.length() == 0){
            lista_prodotti();
        }
    }
}

void fattura::filtro(){

    if(cerca1->text().length() == 0){
        lista_prodotti();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText("Avviso");
        MsgBox.setInformativeText(qApp->translate("fattura","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(cerca_txt->currentIndex() == 0){
        mod_grid->setFilter(QString("id = '" +cerca1->text()+"'").arg(cerca1->text()));
    }
    else if(cerca_txt->currentIndex() == 1){
        mod_grid->setFilter(QString("fornitore = '" +cerca1->text()+"'").arg(cerca1->text()));
    }
    else if(cerca_txt->currentIndex() == 2){
        mod_grid->setFilter(QString("tipo_fattura = '" +cerca1->text()+"'").arg(cerca1->text()));
    }
}

void fattura::stampa_fat(){

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog ft(&printer,this);
    ft.setWindowFlags(Qt::Window);
    ft.setWindowTitle(qApp->translate("fattura","Anteprima di stampa."));
    connect(&ft,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_view(QPrinter*)));
    ft.exec();
}

void fattura::preview_view(QPrinter *printer){

    Q_UNUSED(printer);
    Stampe *ft1 = new Stampe();
    ft1->stampa_fattura(printer,mod_grid);
}

void fattura::clickgrid_prodotti(){
    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    id->setText(id_new);
    cerca_prodotti();
}

void fattura::nuova_fattura_vendita_prodotti(){

    dg = new dialog_fattura(this);
    dg->exec();
}

void fattura::aggiorna_fattura_prodotti(){
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QModelIndex modelIndex = tab_view->selectionModel()->currentIndex();
    int riga = modelIndex.row();
    QModelIndex newindex = modelIndex.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    agg_fatt_prodotti(newindex);
    }
    else
    {
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Selezionare una riga da modificare..."));
    }
}

void fattura::agg_fatt_prodotti(QModelIndex index){
    if(index.row() < mod_grid->rowCount()){
            QSqlQuery query;
            QSqlRecord record = mod_grid->record(index.row());
            if(!record.isEmpty()){
                if(record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura immediata") || record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura proforma") || record.value("tipo_fattura").toString() == qApp->translate("fattura","Nota di credito") || record.value("tipo_fattura").toString() == qApp->translate("fattura","Scontrino")){
            fattura_rg *ft = new fattura_rg(this,record.value("tipo_fattura").toString(),id->text().toInt());
            ft->disable_fatt_acc();
            query.prepare("select * from fatture_vendita_righe where id='"+id->text()+"'");
            query.exec();
            ft->setModal(true);
            QSqlRecord rec = mod_grid->record(index.row());
            ft->id1->setText(rec.value("id").toString());
            if(query.next()){
            ft->data_fatt->setDate(rec.value("data").toDate());
            if(query.value(3).toInt() == 0){
            ft->tipo_cliente->setCurrentIndex(query.value(3).toInt());

            ft->pers->setCurrentText(query.value(2).toString());
            }
            else if(query.value(3).toInt() == 1){
                ft->tipo_cliente->setCurrentIndex(query.value(3).toInt());
                ft->pers->setCurrentText(query.value(2).toString());
            }
            ft->sconto1->setText(query.value(4).toString());
            ft->sconto2->setText(query.value(5).toString());
            ft->sconto3->setText(query.value(6).toString());
            ft->spes_tr->setText(query.value(7).toString());
            ft->spes_in->setText(query.value(8).toString());
            ft->ann->setText(query.value(9).toString());
            ft->ordine->setText(query.value(10).toString());
            ft->pagam->setCurrentText(query.value(11).toString());
            ft->lineEdit->setText(query.value(12).toString());
            ft->rag_soc->setText(query.value(13).toString());
            ft->indirizzo->setText(query.value(14).toString());
            ft->cap->setText(query.value(15).toString());
            ft->local->setText(query.value(16).toString());
            ft->prov->setText(query.value(17).toString());
            ft->tel->setText(query.value(18).toString());
            ft->cell->setText(query.value(19).toString());
            ft->paese->setText(query.value(20).toString());
            ft->vis_banca->setCurrentText(query.value(21).toString());
            ft->iban->setText(query.value(22).toString());
            ft->lineEdit_2->setCurrentText(query.value(23).toString());
            ft->lineEdit_3->setText(query.value(24).toString());
	    ft->tipo_listino->setCurrentText(query.value(44).toString());
            }
            ft->formatta_num();
            ft->totale_impo_iva();
            ft->lista_prodotti();
	    ft->vis_allegati();
            ft->exec();
        }
        else if(record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura accompagnatoria")){
        fattura_rg *ft = new fattura_rg(this,record.value("tipo_fattura").toString(),id->text().toInt());
        ft->enable_fatt_acc();
        query.prepare("select * from fatture_vendita_righe where id='"+id->text()+"'");
        query.exec();
        ft->setModal(true);
        QSqlRecord rec = mod_grid->record(index.row());
        ft->id1->setText(rec.value("id").toString());
        if(query.next()){
        ft->data_fatt->setDate(rec.value("data").toDate());
        if(query.value(3).toInt() == 0){
        ft->tipo_cliente->setCurrentIndex(query.value(3).toInt());

        ft->pers->setCurrentText(query.value(2).toString());
        }
        else if(query.value(3).toInt() == 1){
            ft->tipo_cliente->setCurrentIndex(query.value(3).toInt());
            ft->pers->setCurrentText(query.value(2).toString());
        }
        ft->sconto1->setText(query.value(4).toString());
        ft->sconto2->setText(query.value(5).toString());
        ft->sconto3->setText(query.value(6).toString());
        ft->spes_tr->setText(query.value(7).toString());
        ft->spes_in->setText(query.value(8).toString());
        ft->ann->setText(query.value(9).toString());
        ft->ordine->setText(query.value(10).toString());
        ft->pagam->setCurrentText(query.value(11).toString());
        ft->lineEdit->setText(query.value(12).toString());
        ft->rag_soc->setText(query.value(13).toString());
        ft->indirizzo->setText(query.value(14).toString());
        ft->cap->setText(query.value(15).toString());
        ft->local->setText(query.value(16).toString());
        ft->prov->setText(query.value(17).toString());
        ft->tel->setText(query.value(18).toString());
        ft->cell->setText(query.value(19).toString());
        ft->paese->setText(query.value(20).toString());
        ft->vis_banca->setCurrentText(query.value(21).toString());
        ft->iban->setText(query.value(22).toString());
        ft->lineEdit_2->setCurrentText(query.value(23).toString());
        ft->lineEdit_3->setText(query.value(24).toString());
        ft->causali->setCurrentText(query.value(34).toString());
        ft->aspetto_est->setCurrentText(query.value(35).toString());
        ft->vettore->setCurrentText(query.value(36).toString());
        ft->consegna->setCurrentText(query.value(37).toString());
        ft->porto_ass->setCurrentText(query.value(38).toString());
        ft->num_colli->setText(query.value(39).toString());
        ft->peso_lordo->setText(query.value(40).toString());
        ft->data_ora->setDateTime(query.value(41).toDateTime());
        ft->peso_netto->setText(query.value(42).toString());
	ft->tipo_listino->setCurrentText(query.value(44).toString());
        }
        ft->formatta_num();
        ft->totale_impo_iva();
        ft->lista_prodotti();
	ft->vis_allegati();
        ft->exec();
    }
    }
    }
        vis_dati_fattura_prodotti();

        lista_prodotti();


}

void fattura::vis_dati_fattura_prodotti(){
    QSqlQuery query,query1;
    query.prepare("select count(id) from fattura_vendita where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        QString text = QString::fromUtf8(qApp->translate("fattura","Le fatture di vendita sono: ").toStdString().c_str());
	QString txt = query.value(0).toString();
	QString testo = text+txt;
        tot_fatt->setText(testo);
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Impossibile ricavare le fatture di vendita..."));
    }

    query1.prepare("select sum(totale) from fattura_vendita where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query1.exec();
    if(query1.next()){
        QString txt = QString::fromUtf8(qApp->translate("fattura","Il totale delle fatture di vendita è: ").toStdString().c_str());
	QString euro = QString::fromUtf8("\u20AC");
	QString text =query1.value(0).toString().toStdString().c_str();
	QString testo = txt+euro+" "+text;
        tot_eur->setText(testo);
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Impossibile ricavare le fatture di vendita..."));
    }
}

void fattura::elimina_fattura(){

            if (!id->text().isEmpty())
            {
                // Si controlla che il cd_voce non sia usato in Spese o Budget

                QSqlQuery qctrl;
                qctrl.prepare("SELECT * FROM fattura_vendita WHERE id = '"+id->text()+"' LIMIT 1");
                qctrl.exec();

                        QMessageBox *box= new QMessageBox(this);
                        box->setWindowTitle("FabariaGest");
                        box->setInformativeText(qApp->translate("fattura","Vuoi eliminare veramente \n il record selezionato?...."));
                        box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                        box->setDefaultButton(QMessageBox::Ok);
                        int ret = box->exec();
                        switch(ret){
                        case QMessageBox::Ok:
                             //Close
                             elimina_riga();
                             box->close();
                             break;
                        case QMessageBox::Cancel:
                            //return
                            box->close();
                            break;
                        }
            }
}

void fattura::elimina_riga(){

    // Se si passano i controlli di cui sopra si procede all'eliminazione
    QSqlQuery query,query1,query2;
    const char* error="";
    query.prepare("DELETE FROM fattura_vendita WHERE id = '"+id->text()+"' LIMIT 1");
    query1.prepare("DELETE FROM fatture_righe_vendita_art WHERE id = '"+id->text()+"'");
    query2.prepare("DELETE FROM fatture_vendita_righe where id='"+id->text()+"'");

    try{
    if (query.exec() & query1.exec() & query2.exec()) //Se esito OK Eliminato da DB
    {
        LOG_FILE(qApp->translate("fattura","Eliminazione avvenuta con successo(fattura::elimina_riga)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
	LOG_FILE(qApp->translate("fattura","Impossibile eliminare(fattura::elimina_riga)"),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(fattura::elimina_riga): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(fattura::elimina_riga): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(fattura::elimina_riga): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw error  ? InvalidQuery(query1.lastError().text().toStdString().c_str()) : InvalidQuery(query2.lastError().text().toStdString().c_str());
	throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    lista_prodotti();
    vis_dati_fattura_prodotti();
    textEdit->setText("");
}

void fattura::lista_prodotti(){
    mod_grid->setTable("fattura_vendita");
    mod_grid->setFilter("anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    model->setSourceModel(mod_grid);
    model->sort(0,Qt::AscendingOrder);

    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("fattura","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("fattura","Data"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("fattura","Fornitore"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("fattura","Tipo fattura"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("fattura","Totale"));

    tab_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    tab_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab_view->setSelectionMode(QAbstractItemView::SingleSelection);
    tab_view->setSortingEnabled(true);
    tab_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab_view->verticalHeader()->hide();
    tab_view->setModel(model);
    mod_grid->select();
    tab_view->setColumnHidden(5,true);
    tab_view->setColumnHidden(6,true);
    vis_dati_fattura_prodotti();
}

void fattura::cerca_prodotti(){
    QSqlQuery query;
    query.prepare("select * from fattura_vendita where id='"+id->text()+"'");
    query.exec();
    if(query.next()){
        textEdit->setText(QString::fromUtf8(qApp->translate("fattura","Informazioni della fattura di vendita: ").toStdString().c_str())+"\n"
                          +qApp->translate("fattura","ID:")+query.value(0).toString()+"\n"
                          +qApp->translate("fattura","Data: ")+query.value(1).toString()+"\n"
                          +qApp->translate("fattura","Fornitore: ")+query.value(2).toString()+"\n"
                          +qApp->translate("fattura","Tipo fattura: ")+query.value(3).toString()+"\n"
                          +qApp->translate("fattura","Totale: ")+QString::fromUtf8("\u20AC")+" "+query.value(4).toString()+"\n");
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Impossibile trovare le informazioni..."));
    }
}

void fattura::stampa_prodotti()
{
        if(tab_view->selectionModel()->currentIndex().isValid()){
        QPrinter printer(QPrinter::HighResolution);
        QPrintPreviewDialog preview(&printer);
        preview.setWindowFlags(Qt::Window);
        preview.setWindowTitle(qApp->translate("fattura","Anteprima di stampa fattura di vendita"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
        preview.setWindowIcon(icon);
        connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_prodotti(QPrinter*)));
        preview.exec();
        }
        else{
            QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Selezionare una fattura da stampare..."));
        }
}

void fattura::preview_prodotti(QPrinter *printer)
{
    Q_UNUSED(printer);
    QModelIndex in = tab_view->selectionModel()->currentIndex();
    int riga = in.row();
    QModelIndex newindex = in.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    tab_view->selectRow(riga);
    Stampe *pr = new Stampe();
    pr->stampa_fatt_vendita_libri(printer,mod_grid,newindex,id->text().toInt());
}

void fattura::esporta_pdf_prodotti()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("fattura","Esporta PDF"),
                                                    "*.pdf", qApp->translate("fattura","Pdf Files(*.pdf);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".pdf") < 0) {
                fileName += ".pdf";
            }
        }
       QPrinter *printer = new QPrinter(QPrinter::HighResolution);
       printer->setPageSize(QPrinter::A4);
       printer->setOrientation(QPrinter::Portrait);
       Q_UNUSED(printer);
       printer->setOutputFileName(fileName);
       Stampe *st = new Stampe();
       st->stampa_fatt_vendita_libri(printer,mod_grid,tab_view->currentIndex(),id->text().toDouble());
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Selezionare una riga da esportare"));
    }

}

void fattura::invia_email()
{
  pref *imp = new pref(this);
  if(settingsManager->generalValue("EmailServer/server_smtp",QVariant()).toString() != ""){
    if(tab_view->selectionModel()->currentIndex().isValid()){
      QModelIndex index = tab_view->selectionModel()->currentIndex();
    if(index.row() < mod_grid->rowCount()){
    QSqlRecord record = mod_grid->record(index.row());
    if(!record.isEmpty()){
      if(record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura immediata") || 
	  record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura accompagnatoria") || 
	  record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura proforma") ||
	  record.value("tipo_fattura").toString() == qApp->translate("fattura","Nota di credito")){
#if defined(Q_OS_WIN)
       QString fileName = settingsManager->generalValue("Cartella/destinazione",QVariant()).toString()+"\\"+record.value("tipo_fattura").toString()+"_"+record.value("id").toString()+"_"+record.value("data").toDate().toString("dd-MM-yyyy")+".pdf";
#else
       QString fileName = settingsManager->generalValue("Cartella/destinazione",QVariant()).toString()+"/"+record.value("tipo_fattura").toString()+"_"+record.value("id").toString()+"_"+record.value("data").toDate().toString("dd-MM-yyyy")+".pdf";
#endif
       QPrinter *printer = new QPrinter(QPrinter::HighResolution);
       printer->setPageSize(QPrinter::A4);
       printer->setOrientation(QPrinter::Portrait);
       printer->setOutputFileName(fileName);
       Stampe *st = new Stampe();
       st->stampa_fatt_vendita_libri(printer,mod_grid,tab_view->currentIndex(),record.value("id").toString().toDouble());
       
       email = new email_send(this,record.value("id").toString().toDouble(),record.value("fornitore").toString(),record.value("data").toDate().toString("dd-MM-yyyy"),record.value("tipo_fattura").toString());
       
       QString nome_file = record.value("tipo_fattura").toString()+QString::fromUtf8(" N° ")+record.value("id").toString()+" del "+record.value("data").toDate().toString("dd-MM-yyyy");
       email->oggetto->setText(nome_file);
       email->a_email->setText(settingsManager->generalValue("EmailServer/mail", QVariant()).toString());
       //Visualizzo allegati
       QSqlQuery query_all("select * from allegati_email where id='"+record.value("id").toString()+"'");
       query_all.exec();
       if(query_all.next())
       {
	   update_all_email(record.value("id").toString().toDouble(),fileName);
	   if(settingsManager->generalValue("EmailServer/vis_allegati_fattura",QVariant()).toBool() == true)
	   {
	      email->vis_allegati();
	   }
	   email->vis_allegati_email();
	   email->show();
       }
       else{
	 insert_all_email(record.value("id").toString().toDouble(),fileName);
	 if(settingsManager->generalValue("EmailServer/vis_allegati_fattura",QVariant()).toBool() == true)
	 {
	      email->vis_allegati();
	 }
	 email->vis_allegati_email();
	 email->show();
       }
    }
    else{
      QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Non puoi inviare uno scontrino tramite email.\nNon e' previsto dalla normativa vigente..."));
    }
    }
    }
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Selezionare una riga da inviare tramite email"));
    }
  }
  else{
    QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Imposta i paramentri di configurazione per l'invio delle email"));
    imp->tabWidget->setCurrentIndex(2);
    imp->exec();
  }
}

void fattura::insert_all_email(double id, QString fileName)
{
      QSqlQuery query;
       
       query.prepare("INSERT INTO allegati_email "
		     "VALUES('"+QString::number(id)+"','"+fileName+"','"+QString(fileName.toLatin1().toBase64())+"')");

      try{
       if(query.exec())
       {	 
	  LOG_FILE(qApp->translate("fattura","Inserimento effettuato con successo(fattura::insert_all_email)"),LOG_INFO_ALL,QIODevice::Append);
       }
       else{
	  LOG_FILE(qApp->translate("fattura","Impossibile inserire(fattura::insert_all_email)"),LOG_ERROR_ALL,QIODevice::Append);
	  LOG_FILE("(fattura::insert_all_email): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	  throw InvalidQuery(query.lastError().text().toStdString().c_str());
       }
      }
      catch(FabariaException e)
      {
	 LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
}

void fattura::update_all_email(double id, QString fileName)
{
      QSqlQuery query;
      query.prepare("UPDATE allegati_email SET nome_file='"+fileName+"',"
		     " chiave_univoca='"+QString(fileName.toLatin1().toBase64())+"' WHERE id='"+QString::number(id)+"'");
	  
      try{
       if(query.exec())
       {	 
	  LOG_FILE(qApp->translate("fattura","Aggiornamento effettuato con successo(fattura::update_all_email)"),LOG_INFO_ALL,QIODevice::Append);
       }
       else{
	  LOG_FILE(qApp->translate("fattura","Impossibile aggiornare(fattura::update_all_email)"),LOG_ERROR_ALL,QIODevice::Append);
	  LOG_FILE("(fattura::update_all_email): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	  throw InvalidQuery(query.lastError().text().toStdString().c_str());
       }
      }
      catch(FabariaException e)
      {
	 LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
}

void fattura::trasforma()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QModelIndex index = tab_view->selectionModel()->currentIndex();
    if(index.row() << mod_grid->rowCount()){
    QSqlRecord record = mod_grid->record(index.row());
    if(!record.isEmpty()){
        if(record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura proforma")){
            QSqlQuery query1;
            query1.prepare("UPDATE fattura_vendita SET tipo_fattura=:tipo_fattura WHERE id=:id");
            query1.bindValue(":id",mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),tab_view->selectionModel()->currentIndex().column()),Qt::DisplayRole).toString());
            query1.bindValue(":tipo_fattura",qApp->translate("fattura","Fattura immediata"));

            if(query1.exec()){
                QMessageBox::information(this,"FabariaGest",qApp->translate("fattura","Aggiornamento effettuato correttamente..."));
            }
            else{
                QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Impossibile inserire ")+query1.lastError().text());
            }
        }
        else if(record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura immediata") || record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura accompagnatoria") || record.value("tipo_fattura").toString() == qApp->translate("fattura","Nota di credito") || record.value("tipo_fattura").toString() == qApp->translate("fattura","Scontrino")){
            QMessageBox::information(this,"FabariaGest",qApp->translate("fattura","Impossibile cambiare il tipo di fattura..."));
        }
    }
    }
    }
    else{
       QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Selezionare una riga da trasformare"));
    }
    lista_prodotti();
}

void fattura::esporta_fatt_elett_forn()
{
   if(tab_view->selectionModel()->currentIndex().isValid()){
     QModelIndex modelIndex = tab_view->selectionModel()->currentIndex();
     int riga = modelIndex.row();
     QModelIndex newindex = modelIndex.sibling(riga,0);
     tab_view->setCurrentIndex(newindex);
     esporta_fatt_elettronica_fornitori(newindex);
   }
    else{
       QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura","Selezionare una riga da\nesportare in fattura elettronica"));
    }
}

void fattura::esporta_fatt_elettronica_fornitori(QModelIndex index)
{
    if(index.row() < mod_grid->rowCount()){
    QSqlRecord record = mod_grid->record(index.row());
    if(!record.isEmpty()){
    if(record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura immediata") || 
      record.value("tipo_fattura").toString() == qApp->translate("fattura","Fattura accompagnatoria") )
    {
    fatt_elet = new fattura_elettronica(this,id->text().toInt(),mod_grid,index);

    QSqlQuery query("select max(id)+1 from fatt_elettronica");
    QSqlQuery query_forn("select * from azienda");
    query.exec();
    query_forn.exec();
    QSqlRecord rec = mod_grid->record(index.row());
    if(query.next() && query_forn.next()){
        if(query.value(0).toInt() == 0){
            fatt_elet->id_codice->setText(query_forn.value(2).toString());
            fatt_elet->tel_trasm->setText(query_forn.value(8).toString());
            fatt_elet->email_trasm->setText(query_forn.value(11).toString());
            fatt_elet->progree_invio->setText("1");
	    fatt_elet->lineEdit->setText(rec.value("id").toString());
	    fatt_elet->data_doc->setDate(rec.value("data").toDate());
	    fatt_elet->importo->setText(QString::number(rec.value("totale").toDouble()));
            fatt_elet->show();
        }
        else{
            fatt_elet->id_codice->setText(query_forn.value(2).toString());
            fatt_elet->tel_trasm->setText(query_forn.value(8).toString());
            fatt_elet->email_trasm->setText(query_forn.value(11).toString());
            fatt_elet->progree_invio->setText(query.value(0).toString());
	    fatt_elet->lineEdit->setText(rec.value("id").toString());
	    fatt_elet->data_doc->setDate(rec.value("data").toDate());
	    fatt_elet->importo->setText(QString::number(rec.value("totale").toDouble()));
            fatt_elet->show();
        }
    }
    }
    }
    }
}

void fattura::Popup(const QPoint &pt){
    /**************************************************
      * Imposta la variabile pt per abilitare il
      * menu.
      * @param global = mapToGlobal(pt)
      * @author Angelo Scarnà
      *************************************************/
    QModelIndex global = tab_view->indexAt(pt);
    QMenu* pPopup = new QMenu(this);
    if(global.isValid()){
    /***********************************************************
    *  @author Angelo Scarnà
    *  Immagini per il menu a tendina
    ************************************************************/
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/split.png"));

    QIcon icona;
    icona.addFile(QString::fromUtf8(":/images/fattura.png"));

    QIcon icon_mail;
    icon_mail.addFile(QString::fromUtf8(":/images/invio_mail.png"));

    /***********************************************************
    *  @author Angelo Scarnà
    *  Menu a tendina
    ************************************************************/
    QAction* pAction1 = new QAction(qApp->translate("fattura","Trasforma fattura proforma in fattura"), this);
    pAction1->setIcon(icon);
    pAction1->setIconVisibleInMenu(true);
    QAction* pAction2 = new QAction(qApp->translate("fattura","Esporta in fattura elettronica"),this);
    pAction2->setIcon(icona);
    pAction2->setIconVisibleInMenu(true);
    QAction* pAction3 = new QAction(qApp->translate("fattura","Invia tramite email"),this);
    pAction3->setIcon(icon_mail);
    pAction3->setIconVisibleInMenu(true);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Instanzio il menu
    ************************************************************/
    pPopup->addAction(pAction1);
    pPopup->addAction(pAction2);
    pPopup->addAction(pAction3);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Imposto la connessione ai moduli
    ************************************************************/
    QAction* pItem = pPopup ->exec(tab_view->mapToGlobal(pt));

    if(pItem == pAction1)
    {
        trasforma();
    }
    else if(pItem == pAction2)
    {
        esporta_fatt_elett_forn();
    }
    else if(pItem == pAction3)
    {
	invia_email();
    }
    }
}

fattura::~fattura()
{
}
