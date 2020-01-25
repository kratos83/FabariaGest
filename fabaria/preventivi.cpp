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
#include "preventivi.h"
#include "ui_preventivi.h"
#include "print.h"
#include "settingsmanager.h"
#include "defines.h"
#include "queryexception.h"

#include <QtSql>
#include <QtGui>
#include <QtXml>

preventivi *prev_wid = 0;

using namespace Defines;

preventivi::preventivi(QWidget *parent) :
    QWidget(parent)
{
    prev_wid = this;

    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    id_2->setVisible(false);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Immagini per il menu a tendina
    ************************************************************/
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/split.png"));
    
    QIcon icon_email;
    icon_email.addFile(QString::fromUtf8(":/images/invio_mail.png"));
    
    /*
     * Menu bottone trasforma
     */    
    send_mail = new QAction(qApp->translate("preventivi","Invia preventivo tramite email"),this);
    send_mail->setIcon(icon_email);
    send_mail->setIconVisibleInMenu(true);

    /*
     * Imposto il menu
     */
    menu = new QMenu(this);
    menu->addAction(send_mail);
    trasform_2->setMenu(menu);

    //Esco dalla finestra di dialogo
    connect(esci_2,SIGNAL(clicked()),this,SLOT(close()));
    esci_2->setShortcut(qApp->translate("preventivi","Ctrl+E"));
    
    //Stampa lista preventivi
    connect(stampa_2,SIGNAL(clicked()),this,SLOT(stampa_prev()));
    stampa_2->setShortcut(qApp->translate("preventivi","Ctrl+P"));
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid_prodotti()));
    connect(tab_view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(aggiorna_prev_prodotti()));
    tab_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tab_view,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(Popup(const QPoint &)));
    
    //Cerca un preventivo
    connect(find_cer_2,SIGNAL(clicked()),this,SLOT(filtro()));
    find_cer_2->setShortcut(qApp->translate("preventivi","Ctrl+F"));
    
    //Aggiungo un nuovo ordine
    connect(Aggiungi_2,SIGNAL(clicked()),this,SLOT(nuovo_prev_prodotti()));
    Aggiungi_2->setShortcut(qApp->translate("preventivi","Ctrl+N"));
    
    //Modifico un ordine
    connect(modifica_2,SIGNAL(clicked()),this,SLOT(aggiorna_prev_prodotti()));
    modifica_2->setShortcut(qApp->translate("preventivi","Ctrl+M"));
    
    //Stampa un ordine
    connect(p_fatt_s_2,SIGNAL(clicked()),this,SLOT(stampa_prodotti()));
    p_fatt_s_2->setShortcut(qApp->translate("preventivi","Ctrl+P"));
    
    //Elimino un ordine
    connect(elimina_2,SIGNAL(clicked()),this,SLOT(elimina_prev()));
    elimina_2->setShortcut(qApp->translate("preventivi","Ctrl+D"));
    
    //Esporto in pdf un ordine
    connect(pushButton_2,SIGNAL(clicked()),this,SLOT(esporta_pdf_prodotti()));
    pushButton_2->setShortcut(qApp->translate("preventivi","Ctrl+O"));
    
    //Invio email di un ordine
    connect(send_mail,SIGNAL(triggered()),this,SLOT(invia_email()));
    send_mail->setShortcut(qApp->translate("preventivi","Ctrl+S"));

    /**************************************************
     * Impostazione tabellare di dati sql
     ***************************************************/
    mod_grid = new QSqlRelationalTableModel(this);
    model = new QSortFilterProxyModel;
    textEdit_2->setReadOnly(true);

    /************************************************
     * Impostazione combBox cerca_txt
     ************************************************/
    QStringList list1;
    list1 << qApp->translate("preventivi","Cerca per ID") << qApp->translate("preventivi","Cerca per fornitore o cliente");
    cerca_txt_2->addItems(list1);
    lista_prev();
    vis_dati_prev_prodotti();
}

void preventivi::cerca_dxt_prodotti(const QString &)
{
    if(cerca_txt_2->currentIndex() == 0){
    QString text1;
    text1 = cerca1_2->text();
    QStringList list_st;
    QSqlQuery querys("select id from preventivi");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    cerca1_2->setCompleter(complete);
    if(text1.length() == 0){
        lista_prev();
    }
    }
    else if(cerca_txt_2->currentIndex() == 1){
        QString text2;
        text2 = cerca1_2->text();
        QStringList list_st1;
        QSqlQuery querys1("select fornitore from preventivi");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        cerca1_2->setCompleter(complete1);
        if(text2.length() == 0){
            lista_prev();
        }
    }
}

