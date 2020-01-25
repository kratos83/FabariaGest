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
#include "ordine_righe_art.h"
#include "ui_ordine_righe_art.h"
#include "settingsmanager.h"
#include "defines.h"
#include "queryexception.h"

using namespace Defines;

ordine_righe_art::ordine_righe_art(QWidget *parent) :
    QDialog(parent)
{
    interfaccia();
}

ordine_righe_art::ordine_righe_art(QWidget *parent, int id) :
    QDialog(parent)
{
    interfaccia();
    m_id = id;
}

void ordine_righe_art::interfaccia()
{
    setupUi(this);

    //Inizializzazione preventivo
    ord_rg = new ordine_righe;

    //Imposto local_setting di sistema
    local_settings = new QLocale(QLocale::system());
    /*********************************************
     *Imposto i dati in virgola mobile
     *********************************************/
    p_s_iva->setValidator(new QDoubleValidator(-9999999999.99,
                                                   9999999999.99,2,p_s_iva));
    p_c_iva->setValidator(new QDoubleValidator(-9999999999.99,
                                                   9999999999.99,2,p_c_iva));
    totale->setValidator(new QDoubleValidator(-9999999999.99,
                                                  9999999999.99,2,totale));

    sconto->setValidator(new QDoubleValidator(-9999999999.99,
                                                  9999999999.99,2,sconto));

    /**************************************
     * Disabilita pulsanti
     **************************************/
    found = false;
    /************************************
     * Avvio i metodi per la
     * visualizzazione dei dati
     ************************************/
    formattanum();
    comboiva();
    comboum();
    combo_categ();
    listino();
    /************************************************
     * Connessione dei pulsanti
     ************************************************/
    //Imposto scorciatoia per uscire dalla finestra
    exit_art->setShortcut(qApp->translate("ordine_righe_art","Ctrl+C"));
    connect(exit_art,SIGNAL(clicked()),this,SLOT(close()));
    //Salvo la riga tramite scorciatoia
    agg_art->setShortcut(qApp->translate("ordine_righe_art","Ctrl+S"));
    connect(agg_art,SIGNAL(clicked()),this,SLOT(inserisci_riga_prod()));
    connect(ord_rg->tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(prezzo_unitario,SIGNAL(textEdited(const QString &)),this,SLOT(text(const QString &)));
    connect(quant,SIGNAL(textEdited(const QString &)),this,SLOT(prezzosiva(const QString &)));
    connect(quant,SIGNAL(textEdited(QString)),this,SLOT(complete_tot(const QString &)));
    connect(quant,SIGNAL(textEdited(QString)),this,SLOT(p_iv(const QString &)));
    connect(sconto,SIGNAL(textEdited(const QString)),this,SLOT(sconto_tot(const QString &)));
    connect(comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(text(QString)));
    connect(p_c_iva,SIGNAL(textChanged(QString)),this,SLOT(complete_tot(QString)));
    //Aggiorno la riga
    aggiorna->setShortcut(qApp->translate("ordine_righe_art","Ctrl+U"));
    connect(aggiorna,SIGNAL(clicked()),this,SLOT(aggiorna_riga()));
    connect(cod_art,SIGNAL(textChanged(const QString &)),this,SLOT(cerca_codart(const QString &)));
    connect(um,SIGNAL(currentIndexChanged(QString)),this,SLOT(indexComboMisura(QString)));
    label_slotto->setVisible(false);
    data_lotto->setVisible(false);
    label_nlotto->setVisible(false);
    numero_lotto->setVisible(false);
    data_lotto->setText(QDate::currentDate().toString("dd/MM/yyyy"));
}

/*****************************************************
 * La funzione clear_Testo() serve a cancellare i dati
 * presenti nella finestra di dialogo a causa del non
 * ritrovamento del prodotto
 *****************************************************/
void ordine_righe_art::clear_Testo()
{
    cod_barre->setText("");
    art_nom->setText("");
    descrizione->setText("");
    quant->setText("");
}

void ordine_righe_art::indexComboMisura(QString combo)
{
    if(combo == qApp->translate("ordine_righe_art","Lotto") || combo == qApp->translate("ordine_righe_art","lto") || combo == qApp->translate("ordine_righe_art","lotto"))
    {
        label_slotto->setVisible(true);
        data_lotto->setVisible(true);
        label_nlotto->setVisible(true);
        numero_lotto->setVisible(true);
        bool ok;
        QString text = QInputDialog::getText(this, qApp->translate("ordine_righe_art","Inserisci numero lotto"),
                                         qApp->translate("ordine_righe_art","Inserisci numero lotto"), QLineEdit::Normal,
                                         "", &ok);
        if (ok && !text.isEmpty()){
            numero_lotto->setText(text);
            bool m_ok;
            QString m_text = QInputDialog::getText(this, qApp->translate("ordine_righe_art","Inserisci le quantità"),
                                            qApp->translate("ordine_righe_art","Quante sono le quantità?"), QLineEdit::Normal,
                                            "", &m_ok);
            double qt = m_text.toDouble();
            quant->setText(QString::number(qt));
        }
    }
    else if(combo == qApp->translate("ordine_righe_art","bc") || combo == qApp->translate("ordine_righe_art","bancale") || combo == qApp->translate("ordine_righe_art","Bancale"))
    {
        label_slotto->setVisible(false);
        data_lotto->setVisible(false);
        label_nlotto->setVisible(false);
        numero_lotto->setVisible(false);
        bool ok;
        QString text = QInputDialog::getText(this, qApp->translate("ordine_righe_art","Inserisci le quantità in un bancale"),
                                         qApp->translate("ordine_righe_art","Quanti pacchi ci sono in un bancale?"), QLineEdit::Normal,
                                         "", &ok);
        if (ok && !text.isEmpty()){
            double totale = 0;
            double pc = text.toDouble();
            bool m_ok;
            QString m_text = QInputDialog::getText(this, qApp->translate("ordine_righe_art","Inserisci le quantità in un pacco"),
                                         qApp->translate("ordine_righe_art","Quante quantità ci sono in pacco?"), QLineEdit::Normal,
                                         "", &m_ok);
            double qt = m_text.toDouble();
            totale = pc * qt;
            quant->setText(QString::number(totale));
        }
    }
    else if(combo == qApp->translate("ordine_righe_art","pc") || combo == qApp->translate("ordine_righe_art","pacchi") || combo == qApp->translate("ordine_righe_art","Pacchi"))
    {
        label_slotto->setVisible(false);
        data_lotto->setVisible(false);
        label_nlotto->setVisible(false);
        numero_lotto->setVisible(false);
         bool m_ok;
        QString m_text = QInputDialog::getText(this, qApp->translate("ordine_righe_art","Inserisci le quantità in un pacco"),
                                        qApp->translate("ordine_righe_art","Quante quantità ci sono in pacco?"), QLineEdit::Normal,
                                        "", &m_ok);
        double qt = m_text.toDouble();
        quant->setText(QString::number(qt));
    }
}
/******************************************
 * Imposta tutte le qlineedit in text() == 0
 *******************************************/

void ordine_righe_art::nuovo()
{
    cod_art->setText("");
    art_nom->setText("");
    descrizione->setText("");
    sconto->setText("");
    comboBox->clear();
    p_s_iva->setText("");
    p_c_iva->setText("");
    quant->setText("");
    totale->setText("");
    prezzo_unitario->setText("");
    formattanum();
    comboiva();
    comboum();
    combo_categ();
    listino();
}

/***************************************************
 * Questo metodo formata i numeri in virgola mobile
 * per essere più comprensibili
 * Es: 53.2
 **************************************************/
void ordine_righe_art::formattanum()
{
    QString text = local_settings->toString(p_c_iva->text().toDouble(),'f',2);
    p_c_iva->setText(text);
    QString ps = local_settings->toString(p_s_iva->text().toDouble(),'f',2);
    p_s_iva->setText(ps);
    QString tot = local_settings->toString(totale->text().toDouble(),'f',2);
    totale->setText(tot);
}

/****************************************************
 * Questo metodo imposta la combobox dell'iva
 ****************************************************/
void ordine_righe_art::comboiva()
{
    QStringList list;
    QSqlQuery query("select iva from aliquota");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString();
    }
    complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(comboBox->model());
    comboBox->setCompleter(complete);
    comboBox->addItems(list);
    }
}

