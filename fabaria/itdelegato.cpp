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
#include "itdelegato.h"
#include <QDate>


/*--------------------------------------------------------------------------------------------
Colonna numerica allineata a destra senza decimali
--------------------------------------------------------------------------------------------*/

coldxdelegato::coldxdelegato(QObject *parent)
    :QItemDelegate(parent)
{

}

void coldxdelegato::paint(QPainter *painter,
                 const QStyleOptionViewItem & option,
                 const QModelIndex & index) const
{

    QString text = index.model()->data(index, Qt::DisplayRole).toString();
    QStyleOptionViewItem myOption = option;
    myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;

    drawDisplay(painter, myOption, myOption.rect, text);
    drawFocus(painter, myOption, myOption.rect);

}

/*--------------------------------------------------------------------------------------------
Colonna numerica allineata a destra con decimali
--------------------------------------------------------------------------------------------*/

ColNumDelegato::ColNumDelegato(QObject *parent)
    :QItemDelegate(parent)
{

}

void ColNumDelegato::paint(QPainter *painter,
                 const QStyleOptionViewItem & option,
                 const QModelIndex & index) const
{

    double numero = index.model()->data(index, Qt::DisplayRole).toDouble();

    QString text = QString("%L1").arg(numero,13,'f',2);

    QStyleOptionViewItem myOption = option;
    myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;

    drawDisplay(painter, myOption, myOption.rect, text);
    drawFocus(painter, myOption, myOption.rect);

}


/*--------------------------------------------------------------------------------------------
Colonna data formattata
--------------------------------------------------------------------------------------------*/

ColDataDelegato::ColDataDelegato(QObject *parent)
    :QItemDelegate(parent)
{

}

void ColDataDelegato::paint(QPainter *painter,
                 const QStyleOptionViewItem & option,
                 const QModelIndex & index) const
{

    QDate text = index.model()->data(index, Qt::DisplayRole).toDate();

    QStyleOptionViewItem myOption = option;
    myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;

    drawDisplay(painter, myOption, myOption.rect, text.toString("dd-MM-yyyy"));
    drawFocus(painter, myOption, myOption.rect);

}
