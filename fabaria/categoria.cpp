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
#include "categoria.h"
#include "ui_categoria.h"
#include <QSqlQuery>
#include <QtSql>
#include <QModelIndex>
#include <QDebug>
#include <QMessageBox>
#include "itdelegato.h"
#include "defines.h"
#include "queryexception.h"
#include "settingsmanager.h"
#include "databasesql.h"

using namespace std;
using namespace Defines;
using namespace DatabaseSql;

porto::porto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::porto)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    //Chiamata della tastiera per uscire da categorie
    ui->esci->setShortcut(tr("Ctrl+E"));
    //Chiamata della tastiera per salvare e aggiornare i dati nel db
    ui->salva->setShortcut(tr("Ctrl+S"));
    connect(ui->salva,SIGNAL(clicked()),this,SLOT(aggiorna()));
    //Chiamata della tastiera per eliminare i dati dal db
    ui->elimina->setShortcut(tr("Ctrl+D"));
    connect(ui->elimina,SIGNAL(clicked()),this,SLOT(elimina()));
    connect(ui->nuovo,SIGNAL(clicked()),this,SLOT(nuovo()));
    //Chiamata della tastiera per inserire una nuova categoria
    ui->nuovo->setShortcut(tr("Ctrl+N"));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(ui->search,SIGNAL(textEdited(QString)),this,SLOT(cerca_libri(QString)));
    connect(ui->cerca_categ,SIGNAL(clicked()),this,SLOT(filtro()));
    mod_grid = new QSqlRelationalTableModel;
    ui->id->setEnabled(false);
    lista();
    nuovo();
}

void porto::nuovo(){
    if(selectQuery("select count(categ)+1 from categoria",0).toString() == "0")
            ui->id->setText("1");
    else {
        QString text = selectQuery("select count(categ)+1 from categoria",0).toString();
        ui->id->setText(text);
    }
    ui->lineEdit->setText("");
}

void porto::cerca_libri(const QString &){

    QString text;
    text = ui->search->text();
    QStringList list_st;
    if(ui->comboBox->currentIndex() == 0){
        list_st << selectQueryWhile("select tipo_categ from categoria",0);
    }
    else if(ui->comboBox->currentIndex() == 1){
        list_st << selectQueryWhile("select categ from categoria",0);
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->search->setCompleter(complete);
    if(text.length() == 0){
        lista();
    }
}

void porto::filtro(){
    if(ui->search->text().length() == 0){
        lista();
    }
    else if(ui->comboBox->currentIndex() == 0){
        mod_grid->setFilter(QString("tipo_categ = '" +ui->search->text()+"'").arg(ui->search->text()));
    }
    else if(ui->comboBox->currentIndex() == 1){
        mod_grid->setFilter(QString("categ = '" +ui->search->text()+"'").arg(ui->search->text()));
    }
}

void porto::aggiorna(){

    if(ui->id->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("porto","Inserisci correttamente i dati"));
    }
    else{
    QSqlQuery querys;
    if(!ui->id->text().isEmpty()){

    querys.prepare("select categ from categoria where categ='"+ui->id->text()+"'");
    querys.exec();
    try{
    if(querys.next()){
        updateQuery("UPDATE categoria SET tipo_categ = '"+ui->lineEdit->text()+"' "
                      " where categ='"+ui->id->text()+"'","categoria");
    }
    else{
        salva();
    }
    }
    catch(FabariaException e)
    {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    }
    }
    lista();
    nuovo();

}

void porto::clearText()
{
    ui->id->clear();
    ui->lineEdit->clear();
}

void porto::salva(){
    insertQuery("INSERT INTO categoria VALUES('"+ui->id->text()+"','"+ui->lineEdit->text()+"')","categoria");    
    nuovo();
    lista();
}

void porto::elimina(){

        if (ui->tableView->selectionModel()->currentIndex().isValid())
        {
            eliminaQuery("DELETE FROM categoria WHERE categ='"+ui->id->text()+"'","categoria");
        }
        else{
            QMessageBox MsgBox;
            MsgBox.setText(qApp->translate("porto","Errore"));
            MsgBox.setInformativeText(qApp->translate("porto","Seleziona una riga per eliminarla..."));
            MsgBox.setIcon(QMessageBox::Warning);
            MsgBox.exec();
        }

    clearText();
    lista();
    nuovo();
}

void porto::clickgrid(){
    QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex index = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(index).toString();
    ui->id->setText(id_new);

    cerca();
}

void porto::cerca(){
    ui->id->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString());
    ui->lineEdit->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString());
}

