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
#ifndef USER_H
#define USER_H

#include <QDialog>
#include <QtSql/QtSql>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

namespace Ui {
    class user;
}

class user : public QDialog {
    Q_OBJECT
public:
    user(QWidget *parent = 0);
    ~user();
    QString versione;
    bool eventFilter(QObject *o, QEvent *e);

public slots:
    void del_rec();
    void save();
    void lista();
    void clickgrid();
    void cerca();
    void inserisci();
    void on_esci_clicked();
    void nuovo();
    void somma();
    void anteprima();
    void preview_new(QPrinter*);
    void cerca_utente(const QString &);
    void filtro();
    void stampa_pdf();
    void esporta_cvs();
    void mousePressEvent(QMouseEvent *);
    void Popup(const QPoint& pt);
    void esporta_excel_clienti();
    void clear_text();
    void inst_cap();
    void inst_paese();
    bool per_giud_fis_ced(bool vero);
    void message();
    void leggi_codFis(const QString &);
    void leggi_codPartIva(const QString &);

private:
    Ui::user *ui;
    QSqlRelationalTableModel *mod_grid;
    QAction *esporta_pdf,*esporta_excel,*export_csv;
    QMenu *menu;

signals:
    void clienti_rel();
};

#endif // USER_H