void preventivi::filtro()
{
    if(cerca1_2->text().length() == 0){
        lista_prev();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("FabariaGest");
        MsgBox.setText(qApp->translate("preventivi","Avviso"));
        MsgBox.setInformativeText(qApp->translate("preventivi","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(cerca_txt_2->currentIndex() == 0){
        mod_grid->setFilter(QString("id = '" +cerca1_2->text()+"'").arg(cerca1_2->text()));
    }
    else if(cerca_txt_2->currentIndex() == 1){
        mod_grid->setFilter(QString("fornitore = '" +cerca1_2->text()+"'").arg(cerca1_2->text()));
    }
}

void preventivi::stampa_prev()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog ft(&printer,this);
    ft.setWindowFlags(Qt::Window);
    ft.setWindowTitle(qApp->translate("preventivi","Anteprima di stampa."));
    connect(&ft,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_view(QPrinter*)));
    ft.exec();
}

void preventivi::preview_view(QPrinter *printer){
    Q_UNUSED(printer);
    Stampe *ft1 = new Stampe();
    ft1->stampa_preventivi(printer,mod_grid);
}

void preventivi::nuovo_prev_prodotti()
{
    prev_rg = new preventivi_rg(this);
    QSqlQuery query("select max(id)+1 from preventivi where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        if(query.value(0).toInt() == 0){
            prev_rg->id1->setText("1");
            prev_rg->formatta_num();
            connect(prev_rg,SIGNAL(salva_prev()),this,SLOT(lista_prev()));
            prev_rg->exec();
    }
    else{
            prev_rg->id1->setText(query.value(0).toString());
            prev_rg->formatta_num();
            connect(prev_rg,SIGNAL(salva_prev()),this,SLOT(lista_prev()));
            prev_rg->exec();
        }
    }
}

void preventivi::clickgrid_prodotti()
{
    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    id_2->setText(id_new);
    cerca_prev();
}

void preventivi::aggiorna_prev_prodotti()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QModelIndex modelIndex = tab_view->selectionModel()->currentIndex();
    int riga = modelIndex.row();
    QModelIndex newindex = modelIndex.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    agg_prev_prodotti(newindex);
    }
    else
    {
        QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Selezionare una riga da modificare..."));
    }
}

void preventivi::agg_prev_prodotti(QModelIndex index)
{
    if(index.row() < mod_grid->rowCount()){
            QSqlQuery query;
            preventivi_rg *ft = new preventivi_rg(this);
            query.prepare("select * from prev_righe where id='"+id_2->text()+"'");
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
            ft->causali->setCurrentText(query.value(24).toString());
            ft->aspetto_est->setCurrentText(query.value(25).toString());
            ft->vettore->setCurrentText(query.value(26).toString());
            ft->consegna->setCurrentText(query.value(27).toString());
            ft->porto_ass->setCurrentText(query.value(28).toString());
            ft->num_colli->setText(query.value(29).toString());
            ft->peso_lordo->setText(query.value(30).toString());
            ft->data_ora->setDateTime(query.value(31).toDateTime());
            ft->peso_netto->setText(query.value(32).toString());
	    ft->tipo_listino->setCurrentText(query.value(34).toString());
            }
            ft->formatta_num();
            ft->totale_impo_iva();
            ft->lista_prodotti();
            ft->exec();
        }


}

