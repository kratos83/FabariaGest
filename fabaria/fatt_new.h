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
#ifndef FATT_NEW_H
#define FATT_NEW_H

#include <QDialog>
#include <QtSql>
#include <QLocale>
#include <QCompleter>
#include <QtGui>
#include <QLoggingCategory>
#include <QtCore>
#include "ui_fatt_new.h"

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

using namespace std;
using namespace QXlsx;

namespace Ui {
    class fatt_new;
}

class fatt_new : public QDialog, public Ui::fatt_new
{
    Q_OBJECT

public:
    explicit fatt_new(QWidget *parent = 0);
    fatt_new(QWidget *parent,QString title, int id);
    ~fatt_new();
    QSqlRelationalTableModel *mod_grid;
    double cl,cn,ts;
    void interfaccia();
    QString tipo_fattura_acq;
    int id_fattura;

private:
    QCompleter *complete;
    QLocale *local_settings;
    bool found, isCreateRow,m_submit;
    double m_prezzos_iva,m_prezzo_c_iva,m_perc,m_totale;

protected:
    bool eventFilter(QObject *o, QEvent *j);
public slots:
    void fattura_riga_open();
    void formatta_num();
    void clickgrid();
    void clickgrid_add_row();
    void salva_fattura_ric();
    void open_fornitore();
    void modifica_riga();
    void modifica_riga_one(QModelIndex index);
    void combo_fornitori();
    void combo_pagamenti();
    void elimina_riga();
    void elimina();
    void sconto_1t(const QString &);
    void sconto_2t(const QString &);
    void sconto_3t(const QString &);
    void spesa_trasp(const QString &);
    void spesa_in(const QString &);
    void fatt_aggiorna();
    void fatt_inserisci();
    void lista();
    void tot_imp_iva();
    void totale_impo_iva();
    void insert_f_prodotti();
    void update_f_prodotti();
    double s_iva(double psiva);
    double c_iva(double pciva);
    double iva_tot(double ivatot);
    void chiudi();
    void listino();
    //Importo righe da file excel
    void importa();
    void import_file_excel();
    void aggiorna_file_excel(QString doc);
    //Aggiungo riga da tabella
    void addRow();
    bool InserisciRiga();
    void submit();
    //Aggiungo righe
    void update_articoli(double qua, double _p_siva, double _p_civa, double _tot);
    void update_carico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    void insert_articoli();
    void insert_carico_magazzino();
    //recupero dati dalla QTableView
    QString getFATT_ACQID();
    QString getCodArt();
    QString getCodBarre();
    QString getNomeArticolo();
    QString getDesc();
    QString getUnita();
    QString getPrezzoUnit();
    QString getPrezzoSIVA();
    QString getPrezzoCIVA();
    QString getIVA();
    QString getQuantita();
    QString getSconto();
    QString getTotale();
    QString getAnnoIns();
    QString getListino();
    QString getCateg();
    QString getDataInsert();
    QString getDataLotto();
    QString getNumeroLotto();

signals:
    void salvafatturanew();
    void savefatt();
};

extern fatt_new *fattura_acq_insert;
#endif // FATT_NEW_H