/****************************************************
 * Questo metodo imposta la combobox dell'unità
 * di misura.
 ****************************************************/
void ordine_righe_art::comboum()
{
    QStringList lista;
    QSqlQuery query("select unita from misura");
    if(query.exec()){
        while(query.next()){
           lista << query.value(0).toString();
        }
    }
    complete = new QCompleter(lista,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(um->model());
    um->setCompleter(complete);
    um->addItems(lista);
}

void ordine_righe_art::combo_categ()
{
    categ->clear();
    QStringList list;
    QSqlQuery query("select tipo_categ from categoria");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString();
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(categ->model());
    categ->setCompleter(complete);
    categ->addItems(list);
    }
}

void ordine_righe_art::listino()
{
	QStringList lista;
        QSqlQuery query("select descrizione from tipo_listino");
        if(query.exec()){
            while(query.next()){
               lista << query.value(0).toString();
            }
        }
        QCompleter *complete = new QCompleter(lista,this);
        complete->setCaseSensitivity(Qt::CaseInsensitive);
        complete->setCompletionMode(QCompleter::PopupCompletion);
        complete->setModel(tipo_listino->model());
        tipo_listino->setCompleter(complete);
        tipo_listino->addItems(lista);
}

void ordine_righe_art::cerca_codart(const QString &)
{
    QSqlQuery query("SELECT * from carico_magazzino where cod_articolo='"+cod_art->text()+"'");
    QStringList list;
    query.exec();
    if(query.next()){
       found = true;
       testo_show->setText(qApp->translate("ordine_righe_art","Articolo trovato..."));
      cod_barre->setText(query.value(1).toString());
      art_nom->setText(query.value(2).toString());
      descrizione->setText(query.value(3).toString());
      um->setCurrentText(query.value(4).toString());
      prezzo_unitario->setText(query.value(5).toString());
      p_s_iva->setText(query.value(6).toString());
      p_c_iva->setText(query.value(7).toString());
      comboBox->setCurrentText(query.value(8).toString());
      quant->setText(query.value(9).toString());
      sconto->setText(query.value(10).toString());
      totale->setText(query.value(11).toString());
      categ->setCurrentText(query.value(14).toString());
      tipo_listino->setCurrentText(query.value(13).toString());
      numero_lotto->setText(query.value("numero_lotto").toString());
      data_lotto->setText(query.value("data_lotto").toString());
      }
      else{
        found = false;
        testo_show->setText(qApp->translate("ordine_righe_art","Articolo non trovato..."));
        clear_Testo();
      }
      QCompleter *complete = new QCompleter(list,this);
      complete->setCaseSensitivity(Qt::CaseInsensitive);
      complete->setCompletionMode(QCompleter::PopupCompletion);
      cod_art->setCompleter(complete);
      if(cod_art->text() == ""){
            clear_Testo();
     }
}

