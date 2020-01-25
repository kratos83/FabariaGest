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
#include "LineEditDelegate.h"
#include "mainwindow.h"
#include <QtWidgets>
#include "defines.h"

LineEditDelegate::LineEditDelegate(QTableView *view, QObject *parent)
        : QItemDelegate(parent),table(view) {}

QWidget *LineEditDelegate::createEditor(QWidget *parent,
                                          const QStyleOptionViewItem &,
                                          const QModelIndex &index) const
{
    if(index.column() == 1)
    {
	QLineEdit *editor = new QLineEdit(parent);
        connect(editor, SIGNAL(returnPressed()), this, SLOT(cerca_id()));
        return editor;
    }
    else if(index.column() == 3)
    {
        QLineEdit *editor = new QLineEdit(parent);
        connect(editor, SIGNAL(returnPressed()), this, SLOT(cerca_txt()));
        return editor;
    }
    else{
        QLineEdit *editor = new QLineEdit(parent);
        connect(editor, SIGNAL(textEdited(QString)), this, SLOT(commitAndCloseEditor(QString)));
        return editor;
    }
}

void LineEditDelegate::commitAndCloseEditor(const QString &)
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());        
    emit commitData(editor);
}

void LineEditDelegate::cerca_txt()
{
        QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
        frame *frame_popup = new frame(table,Qt::Popup | Qt::FramelessWindowHint);
        QSqlQuery query;
        query.exec("select nome_articolo from carico_magazzino where nome_articolo='"+editor->text()+"'");
        if(query.next())
        {
            editor->setText(query.value(0).toString());
            QString txt =qApp->translate("LineEditDelegate","Prodotto trovato");
            frame_popup->resize(112,20);
            frame_popup->setStyleSheet("QFrame {border: 1px solid gray; border-radius: 10px; background-color: yellow } QLabel { color: black; border: 1px solid gray; border-radius: 10px;}");
            cl_popup = new QLabel(txt,frame_popup);
            frame_popup->setFocus();
            frame_popup->move(table->mapToGlobal(QPoint(QCursor::pos().x(),QCursor::pos().y()-95)));
        }
        else{
            QString txt =qApp->translate("LineEditDelegate","Prodotto non trovato");
            frame_popup->resize(140,20);
            frame_popup->setStyleSheet("QFrame {border: 1px solid gray; border-radius: 10px; background-color: red } QLabel { color: yellow; border: 1px solid gray; border-radius: 10px;}");
            cl_popup = new QLabel(txt,frame_popup);
            frame_popup->setFocus();
            frame_popup->move(table->mapToGlobal(QPoint(QCursor::pos().x(),QCursor::pos().y()-95)));
        }
        frame_popup->show();
}

void LineEditDelegate::cerca_id()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    frame *frame_popup = new frame(table,Qt::Popup | Qt::FramelessWindowHint);
    QSqlQuery query;
    query.exec("select cod_articolo from carico_magazzino where cod_articolo='"+editor->text()+"'");
    if(query.next())
    {
        editor->setText(query.value(0).toString());
        QString txt =qApp->translate("LineEditDelegate","Prodotto trovato");
        frame_popup->resize(112,20);
        frame_popup->setStyleSheet("QFrame {border: 1px solid gray; border-radius: 10px; background-color: yellow } QLabel { color: black; border: 1px solid gray; border-radius: 10px;}");
        cl_popup = new QLabel(txt,frame_popup);
        frame_popup->setFocus();
        frame_popup->move(table->mapToGlobal(QPoint(QCursor::pos().x(),QCursor::pos().y()-95)));
    }
    else{
        QString txt =qApp->translate("LineEditDelegate","Prodotto non trovato");
        frame_popup->resize(140,20);
        frame_popup->setStyleSheet("QFrame {border: 1px solid gray; border-radius: 10px; background-color: red } QLabel { color: yellow; border: 1px solid gray; border-radius: 10px;}");
        cl_popup = new QLabel(txt,frame_popup);
        frame_popup->setFocus();
        frame_popup->move(table->mapToGlobal(QPoint(QCursor::pos().x(),QCursor::pos().y()-95)));
    }
    frame_popup->show();
}

void LineEditDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
        return;
    }
}

void LineEditDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) {
        model->setData(index, edit->text());
        return;
    }
}

//Box unità di massa
combo_um::combo_um(QObject *parent)
    : QItemDelegate(parent)
{
}
//! [0]

//! [1]
QWidget *combo_um::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & ) const
{
    QStringList lista;
    QSqlQuery query("select unita from misura");
    if(query.exec()){
        while(query.next()){
           lista << query.value(0).toString();
        }
    }
    QComboBox *editor = new QComboBox(parent);
    QCompleter *complete = new QCompleter(lista,parent);
    complete->setCaseSensitivity(Qt::CaseSensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(editor->model());
    editor->setCompleter(complete);
    editor->addItems(lista);
    editor->setEditable(true);
    editor->installEventFilter(const_cast<combo_um*>(this));
    return editor;
}

void combo_um::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();

    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    comboBox->addItem(value);
}

void combo_um::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void combo_um::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

/****************************************************************************
* Box Iva
*****************************************************************************/
Iva::Iva(QObject *parent1)
    : QItemDelegate(parent1)
{
}

QWidget *Iva::createEditor(QWidget *parent1,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & ) const
{
            QStringList list;
            QSqlQuery query("select iva from aliquota");
            if(query.exec()){
            while(query.next()){
                list << query.value(0).toString();
            }
            }
            QCompleter *comp = new QCompleter(list,parent1);
            comp->setCaseSensitivity(Qt::CaseInsensitive);
            comp->setCompletionMode(QCompleter::PopupCompletion);
            QComboBox *edi = new QComboBox(parent1);
            comp->setModel(edi->model());
            edi->setCompleter(comp);
            edi->addItems(list);
            edi->setEditable(true);
            edi->installEventFilter(const_cast<Iva*>(this));
            return edi;
}

void Iva::setEditorData(QWidget *editor1, const QModelIndex &index1) const
{
    QString value = index1.model()->data(index1, Qt::DisplayRole).toString();

    QComboBox *wi = static_cast<QComboBox*>(editor1);
    wi->addItem(value);
}

void Iva::setModelData(QWidget *editor1, QAbstractItemModel *model1,
                                   const QModelIndex &index1) const
{
    QComboBox *wi = static_cast<QComboBox*>(editor1);
    model1->setData(index1, wi->currentText(), Qt::EditRole);
}

void Iva::updateEditorGeometry(QWidget *editor1,
    const QStyleOptionViewItem &option1, const QModelIndex &/* index */) const
{
    editor1->setGeometry(option1.rect);
}
