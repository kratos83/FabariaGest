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
#include "LineEdit.h"
#include "defines.h"

LineEdit::LineEdit(QWidget *parent) :
        QLineEdit(parent)
{
        setText(QDate::currentDate().toString("dd/MM/yyyy"));
        setInputMask("00/00/0000");
        m_data = addAction(QIcon(":/images/calendario.png"),QLineEdit::TrailingPosition);
        m_data->setToolTip(qApp->translate("LineEdit","Apri calendario"));
        connect(m_data, &QAction::triggered, this, &LineEdit::setData);
}

void LineEdit::setData()
{
        m_cal = new QDialog(this,Qt::Popup);
        QStyleOptionComboBox opt;
        QRect arrowRect = style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                                                QStyle::SC_ComboBoxArrow, this);
        m_cal->move(mapToGlobal(arrowRect.topLeft()));
        QGridLayout *layout = new QGridLayout(m_cal);
        calendar = new QCalendarWidget;
        calendar->setGridVisible(true);
        layout->addWidget(calendar);
        connect(calendar, &QCalendarWidget::clicked,this,&LineEdit::Data);
        m_cal->exec();
}

void LineEdit::Data(QDate data)
{
    setText(data.toString("dd/MM/yyyy"));
    calendar->hide();
    m_cal->hide();
}

//Completamento in maiuscolo o minuscolo
LineEditComplete::LineEditComplete(QWidget* parent) : 
        QLineEdit(parent), m_complete(0)
{
}

LineEditComplete::~LineEditComplete()
{
}

void LineEditComplete::setCompleter(Completer* complete)
{
    if(m_complete)
        disconnect(m_complete,0,this,0);
    
    m_complete = complete;
    
    if(!m_complete)
        return;
    
    m_complete->setWidget(this);
    
    connect(m_complete,SIGNAL(activated(const QString&)),this,SLOT(insertComplete(const QString &)));
}

Completer * LineEditComplete::completer() const
{
    return m_complete;
}

void LineEditComplete::insertComplete(const QString& com)
{
    setText(com);
    selectAll();
}

void LineEditComplete::keyPressEvent(QKeyEvent* e)
{
    if(m_complete && m_complete->popup()->isVisible())
    {
        switch (e->key())
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
            e->ignore();
            return; // Let the completer do default behavior
        }
    }
    
    bool isShortcut = (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E;
    if (!isShortcut)
        QLineEdit::keyPressEvent(e); // Don't send the shortcut (CTRL-E) to the text edit.

    if (!m_complete)
        return;

    bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!isShortcut && !ctrlOrShift && e->modifiers() != Qt::NoModifier)
    {
        m_complete->popup()->hide();
        return;
    }

    m_complete->update(text());
    m_complete->popup()->setCurrentIndex(m_complete->completionModel()->index(0, 0));
}
