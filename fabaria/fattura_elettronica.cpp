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
#include "fattura_elettronica.h"
#include "ui_fattura_elettronica.h"

#include "defines.h"
#include "queryexception.h"

/*
 * Classi Qt da utilizzare
 */
#include <QFile>
#include <QXmlStreamWriter>
#include <QDataStream>
#include <QTextStream>
#include <QMessageBox>
#include "defines.h"

using namespace Defines;

fattura_elettronica::fattura_elettronica(QWidget *parent, int id, QSqlRelationalTableModel *model, QModelIndex index) :
    QDialog(parent)
{
    setupUi(this);
    
    id_fatture = id;
    modello = model;
    indice = index;
	
    leggi_comuni();
    leggi_nazioni();
    leggi_cap();
    file_salvato();
    combo_regime_fiscale();
    combo_cc_cp();
    combo_doc();
    combo_mod_pagam();
    provincie();
    caus_pagam_rit();
    vis_allegati();
    id_paese->setCurrentIndex(111);
    id_ced_prest->setCurrentIndex(111);
    id_ced_prest_2->setCurrentIndex(111);
    naz->setCurrentIndex(111);
    naz_2->setCurrentIndex(111);
    naz_cess->setCurrentIndex(111);
    lista_file->setViewMode(QListWidget::IconMode);
    lista_file->sortItems(Qt::AscendingOrder);
    vis_sede_legale(false);
    vis_dati_ritenuta(false);
    //Date inizializzate
    data_doc->setCalendarPopup(true);
    data_conv_ddt->setDate(QDate::currentDate());
    data_conv_ddt->setCalendarPopup(true);
    data_ordini_acq->setDate(QDate::currentDate());
    data_ordini_acq->setCalendarPopup(true);
    dateEdit_2->setDate(QDate::currentDate());
    dateEdit_2->setCalendarPopup(true);
    data_convenzione->setDate(QDate::currentDate());
    data_convenzione->setCalendarPopup(true);
    data_ric->setDate(QDate::currentDate());
    data_ric->setCalendarPopup(true);

    //Esci dalla fattura elettronica
    connect(esci,SIGNAL(clicked()),this,SLOT(close()));
    esci->setShortcut(qApp->translate("fattura_elettronica","Ctrl+E"));
    
    //Salva fattura ellettronica
    connect(export_xml,SIGNAL(clicked()),this,SLOT(scrivi_fattura_xml()));
    export_xml->setShortcut(qApp->translate("fattura_elettronica","Ctrl+F"));
    
    connect(cap,SIGNAL(currentIndexChanged(int)),this,SLOT(inst_cap()));
    connect(cap_2,SIGNAL(currentIndexChanged(int)),this,SLOT(inst_cap_2()));
    connect(cap_cess,SIGNAL(currentIndexChanged(int)),this,SLOT(inst_cap_cess()));
    connect(sede_legale,SIGNAL(clicked(bool)),this,SLOT(vis_sede_legale(bool)));
    connect(ritenuta,SIGNAL(clicked(bool)),this,SLOT(vis_dati_ritenuta(bool)));
    connect(pers_giud,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_ced(bool)));
    connect(pers_fis,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_ced(bool)));
    connect(tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(tab_number(int)));
    connect(pers_giud_cess,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_cess(bool)));
    connect(pers_fisica_cess,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_cess(bool)));
    connect(pers_giud_rap_fis,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_rapp(bool)));
    connect(pers_fis_rapp_fis,SIGNAL(clicked(bool)),this,SLOT(per_giud_fis_rapp(bool)));
}

void fattura_elettronica::leggi_nazioni()
{
  QSqlQuery query;
    query.prepare("select * from paesi");

    QStringList list,list_sigla;
    if(query.exec())
    {
        while(query.next()){
            list << query.value(0).toString();
            list_sigla << query.value(1).toString();
        }
        id_paese->addItems(list);
        id_ced_prest->addItems(list);
        id_ced_prest_2->addItems(list);
        naz->addItems(list_sigla);
        naz_2->addItems(list_sigla);
        naz_cess->addItems(list_sigla);
    }

    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    id_paese->setCompleter(complete);
    id_ced_prest->setCompleter(complete);
    id_ced_prest_2->setCompleter(complete);
    QCompleter *m_complete = new QCompleter(list_sigla,this);
    m_complete->setCaseSensitivity(Qt::CaseInsensitive);
    naz->setCompleter(m_complete);
    naz_2->setCompleter(m_complete);
    naz_cess->setCompleter(m_complete);
}

void fattura_elettronica::leggi_cap()
{
    QSqlQuery query;
    query.prepare("select * from comuni");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
            list << query.value(2).toString();
        }
        cap->addItems(list);
        cap_2->addItems(list);
        cap_cess->addItems(list);
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    cap->setCompleter(complete);
    cap_2->setCompleter(complete);
    cap_cess->setCompleter(complete);
}

void fattura_elettronica::leggi_comuni()
{
    QSqlQuery query;
    query.prepare("select * from comuni");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
            list << query.value(0).toString();
        }
        comune->addItems(list);
        comune_2->addItems(list);
        comune_cess->addItems(list);
    }

    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    comune->setCompleter(complete);
    comune_2->setCompleter(complete);
    comune_cess->setCompleter(complete);
}

void fattura_elettronica::provincie()
{
    QSqlQuery query;
    query.prepare("select * from comuni");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
        list << query.value(1).toString();
    }
        uff_rea->addItems(list);
    }
    QCompleter *m_complete = new QCompleter(list,this);
    m_complete->setCaseSensitivity(Qt::CaseInsensitive);
    uff_rea->setCompleter(m_complete);
}

