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
#ifndef PREVENTIVI_RG_H
#define PREVENTIVI_RG_H

#include <QDialog>
#include <QtSql>
#include "ui_preventivi_rg.h"

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

using namespace std;
using namespace QXlsx;

namespace Ui {
class preventivi_rg;
}

class preventivi_rg : public QDialog, public Ui::preventivi_rg
{
    Q_OBJECT
    
public:
    explicit preventivi_rg(QWidget *parent = 0);
    ~preventivi_rg();
    QSqlRelationalTableModel *mod_grid;
    
public slots:
    void formatta_num();
    void cliente_add();
    void open_cliente();
    void open_fornitore();
    void combo_fornitore();
    int open_cl(int index);
    int open_fl(int index);
    void filtro();
    void pagamento_prev();
    void modifica_riga();
    void modifica_riga_one(QModelIndex index);
    void elimina_riga();
    void elimina();
    void sconto_1t(const QString &);
    void sconto_2t(const QString &);
    void sconto_3t(const QString &);
    void spesa_trasp(const QString &);
    void spesa_in(const QString &);
    void prev_aggiorna();
    void tot_imp_iva();
    void totale_impo_iva();
    void prev_inserisci();
    void lista_prodotti();
    void insert_f_prodotti();
    void update_f_prodotti();
    void clickgrid();
    void prev_riga_open();
    void chiudi();
    //Causali di trasporto
    void trasp_causali();
    //Porto
    void porto();
    //Aspetto dei beni
    void aspetto_beni();
    //Consegna
    void consegna_trasp();
    //Combo Listino
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
    void insert_scarico_prod();
    void update_articoli(double qua, double _p_siva, double _p_civa, double _tot);
    void update_carico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    void update_scarico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    //recupero dati dalla QTableView
    QString getPREV();
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
    bool found;
    double prezzos_iva,prezzo_c_iva,perc,totale;

signals:
    void salva_prev();
protected:
    bool eventFilter(QObject *o, QEvent *j);
};

#endif // PREVENTIVI_RG_H