void ordine_righe_art::text(const QString &){
    double iva_art = comboBox->currentText().toDouble();
    double p_iva = prezzo_unitario->text().toDouble();
    double perc = (((p_iva*iva_art)/100)+p_iva);
    p_c_iva->setText(QString::number(perc));
}

void ordine_righe_art::sconto_tot(const QString &)
{
    double iva_art = comboBox->currentText().toDouble();
    double ps_iva = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double tot = totale->text().toDouble();
    double perc = ((ps_iva*iva_art)/100)+ps_iva;
    tot = (perc*quan);
    double sc_tot = sconto->text().toDouble();
    double applica = ((tot*sc_tot)/100);
    double set = tot -applica;
    totale->setText(QString::number(set));
}

void ordine_righe_art::prezzosiva(const QString &){
    double pr_un = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double p_s_q = pr_un*quan;
    p_s_iva->setText(QString::number(p_s_q));
}

void ordine_righe_art::complete_tot(const QString &){
    double iva_art = comboBox->currentText().toDouble();
    double ps_iva = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double tot = totale->text().toDouble();
    double perc = ((ps_iva*iva_art)/100)+ps_iva;
    tot = (perc*quan);
    totale->setText(QString::number(tot));
}

void ordine_righe_art::p_iv(const QString &){
    double iva_art = comboBox->currentText().toDouble();
    double p_iva = prezzo_unitario->text().toDouble();
    double perc = (((p_iva*iva_art)/100)+p_iva);
    double quan = quant->text().toDouble();
    double mul = perc*quan;
    p_c_iva->setText(QString::number(mul));
}

