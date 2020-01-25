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
#ifndef PREVENTIVI_H
#define PREVENTIVI_H

#include <QDialog>
#include <QtGui>
#include <QtSql>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>
#include "email_send.h"
#include "ui_preventivi.h"
#include "preventivi_rg.h"

namespace Ui {
class preventivi;
}

class preventivi : public QWidget, public Ui::preventivi
{
    Q_OBJECT
    
public:
    explicit preventivi(QWidget *parent = 0);
    ~preventivi();
    

public slots:

    void nuovo_prev_prodotti();
    void lista_prev();
    void stampa_prev();
    void preview_view(QPrinter *printer);
    void elimina_prev();
    void elimina_riga();
    void vis_dati_prev_prodotti();
    void cerca_prev();
    void aggiorna_prev_prodotti();
    void agg_prev_prodotti(QModelIndex index);
    void clickgrid_prodotti();
    void filtro();
    void cerca_dxt_prodotti(const QString &);
    void esporta_pdf_prodotti();
    void stampa_prodotti();
    void preview_prodotti(QPrinter *printer);
    void trasforma(QString fattura);
    void Popup(const QPoint& pt);
    void tr_ordine(QString ordine);
    void tr_fattura(QString fattura);
    void invia_email();
    void insert_all_email(double id, QString fileName);
    void update_all_email(double id, QString fileName);
    
private:
    QSqlRelationalTableModel *mod_grid;
    QSortFilterProxyModel *model;
    QAction* pAction1,*pAction2,*pAction3,*send_mail;
    QMenu *menu;
    preventivi_rg *prev_rg;
    email_send *email;
};

extern preventivi *prev_wid;
#endif // PREVENTIVI_H
