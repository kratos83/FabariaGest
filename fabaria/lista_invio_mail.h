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
#ifndef LISTA_INVIO_MAIL_H
#define LISTA_INVIO_MAIL_H

#include <QDialog>
#include <QtGui>
#include <QtSql>
#include <QCompleter>
#include <QKeyEvent>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>
#include "queryexception.h"
#include "print.h"
#include "ui_lista_invio_mail.h"

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

#include <iostream>


namespace Ui
{
class lista_invio_mail;
}

class lista_invio_mail : public QDialog
{
    Q_OBJECT
public:
    lista_invio_mail(QWidget *parent=0);
    ~lista_invio_mail();

private:
    Ui::lista_invio_mail* ui;
    QSqlRelationalTableModel *mod_grid;
    QAction *esporta_excel,*export_csv, *esporta_pdf;
    QMenu *menu;
    
public slots:
    void lista();
    void clickgrid();
    void cerca();
    void anteprima();
    void preview_new(QPrinter*);
    void cerca_utente(const QString &);
    void filtro();
    void stampa_pdf();
    void esporta_cvs();
    void esporta_excel_email();
};

#endif // LISTA_INVIO_MAIL_H