void ordine_righe_art::cerca()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM articoli where cod_articolo ='"+cod_art->text()+"'");
        query.exec();


        if (query.next())
        {
            cod_barre->setText(query.value(1).toString());
            art_nom->setText(query.value(2).toString());
            descrizione->setText(query.value(3).toString());
            um->setCurrentText(query.value(4).toString());
            prezzo_unitario->setText(query.value(5).toString());
            p_s_iva->setText(query.value(6).toString());
            p_c_iva->setText(query.value(7).toString());
            comboBox->setCurrentText(query.value(8).toString());
            quant->setText(query.value(9).toString());
            sconto->setText(query.value(10).toString());
            totale->setText(query.value(11).toString());
            categ->setCurrentText(query.value(14).toString());
            tipo_listino->setCurrentText(query.value(13).toString());
            numero_lotto->setText(query.value("numero_lotto").toString());
            data_lotto->setText(query.value("data_lotto").toString());
        }
        else
        {
            art_nom->setText("");
            descrizione->setText("");
            sconto->setText("");
            p_s_iva->setText("");
            p_c_iva->setText("");
            quant->setText("");
            totale->setText("");
        }
}

void ordine_righe_art::clickgrid()
{
    QString id_new;
    int riga = ord_rg->tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = ord_rg->mod_grid->index(riga,0,QModelIndex());

    id_new = ord_rg->mod_grid->data(elemento).toString();
    cod_art->setText(id_new);
    cerca();
}