void fattura_elettronica::inst_cap()
{   
   QSqlQuery query;
    query.prepare("select * from comuni where codice='"+cap->currentText()+"'");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
	    
            comune->setCurrentText(query.value(0).toString());
            provincia->setText(query.value(1).toString());
        }
    }
}

void fattura_elettronica::inst_cap_2()
{   
    QSqlQuery query;
    query.prepare("select * from comuni where codice='"+cap_2->currentText()+"'");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
	    
            comune_2->setCurrentText(query.value(0).toString());
            provincia_2->setText(query.value(1).toString());
        }
    }
}


void fattura_elettronica::inst_cap_cess()
{
   QSqlQuery query;
    query.prepare("select * from comuni where codice='"+cap_cess->currentText()+"'");

    QStringList list;
    if(query.exec())
    {
        while(query.next()){
	    
            comune_cess->setCurrentText(query.value(0).toString());
            prov->setText(query.value(1).toString());
        }
    }
}

void fattura_elettronica::scrivi_fattura_xml()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                           qApp->translate("fattura_elettronica","Salva Xml"), (id_paese->currentText()+id_codice->text()+"_"+progree_invio->text()+".xml"),
                                           qApp->translate("fattura_elettronica","Xml files (*.xml)"));

        QFile file(fileName);
        file.open(QIODevice::WriteOnly);

	//Scrivo file xml per fattura PA
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument(); //Inizializzazione documento
        QString testo ="xml-stylesheet type=\"text/xsl\" href=\"fatturapa_v1.2.xsl\"";
        QString txt = "p:FatturaElettronica versione=\"1.1\"";
        QString xmlns_ds="http://www.w3.org/2000/09/xmldsig#";
        QString xmlns_p= "http://ivaservizi.agenziaentrate.gov.it/docs/xsd/fatture/v1.2";
        QString xmlns_xsi = "http://www.w3.org/2001/XMLSchema-instance";
        xmlWriter.writeProcessingInstruction(testo);
        xmlWriter.writeNamespace(xmlns_ds,"ds");
        xmlWriter.writeNamespace(xmlns_p,"p");
        xmlWriter.writeNamespace(xmlns_xsi,"xsi");
        xmlWriter.writeStartElement(txt);
	//Start Fattura Elettronica
        xmlWriter.writeStartElement("FatturaElettronicaHeader");

	//Inizio caricamento dati
        xmlWriter.writeStartElement("DatiTrasmissione");
        xmlWriter.writeStartElement("IdTrasmittente");
        xmlWriter.writeTextElement("IdPaese", id_paese->currentText() );
	
	if(id_codice->text().length() < 11 )
	  QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_elettronica","Inserisci corretamente la partita iva"));
	else
        xmlWriter.writeTextElement("IdCodice",id_codice->text());
	
        xmlWriter.writeEndElement();//End IdTrasmittente

        xmlWriter.writeTextElement("ProgressivoInvio",progree_invio->text());
        xmlWriter.writeTextElement("FormatoTrasmissione","SDI11");
        xmlWriter.writeTextElement("CodiceDestinatario",cod_dest->text());

        xmlWriter.writeStartElement("ContattiTrasmittente");
        xmlWriter.writeTextElement("Telefono",tel_trasm->text());
        xmlWriter.writeTextElement("Email",email_trasm->text());
        xmlWriter.writeEndElement(); //End ContattiTrasmittente
        xmlWriter.writeEndElement(); //End DatiTrasmissione

	//Inizializzazione CedentePrestatore
        xmlWriter.writeStartElement("CedentePrestatore");
        xmlWriter.writeStartElement("DatiAnagrafici");
        xmlWriter.writeStartElement("IdFiscaleIVA");
        xmlWriter.writeTextElement("IdPaese",id_ced_prest->currentText());
	
	if(fis_iva->text().length() < 11 )
	  QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_elettronica","Inserisci corretamente la partita iva"));
	else
        xmlWriter.writeTextElement("IdCodice",fis_iva->text());
        xmlWriter.writeEndElement(); //End IdFiscaleIVA
        xmlWriter.writeStartElement("Anagrafica");
	
	if(pers_giud->isChecked() == true){
	  xmlWriter.writeTextElement("Denominazione",denominazione->text());
	}
	else if(pers_fis->isChecked() == true)
	{
	  xmlWriter.writeTextElement("Nome",nomeced->text());
	  xmlWriter.writeTextElement("Cognome",cognome_ced->text());
	}
        xmlWriter.writeEndElement(); //End Anagrafica

	
	QStringList regfiscale = reg_fisc->currentText().split(" - ");
	QString stringa_reg = regfiscale.at(1);
        QSqlQuery query_reg;
        query_reg.prepare("select * from reg_fiscale where descrizione='"+stringa_reg+"'");
        query_reg.exec();
        if(query_reg.next()){
            xmlWriter.writeTextElement("RegimeFiscale",query_reg.value(0).toString());
        }
	xmlWriter.writeEndElement(); //End DatiAnagrafici
	
	//Inizializzazione sede CedentePrestatore
	xmlWriter.writeStartElement("Sede");
	xmlWriter.writeTextElement("Indirizzo",indirizzo->text());
	xmlWriter.writeTextElement("CAP",cap->currentText());
	xmlWriter.writeTextElement("Comune",comune->currentText());
	xmlWriter.writeTextElement("Provincia",provincia->text());
	xmlWriter.writeTextElement("Nazione",naz->currentText());
	xmlWriter.writeEndElement(); //End sede
	
	if(sede_legale->isChecked() == true){
	  /* Inizializzazione StabileOrganizzazione se l'azienda
	   *  ha una sede diversa dall'attuale
	   */
	xmlWriter.writeStartElement("StabileOrganizzazione");
	xmlWriter.writeTextElement("Indirizzo",indirizzo_2->text());
	xmlWriter.writeTextElement("CAP",cap_2->currentText());
	xmlWriter.writeTextElement("Comune",comune_2->currentText());
	xmlWriter.writeTextElement("Provincia",provincia_2->text());
	xmlWriter.writeTextElement("Nazione",naz_2->currentText());
	xmlWriter.writeEndElement(); //End StabileOrganizzazione
	}
	
	if(pers_giud->isChecked() == true){
	//Iscrizione al registro imprese
	xmlWriter.writeStartElement("IscrizioneREA");
	xmlWriter.writeTextElement("Ufficio",uff_rea->currentText());
	xmlWriter.writeTextElement("NumeroREA",n_rea->text());
	xmlWriter.writeTextElement("CapitaleSociale",cap_soc->text());
	
	if(socio_unico->isChecked() == true)
	  xmlWriter.writeTextElement("SocioUnico","SU");
	else
	  xmlWriter.writeTextElement("SocioUnico","SM");
	
	if(liquid->isChecked() == true)
	  xmlWriter.writeTextElement("StatoLiquidazione","LS");
	else
	  xmlWriter.writeTextElement("StatoLiquidazione","LN");
	xmlWriter.writeEndElement(); //End IscrizioneREA
	
	//RiferimentoAmministrazione a quale è registrata l'azienda
	xmlWriter.writeTextElement("RiferimentoAmministrazione",rif_amm->text());
	}
	
	if(sede_legale->isChecked() == true && tabWidget_2->currentIndex() == 1){
	  /*Inizializzazione RappresentanteFiscale. 
	   * serve a valorizzare i campi in caso il CedentePrestatore si
	   * avvale di un rappresentante fiscale in Italia
	   */
	  xmlWriter.writeStartElement("RappresentanteFiscale");
	  xmlWriter.writeStartElement("DatiAnagrafici");
	  xmlWriter.writeStartElement("IdFiscaleIVA");
	  xmlWriter.writeTextElement("IdPaese",id_ced_prest_2->currentText());
	  
	  if(fis_iva_2->text().length() < 11 )
	  QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_elettronica","Inserisci corretamente la partita iva"));
	  else
	  xmlWriter.writeTextElement("IdCodice",fis_iva_2->text());
	  
	  xmlWriter.writeEndElement(); //End IdFiscaleIVA
	  xmlWriter.writeStartElement("Anagrafica");
	  if(pers_giud_rap_fis->isChecked() == true){
	      xmlWriter.writeTextElement("Denominazione",denominazione_2->text());
	      xmlWriter.writeTextElement("CodEORI",cod_ori->text());
	  }
	  else if(pers_fis_rapp_fis->isChecked() == true){
	      xmlWriter.writeTextElement("CodiceFiscale",lineEdit_2->text());
	      xmlWriter.writeTextElement("Nome",nome->text());
	      xmlWriter.writeTextElement("Cognome",cognome->text());
	      xmlWriter.writeTextElement("Titolo",titolo->text());
	  }
	  xmlWriter.writeEndElement(); //End Anagrafica
	  xmlWriter.writeEndElement(); //End DatiAnagrafici
	  xmlWriter.writeEndElement(); //End RappresentanteFiscale
	}
	xmlWriter.writeEndElement(); //End CedentePrestatore
	
	//CessionarioCommittente
	xmlWriter.writeStartElement("CessionarioCommittente");
	xmlWriter.writeStartElement("DatiAnagrafici");
	xmlWriter.writeStartElement("IdFiscaleIVA");
	xmlWriter.writeTextElement("IdPaese",id_paese->currentText());
	xmlWriter.writeTextElement("IdCodice",id_codice->text());
	xmlWriter.writeEndElement(); //End IdFiscaleIVA
	xmlWriter.writeStartElement("Anagrafica");
	if(pers_giud_cess->isChecked() == true)
	{
	      xmlWriter.writeTextElement("Denominazione",denom_cess->text());
	}
	else if(pers_fisica_cess->isChecked() == true)
	{
	      xmlWriter.writeTextElement("CodiceFiscale",codice_fiscale->text());
	      xmlWriter.writeTextElement("Nome",nome_cess->text());
	      xmlWriter.writeTextElement("Cognome",cognome_cess->text());
	      xmlWriter.writeTextElement("Titolo",titolo->text());
	}
	xmlWriter.writeEndElement(); //End Anagrafica
	xmlWriter.writeEndElement(); //End DatiAnagrafici
	
	//Sede CessionarioCommittente
	xmlWriter.writeStartElement("Sede");
	xmlWriter.writeTextElement("Indirizzo",ind_cess->text());
	xmlWriter.writeTextElement("CAP",cap_cess->currentText());
	xmlWriter.writeTextElement("Comune",comune_cess->currentText());
	xmlWriter.writeTextElement("Provincia",prov->text());
	xmlWriter.writeTextElement("Nazione",naz_cess->currentText());
	xmlWriter.writeEndElement();//End Sede
	xmlWriter.writeEndElement(); //End CessionarioCommittente
	
	QStringList fattem = em_fatt->currentText().split(" - ");
	QString stringa = fattem.at(1);
	QSqlQuery query_sg("select * from cedprest_cesscomm where descrizione='"+stringa+"'");
	query_sg.exec();
	if(query_sg.next()){
	//SoggettoEmittente
	xmlWriter.writeTextElement("SoggettoEmittente",query_sg.value(0).toString());
	}
	
	xmlWriter.writeEndElement(); //End FatturaElettronicaHeader
	
	/* Inizializzazione FatturaElettronicaBody. Inserisco
	 * i dati della fattura.
	 * Il blocco ha molteplicità pari a 1 nel caso di fattura singola; nel caso di lotto di fatture, 
	 * si ripete per ogni fattura componente il lotto stesso
	 */
	xmlWriter.writeStartElement("FatturaElettronicaBody");
	xmlWriter.writeStartElement("DatiGenerali");
	xmlWriter.writeStartElement("DatiGeneraliDocumento");
	
	//Imposto il tipo di documento
	QStringList list_doc = tip_doc->currentText().split(" - ");
	QString st_list_doc = list_doc.at(1);
	QSqlQuery tp("select * from tipo_doc where descrizione='"+st_list_doc+"'");
	tp.exec();
	if(tp.next()){
	  xmlWriter.writeTextElement("TipoDocumento",tp.value(0).toString());
	}
	xmlWriter.writeTextElement("Divisa",euro->text());
	xmlWriter.writeTextElement("Data",data_doc->date().toString("yyyy-MM-dd"));
	xmlWriter.writeTextElement("Numero",lineEdit->text());
	
	//Dati ritenuta d'acconto
	if(ritenuta->isChecked() == true)
	{
	  xmlWriter.writeStartElement("DatiRitenuta");
	  QStringList list_rit = rit_tipo->currentText().split(" - ");
	  QString tp_rt = list_rit.at(0);
	  xmlWriter.writeTextElement("TipoRitenuta",tp_rt);
	  xmlWriter.writeTextElement("ImportoRitenuta",imp_ritenuta->text());
	  xmlWriter.writeTextElement("AliquotaRitenuta",QString::number(al_rit->currentText().toDouble(),'f',2));
	  
	  QStringList rit_doc = caus_pagam_rite->currentText().split(" - ");
	  QString rit_doc_caus = rit_doc.at(0);
	  xmlWriter.writeTextElement("Causale",rit_doc_caus);
	  xmlWriter.writeEndElement();//End DatiRitenuta
	}
	
	//Inizializzazione DatiBollo
	if(imp_bollo->text().toDouble() > 0){
	xmlWriter.writeStartElement("DatiBollo");
	if(sel_bollo->currentText() == qApp->translate("fattura_elettronica","NO"))
	{
	  xmlWriter.writeTextElement("ImportoBollo",QString::number(imp_bollo->text().toDouble(),'f',2));
	}
	else
	  xmlWriter.writeTextElement("BolloVirtuale",sel_bollo->currentText());
	xmlWriter.writeEndElement();//End DatiBollo
	}
	
	int row = 0;
	QSqlQuery query_fatt_righe("select * from fatture_vendita_righe where id='"+QString::number(id_fatture)+"'");
	if(query_fatt_righe.exec())
	{
	  if( row < query_fatt_righe.record().count() )
	  {
	    QSqlRecord rec = query_fatt_righe.record();
		      //ScontoMaggiorazione
		      if(rec.value("sconto_1").toDouble() != 0 || rec.value("sconto_2").toDouble() != 0
			|| rec.value("sconto_3").toDouble() != 0 )
		      {
			xmlWriter.writeStartElement("ScontoMaggiorazione");
			xmlWriter.writeTextElement("TIPO","SC");
			double sum = rec.value("sconto_1").toDouble()+rec.value("sconto_2").toDouble()+rec.value("sconto_3").toDouble();
			xmlWriter.writeTextElement("Percentuale",QString::number(sum,'f',2));
			double tot_parz = (rec.value("totale").toDouble()*100)/(100-sum);
			double tot = tot_parz- rec.value("totale").toDouble();
			xmlWriter.writeTextElement("Importo",QString::number(tot,'f',2));
			xmlWriter.writeTextElement("ImportoTotaleDocumento",modello->record().value("totale").toString());
			xmlWriter.writeEndElement();
		      }
		      else if(rec.value("sconto_1").toDouble() < 0 || rec.value("sconto_2").toDouble() < 0
			|| rec.value("sconto_3").toDouble() < 0 )
		      {
			xmlWriter.writeStartElement("ScontoMaggiorazione");
			xmlWriter.writeTextElement("TIPO","MG");
			double sum = rec.value("sconto_1").toDouble()+rec.value("sconto_2").toDouble()+rec.value("sconto_3").toDouble();
			xmlWriter.writeTextElement("Percentuale",QString::number(sum,'f',2));
			double tot_parz = (rec.value("totale").toDouble()*100)/(100-sum);
			double tot = tot_parz- rec.value("totale").toDouble();
			xmlWriter.writeTextElement("Importo",QString::number(tot,'f',2));
			xmlWriter.writeTextElement("ImportoTotaleDocumento",modello->record().value("totale").toString());
			xmlWriter.writeEndElement();
		      }
	  }
	}
	xmlWriter.writeTextElement("Art73",art_73->currentText());
	xmlWriter.writeEndElement(); //End DatiGeneraliDocumento
	
	if(!num_contratto_2->text().isEmpty() && !cod_comm_cont->text().isEmpty() &&
	  !cod_cup_contratto->text().isEmpty() && !cod_cig_contratto->text().isEmpty())
	{
	    xmlWriter.writeStartElement("DatiOrdineAcquisto");
	    xmlWriter.writeTextElement("RiferimentoNumeroLinea",QString::number(1));
	    xmlWriter.writeTextElement("IdDocumento",num_contratto_2->text());
	    xmlWriter.writeTextElement("Data",data_ordini_acq->date().toString("yyyy-MM-dd"));
	    QSqlQuery query_contratto("select count(id) from ordine_righe_art where id='"+QString::number(id_fatture)+"'");
	    query_contratto.exec();
	    if(query_contratto.next())
	    {
	       xmlWriter.writeTextElement("NumItem",query_contratto.value(0).toString());
	    }
	    xmlWriter.writeTextElement("CodiceCommessaConvenzione",cod_comm_cont->text());
	    xmlWriter.writeTextElement("CodiceCUP",cod_cup_contratto->text());
	    xmlWriter.writeTextElement("CodiceCIG",cod_cig_contratto->text());
	    xmlWriter.writeEndElement(); //End DatiOrdineAcquisto
	}

	if(!num_contratto->text().isEmpty() && !cod_commessa->text().isEmpty() &&
	  !cod_cup->text().isEmpty() && !cod_cig->text().isEmpty())
	{
	    xmlWriter.writeStartElement("DatiContratto");
	    xmlWriter.writeTextElement("IdDocumento",num_contratto->text());
	    xmlWriter.writeTextElement("Data",dateEdit_2->date().toString("yyyy-MM-dd"));
	    xmlWriter.writeTextElement("NumItem",num_righe_contratto->text());
	    xmlWriter.writeTextElement("CodiceCommessaConvenzione",cod_commessa->text());
	    xmlWriter.writeTextElement("CodiceCUP",cod_cup->text());
	    xmlWriter.writeTextElement("CodiceCIG",cod_cig->text());
	    xmlWriter.writeEndElement(); //End DatiContratto
	}
	
	if(!num_convenzione->text().isEmpty() && !cod_convenzione->text().isEmpty() &&
	  !cod_cup_conv->text().isEmpty() && !cod_cig_conv->text().isEmpty())
	{
	    xmlWriter.writeStartElement("DatiConvenzione");
	    xmlWriter.writeTextElement("IdDocumento",num_convenzione->text());
	    xmlWriter.writeTextElement("Data",data_convenzione->date().toString("yyyy-MM-dd"));
	    xmlWriter.writeTextElement("NumItem",num_righe_contratto_2->text());
	    xmlWriter.writeTextElement("CodiceCommessaConvenzione",cod_convenzione->text());
	    xmlWriter.writeTextElement("CodiceCUP",cod_cup_conv->text());
	    xmlWriter.writeTextElement("CodiceCIG",cod_cig_conv->text());
	    xmlWriter.writeEndElement(); //End DatiConvenzione
	}
	
	if(!numero_ric->text().isEmpty() && !cod_ric->text().isEmpty() &&
	  !cod_cup_ric->text().isEmpty() && !cod_cig_ric->text().isEmpty())
	{
	    xmlWriter.writeStartElement("DatiRicezione");
	    xmlWriter.writeTextElement("IdDocumento",numero_ric->text());
	    xmlWriter.writeTextElement("Data",data_ric->date().toString("yyyy-MM-dd"));
	    xmlWriter.writeTextElement("NumItem",num_righe_ric->text());
	    xmlWriter.writeTextElement("CodiceCommessaConvenzione",cod_ric->text());
	    xmlWriter.writeTextElement("CodiceCUP",cod_cup_ric->text());
	    xmlWriter.writeTextElement("CodiceCIG",cod_cig_ric->text());
	    xmlWriter.writeEndElement(); //End DatiRicezione
	}
	
	if(!numero->text().isEmpty())
	{
	    xmlWriter.writeStartElement("DatiDDT");
	    xmlWriter.writeTextElement("NumeroDDT",numero->text());
	    xmlWriter.writeTextElement("DataDDT",data_conv_ddt->date().toString("yyyy/MM/dd"));
	    xmlWriter.writeEndElement(); //End DatiDDT
	}
	
	// se è una fattura accompagnatoria qui inserisco anche i dati relativi al trasporto
	if(indice.row() < modello->rowCount())
	{
	  QSqlRecord record = modello->record(indice.row());
	  if(record.value("tipo_fattura").toString() == qApp->translate("fattura_elettronica","Fattura accompagnatoria"))
	  {
	    QSqlQuery query_fatt_acc("select * from fatture_vendita_righe where id='"+QString::number(id_fatture)+"'");
	    if(query_fatt_acc.exec())
	    {
	      if(query_fatt_acc.next())
	      {
		QSqlRecord rec = query_fatt_acc.record();
		if(!rec.isEmpty())
		{
		//Inizializzazione DatiTrasporto
		xmlWriter.writeStartElement("DatiTrasporto");
		xmlWriter.writeTextElement("MezzoTrasporto",rec.value("vettore").toString());
		xmlWriter.writeTextElement("NumeroColli",QString::number(rec.value("n_colli").toDouble(),'f',2));
		xmlWriter.writeTextElement("Descrizione",rec.value("asp_est").toString());
		xmlWriter.writeTextElement("UnitaMisuraPeso", "KG");
		xmlWriter.writeTextElement("PesoLordo",QString::number(rec.value("peso_lordo").toDouble(),'f',2));
		xmlWriter.writeTextElement("PesoNetto",QString::number(rec.value("peso_netto").toDouble(),'f',2));
		xmlWriter.writeTextElement("DataInizioTrasporto",rec.value("data_ora").toDate().toString("yyyy-MM-DD"));
		xmlWriter.writeEndElement(); //End DatiTrasporto
		}
	      }
	    }
	  }
	}
	xmlWriter.writeEndElement(); //End DatiGenerali
	
	//Inizializzazione DatiBeniServizi
	xmlWriter.writeStartElement("DatiBeniServizi");
	
	QSqlQuery query_fatt("select * from fatture_righe_vendita_art where id='"+QString::number(id_fatture)+"'");
	if(query_fatt.exec()){
	    for(int riga =0; riga < query_fatt.record().count(); riga++)
	    {
		if(query_fatt.next()){
		    QSqlRecord rec = query_fatt.record();
		    if(!rec.isEmpty()){
		      xmlWriter.writeStartElement("DettaglioLinee");
		      xmlWriter.writeTextElement("NumeroLinea",QString::number(1+riga));
		      xmlWriter.writeTextElement("Descrizione",rec.value("descrizione").toString());
		      xmlWriter.writeTextElement("Quantita",QString::number(rec.value("quantita").toDouble(),'f',2));
		      xmlWriter.writeTextElement("UnitaMisura",rec.value("unita").toString().toUpper());
		      xmlWriter.writeTextElement("PrezzoUnitario", QString::number(rec.value("prezzo_unit").toDouble(),'f',2));
		      if(rec.value("sconto").toDouble() > 0)
		      {
			xmlWriter.writeStartElement("ScontoMaggiorazione");
			xmlWriter.writeTextElement("TIPO","SC");
			xmlWriter.writeTextElement("Percentuale",QString::number(rec.value("sconto").toDouble(),'f',2));
			double tot_parz = (rec.value("totale").toDouble()*100)/(100-rec.value("sconto").toDouble());
			double tot = tot_parz- rec.value("totale").toDouble();
			xmlWriter.writeTextElement("Importo",QString::number(tot,'f',2));
			xmlWriter.writeEndElement();
		      }
		      else if(rec.value("sconto").toDouble() < 0)
		      {
			xmlWriter.writeStartElement("ScontoMaggiorazione");
			xmlWriter.writeTextElement("TIPO","MG");
			xmlWriter.writeTextElement("Percentuale",QString::number(rec.value("sconto").toDouble(),'f',2));
			double tot_parz = (rec.value("totale").toDouble()*100)/(100-rec.value("sconto").toDouble());
			double tot = tot_parz- rec.value("totale").toDouble();
			xmlWriter.writeTextElement("Importo",QString::number(tot,'f',2));
			xmlWriter.writeEndElement();
		      }
		      xmlWriter.writeTextElement("PrezzoTotale",QString::number(rec.value("totale").toDouble(),'f',2));
		      xmlWriter.writeTextElement("AliquotaIVA",QString::number(rec.value("iva").toDouble(),'f',2));
		      xmlWriter.writeEndElement();
		    }
		}
	    }
	}
	
	//Inizializzazione DatiRiepilogo
	xmlWriter.writeStartElement("DatiRiepilogo");
	QSqlQuery query_fatt_dati("select * from fatture_righe_vendita_art where id='"+QString::number(id_fatture)+"'");
	if(query_fatt_dati.exec())
	{
	  if(query_fatt_dati.next())
	  {
	    QSqlRecord rec = query_fatt_dati.record();
	    if(!rec.isEmpty())
	      xmlWriter.writeTextElement("AliquotaIVA",QString::number(rec.value("iva").toDouble(),'f',2));
	      xmlWriter.writeTextElement("ImponibileImporto",QString::number(rec.value("prezzo_s_iva").toDouble(),'f',2));
	      double diff = rec.value("prezzo_c_iva").toDouble()-rec.value("prezzo_s_iva").toDouble();
	      xmlWriter.writeTextElement("Imposta",QString::number(diff,'f',2));
	      //xmlWriter.writeTextElement("RiferimentoNormativo","MINISTERO DELL'ECONOMIA E DELLE FINANZE DECRETO 3 aprile 2013, n.55"
	     // "Disposizioni in materia di emissione, trasmissione e ricevimento della fattura elettronica, attraverso il Sistema di interscambio ");
	      xmlWriter.writeEndElement(); //End DatiRiepilogo
	  }
	}
	xmlWriter.writeEndElement(); //End DatiBeniServizi
	
	//Inizializzazione DatiPagamamento
	xmlWriter.writeStartElement("DatiPagamento");
	QStringList list_cond = cond_pagam->currentText().split(" - ");
	QString cond = list_cond.at(0);
	xmlWriter.writeTextElement("CondizioniPagamento",cond);
	xmlWriter.writeStartElement("DettaglioPagamento");
	QStringList list_mod_pag = mod_pagam->currentText().split(" - ");
	QString str_pag = list_mod_pag.at(0);
	if(str_pag == "MP04")
	{
	  xmlWriter.writeTextElement("ModalitaPagamento","MP04");
	  xmlWriter.writeTextElement("CognomeQuietanzante",cognome_ced->text());
	  xmlWriter.writeTextElement("NomeQuietanzante",nomeced->text());
	  xmlWriter.writeTextElement("CFQuietanzante",cod_fis_ced->text());
	  xmlWriter.writeTextElement("TitoloQuietanzante",tit_ced->text());
	}
	else
	  xmlWriter.writeTextElement("ModalitaPagamento",str_pag);
	QSqlQuery query_scad("select * from fatture_vendita_righe where id='"+QString::number(id_fatture)+"'");
	    if(query_scad.exec())
	    {
	      for(int riga =0; riga < query_scad.record().count(); riga++)
	    {
	      if(query_scad.next())
	      {
		QSqlRecord rec = query_scad.record();
		if(!rec.isEmpty()){
		if(!rec.value("scadenza_1").toDate().isNull() && rec.value("scadenza_2").toDate().isNull()
		  && rec.value("scadenza_3").toDate().isNull())
		{
		  xmlWriter.writeTextElement("DataScadenzaPagamento",rec.value("scadenza_1").toDate().toString("yyyy-MM-dd"));
		  xmlWriter.writeTextElement("ImportoPagamento",QString::number(importo->text().toDouble(),'f',2));
		  xmlWriter.writeTextElement("IBAN",rec.value("iban").toString());
		}
		else if(!rec.value("scadenza_1").toDate().isNull() && !rec.value("scadenza_2").toDate().isNull()
		  && rec.value("scadenza_3").toDate().isNull())
		{
		  double div = importo->text().toDouble()/2;
		  xmlWriter.writeTextElement("DataScadenzaPagamento",rec.value("scadenza_1").toDate().toString("yyyy-MM-dd"));
		  xmlWriter.writeTextElement("ImportoPagamento",QString::number(div,'f',2));
		  xmlWriter.writeTextElement("IBAN",rec.value("iban").toString());
		  
		  xmlWriter.writeTextElement("DataScadenzaPagamento",rec.value("scadenza_2").toDate().toString("yyyy-MM-dd"));
		  xmlWriter.writeTextElement("ImportoPagamento",QString::number(div,'f',2));
		  xmlWriter.writeTextElement("IBAN",rec.value("iban").toString());
		}
		else if(!rec.value("scadenza_1").toDate().isNull() && !rec.value("scadenza_2").toDate().isNull()
		  && !rec.value("scadenza_3").toDate().isNull())
		{
		  double div = importo->text().toDouble()/3;
		  xmlWriter.writeTextElement("DataScadenzaPagamento",rec.value("scadenza_1").toDate().toString("yyyy-MM-dd"));
		  xmlWriter.writeTextElement("ImportoPagamento",QString::number(div,'f',2));
		  xmlWriter.writeTextElement("IBAN",rec.value("iban").toString());
		  
		  xmlWriter.writeTextElement("DataScadenzaPagamento",rec.value("scadenza_2").toDate().toString("yyyy-MM-dd"));
		  xmlWriter.writeTextElement("ImportoPagamento",QString::number(div,'f',2));
		  xmlWriter.writeTextElement("IBAN",rec.value("iban").toString());
		  
		  xmlWriter.writeTextElement("DataScadenzaPagamento",rec.value("scadenza_3").toDate().toString("yyyy-MM-dd"));
		  xmlWriter.writeTextElement("ImportoPagamento",QString::number(div,'f',2));
		  xmlWriter.writeTextElement("IBAN",rec.value("iban").toString());
		}
		else{
		  xmlWriter.writeTextElement("DataScadenzaPagamento",data_doc->date().toString("yyyy-MM-dd"));
		  xmlWriter.writeTextElement("ImportoPagamento",QString::number(importo->text().toDouble(),'f',2));
		  xmlWriter.writeTextElement("IBAN",rec.value("iban").toString());
		}
	      }
	      }
	    }
	    }
	xmlWriter.writeEndElement(); //End DettaglioPagamento
	xmlWriter.writeEndElement(); //End DatiPagamamento
	
	//Inizializzazione allegati fattura
	QStringList list;
	QString m_fileName;
	if(lista_file->count() != 0){
	QSqlQuery qinst_allegati("select * from allegati_fattura where id='"+QString::number(id_fatture)+"'");
	if(qinst_allegati.exec())
	{
	  for(int riga_all=0; riga_all < qinst_allegati.record().count(); riga_all++)
	  {
	    if(qinst_allegati.next())
	    {
	      QSqlRecord rec = qinst_allegati.record();
	      if(!rec.isEmpty())
	      {
		list = rec.value(1).toStringList();
		foreach(m_fileName,list){
		QFont f;
		f.setPointSize(10); // Non deve essere 0
		QFileInfo fi(m_fileName);
		//Imposto nome file file allegato
		QString m_name = fi.fileName();
		xmlWriter.writeStartElement("Allegati");
		xmlWriter.writeTextElement("NomeAttachment",m_name);
		QStringList formato = rec.value(1).toString().split(".");
		QString vis_formato = formato.at(1);
		QString vis_p_punto = formato.at(0);
		xmlWriter.writeTextElement("FormatoAttachment",vis_formato);
		xmlWriter.writeTextElement("DescrizioneAttachment",vis_p_punto);
		xmlWriter.writeTextElement("Attachment",rec.value(2).toString());
		xmlWriter.writeEndElement(); //Fine allegati fattura
		}
	      }
	    }
	  }
	}
	}
	xmlWriter.writeEndElement(); //End FatturaElettronicaBody
	xmlWriter.writeStartElement("/p:FatturaElettronica>");
	
        file.close();

	if(!file.exists())
	  QMessageBox::warning(this,"FabariaGest",qApp->translate("fattura_elettronica","Impossibile salvare il file"));
	else{
        //Salva il numero progressivo e data di creazione file
        QSqlQuery query;
	int invio = progree_invio->text().toInt();
	QString data_ora = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        query.prepare("INSERT INTO fatt_elettronica"
                      " VALUES('"+QString::number(invio)+"','"+data_ora+"')");
      
	try{
        if(query.exec())
        {
            LOG_FILE(qApp->translate("fattura_elettronica","FatturaElettronica salvata con successo"),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
	    LOG_FILE(qApp->translate("fattura_elettronica","Errore nel salvataggio della fattura elettronica: "),LOG_ERROR_ALL,QIODevice::Append);
	    LOG_FILE("(FatturaElettronica): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
	    throw InvalidQuery(query.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
	}
	
	file_salvato();
}

void fattura_elettronica::file_salvato()
{
    QSqlQuery query;
    query.prepare("select max(id),max(data_ora) from fatt_elettronica");
    query.exec();
    if(query.next()){
        testo_salva_file->setText(qApp->translate("fattura_elettronica","Ultimo progressivo salvato: ")+query.value(0).toString()+qApp->translate("fattura_elettronica"," il ")+query.value(1).toDate().toString("dd/MM/yyyy")+qApp->translate("fattura_elettronica"," alle ore ")+query.value(1).toTime().toString("hh:mm:ss"));
    }
    else{
        qDebug() << qApp->translate("fattura_elettronica","errore") << query.lastError().text();
    }
}

void fattura_elettronica::combo_regime_fiscale()
{
    QStringList list;
    QSqlQuery query("select * from reg_fiscale");
    if(query.exec()){
        while(query.next()){
            list << query.value(0).toString()+" - "+query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(reg_fisc->model());
    reg_fisc->setCompleter(complete);
    reg_fisc->addItems(list);
}


void fattura_elettronica::combo_cc_cp()
{
    QStringList list;
    QSqlQuery query("select * from cedprest_cesscomm");
    if(query.exec()){
        while(query.next()){
            list << query.value(0).toString()+" - "+query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(em_fatt->model());
    em_fatt->setCompleter(complete);
    em_fatt->addItems(list);
}

void fattura_elettronica::combo_doc()
{
    QStringList list;
    QSqlQuery query("select * from tipo_doc");
    if(query.exec()){
        while(query.next()){
            list << query.value(0).toString()+" - "+query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(tip_doc->model());
    tip_doc->setCompleter(complete);
    tip_doc->addItems(list);

}

void fattura_elettronica::combo_mod_pagam()
{
    QStringList list;
    QSqlQuery query("select * from dett_pagam");
    if(query.exec()){
        while(query.next()){
            list << query.value(0).toString()+" - "+query.value(1).toString();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(mod_pagam->model());
    mod_pagam->setCompleter(complete);
    mod_pagam->addItems(list);
}

bool fattura_elettronica::vis_sede_legale(bool vero)
{
    if(vero == true)
      tabWidget_2->setVisible(true);
    else
      tabWidget_2->setVisible(false);
    
    return vero;
}

bool fattura_elettronica::vis_dati_ritenuta(bool vero)
{
    if(vero == true){
      groupBox_7->setVisible(true);
    }
    else{
      groupBox_7->setVisible(false);
    }
    return vero;
}

int fattura_elettronica::tab_number(int numero)
{
    return numero;
}

bool fattura_elettronica::per_giud_fis_ced(bool vero)
{
    if(pers_giud->isChecked() == vero){
      nomeced->setEnabled(false);
      cognome_ced->setEnabled(false);
      cod_fis_ced->setEnabled(false);
      tit_ced->setEnabled(false);
      denominazione->setEnabled(true);
      uff_rea->setEnabled(true);
      n_rea->setEnabled(true);
      cap_soc->setEnabled(true);
      socio_unico->setEnabled(true);
      liquid->setEnabled(true);
      rif_amm->setEnabled(true);
      sede_legale->setEnabled(true);
    }
    else if(pers_fis->isChecked() == vero){
      denominazione->setEnabled(false);
      nomeced->setEnabled(true);
      cognome_ced->setEnabled(true);
      cod_fis_ced->setEnabled(true);
      tit_ced->setEnabled(true);
      uff_rea->setEnabled(false);
      n_rea->setEnabled(false);
      cap_soc->setEnabled(false);
      socio_unico->setEnabled(false);
      liquid->setEnabled(false);
      rif_amm->setEnabled(false);
      sede_legale->setEnabled(false);
    }
    return vero;
}

bool fattura_elettronica::per_giud_fis_cess(bool vero)
{
    if(pers_giud_cess->isChecked() == vero)
    {
      denom_cess->setEnabled(true);
      nome_cess->setEnabled(false);
      cognome_cess->setEnabled(false);
      codice_fiscale->setEnabled(false);
      tit_cess->setEnabled(false);
    }
    else if(pers_fisica_cess->isChecked() == vero)
    {
      denom_cess->setEnabled(false);
      tit_cess->setEnabled(true);
      nome_cess->setEnabled(true);
      cognome_cess->setEnabled(true);
      codice_fiscale->setEnabled(true);
    }
    return vero;
}

bool fattura_elettronica::per_giud_fis_rapp(bool vero)
{
    if(pers_giud_rap_fis->isChecked() == vero){
      nome->setEnabled(false);
      cognome->setEnabled(false);
      denominazione_2->setEnabled(true);
      cod_ori->setEnabled(true);
      titolo->setEnabled(true);
    }
    else if(pers_fis_rapp_fis->isChecked() == vero)
    {
      nome->setEnabled(true);
      cognome->setEnabled(true);
      denominazione_2->setEnabled(false);
      cod_ori->setEnabled(false);
      titolo->setEnabled(false);
    }
    
    return vero;
}

void fattura_elettronica::vis_allegati()
{
    QStringList list;
    QString m_fileName;
    QSqlQuery qvis_allegati("select * from allegati_fattura where id='"+QString::number(id_fatture)+"'");
    if(qvis_allegati.exec()){
    while(qvis_allegati.next())
    {	
	list = qvis_allegati.value(1).toStringList();
	foreach(m_fileName,list){
	QListWidgetItem *item = new QListWidgetItem(lista_file);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	QFileInfo fi(m_fileName);
	//Imposto nome file file allegato
	QString m_name = fi.fileName();
	item->setText(m_name);
	//Imposto icona file allegato
	QFileIconProvider provider;
	QIcon icon = provider.icon(fi);
	item->setIcon(icon);
	item->setFont(f);
	}
    }
    }
}

void fattura_elettronica::caus_pagam_rit()
{
    QStringList list;
    QSqlQuery query("select * from cond_pagam");
    if(query.exec()){
        while(query.next()){
            list << query.value(0).toString()+" - "+query.value(1).toString().trimmed();
        }
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(caus_pagam_rite->model());
    caus_pagam_rite->setCompleter(complete);
    caus_pagam_rite->addItems(list);
}

fattura_elettronica::~fattura_elettronica()
{
}
