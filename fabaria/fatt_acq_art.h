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
#ifndef FATT_ACQ_ART_H
#define FATT_ACQ_ART_H

#include <QDialog>
#include <QtGui>
#include <QCompleter>
#include <QLocale>
#include <QStatusBar>
#include "ui_fatt_acq_art.h"
#include "fatt_new.h"
#include "fatt_acq.h"

namespace Ui {
    class fatt_acq_art;
}

class fatt_acq_art : public QDialog, public Ui::fatt_acq_art
{
    Q_OBJECT

public:
    explicit fatt_acq_art(QWidget *parent = 0);
    fatt_acq_art(QWidget *parent,QString fattura_tipo, int id);
    ~fatt_acq_art();

private:
    QCompleter *complete;
    QLocale *local_settings;
    fatt_new *fatt;
    fatt_acq *acq;
    QString tipo_fattura_acq;
    bool found;
    QStatusBar *bar;
    int id_fattura_acq;
private slots:


public slots:
    void interfaccia();
    void inserisci_riga_art();
    void inserisci();
    void aggiorna_riga();
    void complete_tot(const QString &);
    void text(const QString &);
    void sconto_tot(const QString &);
    void prezzosiva(const QString &);
    void formattanum();
    void combo_iva();
    void combo_um();
    void nuovo();
    void cerca();
    void clickgrid();
    void combo_categ();
    void insert_articoli();
    void insert_carico_magazzino();
    void update_articoli(double qua, double _p_siva, double _p_civa, double _tot);
    void update_carico_magazzino(double ca, double _p_siva, double _p_civa, double _tot  );
    void p_iv(const QString &);
    void cerca_codart(const QString &);
    void clear_testo();
    void listino();
    void indexComboMisura(QString combo);
    
signals:
    void save_fatt();
};

#endif // FATT_ACQ_ART_H
