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
#include "misure.h"
#include "ui_misure.h"
#include "itdelegato.h"
#include "defines.h"
#include "queryexception.h"
#include <QtSql>

using namespace Defines;

misure::misure(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::misure)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->esci_unita,SIGNAL(clicked()),this,SLOT(close()));
    //Chiamata della tastiera per uscire da misure
    ui->esci_unita->setShortcut(qApp->translate("misure","Ctrl+E"));
    //Chiamata della tastiera per salvare e aggiornare i dati nel db
    ui->save_unita->setShortcut(qApp->translate("misure","Ctrl+S"));
    connect(ui->save_unita,SIGNAL(clicked()),this,SLOT(aggiorna()));
    //Chiamata della tastiera per eliminare i dati dal db
    ui->del_unita->setShortcut(qApp->translate("misure","Ctrl+D"));
    connect(ui->del_unita,SIGNAL(clicked()),this,SLOT(elimina()));
    connect(ui->nuova_unita,SIGNAL(clicked()),this,SLOT(nuovo()));
    //Chiamata della tastiera per inserire una nuova unità misura
    ui->nuova_unita->setShortcut(qApp->translate("misure","Ctrl+N"));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(ui->cerca_txt,SIGNAL(textEdited(QString)),this,SLOT(cerca_libri(QString)));
    connect(ui->unita,SIGNAL(textEdited(QString)),this,SLOT(inserisci_lt_pc_bc(QString)));
    connect(ui->cerca_unita,SIGNAL(clicked()),this,SLOT(filtro()));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(Popup(const QPoint &)));
    mod_grid = new QSqlRelationalTableModel;
    ui->id->setEnabled(false);
    ui->matricola->setEnabled(false);
    ui->data_scadenza->setEnabled(false);
    lista();
    nuovo();
}

void misure::nuovo()
{
    QSqlQuery query;
    query.exec("select max((id)+1) from misura");
    QString id_cl;
    if(query.next()){
        if(query.value(0).toInt() == 0){
           ui->id->setText("1");
        }
        else{
           id_cl = (query.value(0).toString());
           ui->id->setText(id_cl);
        }
    }
    ui->unita->setText("");
}

