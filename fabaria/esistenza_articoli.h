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
#ifndef ESISTENZA_ARTICOLI_H
#define ESISTENZA_ARTICOLI_H

#include <QDialog>
#include <QModelIndex>
#include <QtSql>
#include <QFileDialog>
#include <QCompleter>
#include "../qwt/qwt_plot.h"
#include "../qwt/qwt_plot_curve.h"
#include "../qwt/qwt_plot_grid.h"
#include "../qwt/qwt_symbol.h"
#include "../qwt/qwt_legend.h"
#include "../qwt/qwt_plot_canvas.h"
#include "../qwt/qwt_plot_panner.h"
#include "../qwt/qwt_plot_zoomer.h"
#include "../qwt/qwt_plot_renderer.h"

namespace Ui {
class esistenza_articoli;
}

class esistenza_articoli : public QDialog
{
    Q_OBJECT

public:
    explicit esistenza_articoli(QWidget *parent = 0);
    ~esistenza_articoli();

public slots:
    void vis_quantita(QModelIndex *index, QSqlQueryModel *dataset );
    void esporta();
    void add_text(QString testo);
    
private:
    Ui::esistenza_articoli *ui;
    QwtPlotCurve *m_curve;
};
#endif // ESISTENZA_ARTICOLI_H
