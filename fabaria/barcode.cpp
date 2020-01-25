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
#include "barcode.h"
#include <QDebug>
#include <QSettings>

barcode::barcode(QWidget *parent) :
    QGroupBox(parent)
{
    setGeometry(70,70, 70, 70) ;
    barcode1 = new EAN13(this);
}

/********************************************************************************/
void barcode::resizeEvent(QResizeEvent * /* event */)
{
}

/********************************************************************************/
void barcode::paintEvent(QPaintEvent *event)
{
Q_UNUSED(event)		// evade compiler warning
QPainter painter(this);
painter.setRenderHint(QPainter::Antialiasing, true);

draw(&painter);
}

/********************************************************************************/
void barcode::draw(QPainter *painter)
{
barcode1->makePattern(barcodeStr);

qreal scale = painter->device()->physicalDpiX() / 25.4;	// dots per mm
barcode1->draw(QRectF(14, 50, 37.29 * scale, 25.93 * scale), painter);
}

/********************************************************************************/
void barcode::updateSwtClicked(QString code)
{

barcodeStr = code;
update() ;
}

/********************************************************************************/
void barcode::printSwtClicked()
{
QPrinter *printer = new QPrinter() ;
int Wd, Ht ;

if(width() > height())
 printer->setOrientation(QPrinter::Landscape) ;
else
 printer->setOrientation(QPrinter::Portrait) ;

Wd = printer->width() ;
Ht = printer->height() ;

QPainter *painter = new QPainter(this);
painter->begin(printer);

barcode1->makePattern(barcodeStr);

painter->drawText(50,30,"EAN13 Barcode test generator");

painter->drawText(50,70,"Arbitrary size");
barcode1->draw(QRectF(50, 80, Wd - 100, Ht - 300), painter) ;

painter->drawText(50, Ht - 160,"Nominal size (37.29mm x 25.93mm)");
qreal scale = painter->device()->logicalDpiX() / 25.4;	// dots per mm
barcode1->draw(QRectF(50, Ht - 150, 37.29 * scale, 25.93 * scale), painter);

painter->end();
}

/********************************************************************************/

barcode::~barcode()
{
}