void misure::cerca_libri(const QString &)
{
    if(ui->comboBox->currentIndex() == 0){
    QString text;
    text = ui->cerca_txt->text();
    QStringList list_st;
    QSqlQuery querys("select id from misura");
    if(querys.exec()){
    while(querys.next()){
        list_st << querys.value(0).toString();
    }
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cerca_txt->setCompleter(complete);
    if(text.length() == 0){
        lista();
    }
    }
    else if(ui->comboBox->currentIndex() == 1){
        QString text1;
        text1 = ui->cerca_txt->text();
        QStringList list_st1;
        QSqlQuery querys1("select unita from misura");
        if(querys1.exec()){
        while(querys1.next()){
            list_st1 << querys1.value(0).toString();
        }
        }
        QCompleter *complete1 = new QCompleter(list_st1,this);
        complete1->setCaseSensitivity(Qt::CaseInsensitive);
        ui->cerca_txt->setCompleter(complete1);
        if(text1.length() == 0){
            lista();
        }
    }
}

void misure::filtro()
{
    if(ui->cerca_txt->text().length() == 0){
        lista();
        QMessageBox MsgBox;
        MsgBox.setWindowTitle("Fabaria Gest");
        MsgBox.setText(qApp->translate("misure","Avviso"));
        MsgBox.setInformativeText(qApp->translate("misure","Inserisci il testo nella casella cerca"));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }
    else if(ui->comboBox->currentIndex() == 0){
        mod_grid->setFilter(QString("tipo_categ = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
    }
    else if(ui->comboBox->currentIndex() == 1){
        mod_grid->setFilter(QString("categ = '" +ui->cerca_txt->text()+"'").arg(ui->cerca_txt->text()));
    }
}

void misure::aggiorna()
{
    if(ui->id->text() == ""){
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("misure","Inserisci correttamente i dati"));
    }
    else{
    QSqlQuery querys;
    if(!ui->id->text().isEmpty()){

    querys.prepare("select * from misura where id='"+ui->id->text()+"'");
    querys.exec();
    QString giorno_scadenza = ui->data_scadenza->text().mid(0,2);
    QString mese_scadenza = ui->data_scadenza->text().mid(3, 2);
    QString anno_scadenza = ui->data_scadenza->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    if(querys.next()){
        QSqlQuery query;
        query.prepare("UPDATE misura SET unita ='"+ui->unita->text()+"', "
                    "matricola='"+ui->matricola->text()+"',data_scadenza='"+insieme_scadenza+"'"
                      " where id='"+ui->id->text()+"'");
	try{
        if(query.exec()){
            LOG_FILE(qApp->translate("misure","Aggiornamento effettuato correttamente(misure::aggiorna)"),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE(qApp->translate("misure","Impossibile aggiornare(misure::aggiorna)"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE("(misure::aggiorna): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(query.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
    }
    else{
        salva();
    }
    }
    }
    lista();
    nuovo();
    emit add_misure();
}

void misure::salva()
{
    QString giorno_scadenza = ui->data_scadenza->text().mid(0,2);
    QString mese_scadenza = ui->data_scadenza->text().mid(3, 2);
    QString anno_scadenza = ui->data_scadenza->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    
    QSqlQuery query;
    query.prepare("INSERT INTO misura "
                  "VALUES('"+ui->id->text()+"','"+ui->unita->text()+"','"+ui->matricola->text()+"','"+insieme_scadenza+"')");

    try{
        if(query.exec()){
            LOG_FILE(qApp->translate("misure","Inserimento effettuato correttamente(misure::salva)"),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE(qApp->translate("misure","Impossibile inserire(misure::salva)"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE("(misure::salva): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(query.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}

    lista();
    nuovo();
    emit add_misure();
}

void misure::elimina()
{
    if (ui->tableView->selectionModel()->currentIndex().isValid())
    {
        QSqlQuery qctrl;
        qctrl.prepare("SELECT * FROM misura WHERE id = '"+ui->id->text()+"'");
        qctrl.exec();

	QSqlQuery query;
        query.prepare("DELETE FROM misura WHERE id = '"+ui->id->text()+"'");

        try{
        if(query.exec()){
            LOG_FILE(qApp->translate("misure","Eliminazione effettuata correttamente(misure::elimina)"),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE(qApp->translate("misure","Impossibile eliminare(misure::elimina)"),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE("(misure::elimina): "+query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
            throw InvalidQuery(query.lastError().text().toStdString().c_str());
        }
	}
	catch(FabariaException e)
	{
	   LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
	}
    }
    else{
        QMessageBox MsgBox;
        MsgBox.setText(qApp->translate("misure","Errore"));
        MsgBox.setInformativeText(qApp->translate("misure","Seleziona una riga per eliminarla..."));
        MsgBox.setIcon(QMessageBox::Warning);
        MsgBox.exec();
    }

   lista();
    nuovo();
    emit add_misure();
}

void misure::clickgrid()
{
    QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex index = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(index).toString();
    ui->id->setText(id_new);

    cerca();
}


void misure::cerca()
{
    QSqlQuery query;
    query.prepare("select * from misura where id=:id");
    query.bindValue(":id",ui->id->text());
    query.exec();

    if(query.next()){
        ui->id->setText(query.value(0).toString());
        ui->unita->setText(query.value(1).toString());
        ui->matricola->setText(query.value(2).toString());
        ui->data_scadenza->setText(query.value(3).toString());
    }
    else{
        ui->id->setText("");
        ui->unita->setText("");
    }
}

void misure::lista()
{
    mod_grid->setTable("misura");
    mod_grid->select();
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("misure","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, QString::fromUtf8(qApp->translate("misure","Tipo di unità").toStdString().c_str()));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("misure","Matricola lotto"));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("misure","Data di scadenza"));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setModel(mod_grid);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setItemDelegateForColumn(2,new coldxdelegato(this));
}


bool misure::eventFilter(QObject *o, QEvent *e)
{
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

void misure::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton) {

                 this->Popup(event ->pos());
                 event->accept();
             }
}

void misure::Popup(const QPoint &pt)
{
    QPoint global = ui->tableView->mapToGlobal(pt);
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
    QAction* pAction1 = new QAction(qApp->translate("misure","Chiudi"), this);
    pAction1->setIcon(icon);
    pAction1->setIconVisibleInMenu(true);
    QAction* pAction2 = new QAction(qApp->translate("misure","Elimina"), this);
    pAction2->setIcon(icon1);
    pAction2->setIconVisibleInMenu(true);
    QAction* nw = new QAction(qApp->translate("misure","Nuovo"),this);
    nw->setIcon(icon2);
    nw->setIconVisibleInMenu(true);
    QAction* sa = new QAction(qApp->translate("misure","Salva"),this);
    sa->setIcon(icon3);
    sa->setIconVisibleInMenu(true);
    QAction* md = new QAction(qApp->translate("misure","Aggiorna"),this);
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

void misure::inserisci_lt_pc_bc(const QString &)
{
     if(ui->unita->text() == qApp->translate("misure","Lotto") || ui->unita->text() == qApp->translate("misure","lto") || ui->unita->text() == qApp->translate("misure","lotto"))
    {
        ui->matricola->setEnabled(true);
        ui->data_scadenza->setEnabled(true);
    }
    else{
        ui->matricola->setEnabled(false);
        ui->data_scadenza->setEnabled(false);
    }
}

misure::~misure()
{
    delete ui;
}

void misure::clean_all()
{
    ui->id->clear();
    ui->unita->clear();
}