void preventivi::vis_dati_prev_prodotti()
{
    QSqlQuery query,query1;
    query.prepare("select count(id) from preventivi where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query.exec();
    if(query.next()){
        QString text = QString::fromUtf8(qApp->translate("preventivi","I preventivi sono: ").toStdString().c_str()) + query.value(0).toString();
        tot_fatt_2->setText(text);
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Impossibile ricavare quanti preventivi esistono..."));
    }

    query1.prepare("select sum(totale) from preventivi where anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    query1.exec();
    if(query1.next()){
        QString txt = QString::fromUtf8(qApp->translate("preventivi","Il totale dei preventivi  è: ").toStdString().c_str())+QString::fromUtf8("\u20AC")+" "+query1.value(0).toString();
        tot_eur_2->setText(txt);
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Impossibile ricavare il totale dei preventivi..."));
    }
}

void preventivi::elimina_prev()
{
    if (!id_2->text().isEmpty())
    {
        // Si controlla che il cd_voce non sia usato in Spese o Budget

        QSqlQuery qctrl;
        qctrl.prepare("SELECT * FROM preventivi WHERE id = '"+id_2->text()+"' LIMIT 1");
        qctrl.exec();

                QMessageBox *box= new QMessageBox(this);
                box->setWindowTitle("FabariaGest");
                box->setInformativeText(qApp->translate("preventivi","Vuoi eliminare veramente \n il record selezionato?...."));
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

void preventivi::elimina_riga()
{
    QSqlQuery query,query1,query2;
    const char* error="";
    query.prepare("DELETE FROM preventivi WHERE id ='"+id_2->text()+"' LIMIT 1");
    query1.prepare("DELETE FROM prev_righe_art WHERE id = '"+id_2->text()+"'");
    query2.prepare("DELETE FROM prev_righe where id='"+id_2->text()+"'");

    try{
    if (query.exec() & query1.exec() & query2.exec()) //Se esito OK Eliminato da DB
    {
        LOG_FILE(qApp->translate("preventivi","Eliminazione avvenuta con successo(preventivi::elimina_riga)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
        LOG_FILE(qApp->translate("preventivi","Impossibile eliminare(preventivi::elimina_riga)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(preventivi::elimina_riga): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(preventivi::elimina_riga): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(preventivi::elimina_riga): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw error ? InvalidQuery(query.lastError().text().toStdString().c_str()) : InvalidQuery(query1.lastError().text().toStdString().c_str());
        throw InvalidQuery(query2.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }

    lista_prev();
    vis_dati_prev_prodotti();
    textEdit_2->setText("");
}

void preventivi::lista_prev()
{
    mod_grid->setTable("preventivi");
    mod_grid->setFilter("anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    model->setSourceModel(mod_grid);
    model->sort(0,Qt::AscendingOrder);

    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("preventivi","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("preventivi","Data"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("preventivi","Fornitore"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("preventivi","Tipo preventivo"));
    mod_grid->setHeaderData(4,Qt::Horizontal,qApp->translate("preventivi","Totale"));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("preventivi","Evaso"));

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
    vis_dati_prev_prodotti();
}

void preventivi::cerca_prev()
{
    QSqlQuery query;
    query.prepare("select * from preventivi where id='"+id_2->text()+"'");
    query.exec();
    if(query.next()){
        textEdit_2->setText(QString::fromUtf8(qApp->translate("preventivi","Informazioni sul preventivo: ").toStdString().c_str())+"\n"
                          +qApp->translate("preventivi","ID:")+query.value(0).toString()+"\n"
                          +qApp->translate("preventivi","Data: ")+query.value(1).toString()+"\n"
                          +qApp->translate("preventivi","Fornitore: ")+query.value(2).toString()+"\n"
                          +qApp->translate("preventivi","Totale: ")+QString::fromUtf8("\u20AC")+" "+query.value(3).toString()+"\n");
    }
    else{
        LOG_FILE(qApp->translate("preventivi","Impossibile trovare le informazioni(preventivi::cerca_prev)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(preventivi::cerca_prev): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(query.lastError().text().toStdString().c_str());
    }
}

void preventivi::stampa_prodotti()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("preventivi","Anteprima di stampa preventivo"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview.setWindowIcon(icon);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_prodotti(QPrinter*)));
    preview.exec();
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Selezionare un preventivo da stampare..."));
    }
}

void preventivi::preview_prodotti(QPrinter *printer)
{
    Q_UNUSED(printer);
    QModelIndex in = tab_view->selectionModel()->currentIndex();
    int riga = in.row();
    QModelIndex newindex = in.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    tab_view->selectRow(riga);
    Stampe *pr = new Stampe();
    pr->stampa_prev_prod(printer,mod_grid,newindex,id_2->text().toInt());
}

void preventivi::esporta_pdf_prodotti()
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("preventivi","Esporta PDF"),
                                                    "*.pdf", qApp->translate("preventivi","Pdf Files(*.pdf);;Tutti i file(*.*)"));

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
       st->stampa_prev_prod(printer,mod_grid,tab_view->currentIndex(),id_2->text().toDouble());
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Selezionare una riga da esportare"));
    }
}

void preventivi::trasforma(QString fattura)
{
    if(tab_view->selectionModel()->currentIndex().isValid()){
    QModelIndex index = tab_view->selectionModel()->currentIndex();
    if(index.row() < mod_grid->rowCount()){
    QSqlRecord record = mod_grid->record(index.row());
    if(!record.isEmpty()){
        if(record.value("evaso").toString() == ""){
            QSqlQuery query1;
	    QString id = id_2->text();
	    QString evaso = "Evaso in "+fattura;
            query1.prepare("UPDATE preventivi SET evaso='"+evaso+"' WHERE id='"+id+"'");
            
            if(query1.exec()){
                tr_fattura(fattura);
            }
            else{
                QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Impossibile inserire ")+query1.lastError().text());
            }
        }
        else if(record.value("evaso").toString() == qApp->translate("preventivi","Evaso in Ordine") || record.value("evaso").toString() == qApp->translate("preventivi","Evaso in Fattura immediata") || record.value("evaso").toString() == qApp->translate("preventivi","Evaso in Fattura accompagnatoria")) {
            QMessageBox::warning(this,"FabariaGest",QString::fromUtf8(qApp->translate("preventivi","Non puoi modificare questo preventivo\nin quanto è stato evaso in ordine o in fattura...").toStdString().c_str()));
        }
    }
    }
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Selezionare una riga da trasformare"));
    }
    lista_prev();
}

void preventivi::Popup(const QPoint &pt)
{
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
    
    QIcon icon_mail;
    icon_mail.addFile(QString::fromUtf8(":/images/invio_mail.png"));

    /***********************************************************
    *  @author Angelo Scarnà
    *  Menu a tendina
    ************************************************************/
    pAction1 = new QAction(qApp->translate("preventivi","Trasforma preventivo in ordine"), this);
    pAction1->setIcon(icon);
    pAction1->setIconVisibleInMenu(true);

    pAction2 = new QAction(qApp->translate("preventivi","Trasforma preventivo in fattura immediata"),this);
    pAction2->setIcon(icon);
    pAction2->setIconVisibleInMenu(true);

    pAction3 = new QAction(qApp->translate("preventivi","Trasforma preventivo in fattura accompagnatoria"),this);
    pAction3->setIcon(icon);
    pAction3->setIconVisibleInMenu(true);
    
    QAction* pAction4 = new QAction(qApp->translate("preventivi","Invia tramite email"),this);
    pAction4->setIcon(icon_mail);
    pAction4->setIconVisibleInMenu(true);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Instanzio il menu
    ************************************************************/
    pPopup->addAction(pAction1);
    pPopup->addAction(pAction2);
    pPopup->addAction(pAction3);
    pPopup->addAction(pAction4);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Imposto la connessione ai moduli
    ************************************************************/
    QAction* pItem = pPopup ->exec(tab_view->mapToGlobal(pt));

    if(pItem == pAction1)
    {
        tr_ordine(qApp->translate("preventivi","Ordine"));
    }
    else if(pItem == pAction2)
    {
        trasforma(qApp->translate("preventivi","Fattura immediata"));
    }
    else if(pItem == pAction3)
    {
        trasforma(qApp->translate("preventivi","Fattura accompagnatoria"));
    }
    else if(pItem == pAction4)
    {
	invia_email();
    }

    }
}

void preventivi::tr_ordine(QString ordine)
{
    
    QSqlQuery query1,query2,query3,query4,query5,query6,query7;
    const char* error="", *error1="", *error2="";
    query2.exec("select max(id)+1 from ordine");
    query3.exec("select * from preventivi where id='"+id_2->text()+"'");
    query5.exec("select * from prev_righe where id='"+id_2->text()+"'");
    query7.exec("select * from prev_righe_art where id='"+id_2->text()+"'");
    
    try{
      if(query2.next() && query3.next() && query5.next() && query7.next())
      {
	query1.prepare("INSERT INTO ordine VALUES('"+query2.value(0).toString()+"','"+query3.value(1).toString()+"','"+query3.value(2).toString()+"',"
		       "'"+ordine+"','"+query3.value(4).toString()+"','"+QString::number(QDate::currentDate().year())+"','"+query2.value(6).toString()+"','')");
	
	QString prev_ordine = QString::fromUtf8(qApp->translate("preventivi","Preventivo N°").toStdString().c_str())+query5.value(0).toString();
	query4.prepare("INSERT INTO ordine_righe "
                  " VALUES ('"+query2.value(0).toString()+"','"+query5.value(1).toString()+"','"+query5.value(2).toString()+"','"+query5.value(3).toString()+"',"
		  "'"+query5.value(4).toString()+"','"+query5.value(5).toString()+"','"+query5.value(6).toString()+"','"+query5.value(7).toString()+"',"
		  "'"+query5.value(8).toString()+"','"+query5.value(9).toString()+"','"+prev_ordine+"','"+query5.value(11).toString()+"','"+query5.value(12).toString()+"',"
		  "'"+query5.value(13).toString()+"','"+query5.value(14).toString()+"','"+query5.value(15).toString()+"','"+query5.value(16).toString()+"',"
		  "'"+query5.value(17).toString()+"','"+query5.value(18).toString()+"','"+query5.value(19).toString()+"','"+query5.value(20).toString()+"',"
		  "'"+query5.value(21).toString()+"','"+query5.value(22).toString()+"','"+query5.value(23).toString()+"','"+query5.value(24).toString()+"',"
		  "'"+query5.value(25).toString()+"','"+query5.value(26).toString()+"','"+query5.value(27).toString()+"','"+query5.value(28).toString()+"',"
		  "'"+query5.value(29).toString()+"','"+query5.value(30).toString()+"','"+query5.value(31).toString()+"','"+query5.value(32).toString()+"',"
		  "'"+query5.value(33).toString()+"','"+query5.value(34).toString()+"') ");

	query6.prepare("INSERT INTO ordine_righe_art "
                  " VALUES ('"+query2.value(0).toString()+"','"+query7.value(1).toString()+"','"+query7.value(2).toString()+"','"+query7.value(3).toString()+"',"
		  "'"+query7.value(4).toString()+"','"+query7.value(5).toString()+"','"+query7.value(6).toString()+"','"+query7.value(7).toString()+"',"
		  "'"+query7.value(8).toString()+"','"+query7.value(9).toString()+"','"+query7.value(10).toString()+"','"+query7.value(11).toString()+"',"
		  "'"+query7.value(12).toString()+"','"+query7.value(13).toString()+"','"+query7.value(14).toString()+"','"+query7.value(15).toString()+"','"+query7.value(16).toString()+"','"+query6.value(17).toString()+"','"+query6.value(18).toString()+"')");
      }
      
      if(query1.exec() && query2.exec() && query3.exec() && query4.exec()
            && query5.exec() && query6.exec() && query7.exec())
    {
        QMessageBox::information(this,"FabariaGest",qApp->translate("preventivi","Preventivo transformato in ordine"));
    }
    else{
	LOG_FILE(qApp->translate("preventivi","Impossibile inserire ordine"),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(tr_ordine(QString ordine)): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(tr_ordine(QString ordine)): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(tr_ordine(QString ordine)): "+query3.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(tr_ordine(QString ordine)): "+query4.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(tr_ordine(QString ordine)): "+query5.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(tr_ordine(QString ordine)): "+query6.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	LOG_FILE("(tr_ordine(QString ordine)): "+query7.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	throw error ? InvalidQuery(query1.lastError().text().toStdString().c_str()) : InvalidQuery(query2.lastError().text().toStdString().c_str());
	throw error1 ? InvalidQuery(query3.lastError().text().toStdString().c_str()) : InvalidQuery(query4.lastError().text().toStdString().c_str());
	throw error2 ? InvalidQuery(query5.lastError().text().toStdString().c_str()) : InvalidQuery(query6.lastError().text().toStdString().c_str());
	throw InvalidQuery(query7.lastError().text().toStdString().c_str());
    }
      
    }
    catch(FabariaException e)
    {
    }
}

void preventivi::tr_fattura(QString fattura)
{
    QSqlQuery query,query1,query2,query3,query4,query5,query6,query7;
    const char* error="", *error1="", *error2="", *error3="";
    
    query3.exec("select max(id)+1 from fattura_vendita");
    query4.exec("select * from preventivi where id='"+id_2->text()+"'");
    query5.exec("select * from prev_righe where id='"+id_2->text()+"'");
    query6.exec("select * from prev_righe_art where id='"+id_2->text()+"'");
    query7.exec("select * from banca where nome_banca='"+settingsManager->generalValue("ListaBanca/banca",QVariant()).toString()+"'");
   try{
    if(query3.next() && query4.next() && query5.next() && query6.next() && query7.next())
    {
      QString ordine_fatture = QString::fromUtf8(qApp->translate("preventivi","PREVENTIVO N°").toStdString().c_str())+query5.value(0).toString();
      query.prepare("INSERT INTO fattura_vendita VALUES('"+query3.value(0).toString()+"','"+query4.value(1).toString()+"','"+query4.value(2).toString()+"','"+fattura+"','"+query4.value(4).toString()+"','"+QString::number(QDate::currentDate().year())+"','"+query4.value(6).toString()+"')");
      query1.prepare("INSERT INTO fatture_vendita_righe"
                  " VALUES ('"+query3.value(0).toString()+"','"+query5.value(1).toString()+"','"+query5.value(2).toString()+"','"+query5.value(3).toString()+"','"+query5.value(4).toString()+"','"+query5.value(5).toString()+"','"+query5.value(6).toString()+"',"
		  "'"+query5.value(7).toString()+"','"+query5.value(8).toString()+"','"+query5.value(9).toString()+"','"+ordine_fatture+"','"+query5.value(11).toString()+"','"+query5.value(12).toString()+"','"+query5.value(13).toString()+"','"+query5.value(14).toString()+"',"
		  "'"+query5.value(15).toString()+"','"+query5.value(16).toString()+"','"+query5.value(17).toString()+"','"+query5.value(18).toString()+"','"+query5.value(19).toString()+"','"+query5.value(20).toString()+"','"+query7.value(2).toString()+"','"+query7.value(1).toString()+"',"
		  "'','0.00','"+query5.value(21).toString()+"','"+query5.value(22).toString()+"','"+query5.value(23).toString()+"','"+query5.value(24).toString()+"','"+query5.value(25).toString()+"','"+query5.value(26).toString()+"','0000-00-00','0000-00-00','0000-00-00','0.00','0.00','0.00','"+query5.value(27).toString()+"','"+query5.value(28).toString()+"',"
		  "'"+query5.value(29).toString()+"','"+query5.value(30).toString()+"','"+query5.value(31).toString()+"','"+query5.value(32).toString()+"','"+query5.value(33).toString()+"','"+query5.value(34).toString()+"') ");
      query2.prepare("INSERT INTO fatture_righe_vendita_art"
                  " VALUES ('"+query3.value(0).toString()+"','"+query6.value(1).toString()+"','"+query6.value(2).toString()+"','"+query6.value(3).toString()+"','"+query6.value(4).toString()+"','"+query6.value(5).toString()+"','"+query6.value(6).toString()+"','"+query6.value(7).toString()+"','"+query6.value(8).toString()+"',"
		  "'"+query6.value(9).toString()+"','"+query6.value(10).toString()+"','"+query6.value(11).toString()+"','"+query6.value(12).toString()+"','"+query6.value(13).toString()+"','"+query6.value(14).toString()+"','"+query6.value(15).toString()+"','"+query6.value(16).toString()+"','"+query6.value(17).toString()+"','"+query6.value(18).toString()+"','"+query6.value(19).toString()+"','"+query6.value(20).toString()+"')");
    }

    if(query.exec() && query1.exec() && query2.exec() && query3.exec() && query4.exec()
            && query5.exec() && query6.exec())
    {
        QMessageBox::information(this,"FabariaGest",qApp->translate("preventivi","ORDINE transformato in ")+fattura);
    }
    else{
        LOG_FILE(qApp->translate("preventivi","Impossibile inserire la fattura(preventivo)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query1.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query3.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query4.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query5.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query6.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(tr_fattura(QString fattura)): "+query7.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw error ? InvalidQuery(query.lastError().text().toStdString().c_str()) : InvalidQuery(query1.lastError().text().toStdString().c_str());
        throw error1 ? InvalidQuery(query2.lastError().text().toStdString().c_str()) : InvalidQuery(query3.lastError().text().toStdString().c_str());
        throw error2 ? InvalidQuery(query4.lastError().text().toStdString().c_str()) : InvalidQuery(query5.lastError().text().toStdString().c_str());
        throw error3 ? InvalidQuery(query6.lastError().text().toStdString().c_str()) : InvalidQuery(query7.lastError().text().toStdString().c_str());
    }
  }
  catch(FabariaException e)
  {
     LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
  }
}

void preventivi::invia_email()
{
    if(settingsManager->generalValue("EmailServer/server_smtp",QVariant()).toString() != ""){
    if(tab_view->selectionModel()->currentIndex().isValid()){
      QModelIndex index = tab_view->selectionModel()->currentIndex();
    if(index.row() < mod_grid->rowCount()){
    QSqlRecord record = mod_grid->record(index.row());
    if(!record.isEmpty()){
#if defined(Q_OS_WIN)
       QString fileName = settingsManager->generalValue("Cartella/destinazione",QVariant()).toString()+"\\"+record.value("tipo_preventivo").toString()+"_"+record.value("id").toString()+"_"+record.value("data").toDate().toString("dd-MM-yyyy")+".pdf";
#else
       QString fileName = settingsManager->generalValue("Cartella/destinazione",QVariant()).toString()+"/"+record.value("tipo_preventivo").toString()+"_"+record.value("id").toString()+"_"+record.value("data").toDate().toString("dd-MM-yyyy")+".pdf";
#endif
       QPrinter *printer = new QPrinter(QPrinter::HighResolution);
       printer->setPageSize(QPrinter::A4);
       printer->setOrientation(QPrinter::Portrait);
       printer->setOutputFileName(fileName);
       Stampe *st = new Stampe();
       st->stampa_prev_prod(printer,mod_grid,tab_view->currentIndex(),record.value("id").toString().toDouble());
       
       email = new email_send(this,record.value("id").toString().toDouble(),record.value("fornitore").toString(),record.value("data").toDate().toString("dd-MM-yyyy"),record.value("tipo_ordine").toString());

       QString nome_file = record.value("tipo_preventivo").toString()+QString::fromUtf8(" N° ")+record.value("id").toString()+" del "+record.value("data").toDate().toString("dd-MM-yyyy");
       email->oggetto->setText(nome_file);
       email->a_email->setText(settingsManager->generalValue("EmailServer/mail", QVariant()).toString());
       
       //Visualizzo allegati
       QSqlQuery query_all("select * from allegati_email_prev where id='"+record.value("id").toString()+"'");
       query_all.exec();
       if(query_all.next())
       {
	   update_all_email(record.value("id").toString().toDouble(),fileName);
	   email->vis_allegati_email_prev();
	   email->show();
       }
       else{
	 insert_all_email(record.value("id").toString().toDouble(),fileName);
	 email->vis_allegati_email_prev();
	 email->show();
       }
    }
    }
    }
    else{
        QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Selezionare una riga da inviare tramite email"));
    }
  }
  else{
    QMessageBox::warning(this,"FabariaGest",qApp->translate("preventivi","Imposta i paramentri di configurazione per l'invio delle email"));
  }
}

void preventivi::insert_all_email(double id, QString fileName)
{
       QSqlQuery query;
	
       query.prepare("INSERT INTO allegati_email_prev "
		     "VALUES('"+QString::number(id)+"','"+fileName+"','"+fileName.toLatin1().toBase64()+"')");
    try{
       if(query.exec())
       {
	  LOG_FILE(qApp->translate("preventivi","Inserimento allegati email preventivi"),LOG_INFO_ALL,QIODevice::Append);
       }
       else{
	  LOG_FILE(qApp->translate("preventivi","Impossibile inserire allegati email preventivi"),LOG_ERROR_ALL,QIODevice::Append);
	  throw InvalidQuery(query.lastError().text().toStdString().c_str());
       }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void preventivi::update_all_email(double id, QString fileName)
{
      QSqlQuery query;
      query.prepare("UPDATE allegati_email_prev SET nome_file='"+fileName+"',"
		     " chiave_univoca='"+fileName.toLatin1().toBase64()+"' WHERE id='"+QString::number(id)+"'");
    try{
       if(query.exec())
       {
	  LOG_FILE(qApp->translate("preventivi","Aggiornamento allegati email preventivi"),LOG_INFO_ALL,QIODevice::Append);
       }
       else{
	  LOG_FILE(qApp->translate("preventivi","Impossibile aggiornare allegati email preventivi"),LOG_ERROR_ALL,QIODevice::Append);
	  throw InvalidQuery(query.lastError().text().toStdString().c_str());
       }
    }
    catch(FabariaException e)
    {
	LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

preventivi::~preventivi()
{
}
