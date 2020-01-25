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
#ifndef MISURE_H
#define MISURE_H

#include <QDialog>
#include <QtGui>
#include <QtSql/QSqlRelationalTableModel>
#include <QKeyEvent>
#include <QCompleter>
#include <QMessageBox>
#include <QMenu>

namespace Ui {
class misure;
}

class misure : public QDialog
{
    Q_OBJECT
    
public:
    explicit misure(QWidget *parent = 0);
    ~misure();
    //Funzione o modulo reiterativo alle
    //funzioni di tastiera
    bool eventFilter(QObject *o, QEvent *e);
    
private:
    Ui::misure *ui;
    QSqlRelationalTableModel *mod_grid;

public slots:
    void salva();
    void aggiorna();
    void elimina();
    void nuovo();
    void cerca();
    void clickgrid();
    void lista();
    void filtro();
    void cerca_libri(const QString &);
    void inserisci_lt_pc_bc(const QString &);
    void mousePressEvent(QMouseEvent *event);
    void Popup(const QPoint& pt);
    void clean_all();
    
signals:
    void add_misure();
};

#endif // MISURE_H
