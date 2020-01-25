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
#include "fattura_rg_art.h"
#include "ui_fattura_rg_art.h"
#include "settingsmanager.h"
#include "defines.h"
#include "queryexception.h"

/******************************************************
 * Classe fattura_rg_art.
 * Questa classe serve a gestire i dati per l'inserimento
 * nelle righe della fattura.
 * @autor Angelo Scarna
 ******************************************************/

fattura_rg_art *rg_art = 0;

using namespace Defines;

fattura_rg_art::fattura_rg_art(QWidget *parent) :
    QDialog(parent)
{
    interfaccia();
}

fattura_rg_art::fattura_rg_art(QWidget *parent, QString fattura_tipo, int id_fattura) :
    QDialog(parent)
{
    interfaccia();
    tipo_fattura = fattura_tipo;
    idfattura = id_fattura;
    setWindowTitle(qApp->translate("fattura_rg_art","Inserisci articoli ")+tipo_fattura);
}

void fattura_rg_art::interfaccia()
{
    if(rg_art)
        delete rg_art;
    rg_art = this;

    setupUi(this);

    //Inizializzazione fattura
    fatt = new fattura_rg;

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
    
    //Esci dalla finestra di dialogo
    connect(exit_art,SIGNAL(clicked()),this,SLOT(close()));
    exit_art->setShortcut(qApp->translate("fattura_rg_art","Ctrl+E"));
    
    //Inserisci un nuovo prodotto
    connect(agg_art,SIGNAL(clicked()),this,SLOT(inserisci_riga_prod()));
    agg_art->setShortcut(qApp->translate("fattura_rg_art","Ctrl+S"));
    
    //Aggiorna la riga
    connect(aggiorna,SIGNAL(clicked()),this,SLOT(aggiorna_riga()));
    aggiorna->setShortcut(qApp->translate("fattura_rg_art","Ctrl+U"));
    
    connect(fatt->tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(prezzo_unitario,SIGNAL(textEdited(const QString &)),this,SLOT(text(const QString &)));
    connect(quant,SIGNAL(textEdited(const QString &)),this,SLOT(prezzosiva(const QString &)));
    connect(quant,SIGNAL(textEdited(QString)),this,SLOT(complete_tot(const QString &)));
    connect(quant,SIGNAL(textEdited(QString)),this,SLOT(p_iv(const QString &)));
    connect(sconto,SIGNAL(textEdited(const QString)),this,SLOT(sconto_tot(const QString &)));
    connect(comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(text(QString)));
    connect(p_c_iva,SIGNAL(textChanged(QString)),this,SLOT(complete_tot(QString)));
    connect(cod_art,SIGNAL(textChanged(const QString &)),this,SLOT(cerca_codart(const QString &)));
    connect(um,SIGNAL(currentIndexChanged(QString)),this,SLOT(indexComboMisura(QString)));
    label_slotto->setVisible(false);
    data_lotto->setVisible(false);
    label_nlotto->setVisible(false);
    numero_lotto->setVisible(false);
    data_lotto->setText(QDate::currentDate().toString("dd/MM/yyyy"));
}

void fattura_rg_art::indexComboMisura(QString combo)
{
     if(combo == qApp->translate("fattura_rg_art","Lotto") || combo == qApp->translate("fattura_rg_art","lto") || combo == qApp->translate("fattura_rg_art","lotto"))
    {
        label_slotto->setVisible(true);
        data_lotto->setVisible(true);
        label_nlotto->setVisible(true);
        numero_lotto->setVisible(true);
        bool ok;
        QString text = QInputDialog::getText(this, qApp->translate("fattura_rg_art","Inserisci numero lotto"),
                                         qApp->translate("fattura_rg_art","Inserisci numero lotto"), QLineEdit::Normal,
                                         "", &ok);
        if (ok && !text.isEmpty()){
            numero_lotto->setText(text);
            bool m_ok;
            QString m_text = QInputDialog::getText(this, qApp->translate("fattura_rg_art","Inserisci le quantità"),
                                            qApp->translate("fattura_rg_art","Quante sono le quantità?"), QLineEdit::Normal,
                                            "", &m_ok);
            double qt = m_text.toDouble();
            quant->setText(QString::number(qt));
        }
    }
    else if(combo == qApp->translate("fattura_rg_art","bc") || combo == qApp->translate("fattura_rg_art","bancale") || combo == qApp->translate("fattura_rg_art","Bancale"))
    {
        label_slotto->setVisible(false);
        data_lotto->setVisible(false);
        label_nlotto->setVisible(false);
        numero_lotto->setVisible(false);
        bool ok;
        QString text = QInputDialog::getText(this, qApp->translate("fattura_rg_art","Inserisci le quantità in un bancale"),
                                         qApp->translate("fattura_rg_art","Quanti pacchi ci sono in un bancale?"), QLineEdit::Normal,
                                         "", &ok);
        if (ok && !text.isEmpty()){
            double totale = 0;
            double pc = text.toDouble();
            bool m_ok;
            QString m_text = QInputDialog::getText(this, qApp->translate("fattura_rg_art","Inserisci le quantità in un pacco"),
                                         qApp->translate("fattura_rg_art","Quante quantità ci sono in pacco?"), QLineEdit::Normal,
                                         "", &m_ok);
            double qt = m_text.toDouble();
            totale = pc * qt;
            quant->setText(QString::number(totale));
        }
    }
    else if(combo == qApp->translate("fattura_rg_art","pc") || combo == qApp->translate("fattura_rg_art","pacchi") || combo == qApp->translate("fattura_rg_art","Pacchi"))
    {
        label_slotto->setVisible(false);
        data_lotto->setVisible(false);
        label_nlotto->setVisible(false);
        numero_lotto->setVisible(false);
         bool m_ok;
        QString m_text = QInputDialog::getText(this, qApp->translate("fattura_rg_art","Inserisci le quantità in un pacco"),
                                        qApp->translate("fattura_rg_art","Quante quantità ci sono in pacco?"), QLineEdit::Normal,
                                        "", &m_ok);
        double qt = m_text.toDouble();
        quant->setText(QString::number(qt));
    }
}

/*****************************************************
 * La funzione clear_Testo() serve a cancellare i dati
 * presenti nella finestra di dialogo a causa del non
 * ritrovamento del prodotto
 *****************************************************/
void fattura_rg_art::clear_Testo()
{
    cod_barre->setText("");
    art_nom->setText("");
    descrizione->setText("");
    quant->setText("");
}


/******************************************
 * Imposta tutte le qlineedit in text() == 0
 *******************************************/

void fattura_rg_art::nuovo()
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
void fattura_rg_art::formattanum()
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
void fattura_rg_art::comboiva()
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
void fattura_rg_art::comboum()
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

void fattura_rg_art::listino()
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

void fattura_rg_art::combo_categ()
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

void fattura_rg_art::cerca_codart(const QString &)
{
    QSqlQuery query("SELECT * from carico_magazzino where cod_articolo='"+cod_art->text()+"'");
    QStringList list;
    query.exec();
    if(query.next()){
       found = true;
       testo_show->setText(qApp->translate("fattura_rg_art","Articolo trovato..."));
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
      if(query.value(0).toString() != cod_art->text()){
            found = false;
            testo_show->setText(qApp->translate("fattura_rg_art","Articolo non trovato..."));
            clear_Testo();
      }
      }
      QCompleter *complete = new QCompleter(list,this);
      complete->setCaseSensitivity(Qt::CaseInsensitive);
      complete->setCompletionMode(QCompleter::PopupCompletion);
      cod_art->setCompleter(complete);
      if(cod_art->text() == ""){
            clear_Testo();
     }
}

void fattura_rg_art::text(const QString &){
    double iva_art = comboBox->currentText().toDouble();
    double p_iva = prezzo_unitario->text().toDouble();
    double perc = (((p_iva*iva_art)/100)+p_iva);
    p_c_iva->setText(QString::number(perc));
}

void fattura_rg_art::sconto_tot(const QString &)
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

void fattura_rg_art::prezzosiva(const QString &){
    double pr_un = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double p_s_q = pr_un*quan;
    p_s_iva->setText(QString::number(p_s_q));
}

void fattura_rg_art::complete_tot(const QString &){
    double iva_art = comboBox->currentText().toDouble();
    double ps_iva = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double tot = totale->text().toDouble();
    double perc = ((ps_iva*iva_art)/100)+ps_iva;
    tot = (perc*quan);
    totale->setText(QString::number(tot));
}

void fattura_rg_art::p_iv(const QString &){
    double iva_art = comboBox->currentText().toDouble();
    double p_iva = prezzo_unitario->text().toDouble();
    double perc = (((p_iva*iva_art)/100)+p_iva);
    double quan = quant->text().toDouble();
    double mul = perc*quan;
    p_c_iva->setText(QString::number(mul));
}

void fattura_rg_art::cerca()
{
	QSqlQuery query;
	query.prepare("SELECT * FROM articoli where cod_articolo = '"+cod_art->text()+"'");
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

void fattura_rg_art::clickgrid()
{
    QString id_new;
    int riga = fatt->tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = fatt->mod_grid->index(riga,0,QModelIndex());

    id_new = fatt->mod_grid->data(elemento).toString();
    cod_art->setText(id_new);
    cerca();
}


void fattura_rg_art::inserisci_riga_prod()
{
    //Si controlla se il record esiste già sul DB
    QSqlQuery Qctrl;
    Qctrl.prepare("SELECT * FROM fatture_righe_vendita_art WHERE cod_art = '"+cod_art->text()+"' and anno_ins='"+settingsManager->generalValue("Esercizio/anno",QVariant()).toString()+"'");
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

void fattura_rg_art::inserisci()
{
            //Tentativo di inserimento del record perché trovato
                //Data scadenza lotto
                QString giorno_scadenza = data_lotto->text().mid(0,2);
                QString mese_scadenza = data_lotto->text().mid(3, 2);
                QString anno_scadenza = data_lotto->text().mid(6, 4);
                QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
                  QSqlQuery Query;
		  double iva_art = comboBox->currentText().toDouble();
                  Query.prepare("INSERT INTO fatture_righe_vendita_art "
                                " VALUES ('"+QString::number(idfattura)+"','"+cod_art->text()+"','"+cod_barre->text()+"','"+art_nom->text()+"','"+descrizione->toPlainText()+"','"+um->currentText()+"','"+prezzo_unitario->text()+"',"
				"'"+p_s_iva->text()+"','"+p_c_iva->text()+"','"+QString::number(iva_art)+"','"+quant->text()+"','"+sconto->text()+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"','"+categ->currentText()+"','"+tipo_listino->currentText()+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"',"
			"'"+tipo_listino->currentText()+"','"+categ->currentText()+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"', '"+numero_lotto->text()+"','"+insieme_scadenza+"','"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')");

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
                             LOG_FILE(qApp->translate("fattura_rg_art","Inserimento effettuato con successo(fattura_rg_art::inserisci)"),LOG_INFO_ALL,QIODevice::Append);
                          }
                         else
                         {
                            LOG_FILE(qApp->translate("fattura_rg_art","Impossibile inserire(fattura_rg_art::inserisci)"),LOG_ERROR_ALL,QIODevice::Append);
                            LOG_FILE("(fattura_rg_art::inserisci): "+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
                            throw InvalidQuery(Query.lastError().text().toStdString().c_str());
                          }
		    }
		    catch(FabariaException e)
		    {
		       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
		    }
                         emit save_fatt();
                         fatt->lista_prodotti();
                         fatt->tot_imp_iva();
                         close();

}

void fattura_rg_art::insert_scarico_prod()
{
    //Tentativo di aggiornamento del record perché trovato
        //Data scadenza lotto
        QString giorno_scadenza = data_lotto->text().mid(0,2);
        QString mese_scadenza = data_lotto->text().mid(3, 2);
        QString anno_scadenza = data_lotto->text().mid(6, 4);
        QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    QSqlQuery Query;
    double quan = quant->text().toDouble();
    double iva_art = comboBox->currentText().toDouble();
    if(tipo_fattura == qApp->translate("fattura_rg_art","Nota di credito")){
    Query.prepare("INSERT INTO scarico_mag"
                                " VALUES ('"+cod_art->text()+"','"+cod_barre->text()+"','"+art_nom->text()+"','"+descrizione->toPlainText()+"','"+um->currentText()+"','"+prezzo_unitario->text()+"',"
				"'"+p_s_iva->text()+"','"+p_c_iva->text()+"','"+QString::number(iva_art)+"','"+QString::number(-quan)+"','"+sconto->text()+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"','"+categ->currentText()+"','"+tipo_listino->currentText()+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"', '"+numero_lotto->text()+"','"+insieme_scadenza+"','"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')");
    }
    else{
        Query.prepare("INSERT INTO scarico_mag"
                                " VALUES ('"+cod_art->text()+"','"+cod_barre->text()+"','"+art_nom->text()+"','"+descrizione->toPlainText()+"','"+um->currentText()+"','"+prezzo_unitario->text()+"',"
				"'"+p_s_iva->text()+"','"+p_c_iva->text()+"','"+QString::number(iva_art)+"','"+QString::number(quan)+"','"+sconto->text()+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"','"+categ->currentText()+"','"+tipo_listino->currentText()+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"', '"+numero_lotto->text()+"','"+insieme_scadenza+"','"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"')");
    }
    
    try{
    if(Query.exec()){
        LOG_FILE(qApp->translate("fattura_rg_art","Inserimento effettuato con successo(fattura_rg_art::insert_scarico_prod)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else{
        LOG_FILE(qApp->translate("fattura_rg_art","Impossibile inserire(fattura_rg_art::insert_scarico_prod)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("(fattura_rg_art::insert_scarico_prod): "+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(Query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}


void fattura_rg_art::aggiorna_riga()
{
    //Tentativo di aggiornamento del record perché trovato
        //Data scadenza lotto
        QString giorno_scadenza = data_lotto->text().mid(0,2);
        QString mese_scadenza = data_lotto->text().mid(3, 2);
        QString anno_scadenza = data_lotto->text().mid(6, 4);
        QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
        QSqlQuery Query;
	double iva_art = comboBox->currentText().toDouble();
        Query.prepare("UPDATE fatture_righe_vendita_art SET  id = '"+QString::number(idfattura)+"', nome_articolo='"+art_nom->text()+"', codbarre='"+cod_barre->text()+"', "
                      "prezzo_s_iva='"+p_s_iva->text()+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', prezzo_unit='"+prezzo_unitario->text()+"', "
                      "prezzo_c_iva='"+p_c_iva->text()+"', iva='"+QString::number(iva_art)+"', quantita='"+quant->text()+"', sconto='"+sconto->text()+"', totale='"+totale->text()+"', "
		      "anno_ins='"+QString::number(QDate::currentDate().year())+"',categoria='"+categ->currentText()+"', codice_listino='"+tipo_listino->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
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
    if (Query.exec())
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Aggiornamento effettuato correttamente(fattura_rg_art::aggiorna_riga)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Impossibile aggiornare(fattura_rg_art::aggiorna_riga)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("fattura_rg_art::aggiorna_riga): "+Query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(Query.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }

    emit save_fatt();
    fatt->lista_prodotti();
    fatt->tot_imp_iva();
    close();
}

void fattura_rg_art::update_articoli(double qua, double _p_siva, double _p_civa, double _tot){

    //Tentativo di aggiornamento del record perché trovato
        //Data scadenza lotto
        QString giorno_scadenza = data_lotto->text().mid(0,2);
        QString mese_scadenza = data_lotto->text().mid(3, 2);
        QString anno_scadenza = data_lotto->text().mid(6, 4);
        QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    QSqlQuery Query2;
    double quan = quant->text().toDouble();
    if(tipo_fattura == qApp->translate("fattura_rg_art","Nota di credito")){
        double diff = qua+quan;
        double iva_t= comboBox->currentText().toDouble();
        double sum_pr_siva = _p_siva+p_s_iva->text().toDouble();
        double sum_pr_civa = _p_civa+p_c_iva->text().toDouble();
        double sum_tot = _tot+totale->text().toDouble();
	Query2.prepare("UPDATE articoli SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                  "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', prezzo_unitario='"+prezzo_unitario->text()+"', "
                  "prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(iva_t)+"', quantita='"+QString::number(diff)+"', sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', "
		  "anno_ins='"+QString::number(QDate::currentDate().year())+"',categoria='"+categ->currentText()+"',codice_listino='"+tipo_listino->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"', "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_art ='"+cod_art->text()+"'");
    }
    else{
        double somma = qua-quan;
        double iva_t= comboBox->currentText().toDouble();
        double ps_iva = p_s_iva->text().toDouble();
        double pc_iva = p_c_iva->text().toDouble();
        double sum_tot = _tot-totale->text().toDouble();
        Query2.prepare("UPDATE articoli SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                  "prezzo_s_iva='"+QString::number(ps_iva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', prezzo_unitario='"+prezzo_unitario->text()+"', "
                  "prezzo_c_iva='"+QString::number(pc_iva)+"', iva='"+QString::number(iva_t)+"', quantita='"+QString::number(somma)+"', sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', "
		  "anno_ins='"+QString::number(QDate::currentDate().year())+"',categoria='"+categ->currentText()+"',codice_listino='"+tipo_listino->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"', "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_art ='"+cod_art->text()+"'");
    }

    try{
    if (Query2.exec())
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Aggiornamento effettuato correttamente(fattura_rg_art::update_articoli)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Impossibile aggiornare(fattura_rg_art::update_articoli)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("fattura_rg_art::update_articoli): "+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}



void fattura_rg_art::update_carico_prod(double ca, double _p_siva, double _p_civa, double _tot)
{
    //Tentativo di aggiornamento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = data_lotto->text().mid(0,2);
    QString mese_scadenza = data_lotto->text().mid(3, 2);
    QString anno_scadenza = data_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    QSqlQuery Query2;
    double quan = quant->text().toDouble();
    if(tipo_fattura == qApp->translate("fattura_rg_art","Nota di credito")){
        double diff = ca+quan;
        double iva_t= comboBox->currentText().toDouble();
        double sum_pr_siva = _p_siva+p_s_iva->text().toDouble();
        double sum_pr_civa = _p_civa+p_c_iva->text().toDouble();
        double sum_tot = _tot+totale->text().toDouble();
	Query2.prepare("UPDATE carico_magazzino SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                  "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', prezzo_unitario='"+prezzo_unitario->text()+"', "
                  "prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(iva_t)+"', quantita='"+QString::number(diff)+"', sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', "
		  "anno_ins='"+QString::number(QDate::currentDate().year())+"',categoria='"+categ->currentText()+"',codice_listino='"+tipo_listino->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"', "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_art ='"+cod_art->text()+"'");
    }
    else{
        double somma = ca-quan;
        double iva_t= comboBox->currentText().toDouble();
        double ps_iva = p_s_iva->text().toDouble();
        double pc_iva = p_c_iva->text().toDouble();
        double sum_tot = _tot-totale->text().toDouble();
        Query2.prepare("UPDATE carico_magazzino SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                  "prezzo_s_iva='"+QString::number(ps_iva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', prezzo_unitario='"+prezzo_unitario->text()+"', "
                  "prezzo_c_iva='"+QString::number(pc_iva)+"', iva='"+QString::number(iva_t)+"', quantita='"+QString::number(somma)+"', sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', "
		  "anno_ins='"+QString::number(QDate::currentDate().year())+"',categoria='"+categ->currentText()+"',codice_listino='"+tipo_listino->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_art ='"+cod_art->text()+"'");
    }

    try{
    if (Query2.exec())
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Aggiornamento effettuato correttamente(fattura_rg_art::update_carico_prod)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Impossibile aggiornare(fattura_rg_art::update_carico_prod)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("fattura_rg_art::update_carico_prod): "+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}


void fattura_rg_art::update_scarico_prod(double ca, double _p_siva, double _p_civa, double _tot)
{
    //Tentativo di aggiornamento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = data_lotto->text().mid(0,2);
    QString mese_scadenza = data_lotto->text().mid(3, 2);
    QString anno_scadenza = data_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    QSqlQuery Query2;
    double quan = quant->text().toDouble();
    if(tipo_fattura == qApp->translate("fattura_rg_art","Nota di credito")){
        double diff = ca+quan;
        double iva_t= comboBox->currentText().toDouble();
        double sum_pr_siva = _p_siva+p_s_iva->text().toDouble();
        double sum_pr_civa = _p_civa+p_c_iva->text().toDouble();
        double sum_tot = _tot+totale->text().toDouble();
	Query2.prepare("UPDATE scarico_mag SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                  "prezzo_s_iva='"+QString::number(sum_pr_siva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', prezzo_unitario='"+prezzo_unitario->text()+"', "
                  "prezzo_c_iva='"+QString::number(sum_pr_civa)+"', iva='"+QString::number(iva_t)+"', quantita='"+QString::number(diff)+"', sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', "
		  "anno_ins='"+QString::number(QDate::currentDate().year())+"',categoria='"+categ->currentText()+"',codice_listino='"+tipo_listino->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_art ='"+cod_art->text()+"'");
    }
    else{
        double somma = ca-quan;
        double iva_t= comboBox->currentText().toDouble();
        double ps_iva = p_s_iva->text().toDouble();
        double pc_iva = p_c_iva->text().toDouble();
        double sum_tot = _tot-totale->text().toDouble();
        Query2.prepare("UPDATE scarico_mag SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text()+"', "
                  "prezzo_s_iva='"+QString::number(ps_iva)+"', descrizione='"+descrizione->toPlainText()+"', unita='"+um->currentText()+"', prezzo_unitario='"+prezzo_unitario->text()+"', "
                  "prezzo_c_iva='"+QString::number(pc_iva)+"', iva='"+QString::number(iva_t)+"', quantita='"+QString::number(somma)+"', sconto='"+sconto->text()+"', totale='"+QString::number(sum_tot)+"', "
		  "anno_ins='"+QString::number(QDate::currentDate().year())+"',categoria='"+categ->currentText()+"',codice_listino='"+tipo_listino->currentText()+"',data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"' , "
              "numero_lotto=''"+numero_lotto->text()+",data_lotto='"+insieme_scadenza+"',addMagazzino='"+settingsManager->generalValue("Magazzino/nome",QVariant()).toString()+"' "
                          "WHERE cod_art ='"+cod_art->text()+"'");
    }

    try{
    if (Query2.exec())
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Aggiornamento effettuato correttamente(fattura_rg_art::update_scarico_prod)"),LOG_INFO_ALL,QIODevice::Append);
    }
    else
    {
        LOG_FILE(qApp->translate("fattura_rg_art","Impossibile aggiornare(fattura_rg_art::update_scarico_prod)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE("fattura_rg_art::update_scarico_prod): "+Query2.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw InvalidQuery(Query2.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}


fattura_rg_art::~fattura_rg_art()
{

}
