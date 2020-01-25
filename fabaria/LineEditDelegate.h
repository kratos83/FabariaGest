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
#ifndef LineEditDelegate_H
#define LineEditDelegate_H

#include <QItemDelegate>
#include <QtWidgets>
#include <QtSql>
#include "frame.h"

/****************************************************************************
* QLineEdit delegate
*****************************************************************************/
class LineEditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    LineEditDelegate(QTableView *view,QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

private slots:
    void commitAndCloseEditor(const QString &);
    void cerca_txt();
    void cerca_id();
private:
    QTableView *table;
    QLabel *cl_popup;
signals:
    void editingFinished();
};

/****************************************************************************
* Box unità di misura
*****************************************************************************/

class combo_um : public QItemDelegate
{
    Q_OBJECT

public:
    combo_um(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                     const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

};


/****************************************************************************
* Box Iva
*****************************************************************************/

class Iva : public QItemDelegate
{
    Q_OBJECT

public:
    Iva(QObject *parent1 = 0);

    QWidget *createEditor(QWidget *parent1, const QStyleOptionViewItem &option1,
                          const QModelIndex &index1) const;

    void setEditorData(QWidget *editor1, const QModelIndex &index1) const;
    void setModelData(QWidget *editor1, QAbstractItemModel *model1,
                     const QModelIndex &index1) const;

    void updateEditorGeometry(QWidget *editor1,
        const QStyleOptionViewItem &option1, const QModelIndex &index1) const;
};

#endif // LineEditDelegate_H

