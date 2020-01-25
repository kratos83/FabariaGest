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
#include "resizedialog.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QSpacerItem>
#include <QSettings>

#include <iostream>
#include "pref.h"
#include "settingsmanager.h"
#include "defines.h"

using namespace Defines;

ResizeDialog::ResizeDialog(QWidget *parent, QSize size)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(qApp->translate("ResizeDialog","Ridimensiona immagine"));

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    QGridLayout * topLayout = new QGridLayout();
    QGridLayout * buttonLayout = new QGridLayout();

    labelWidth = new QLabel(qApp->translate("ResizeDialog","Lunghezza"), this);
    spinWidth = new QSpinBox(this);
    spinWidth->setMaximum(10000);

    labelHeight = new QLabel(qApp->translate("ResizeDialog","Altezza"), this);
    spinHeight = new QSpinBox(this);
    spinHeight->setMaximum(10000);

    comboQuality = new QComboBox(this);
    comboQuality->addItem(qApp->translate("ResizeDialog","Media")); //index 0 or AVERAGE
    comboQuality->addItem(qApp->translate("ResizeDialog","Bilineare")); //index 1 or BILINEAR

    checkKeepRatio = new QCheckBox(qApp->translate("ResizeDialog","Mantenere proporzioni"), this);
    checkKeepRatio->setChecked(true);

    resetButton = new QPushButton(qApp->translate("ResizeDialog","Re&set"), this);
    cancelButton = new QPushButton(qApp->translate("ResizeDialog","&Cancella"), this);
    resizeButton = new QPushButton(qApp->translate("ResizeDialog","&Ridimensiona"), this);
    resizeButton->setDefault(true);

    topLayout->addWidget(labelWidth, 1, 0);
    topLayout->addWidget(spinWidth, 1, 1);
    topLayout->addWidget(new QLabel(qApp->translate("ResizeDialog","pixels"), this), 1, 2);

    topLayout->addWidget(labelHeight, 2, 0);
    topLayout->addWidget(spinHeight, 2, 1);
    topLayout->addWidget(new QLabel(qApp->translate("ResizeDialog","pixels"), this), 2, 2);

    QLabel *q = new QLabel(qApp->translate("ResizeDialog","Filtro"), this);
    topLayout->addWidget(q, 3, 0);
    topLayout->addWidget(comboQuality, 3, 1);

    topLayout->addWidget(checkKeepRatio, 4, 0, 1, 3);

    buttonLayout->addWidget(resetButton, 0, 0);
    buttonLayout->addWidget(cancelButton, 0, 1);
    buttonLayout->addWidget(resizeButton, 0, 2);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addLayout(topLayout);
    mainLayout->addSpacerItem(spacer);
    mainLayout->addLayout(buttonLayout);

    setTabOrder(spinWidth, spinHeight);
    setTabOrder(spinHeight, checkKeepRatio);
    setTabOrder(checkKeepRatio, comboQuality);

    adjustSize();
    spinWidth->setFocus();

    connect(spinWidth, SIGNAL(valueChanged(int)), this, SLOT(wChanged(int)));
    connect(spinHeight, SIGNAL(valueChanged(int)), this, SLOT(hChanged(int)));
    connect(spinWidth, SIGNAL(editingFinished()), this, SLOT(checkRatioW()));
    connect(spinHeight, SIGNAL(editingFinished()), this, SLOT(checkRatioH()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(resizeButton, SIGNAL(clicked()), this, SLOT(accept()));

    setDefaultSize(size);
    setFixedSize(300,160);
    wModified = false;
    hModified = false;
}

ResizeDialog::~ResizeDialog()
{
}

void ResizeDialog::setDefaultSize(QSize size)
{
    dWidth = size.width();
    spinWidth->setValue(dWidth);
    dHeight = size.height();
    spinHeight->setValue(dHeight);

    ratio = (double)dWidth/(double)dHeight;
}

void ResizeDialog::reset()
{
    spinWidth->setValue(dWidth);
    spinHeight->setValue(dHeight);
}

void ResizeDialog::accept()
{
    if(spinWidth->value() == dWidth && spinHeight->value() == dHeight)
        close();    //Nothing changed
    else {
        emit resizeImage(QSize(spinWidth->value(), spinHeight->value()), comboQuality->currentIndex());
        settingsManager->setGeneralValue("larghezza",spinWidth->value());
        settingsManager->setGeneralValue("altezza",spinHeight->value());
        close();
    }
}

void ResizeDialog::checkRatioW()
{
    if (checkKeepRatio->isChecked() && wModified) {
        double res = (double)(spinWidth->value()) / ratio;
        spinHeight->setValue((int)(res+0.5));
        hModified = false;
        wModified = false;
    }
}

void ResizeDialog::checkRatioH()
{
    if (checkKeepRatio->isChecked() && hModified) {
        double res = (double)(spinHeight->value()) * ratio;
        spinWidth->setValue((int)(res+0.5));
        hModified = false;
        wModified = false;
    }
}

void ResizeDialog::wChanged(int)
{
    wModified = true;
}

void ResizeDialog::hChanged(int)
{
    hModified = true;
}
