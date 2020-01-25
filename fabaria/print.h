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
#ifndef PRINT_H
#define PRINT_H

#include <QtSql>
#include <QPainter>
#include <QDialog>
#include <QLocale>
#include <QModelIndex>
#include <QtXml>
#include <QXmlStreamWriter>
#include "barcode.h"
#include "defines.h"
#include <QtGui>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>

class Stampe
{
public:
    Stampe();
    void StampaVoci1(QPrinter *printer,QSqlQueryModel *dataset);
    void creaPagVoci1(int pagNo);
    void stampaarticoli(QPrinter *printer,QSqlQueryModel *dataset);
    void creapagart(int pagNo);
    void stampabarcode(QPrinter *printer,QSqlQueryModel *dataset,QModelIndex *index, EAN13 *codice);
    void stampabarcodeart(QPrinter *printer, QSqlQueryModel *dataset, QModelIndex *index, EAN13 *codice);
    void stampaforn(QPrinter *printer,QSqlQueryModel *dataset);
    void fornitori(int pagNo);
    void stampafattacq(QPrinter *printer,QSqlQueryModel *dataset);
    void fattacq(int pagNo);
    void stampa_fattura(QPrinter *printer,QSqlRelationalTableModel *dataset);
    void fattura(int pagNo);
    void esporta_pdf(QPrinter *printer,QSqlRelationalTableModel *dataset);
    void pagina(int pagNo);
    void esporta_scarico(QPrinter *printer,QSqlRelationalTableModel *dataset);
    void pagina_scarico(int pagNo);

    //Fattura
    void stampa_fatt_vendita_libri(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id);
    void pagina_fatt_vendita_libri(int pagNo, int id);

    //Preventivi
    void stampa_prev_prod(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id);
    void pagina_prev_prod(int pagNo,int id);

    //Stampa lista preventivi
    void stampa_preventivi(QPrinter *printer,QSqlRelationalTableModel *dataset);
    void preventivi(int pagNo);

    //Ordini
    void stampa_ord_prod(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id);
    void pagina_ord_prod(int pagNo,int id);

    //Stampa lista ordini
    void stampa_ordini(QPrinter *printer,QSqlRelationalTableModel *dataset);
    void ordini(int pagNo);

    //DDT
    void stampa_ddt_prod(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id);
    void pagina_ddt_prod(int pagNo,int id);

    //Stampa lista ddt
    void stampa_ddt(QPrinter *printer,QSqlRelationalTableModel *dataset);
    void ddt(int pagNo);

    //Intestazione
    void intestazione();

    //Layout fattura
    void layout_fattura();

    //Layout preventivi
    void layout_preventivi();

    //Layout ordini
    void layout_ordini();

    //Layout ddt
    void layout_ddt();

    //Stampa lista scadenze
    void stampa_lista_scadenze(QPrinter *printer, QSqlRelationalTableModel *model);
    void pagina_lista_scadenze(int pagNo);

    //Stampa distinta riba
    void stampa_distinta(QPrinter *printer, QSqlRelationalTableModel *model, QModelIndexList index,QModelIndex *index_row, int cli_forn);
    void pagina_riba(int pagNo);
    void totale_riba();

    //Stampa fattura elettronica
    void stampa_fattura_elettronica(QFile *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id);

    //Stampa codice fiscale
    void print_codice(QPrinter *printer, QImage img, QString cognome, QString nome, bool maschio, bool femmina,
                          QString com_st, QString pr_st, QString codice, QDate date);

    //Stampa lista email
    void stampa_lista_email(QPrinter *printer, QSqlRelationalTableModel *model);
    void pagina_email(int pagNo);
    
    //Stampa lista iva
    void stampa_lista_iva_acq(QPrinter *printer, QSqlQuery query_acq, QString mese, QString iva_cr);
    void pagina_lista_iva_acq(int pagNo);
    void stampa_lista_iva_vnd(QPrinter *printer, QSqlQuery query_vnd, QString mese, QString iva_cr);
    void pagina_lista_iva_vnd(int pagNo);
    
    //Stampa lista agenti
    void stampaAgenti(QPrinter *printer,QSqlQueryModel *dataset);
    void pagina_agenti(int pagNo);
    
    //Stampa riepilogo agenti
    void stampa_riep_agenti(QPrinter *printer, QSqlRelationalTableModel *model, QModelIndexList index,QModelIndex *index_row, QString mese);
    void pagina_riep_agenti(int pagNo);
    void totale_riep_agenti();
    
    QPrinter *printer;
    QPainter *painter;
    QLocale *local_settings;
    QPrintDialog *stampaDlg;
    barcode *bar;
};

#endif // PRINT_H