void porto::lista(){

    mod_grid->setTable("categoria");
    mod_grid->select();
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("porto","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("porto","Tipo di categoria"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setModel(mod_grid);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setItemDelegateForColumn(2,new coldxdelegato(this));
}
 
bool porto::eventFilter(QObject *o, QEvent *e){

    if (o==ui->tableView && e->type() == QEvent::KeyPress) {
                        QKeyEvent* ke = (QKeyEvent*)e;
                        if (ke->key() == Qt::Key_Delete) {
                            // ESEGUO IL CODICE DI ELIMINAZIONE
                            elimina();
                        }
                        if(ke->key() == Qt::Key_S){
                            //ESEGUO IL CODICE PER INSERIRE
                            salva();
                        }
                        if(ke->key() == Qt::Key_U){
                            //ESEGUO IL CODICE PER AGGIORNARE
                            aggiorna();
                        }
                        if(ke->key() == Qt::Key_N){
                            //ESEGUO IL CODICE PER UN NUOVO ARTICOLO
                            nuovo();
                        }
                        ke->accept();
                        return true;
                    }

            if( o==ui->tableView && e->type() == QEvent::ContextMenu)
                    {
                        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (e);
                        this ->Popup(mouseEvent->pos());
                        return false;
            }
            
            return QDialog::eventFilter(o,e);
}

void porto::mousePressEvent(QMouseEvent *event){

    if (event->button() == Qt::RightButton) {

                 this->Popup(event ->pos());
                 event->accept();
             }
}

void porto::Popup(const QPoint& pt){

    QPoint global = this ->mapToGlobal(pt);
    QMenu* pPopup = new QMenu(this);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Immagini per il menu a tendina
    ************************************************************/
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/dialog-close.png"));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/edit-delete.png"));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/images/filenew.png"));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/images/document-save.png"));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/images/view-refresh.png"));

    /***********************************************************
    *  @author Angelo Scarnà
    *  Menu a tendina
    ************************************************************/
    QAction* pAction1 = new QAction(qApp->translate("porto","Chiudi"), this);
    pAction1->setIcon(icon);
    pAction1->setIconVisibleInMenu(true);
    QAction* pAction2 = new QAction(qApp->translate("porto","Elimina"), this);
    pAction2->setIcon(icon1);
    pAction2->setIconVisibleInMenu(true);
    QAction* nw = new QAction(qApp->translate("porto","Nuovo"),this);
    nw->setIcon(icon2);
    nw->setIconVisibleInMenu(true);
    QAction* sa = new QAction(qApp->translate("porto","Salva"),this);
    sa->setIcon(icon3);
    sa->setIconVisibleInMenu(true);
    QAction* md = new QAction(qApp->translate("porto","Aggiorna"),this);
    md->setIcon(icon4);
    md->setIconVisibleInMenu(true);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Instanzio il menu
    ************************************************************/
    pPopup->addAction(nw);
    pPopup->addSeparator();
    pPopup->addAction(sa);
    pPopup->addSeparator();
    pPopup->addAction(md);
    pPopup->addSeparator();
    pPopup->addAction(pAction2);
    pPopup->addSeparator();
    pPopup->addAction(pAction1);

    /***********************************************************
    *  @author Angelo Scarnà
    *  Imposto la connessione ai moduli
    ************************************************************/
    QAction* pItem = pPopup ->exec(global);

    if(pItem == pAction1)
    {
        close();
    }
    else if(pItem == pAction2)
    {
        elimina();
    }
    else if(pItem == nw){
        nuovo();
    }
    else if(pItem == sa)
    {
        salva();
    }
    else if(pItem == md)
    {
        aggiorna();
    }
}
porto::~porto()
{
    delete ui;
}
