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
#ifndef PREVENTIVI_WID_H
#define PREVENTIVI_WID_H

#include <QDialog>
#include <QtGui>
#include <QCompleter>
#include <QLocale>
#include <QStatusBar>
#include "preventivi_rg.h"
#include "ui_preventivi_wid.h"

namespace Ui {
class preventivi_wid;
}

class preventivi_wid : public QDialog, public Ui::preventivi_wid
{
    Q_OBJECT
    
public:
    explicit preventivi_wid(QWidget *parent = 0);
    preventivi_wid(QWidget *parent, int id);
    ~preventivi_wid();

public slots:
    void interfaccia();
    void clear_Testo();
    void nuovo();
    void formattanum();
    void comboiva();
    void comboum();
    void combo_categ();
    void cerca_codart(const QString &);
    void complete_tot(const QString &);
    void text(const QString &);
    void sconto_tot(const QString &);
    void prezzosiva(const QString &);
    void p_iv(const QString &);
    void cerca();
    void clickgrid();
    void inserisci_riga_prod();
    void inserisci();
    void aggiorna_riga();
    void insert_scarico_prod();
    void update_articoli(double qua, double _p_siva, double _p_civa, double _tot);
    void update_carico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    void update_scarico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    void listino();
    void indexComboMisura(QString combo);
    
private:
    QCompleter *complete;
    QLocale *local_settings;
    preventivi_rg *prev_rg;
    bool found;
    int m_id;

signals:
    void save_prev();
};

extern preventivi_wid *wid_rg;
#endif // PREVENTIVI_WID_H
