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
#ifndef AZIENDA_H
#define AZIENDA_H

#include <QDialog>
#include <QtSql>
#include <QKeyEvent>
#include <QtWidgets>
#include "ui_azienda.h"
#include "settingsmanager.h"
#include "comuni.h"

namespace Ui {
    class azienda;
}

class azienda : public QDialog, public Ui::azienda
{
    Q_OBJECT

public:
    azienda(QWidget *parent = nullptr);
    ~azienda();
    bool eventFilter(QObject *o, QEvent *e);

private:
    QSqlQueryModel *mod_grid;
    SettingsManager *manager;
    QAction *m_cap, *m_comuni;
    Comuni *_comune;
private slots:
    void inserisci();
    void esci();
    void cerca();
    void cerca_query();
    void elimina();
    void clickgrid();
    void clearText();
    void nuovo();
    void aggiorna();
    void inst_cap();
    void instPaese();
    void inst_paese(QString m_com, QString m_pr, QString m_cp);
    void lista();
    void azienda_predefinita();
    void messaggioAzienda();
    void leggi_codFis(const QString &);
    void leggi_codPartIva(const QString &);
signals:
    void readReady(QString name);
};

#endif // AZIENDA_H