void ordine_righe_art::inserisci_riga_prod()
{
    //Si controlla se il record esiste già sul DB
    QSqlQuery Qctrl;
    Qctrl.prepare("SELECT * FROM ordine_righe_art WHERE cod_art = '"+cod_art->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
    Qctrl.exec();

    if (Qctrl.next()) //Se esiste già
    {
        inserisci();
        close();
    }
    else
        {
         inserisci();
         close();
        }
}

void ordine_righe_art::inserisci()
{                   
                    //Tentativo di inserimento del record perché trovato
                //Data scadenza lotto
                QString giorno_scadenza = data_lotto->text().mid(0,2);
                QString mese_scadenza = data_lotto->text().mid(3, 2);
                QString anno_scadenza = data_lotto->text().mid(6, 4);
                QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
                     QSqlQuery Query;
                     Query.prepare("INSERT INTO ordine_righe_art VALUES ('"+QString::number(m_id)+"','"+cod_art->text()+"','"+cod_barre->text()+"','"+art_nom->text()+"','"+descrizione->toPlainText()+"','"+um->currentText()+"',"
			"'"+prezzo_unitario->text()+"','"+p_s_iva->text()+"','"+p_c_iva->text()+"',"
			"'"+QString::number(comboBox->currentText().toDouble())+"','"+quant->text()+"','"+sconto->text()+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"',"
			"'"+tipo_listino->currentText()+"','"+categ->currentText()+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"', '"+numero_lotto->text()+"','"+insieme_scadenza+"','"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')");

                     formattanum();
                     QSqlQuery query("select quantita,prezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+cod_art->text()+"'");
                     QSqlQuery query1("select quantita,prezzo_s_iva,prezzo_c_iva,totale from scarico_mag where cod_articolo='"+cod_art->text()+"'");
                     query.exec();
                     query1.exec();
                     if(query.next()){
                          found = true;
                          double qa = query.value(0).toDouble();
                          double pr_s = query.value(1).toDouble();
                          double pr_c = query.value(2).toDouble();
                          double tot_ = query.value(3).toDouble();
                          update_articoli(qa,pr_s,pr_c,tot_);
                          update_carico_prod(qa,pr_s,pr_c,tot_);
                          if(query1.next()){
                               found = true;
                               double qa = query1.value(0).toDouble();
                               double pr_s = query1.value(1).toDouble();
                               double pr_c = query1.value(2).toDouble();
                               double tot_ = query1.value(3).toDouble();
                               update_scarico_prod(qa,pr_s,pr_c,tot_);
                               }
                               else{
                                   found = false;
                                   insert_scarico_prod();
                               }
                       }

                         try{
                         if (Query.exec()) //Se esito OK inserimento DB
                         {
                             LOG_FILE(qApp->translate("ordine_righe_art","Inserimento ordine_righe_art effettuato con successo"),LOG_INFO_ALL,QIODevice::Append);
                          }
                         else
                         {
				 LOG_FILE(qApp->translate("ordine_righe_art","Impossibile inserire ordine_righe_art: ")+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
				 throw InvalidQuery(Query.lastError().text().toStdString().c_str());
                         }
			}
			catch(FabariaException e)
			{
			  LOG_FILE(QString(),LOG_ERROR_ALL,QIODevice::Append);
			}
                         emit save_ord();
                         ord_rg->lista_prodotti();
                         ord_rg->tot_imp_iva();
                         close();

}

void ordine_righe_art::aggiorna_riga()
{
    //Tentativo di inserimento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = data_lotto->text().mid(0,2);
    QString mese_scadenza = data_lotto->text().mid(3, 2);
    QString anno_scadenza = data_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
	QSqlQuery Query;
        Query.prepare("UPDATE ordine_righe_art SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                          "prezzo_s_iva='"+p_s_iva->text()+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', " 
			  "prezzo_unitario='"+prezzo_unitario->text()+"',prezzo_c_iva='"+p_c_iva->text()+"', iva='"+QString::number(comboBox->currentText().toDouble())+"', quantita='"+quant->text()+"',  "
                          "sconto='"+sconto->text()+"', totale='"+totale->text()+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+tipo_listino->currentText()+"', categoria='"+categ->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' ', "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_art ='"+cod_art->text()+"'");

        QSqlQuery query("select quantitaprezzo_s_iva,prezzo_c_iva,totale from carico_magazzino where cod_articolo='"+cod_art->text()+"'");
        query.exec();
        if(query.next()){
            found = true;
            double qa = query.value(0).toDouble();
            double pr_s = query.value(1).toDouble();
            double pr_c = query.value(2).toDouble();
            double tot_ = query.value(3).toDouble();
                update_articoli(qa,pr_s,pr_c,tot_);
                update_scarico_prod(qa,pr_s,pr_c,tot_);
                update_carico_prod(qa,pr_s,pr_c,tot_);
        }

    try{
        if (Query.exec()) //Se esito OK inserimento DB
        {
            LOG_FILE(qApp->translate("ordine_righe_art","Inserimento ordine_righe_art effettuato con successo"),LOG_INFO_ALL,QIODevice::Append);
        }
        else
        {
	    LOG_FILE(qApp->translate("ordine_righe_art","Impossibile inserire ordine_righe_art: ")+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	    throw InvalidQuery(Query.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	    LOG_FILE(QString(),LOG_ERROR_ALL,QIODevice::Append);
	}


    emit save_ord();
    ord_rg->lista_prodotti();
    ord_rg->tot_imp_iva();
    close();
}

void ordine_righe_art::update_articoli(double qua, double _p_siva, double _p_civa, double _tot)
{
    //Tentativo di aggiornamento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = data_lotto->text().mid(0,2);
    QString mese_scadenza = data_lotto->text().mid(3, 2);
    QString anno_scadenza = data_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
        //Imposto la quantità
	double quan = quant->text().toDouble();
        double somma = qua-quan;
	//Aggiorno il prezzo senza iva 
	double ps_iva = p_s_iva->text().toDouble();
        double sum_pr_siva = _p_siva-ps_iva;
	//Aggiorno il prezzo con iva
	double pc_iva = p_c_iva->text().toDouble();
        double sum_pr_civa = _p_civa-pc_iva;
	//Aggiorno il totale
	double tot = totale->text().toDouble();
        double sum_tot = _tot-tot;
  
        QSqlQuery Query2;
        Query2.prepare("UPDATE articoli SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                          "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', " 
			  "prezzo_unitario='"+prezzo_unitario->text()+"',prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(comboBox->currentText().toDouble())+"', quantita='"+QString::number(somma)+"',  "
                          "sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+tipo_listino->currentText()+"', categoria='"+categ->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+cod_art->text()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("ordine_righe_art","Aggiornamento articoli da Ordine effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("ordine_righe_art","Impossibile aggiornare articoli da Ordine: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

void ordine_righe_art::update_carico_prod(double ca, double _p_siva, double _p_civa, double _tot)
{
    //Tentativo di aggiornamento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = data_lotto->text().mid(0,2);
    QString mese_scadenza = data_lotto->text().mid(3, 2);
    QString anno_scadenza = data_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
	//Imposto la quantità
	double quan = quant->text().toDouble();
        double somma = ca-quan;
	//Aggiorno il prezzo senza iva 
	double ps_iva = p_s_iva->text().toDouble();
        double sum_pr_siva = _p_siva-ps_iva;
	//Aggiorno il prezzo con iva
	double pc_iva = p_c_iva->text().toDouble();
        double sum_pr_civa = _p_civa-pc_iva;
	//Aggiorno il totale
	double tot = totale->text().toDouble();
        double sum_tot = _tot-tot;
  
        QSqlQuery Query2;
        Query2.prepare("UPDATE carico_magazzino SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                          "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', " 
			  "prezzo_unitario='"+prezzo_unitario->text()+"',prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(comboBox->currentText().toDouble())+"', quantita='"+QString::number(somma)+"',  "
                          "sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+tipo_listino->currentText()+"', categoria='"+categ->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+cod_art->text()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("ordine_righe_art","Aggiornamento carico magazzino da Ordine effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("ordine_righe_art","Impossibile aggiornare carico magazzino da Ordine: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}


void ordine_righe_art::update_scarico_prod(double ca, double _p_siva, double _p_civa, double _tot)
{
    //Tentativo di aggiornamento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = data_lotto->text().mid(0,2);
    QString mese_scadenza = data_lotto->text().mid(3, 2);
    QString anno_scadenza = data_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
	//Imposto la quantità
	double quan = quant->text().toDouble();
        double somma = ca+quan;
	//Aggiorno il prezzo senza iva 
	double ps_iva = p_s_iva->text().toDouble();
        double sum_pr_siva = _p_siva+ps_iva;
	//Aggiorno il prezzo con iva
	double pc_iva = p_c_iva->text().toDouble();
        double sum_pr_civa = _p_civa+pc_iva;
	//Aggiorno il totale
	double tot = totale->text().toDouble();
        double sum_tot = _tot+tot;
  
        QSqlQuery Query2;
        Query2.prepare("UPDATE scarico_mag SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                          "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', " 
			  "prezzo_unitario='"+prezzo_unitario->text()+"',prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(comboBox->currentText().toDouble())+"', quantita='"+QString::number(somma)+"',  "
                          "sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
			  "codice_listino='"+tipo_listino->currentText()+"', categoria='"+categ->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_articolo ='"+cod_art->text()+"'");

	try{
        if(Query2.exec()){
              LOG_FILE(qApp->translate("ordine_righe_art","Aggiornamento scarico magazzino da Ordine effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	       LOG_FILE(qApp->translate("ordine_righe_art","Impossibile aggiornare scarico magazzino da Ordine: ")+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	       throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
}

void ordine_righe_art::insert_scarico_prod()
{
    //Tentativo di aggiornamento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = data_lotto->text().mid(0,2);
    QString mese_scadenza = data_lotto->text().mid(3, 2);
    QString anno_scadenza = data_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    QSqlQuery Query;
    Query.prepare("INSERT INTO scarico_mag VALUES ('"+cod_art->text()+"','"+cod_barre->text()+"','"+art_nom->text()+"','"+descrizione->toPlainText()+"','"+um->currentText()+"',"
			"'"+prezzo_unitario->text()+"','"+p_s_iva->text()+"','"+p_c_iva->text()+"',"
			"'"+QString::number(comboBox->currentText().toDouble())+"','"+quant->text()+"','"+sconto->text()+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"',"
			"'"+tipo_listino->currentText()+"','"+categ->currentText()+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"', '"+numero_lotto->text()+"','"+insieme_scadenza+"','"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')");
    
    try{
      if(Query.exec()){
           LOG_FILE(qApp->translate("ordine_righe_art","Inserimento scarico magazzino da Ordine effettuato con successo..."),LOG_INFO_ALL,QIODevice::Append);
      }
      else{
	   LOG_FILE(qApp->translate("ordine_righe_art","Impossibile inserire scarico magazzino da Ordine: ")+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	   throw InvalidQuery(Query.lastError().text().toStdString().c_str());
      }
      }
      catch(FabariaException e)
      {
	  LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
      }
}

ordine_righe_art::~ordine_righe_art()
{

}
