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
#ifndef FATTURA_RG_H
#define FATTURA_RG_H

#include <QDialog>
#include <QLocale>
#include <QtGui>
#include <QtSql>
#include "ui_fattura_rg.h"

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

using namespace std;
using namespace QXlsx;

namespace Ui {
class fattura_rg;
}

class fattura_rg : public QDialog, public Ui::fattura_rg
{
    Q_OBJECT
    
public:
    explicit fattura_rg(QWidget *parent = 0);
    fattura_rg(QWidget *wig,QString fatt, int id_ft);
    ~fattura_rg();
    QSqlRelationalTableModel *mod_grid;
    
public slots:
    void formatta_num();
    void cliente_add();
    void open_cliente();
    void open_fornitore();
    void open_agenti();
    void combo_fornitore();
    void combo_agenti();
    int open_cl(int index);
    int open_fl(int index);
    int open_banca(int index);
    int open_agenti(int index);
    void filtro();
    void add_banca();
    void bank();
    void filtro_banca();
    void pagamento_fatt();
    void modifica_riga();
    void modifica_riga_one(QModelIndex index);
    void elimina_riga();
    void elimina();
    void sconto_1t(const QString &);
    void sconto_2t(const QString &);
    void sconto_3t(const QString &);
    void spesa_trasp(const QString &);
    void spesa_in(const QString &);
    void fatt_aggiorna();
    void fatt_inserisci();
    void lista_prodotti();
    void tot_imp_iva();
    void totale_impo_iva();
    void insert_f_prodotti();
    void update_f_prodotti();
    void clickgrid();
    void clickgrid_add_row();
    void fattura_riga_open();
    void chiudi();
    void tipo_pagamento();
    void visual_scadenze();
    void interfaccia();
    void enable_fatt_acc();
    void disable_fatt_acc();
    //Causali di trasporto
    void trasp_causali();
    //Porto
    void porto();
    //Aspetto dei beni
    void aspetto_beni();
    //Consegna
    void consegna_trasp();
    //INSERIMENTO SCADENZE
    void scadenza1();
    void scadenza2();
    void scadenza3();
    /* Visualizzazione allegati, da rimuovere in caso 
     * se selezionati scontrino o nota di credito.
     */
    void dis_allegati();
    
    /*
     * Visualizza i file all'interno della lista 
     * allegati
     */
    void open_file();
    
    /*
     * Elimina i file dalla lista allegati
     */
    void erase_file();
    
    //Inserimento file_allegati
    void inserisci_allegati(QString file_name,int riga);
    
    //Visualizza file allegati
    void vis_allegati();
    //Visualizzo listino
    void listino();
    //Inserimento e aggionamento riepilogo agenti
    void inserisci_agenti(QString m_agente, QDate date);
    void aggiorna_agenti(QString m_agente, QDate date);
    //Importo righe da file excel
    void importa();
    void import_file_excel();
    void aggiorna_file_excel(QString doc);
    //Aggiungo riga da tabella
    void addRow();
    bool InserisciRiga();
    void submit();
    //Aggiungo righe
    void insert_scarico_prod();
    void update_articoli(double qua, double _p_siva, double _p_civa, double _tot);
    void update_carico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    void update_scarico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    //recupero dati dalla QTableView
    QString getFATTURAID();
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
private:
    QLocale *local_settings;
    QString tipo_fattura;
    int idfatt_new;
    QString name;
    bool found;
    double prezzos_iva,prezzo_c_iva,perc,totale;

signals:
    void salvafattura();
    void savefatt();
protected:
    bool eventFilter(QObject *o, QEvent *j);
};

#endif // FATTURA_RG_H
