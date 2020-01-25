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
#ifndef FATTURA_ELETTRONICA_H
#define FATTURA_ELETTRONICA_H

#include <QDialog>
#include <QtGui>
#include <QtXml>
#include <QtSql>
#include <QCompleter>
#include <QFileDialog>
#include <QFileIconProvider>
#include "ui_fattura_elettronica.h"

namespace Ui {
class fattura_elettronica;
}

class fattura_elettronica : public QDialog, public Ui::fattura_elettronica
{
    Q_OBJECT
    void combo_cond_pagam();
    
public:
    explicit fattura_elettronica(QWidget *parent, int id, QSqlRelationalTableModel *model, QModelIndex index);
    ~fattura_elettronica();
    
public slots:
    void scrivi_fattura_xml();
    void leggi_nazioni();
    void leggi_cap();
    void leggi_comuni();
    void inst_cap();
    void inst_cap_cess();
    void file_salvato();
    void combo_regime_fiscale();
    void combo_cc_cp();
    void combo_doc();
    void combo_mod_pagam();
    bool vis_sede_legale(bool vero);
    bool vis_dati_ritenuta(bool vero);
    bool per_giud_fis_ced(bool vero);
    bool per_giud_fis_rapp(bool vero);
    bool per_giud_fis_cess(bool vero);
    void inst_cap_2();
    void provincie();
    int tab_number(int numero);
    void caus_pagam_rit();
    void vis_allegati();
private:
    QSqlRelationalTableModel *modello;
    QModelIndex indice;
    int id_fatture;
};

#endif // FATTURA_ELETTRONICA_H
