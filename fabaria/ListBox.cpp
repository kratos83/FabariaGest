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
#include <QtWidgets>
#include <QFileIconProvider>
#include "ListBox.h"
#include "IconFileProvider.h"
#include "user.h"
#include "defines.h"
Q_LOGGING_CATEGORY(FastAssListBox, "fastass")

using namespace Defines;

ListBox::ListBox(QWidget* parent) 
    : QListWidget(parent)
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(55, 55));
    setDragEnabled(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDefaultDropAction(Qt::CopyAction);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    label = new QLabel(viewport());
    label->setGeometry(8, 34, 250, 300);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setAlignment(Qt::AlignCenter);
    label->setText(qApp->translate("ListBox","Trascina qua i file"));
    connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(Popup(const QPoint &)));
    connect(this,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(changePage(QListWidgetItem*)));
    connect(this,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(ReadPdf(QListWidgetItem*)));
    setContextMenuPolicy(Qt::CustomContextMenu);
}
 
void ListBox::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QUrl url;
            foreach (url,urls)
            {
                    label->clear();
                    QFileInfo info(url.toLocalFile());                
                    IconFileProvider provider;
                    QIcon icon(provider.getIcon(info));
                    QString name = info.fileName();
                    new QListWidgetItem(icon, name, this);
            }
        }
        event->acceptProposedAction();
    }
    QListWidget::dropEvent(event);
}

void ListBox::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    } else {
        QListWidget::dragEnterEvent(event);
    }
}
 
void ListBox::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    } else {
        QListWidget::dragMoveEvent(e);
    }
}

Qt::DropAction ListBox::supportedDropActions()
{
    return Qt::CopyAction;
}

void ListBox::changePage(QListWidgetItem *current){
    current->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    setItem(current->text());
}

void ListBox::setItem(QString item)
{
    m_item = item;
}

QString ListBox::getItem()
{
    return m_item;
}

void ListBox::eliminaItem()
{
    takeItem(currentRow());
    if(count() == 0)
        labelText();
}

void ListBox::labelText()
{
    label->setText(qApp->translate("ListBox","Trascina qua i file"));
}

void ListBox::mousePressEvent(QMouseEvent* event)
{
    /******************************************
      * Imposta il tasto destro per il mouse
      * riproducendo un menu contestuale
      *****************************************/
    if (event->button() == Qt::RightButton) {
        Popup(event ->pos());
        event->accept();
    }
    QListWidget::mousePressEvent(event);
}

void ListBox::Popup(const QPoint& pt)
{
        /**************************************************
        * Imposta la variabile pt per abilitare il
        * menu.
        * @param global = mapToGlobal(pt)
        * @author Angelo Scarnà
        *************************************************/
        QPoint global = this ->mapToGlobal(pt);
        QMenu* pPopup = new QMenu(this);

        /***********************************************************
        *  @author Angelo Scarnà
        *  Immagini per il menu a tendina
        ************************************************************/
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/edit-delete.png"));
        //QIcon icon_file;
        //icon.addFile(QString::fromUtf8(":/images/edit-delete.png"));

        /***********************************************************
        *  @author Angelo Scarnà
        *  Menu a tendina
        ************************************************************/
        QAction* pAction1 = new QAction(qApp->translate("ListBox","Elimina"), this);
        pAction1->setIcon(icon);
        pAction1->setIconVisibleInMenu(true);

        /***********************************************************
        *  @author Angelo Scarnà
        *  Instanzio il menu
        ************************************************************/
        pPopup->addAction(pAction1);

        /***********************************************************
        *  @author Angelo Scarnà
        *  Imposto la connessione ai moduli
        ************************************************************/
        QAction* pItem = pPopup ->exec(global);
        if(pItem == pAction1){eliminaItem();}
}

//ListBoxEmail
ListBoxEmail::ListBoxEmail(QWidget* parent) 
    : QListWidget(parent)
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(55, 55));
    setDragEnabled(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDefaultDropAction(Qt::CopyAction);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    manager = new SettingsManager(this);
    connect(this,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(changePage(QListWidgetItem*)));
    connect(this,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(ReadPdf(QListWidgetItem*)));
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void ListBoxEmail::changePage(QListWidgetItem *current){
    current->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    setItem(current->text());
}

void ListBoxEmail::setItem(QString item)
{
    m_item = item;
}

QString ListBoxEmail::getItem()
{
    return m_item;
}
