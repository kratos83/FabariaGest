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
#ifndef REG_IVA_H
#define REG_IVA_H

#include <QDialog>
#include <QtSql>

#include <QtGui>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>

#include "settingsmanager.h"

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"
using namespace std;
using namespace QXlsx;

namespace Ui {
class reg_iva;
}

class reg_iva : public QDialog
{
    Q_OBJECT

public:
    explicit reg_iva(QWidget *parent = 0);
    ~reg_iva();
    
public slots:
    void stampa_registro();
    void preview_view(QPrinter *printer);
    void stampa_mese_print(QPrinter *print, QString numero, int id);
    void stampa_three_month_print(QPrinter *print, QString numero, int id);
    void stampa_year_print(QPrinter *print, QString numero, int id); 
    void stampa_mese_excel(Document &excel, QString numero, int id);
    void stampa_three_month_excel(Document &excel, QString numero, int id);
    void stampa_year_excel(Document &excel, QString numero, int id);
    void export_pdf();
    void export_excel();
    void filtro();
    void add_combo_mese();
    void add_combo_three_month();
    void excel_fattura_acq(Document &excel, QSqlQuery query_mese_acq, QString fattura);
    void excel_fattura_vnd(Document &excel, QSqlQuery query_mese_vnd, QString fattura);

private:
    Ui::reg_iva *ui;
    QAction *esporta_excel,*esporta_pdf,*stampa;
    QMenu *menu;
    SettingsManager *settings;
    QString data,id_fatt;
    QSqlRelationalTableModel *mod_grid_acq, *mod_grid_vnd;
};

#endif // REG_IVA_H
