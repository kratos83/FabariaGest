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
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QCompleter>
#include <QStringList>
#include <QVariant>
#include <QMessageBox>
#include <math.h>
#include <time.h>
#include "settingsmanager.h"
#include "articoli.h"
#include "ui_articoli.h"
#include "itdelegato.h"
#include "print.h"
#include "queryexception.h"
#include "defines.h"
#include "esistenza_articoli.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;
using namespace QXlsx;

articoli *prodotto = 0;

articoli::articoli(QWidget *parent) : QDialog(parent)
{

    prodotto = this;
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(exit_art,SIGNAL(clicked()),this,SLOT(close_art()));
    exit_art->setShortcut(qApp->translate("articoli","Ctrl+E"));
    connect(del_art,SIGNAL(clicked()),this,SLOT(delete_art()));
    //Chiamata della tastiera per eliminare dati dal db
    del_art->setShortcut(qApp->translate("articoli","Ctrl+D"));
    connect(agg_art,SIGNAL(clicked()),this,SLOT(aggiungi()));
    //Chiamata della tastiera per salvare dati sul db
    agg_art->setShortcut(qApp->translate("articoli","Ctrl+S"));
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));
    connect(tab_view,SIGNAL(pressed(QModelIndex)),this,SLOT(clickgrid()));
    tab_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tab_view,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(Popup(const QPoint &)));
    connect(print_art,SIGNAL(clicked()),this,SLOT(stampa()));
    //Chiamata della tastiera per stampare
    print_art->setShortcut(qApp->translate("articoli","Ctrl+P"));
    connect(nuovo1,SIGNAL(clicked()),this,SLOT(nuovo()));
    //Chiamata della tastiera per inserire un nuovo articolo
    nuovo1->setShortcut(qApp->translate("articoli","Ctrl+N"));
    connect(modifica,SIGNAL(clicked()),this,SLOT(agg_ass()));
    //Chiamata della tastiera per modificare l'articolo
    modifica->setShortcut(qApp->translate("articoli","Ctrl+M"));
    connect(cerca1,&QLineEdit::textChanged,this,&articoli::cerca_libri);
    //codice_barre = new barcode(groupBox_2);
    cod_barre->setMaxLength(13);
    connect(tab_view,SIGNAL(clicked(QModelIndex)),this,SLOT(ist_codbarre()));
    connect(pr_bar,SIGNAL(clicked()),this,SLOT(stampacodbarreart()));
    connect(m_iva,SIGNAL(clicked()),this,SLOT(gestIva()));
    connect(m_misura,SIGNAL(clicked()),this,SLOT(gestMisure()));
    connect(prezzo_unitario,SIGNAL(textEdited(const QString &)),this,SLOT(text(const QString &)));
    connect(quant,SIGNAL(textEdited(const QString &)),this,SLOT(prezzosiva(const QString &)));
    connect(quant,SIGNAL(textEdited(QString)),this,SLOT(complete_tot(const QString &)));
    connect(sconto,SIGNAL(textEdited(const QString)),this,SLOT(sconto_tot(const QString &)));
    connect(comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(text(QString)));
    connect(p_c_iva,SIGNAL(textChanged(QString)),this,SLOT(complete_tot(QString)));
    connect(um,SIGNAL(currentIndexChanged(QString)),this,SLOT(indexComboMisura(QString)));
    //Imposto il menu del bottone exporta
    menu = new QMenu(this);
    esporta_excel = new QAction(QIcon(":/images/excel.png"),qApp->translate("articoli","Esporta in excel"),this);
    import_excel = new QAction(QIcon(":/images/excel.png"),qApp->translate("articoli","Importa da excel"),this);
    menu->addAction(esporta_excel);
    menu->addSeparator();
    menu->addAction(import_excel);
    export_excel->setMenu(menu);
    connect(esporta_excel,SIGNAL(triggered()),this,SLOT(esporta_file_excel()));
    connect(import_excel,SIGNAL(triggered()),this,SLOT(importa()));
    label_lotto->setVisible(false);
    scadenza_lotto->setVisible(false);
    label_nlotto->setVisible(false);
    numero_lotto->setVisible(false);
    scadenza_lotto->setText(QDate::currentDate().toString("dd/MM/yyyy"));
    mod_grid = new QSqlRelationalTableModel;
    local_settings = new QLocale(QLocale::system());
    descrizione->adjustSize();
    dettagli->setReadOnly(true);
    lista();
    clickgrid();
    combo_iva();
    combo_um();
    vis_codbarre();
    combo_categ();
    listino();
    nuovo();
}

void articoli::close_art() {
    close();
}

void articoli::indexComboMisura(QString combo)
{
    if(combo == qApp->translate("articoli","Lotto") || combo == qApp->translate("articoli","lto") || combo == qApp->translate("articoli","lotto"))
    {
        label_lotto->setVisible(true);
        scadenza_lotto->setVisible(true);
        label_nlotto->setVisible(true);
        numero_lotto->setVisible(true);
        bool ok;
        QString text = QInputDialog::getText(this, qApp->translate("articoli","Inserisci numero lotto"),
                                             qApp->translate("articoli","Inserisci numero lotto"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            numero_lotto->setText(text);
            bool m_ok;
            QString m_text = QInputDialog::getText(this, qApp->translate("articoli","Inserisci le quantità"),
                                                   qApp->translate("articoli","Quante sono le quantità?"), QLineEdit::Normal,
                                                   "", &m_ok);
            double qt = m_text.toDouble();
            quant->setText(QString::number(qt));
        }
    }
    else if(combo == qApp->translate("articoli","bc") || combo == qApp->translate("articoli","bancale") || combo == qApp->translate("articoli","Bancale"))
    {
        bool ok;
        QString text = QInputDialog::getText(this, qApp->translate("articoli","Inserisci le quantità in un bancale"),
                                             qApp->translate("articoli","Quanti pacchi ci sono in un bancale?"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            double totale = 0;
            double pc = text.toDouble();
            bool m_ok;
            QString m_text = QInputDialog::getText(this, qApp->translate("articoli","Inserisci le quantità in un pacco"),
                                                   qApp->translate("articoli","Quante quantità ci sono in pacco?"), QLineEdit::Normal,
                                                   "", &m_ok);
            double qt = m_text.toDouble();
            totale = pc * qt;
            quant->setText(QString::number(totale));
        }
    }
    else if(combo == qApp->translate("articoli","pc") || combo == qApp->translate("articoli","pacchi") || combo == qApp->translate("articoli","Pacchi"))
    {
        bool m_ok;
        QString m_text = QInputDialog::getText(this, qApp->translate("articoli","Inserisci le quantità in un pacco"),
                                               qApp->translate("articoli","Quante quantità ci sono in pacco?"), QLineEdit::Normal,
                                               "", &m_ok);
        double qt = m_text.toDouble();
        quant->setText(QString::number(qt));
    }
}

void articoli::gestIva()
{
    mIva = new iva(this);
    mIva->nuovo();
    connect(mIva,SIGNAL(add_iva()),this,SLOT(combo_iva()));
    mIva->exec();
}

void articoli::gestMisure()
{
    mMisure = new misure(this);
    mMisure->nuovo();
    connect(mMisure,SIGNAL(add_misure()),this,SLOT(combo_um()));
    mMisure->exec();
}

void articoli::cerca_libri(const QString &) {

    QString text1;
    text1 = cerca1->text();
    QStringList list_st;
    if(comboBox_2->currentIndex() == 0) { //codice a barre
        list_st = selectQueryWhile("select codbarre from articoli",0);
    }
    else if(comboBox_2->currentIndex() == 1) { //codice prodotto
        list_st = selectQueryWhile("select cod_articolo from articoli",0);
    }
    else if(comboBox_2->currentIndex() == 2) { //prodotto
        list_st = selectQueryWhile("select nome_articolo from articoli",0);
    }
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    cerca1->setCompleter(complete);
    filtro(text1);
    if(text1.length() == 0) {
        lista();
    }
}

void articoli::filtro(QString m_cerca) {
    if(m_cerca.length() == 0) {
        lista();
        tabWidget->setCurrentIndex(0);
    }
    else if(comboBox_2->currentIndex() == 0) {
        mod_grid->setFilter(QString("codbarre = '" +m_cerca+"'").arg(m_cerca));
        tabWidget->setCurrentIndex(1);
    }
    else if(comboBox_2->currentIndex() == 1) {
        mod_grid->setFilter(QString("cod_articolo = '" +m_cerca+"'").arg(m_cerca));
        tabWidget->setCurrentIndex(1);
    }
    else if(comboBox_2->currentIndex() == 2) {
        mod_grid->setFilter(QString("nome_articolo = '" +m_cerca+"'").arg(m_cerca));
        tabWidget->setCurrentIndex(1);
    }
}


void articoli::nuovo() {

    cod_art->setText("");
    cod_barre->setText("");
    art_nom->setText("");
    descrizione->setText("");
    sconto->setText("");
    quant->setText("");
    prezzo_unitario->setText("");
    p_s_iva->setText("");
    p_c_iva->setText("");
    totale->setText("");
    scadenza_lotto->setText(QDate::currentDate().toString("dd/MM/yyyy"));
    combo_iva();
    combo_um();
    setImporti();
}

void articoli::combo_iva() {

    comboBox->clear();
    QStringList list;
    list = selectQueryWhile("select iva from aliquota",0);
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    comboBox->setCompleter(complete);
    comboBox->addItems(list);

    if(comboBox->currentText().length() == 0) {
        articoli::combo_iva();
    }
}

void articoli::combo_um()
{
    um->clear();
    QStringList lista;
    lista = selectQueryWhile("select unita from misura",0);
    QCompleter *complete = new QCompleter(lista,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(um->model());
    um->setCompleter(complete);
    um->addItems(lista);
}

void articoli::combo_categ()
{
    categ->clear();
    QStringList lista;
    lista = selectQueryWhile("select tipo_categ from categoria",0);
    QCompleter *complete = new QCompleter(lista,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(categ->model());
    categ->setCompleter(complete);
    categ->addItems(lista);
}

void articoli::listino()
{
    list_listino->clear();
    QStringList lista;
    lista = selectQueryWhile("select descrizione from tipo_listino",0);
    QCompleter *complete = new QCompleter(lista,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    complete->setModel(list_listino->model());
    list_listino->setCompleter(complete);
    list_listino->addItems(lista);
}

void articoli::text(const QString &)
{
    double iva_art = comboBox->currentText().toDouble();
    double p_iva = prezzo_unitario->text().toDouble();
    double perc = (((p_iva*iva_art)/100)+p_iva);
    p_c_iva->setText(QString::number(perc,'f',2));
}

void articoli::complete_tot(const QString &)
{
    double iva_art = comboBox->currentText().toDouble();
    double ps_iva = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double tot = totale->text().toDouble();
    double perc = ((ps_iva*iva_art)/100)+ps_iva;
    tot = (perc*quan);
    totale->setText(QString::number(tot,'f',2));
    p_c_iva->setText(QString::number(tot,'f',2));
}

void articoli::sconto_tot(const QString &)
{
    double iva_art = comboBox->currentText().toDouble();
    double ps_iva = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double tot = totale->text().toDouble();
    double perc = ((ps_iva*iva_art)/100)+ps_iva;
    tot = (perc*quan);
    double sc_tot = sconto->text().toDouble();
    double applica = ((tot*sc_tot)/100);
    double set = tot -applica;
    totale->setText(QString::number(set,'f',2));
    p_c_iva->setText(QString::number(set,'f',2));
}

void articoli::prezzosiva(const QString &)
{
    double pr_un = prezzo_unitario->text().toDouble();
    double quan = quant->text().toDouble();
    double p_s_q = pr_un*quan;
    p_s_iva->setText(QString::number(p_s_q,'f',2));
}

void articoli::delete_art() {

    QString flag_controllo = "NO";
    if(!tab_view->selectionModel()->currentIndex().isValid()) {

        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("articoli","Selezionare una riga da eliminare..."));
    }
    else if (!cod_art->text().isEmpty())
    {
        eliminaQuery("DELETE FROM articoli WHERE cod_articolo ='"+cod_art->text()+"' LIMIT 1","articoli");
    }
    clearText();
    lista();
    nuovo();
    emit article_prod();
}

void articoli::aggiungi() {

    inserisci();
    lista();
    emit article_prod();
}

void articoli::agg_ass() {

    if(!tab_view->selectionModel()->currentIndex().isValid()) {
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("articoli","Selezionare una riga da modificare..."));
    }
    else {
        QModelIndex modelIndex = tab_view->selectionModel()->currentIndex();
        int riga = modelIndex.row();
        QModelIndex newindex = modelIndex.sibling(riga,0);
        tab_view->setCurrentIndex(newindex);
        tab_view->selectRow(riga);
        aggiorna(newindex);
    }
}

void articoli::aggiorna(QModelIndex index) {

    //Tentativo di aggiornamento del record perché trovato
    //Data scadenza lotto
    QString giorno_scadenza = scadenza_lotto->text().mid(0,2);
    QString mese_scadenza = scadenza_lotto->text().mid(3, 2);
    QString anno_scadenza = scadenza_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"-"+mese_scadenza+"-"+giorno_scadenza;
    if(index.row() < mod_grid->rowCount()) {
        updateQuery("UPDATE articoli SET  codbarre='"+cod_barre->text()+"',nome_articolo='"+art_nom->text().replace("'","''")+"', "
                      "prezzo_s_iva='"+p_s_iva->text()+"', descrizione='"+descrizione->toPlainText().replace("'","''")+"', unita='"+um->currentText()+"', "
                      "prezzo_unitario='"+prezzo_unitario->text()+"',prezzo_c_iva='"+p_c_iva->text()+"', iva='"+QString::number(comboBox->currentText().toDouble())+"', quantita='"+quant->text()+"',  "
                      "sconto='"+sconto->text()+"', totale='"+totale->text()+"', anno_ins='"+QString::number(QDate::currentDate().year())+"', "
                      "codice_listino='"+list_listino->currentText().replace("'","''")+"', categoria='"+categ->currentText().replace("'","''")+"', data_insert='"+QDate::currentDate().toString("yyyy/MM/dd")+"', "
                      "numero_lotto='"+numero_lotto->text()+"', data_lotto='"+insieme_scadenza+"', addMagazzino='"+getConfigurazioneSql("MagazzinoNome").toString()+"' "
                      "WHERE cod_articolo ='"+cod_art->text()+"'","articoli");
    }
    clearText();
    lista();
    nuovo();
    emit article_prod();
}

void articoli::clearText()
{
    cod_art->setText("");
    cod_barre->setText("");
    art_nom->setText("");
    descrizione->setText("");
    sconto->setText("");
    quant->setText("");
    prezzo_unitario->setText("");
    p_s_iva->setText("");
    p_c_iva->setText("");
    totale->setText("");
    combo_iva();
    combo_um();
    combo_categ();
}
void articoli::inserisci() {

    //Data scadenza lotto
    QString giorno_scadenza = scadenza_lotto->text().mid(0,2);
    QString mese_scadenza = scadenza_lotto->text().mid(3, 2);
    QString anno_scadenza = scadenza_lotto->text().mid(6, 4);
    QString insieme_scadenza = anno_scadenza+"/"+mese_scadenza+"/"+giorno_scadenza;
    if(cod_art->text().length() == 0 || cod_barre->text().length() == 0 || art_nom->text().length() == 0 || 
        descrizione->toPlainText().length() == 0  || quant->text().length() == 0) {
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("articoli","Inserisci prima i dati correttamente"));
        cod_art->setStyleSheet("background-color: rgb(249, 22, 5)");
        art_nom->setStyleSheet("background-color: rgb(249, 22, 5)");
        cod_barre->setStyleSheet("background-color: rgb(249, 22, 5)");
        descrizione->setStyleSheet("background-color: rgb(249, 22, 5)");
        quant->setStyleSheet("background-color: rgb(249, 22, 5)");
    }
    else {
        insertQuery("INSERT INTO articoli VALUES ('"+cod_art->text()+"','"+cod_barre->text()+"','"+art_nom->text().replace("'","''")+"','"+descrizione->toPlainText().replace("'","''")+"','"+um->currentText()+"',"
                      "'"+prezzo_unitario->text()+"','"+p_s_iva->text()+"','"+p_c_iva->text()+"',"
                      "'"+QString::number(comboBox->currentText().toDouble())+"','"+quant->text()+"','"+sconto->text()+"','"+totale->text()+"','"+QString::number(QDate::currentDate().year())+"',"
                      "'"+list_listino->currentText().replace("'","''")+"','"+categ->currentText().replace("'","''")+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"',"
                      "'"+numero_lotto->text()+"','"+insieme_scadenza+"', '"+getConfigurazioneSql("MagazzinoNome").toString()+"')","articoli");
        lista();
        clearText();
        nuovo();
        emit article_prod();
    }
}

void articoli::lista() {

    mod_grid->setTable("articoli");
    mod_grid->select();
    mod_grid->setHeaderData(0, Qt::Horizontal, qApp->translate("articoli","ID"));
    mod_grid->setHeaderData(1, Qt::Horizontal, qApp->translate("articoli","Codice a barre."));
    mod_grid->setHeaderData(2, Qt::Horizontal, qApp->translate("articoli","Prodotto."));
    mod_grid->setHeaderData(3, Qt::Horizontal, qApp->translate("articoli","Descrizione."));
    mod_grid->setHeaderData(4,Qt::Horizontal,  qApp->translate("articoli","Unita' di misura"));
    mod_grid->setHeaderData(5, Qt::Horizontal, qApp->translate("articoli","Prezzo unitario"));
    mod_grid->setHeaderData(6, Qt::Horizontal, qApp->translate("articoli","Prezzo senza iva"));
    mod_grid->setHeaderData(7, Qt::Horizontal, qApp->translate("articoli","Prezzo con iva"));
    mod_grid->setHeaderData(8, Qt::Horizontal, qApp->translate("articoli","Iva"));
    mod_grid->setHeaderData(9,Qt::Horizontal,  qApp->translate("articoli","Quantita'"));
    mod_grid->setHeaderData(10, Qt::Horizontal, qApp->translate("articoli","Sconto"));
    mod_grid->setHeaderData(11, Qt::Horizontal, qApp->translate("articoli","Totale"));
    mod_grid->setHeaderData(12, Qt::Horizontal, qApp->translate("articoli","Anno inserimento"));
    mod_grid->setHeaderData(13, Qt::Horizontal, qApp->translate("articoli","Listino"));
    mod_grid->setHeaderData(14,Qt::Horizontal, qApp->translate("articoli","Categoria"));
    mod_grid->setHeaderData(15, Qt::Horizontal, qApp->translate("articoli","Data inserimento"));
    mod_grid->setHeaderData(16, Qt::Horizontal, qApp->translate("articoli","Numero lotto"));
    mod_grid->setHeaderData(17, Qt::Horizontal, qApp->translate("articoli","Scadenza lotto"));
    mod_grid->setHeaderData(18, Qt::Horizontal, qApp->translate("articoli","Magazzino"));
    tab_view->setSelectionBehavior(QAbstractItemView::SelectRows);

    tab_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tab_view->setSelectionMode(QAbstractItemView::SingleSelection);
    tab_view->setSortingEnabled(true);
    tab_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tab_view->verticalHeader()->hide();
    tab_view->setModel(mod_grid);
    tab_view->setColumnHidden(12,true);
    tab_view->setColumnHidden(15,true);
    tab_view->setAlternatingRowColors(true);

    tab_view->setItemDelegateForColumn(15, new coldxdelegato(this));
}

void articoli::clickgrid() {

    QString id_new;
    int riga = tab_view->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());
    id_new = mod_grid->data(elemento).toString();
    cod_art->setText(id_new);
    tabWidget->setCurrentIndex(0);
    cerca();
}

void articoli::cerca() {

        dettagli->setText(qApp->translate("articoli","Id: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Codice a barre: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Nome prodotto: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),2),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Descrizione: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),3),Qt::DisplayRole).toString()+"\n"+
                          QString::fromUtf8(qApp->translate("articoli","Unità di misura: ").toStdString().c_str())+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),4),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Prezzo unitario: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),5),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Prezzo senza iva: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),6),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Aliquota IVA: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),8),Qt::DisplayRole).toString()+"%\n"+
                          QString::fromUtf8(qApp->translate("articoli","Quantità: ").toStdString().c_str())+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),9),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Sconto: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),10),Qt::DisplayRole).toString()+"%\n"+
                          qApp->translate("articoli","Totale: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),11),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Listino: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),13),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("articoli","Categoria: ")+mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),14),Qt::DisplayRole).toString());
        cod_art->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString());
        cod_barre->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString());
        art_nom->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),2),Qt::DisplayRole).toString());
        descrizione->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),3),Qt::DisplayRole).toString());
        um->setCurrentText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),4),Qt::DisplayRole).toString());
        prezzo_unitario->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),5),Qt::DisplayRole).toString());
        p_s_iva->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),6),Qt::DisplayRole).toString());
        p_c_iva->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),7),Qt::DisplayRole).toString());
        comboBox->setCurrentText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),8),Qt::DisplayRole).toString());
        quant->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),9),Qt::DisplayRole).toString());
        sconto->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),10),Qt::DisplayRole).toString());
        totale->setText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),11),Qt::DisplayRole).toString());
        list_listino->setCurrentText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),13),Qt::DisplayRole).toString());
        categ->setCurrentText(mod_grid->data(mod_grid->index(tab_view->selectionModel()->currentIndex().row(),14),Qt::DisplayRole).toString());
}


void articoli::stampa() {

    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    QPrintPreviewDialog *preview = new QPrintPreviewDialog(printer,this);
    preview->setWindowFlags(Qt::Window);
    preview->setWindowTitle(qApp->translate("articoli","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview->setWindowIcon(icon);
    connect(preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_new(QPrinter*)));
    preview->exec();

}

void articoli::preview_new(QPrinter *printer) {

    Q_UNUSED(printer);
    printer->setResolution(90);
    printer->setPageMargins(10.0,10.0,10.0,10.0,QPrinter::Millimeter);
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);
    QString id,barcode,prodotto,descrizione,misura,prezzo_unitario,prezzo_senza_iva,prezzo_con_iva,
            iva,quantita,sconto,totale,listino,categoria,numero_lotto,scadenza_lotto,magazzino,tabella;
    QFile file(":/stampa/articoli.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    while(!file.atEnd())
    {
        tabella = file.readAll();
        for(int i = 0; i < mod_grid->rowCount(); ++i)
        {
            QSqlRecord record = mod_grid->record(i);
            if(!record.isEmpty())
            {
                id = record.value("cod_articolo").toString();
                barcode = record.value("cod_barre").toString();
                prodotto = record.value("nome_articolo").toString();
                descrizione = record.value("descrizione").toString();
                misura = record.value("unita").toString();
                prezzo_unitario = QString::number(record.value("prezzo_unitario").toString().toDouble(),'f',2);
                prezzo_senza_iva = QString::number(record.value("prezzo_s_iva").toString().toDouble(),'f',2);
                prezzo_con_iva = QString::number(record.value("prezzo_c_iva").toString().toDouble(),'f',2);
                iva = record.value("iva").toString();
                quantita = record.value("quantita").toString();
                sconto = QString::number(record.value("sconto").toString().toDouble(),'f',2);
                totale = QString::number(record.value("totale").toString().toDouble(),'f',2);
                listino = record.value("listino").toString();
                categoria = record.value("categoria").toString();
                numero_lotto = record.value("numero_lotto").toString();
                scadenza_lotto = record.value("data_lotto").toString();
                magazzino = record.value("addMagazzino").toString();
                tabella.replace("PRODUCT LIST",qApp->translate("articoli","LISTA PRODOTTI"));
                tabella.replace("ID",qApp->translate("articoli","Id"));
                tabella.replace("BARCODE",qApp->translate("articoli","Codice a barre"));
                tabella.replace("PRODUCT",qApp->translate("articoli","Prodotti"));
                tabella.replace("DESCRIPTION",qApp->translate("articoli","Descrizione"));
                tabella.replace("UNIT_PESO",qApp->translate("articoli","Unità"));
                tabella.replace("UNIT PRICE",qApp->translate("articoli","Prezzo unitario"));
                tabella.replace("PRICE WITHOUT VAT",qApp->translate("articoli","Prezzo senza IVA"));
                tabella.replace("PRICE WITH VAT",qApp->translate("articoli","Prezzo con IVA"));
                tabella.replace("VAT",qApp->translate("articoli","IVA"));
                tabella.replace("AMOUNT",qApp->translate("articoli","Quantità"));
                tabella.replace("DISCOUNT",qApp->translate("articoli","Sconto"));
                tabella.replace("TOTAL",qApp->translate("articoli","Totale"));
                tabella.replace("LISTINO",qApp->translate("articoli","Listino"));
                tabella.replace("CATEGORY",qApp->translate("articoli","Categoria"));
                tabella.replace("LOT NUMBER",qApp->translate("articoli","Lotto"));
                tabella.replace("EXPIRY LOT",qApp->translate("articoli","Scadenza lotto"));
                tabella.replace("STOREHOUSE",qApp->translate("articoli","Magazzino"));
                tabella.replace("<td id=\"id\" valign=\"top\" align=\"center\">","<td id=\"id\" valign=\"top\" align=\"center\">"+id+"<br>");
                tabella.replace("<td id=\"barcode\" valign=\"top\" align=\"center\">","<td id=\"barcode\" valign=\"top\" align=\"center\">"+barcode+"<br>");
                tabella.replace("<td id=\"product\" valign=\"top\" align=\"center\">","<td id=\"product\" valign=\"top\" align=\"center\">"+prodotto+"<br>");
                tabella.replace("<td id=\"description\" valign=\"top\" align=\"center\">","<td id=\"description\" valign=\"top\" align=\"center\">"+descrizione+"<br>");
                tabella.replace("<td id=\"unit\" valign=\"top\" align=\"center\">","<td id=\"unit\" valign=\"top\" align=\"center\">"+misura+"<br>");
                tabella.replace("<td id=\"price_unit\" valign=\"top\" align=\"center\">","<td id=\"price_unit\" valign=\"top\" align=\"center\">€ "+prezzo_unitario+"<br>");
                tabella.replace("<td id=\"price_without_vat\" valign=\"top\" align=\"center\">","<td id=\"price_without_vat\" valign=\"top\" align=\"center\">€ "+prezzo_senza_iva+"<br>");
                tabella.replace("<td id=\"price_with_vat\" valign=\"top\" align=\"center\">","<td id=\"price_with_vat\" valign=\"top\" align=\"center\">€ "+prezzo_con_iva+"<br>");
                tabella.replace("<td id=\"vat\" valign=\"top\" align=\"center\">","<td id=\"vat\" valign=\"top\" align=\"center\">"+iva+"%"+"<br>");
                tabella.replace("<td id=\"amount\" valign=\"top\" align=\"center\">","<td id=\"amount\" valign=\"top\" align=\"center\">"+quantita+"<br>");
                tabella.replace("<td id=\"discount\" valign=\"top\" align=\"center\">","<td id=\"discount\" valign=\"top\" align=\"center\">"+sconto+"%"+"<br>");
                tabella.replace("<td id=\"total\" valign=\"top\" align=\"center\">","<td id=\"total\" valign=\"top\" align=\"center\">€ "+totale+"<br>");
                tabella.replace("<td id=\"list\" valign=\"top\" align=\"center\">","<td id=\"list\" valign=\"top\" align=\"center\">"+listino+"<br>");
                tabella.replace("<td id=\"category\" valign=\"top\" align=\"center\">","<td id=\"category\" valign=\"top\" align=\"center\">"+categoria+"<br>");
                tabella.replace("<td id=\"lot_number\" valign=\"top\" align=\"center\">","<td id=\"lot_number\" valign=\"top\" align=\"center\">"+numero_lotto+"<br>");
                tabella.replace("<td id=\"expiry_lot\" valign=\"top\" align=\"center\">","<td id=\"expiry_lot\" valign=\"top\" align=\"center\">"+scadenza_lotto+"<br>");
                tabella.replace("<td id=\"storehouse\" valign=\"top\" align=\"center\">","<td id=\"storehouse\" valign=\"top\" align=\"center\">"+magazzino+"<br>");
            }
        }
    }
    QTextDocument *doc = new QTextDocument;
    QSizeF paperSize;
    paperSize.setWidth(printer->width());
    paperSize.setHeight(printer->height());
    doc->setHtml(tabella);
    doc->setPageSize(paperSize);
    doc->print(printer);
    file.close();
}

void articoli::vis_codbarre() {

    //gridLayout->addWidget(codice_barre,2,1,2,9);
    groupBox_2->show();
}

void articoli::ist_codbarre() {

    QModelIndex modelIndex1 = tab_view->selectionModel()->currentIndex();
    int riga = modelIndex1.row();
    QModelIndex newindex1 = modelIndex1.sibling(riga,0);
    tab_view->setCurrentIndex(newindex1);
    tab_view->selectRow(riga);
    view_barre(newindex1);
}

void articoli::view_barre(QModelIndex av) {

    if(av.row() < mod_grid->rowCount()) {
        QSqlQuery qr;
        qr.prepare("select codbarre from articoli where cod_articolo='"+cod_art->text()+"'");
        qr.exec();

        if(qr.next()) {
            QFont nv("IDAutomationHC39M", 8, QFont::Bold);
            cod_barre->setFont(nv);
            groupBox_2->updateSwtClicked(qr.value(0).toString());
        }
        else {
            QMessageBox::warning(this,"Fabaria Gest",qApp->translate("articoli","Impossibile ottenere il barcode....\n")+qr.lastError().text());
        }
    }
}

void articoli::visualizza_lista()
{
    QModelIndex modelIndex1 = tab_view->selectionModel()->currentIndex();
    int riga = modelIndex1.row();
    QModelIndex newindex1 = modelIndex1.sibling(riga,0);
    tab_view->setCurrentIndex(newindex1);
    tab_view->selectRow(riga);
    vis_lista(newindex1);
}

void articoli::vis_lista(QModelIndex index)
{
    if(index.row() < mod_grid->rowCount())
    {
        esistenza_articoli *es_art = new esistenza_articoli(this);
        es_art->vis_quantita(&index,mod_grid);
        es_art->exec();
    }
}

void articoli::bar_code_en() {
    cod_barre->setVisible(true);
    codice_barre->setVisible(true);
    groupBox_2->setVisible(true);
}

void articoli::stampacodbarreart() {

    if(!tab_view->selectionModel()->currentIndex().isValid()) {
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("articoli","Seleziona una riga per stampare l'etichetta...."));
    }
    else {
        QPrinter *printer = new QPrinter(QPrinter::HighResolution);
        QPrintPreviewDialog *preview = new QPrintPreviewDialog(printer,this);
        preview->setWindowFlags(Qt::Window);
        connect(preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_printbarreart(QPrinter*)));
        preview->exec();
    }
}

void articoli::preview_printbarreart(QPrinter *printer) {

    Q_UNUSED(printer);
    QModelIndex in = tab_view->selectionModel()->currentIndex();
    int riga = in.row();
    QModelIndex newindex = in.sibling(riga,0);
    tab_view->setCurrentIndex(newindex);
    tab_view->selectRow(riga);
    Stampe *report = new Stampe();
    EAN13 *en = new EAN13();
    report->stampabarcode(printer,mod_grid,&newindex,en);
}

bool articoli::eventFilter(QObject *o, QEvent *e) {

    QKeyEvent* ke = (QKeyEvent*)e;
    //Add event to menu key
    if (o==tab_view && e->type() == QEvent::KeyPress) {

        if (ke->key() == Qt::Key_Delete) {
            // ESEGUO IL CODICE DI ELIMINAZIONE
            delete_art();
        }
        if(ke->key() == Qt::Key_S) {
            //ESEGUO IL CODICE PER INSERIRE
            aggiungi();
        }
        if(ke->key() == Qt::Key_U) {
            //ESEGUO IL CODICE PER AGGIORNARE
            agg_ass();
        }
        if(ke->key() == Qt::Key_N) {
            //ESEGUO IL CODICE PER UN NUOVO ARTICOLO
            nuovo();
        }
        if(ke->key() == Qt::Key_P) {
            //ESEGUO IL CODICE DI STAMPA
            stampa();
        }
    }

    //Installazione eventi Qt::Key_Tab



    if( o==tab_view && e->type() == QEvent::ContextMenu)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (e);
        this ->Popup(mouseEvent->pos());
        return false;
    }



    return QDialog::eventFilter(o,e);

}

void articoli::mousePressEvent(QMouseEvent *event) {

    /******************************************
      * Imposta il tasto destro per il mouse
      * riproducendo un menu contestuale
      *****************************************/
    if (event->button() == Qt::RightButton) {
        if(tab_view->currentIndex().isValid()){
            this->Popup(event ->pos());
        }
        event->accept();
    }
}

void  articoli::Popup(const QPoint &pt) {

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
    icon.addFile(QString::fromUtf8(":/images/dialog-close.png"));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/edit-delete.png"));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/images/filenew.png"));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/images/document-save.png"));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/images/view-refresh.png"));
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/images/document-print.png"));
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/images/torta.png"));

    /***********************************************************
    *  @author Angelo Scarnà
    *  Menu a tendina
    ************************************************************/
    QAction* pAction1 = new QAction(qApp->translate("articoli","Chiudi"), this);
    pAction1->setIcon(icon);
    pAction1->setIconVisibleInMenu(true);
    QAction* pAction2 = new QAction(qApp->translate("articoli","Elimina"), this);
    pAction2->setIcon(icon1);
    pAction2->setIconVisibleInMenu(true);
    QAction* nw = new QAction(qApp->translate("articoli","Nuovo"),this);
    nw->setIcon(icon2);
    nw->setIconVisibleInMenu(true);
    QAction* sa = new QAction(qApp->translate("articoli","Salva"),this);
    sa->setIcon(icon3);
    sa->setIconVisibleInMenu(true);
    QAction* md = new QAction(qApp->translate("articoli","Aggiorna"),this);
    md->setIcon(icon4);
    md->setIconVisibleInMenu(true);
    QAction* pr = new QAction(qApp->translate("articoli","Stampa"), this);
    pr->setIcon(icon5);
    pr->setIconVisibleInMenu(true);
    QAction* gr = new QAction(qApp->translate("articoli","Visualizza esistenza articoli"),this);
    gr->setIcon(icon6);
    gr->setIconVisibleInMenu(true);

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
    pPopup->addAction(gr);
    pPopup->addSeparator();
    pPopup->addAction(pr);
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
        delete_art();
    }
    else if(pItem == nw) {
        nuovo();
    }
    else if(pItem == sa)
    {
        aggiungi();
    }
    else if(pItem == md)
    {
        agg_ass();
    }
    else if(pItem == pr)
    {
        stampa();
    }
    else if(pItem == gr)
    {
        visualizza_lista();
    }
}

void articoli::reload_list()
{
    time_t t;
    t = time(NULL);

    while( difftime(time(NULL),t) < 2.0  ) {
        lista();
    }
}

void articoli::esporta_file_excel()
{
    esportaExcelDefine("articoli",mod_grid);
}

void articoli::importa()
{
    QString testo = qApp->translate("articoli","ID\nCodice a barre\nNome prodotto\nDescrizione\nUnità di misura\n"
                                    "Prezzo unitario\nPrezzo senza iva\nPrezzo con iva\nAliquota IVA\n"
                                    "Quantità\nSconto\nTotale\nAnno inserimento\nListino\nCategoria\n"
                                    "Data inserimento\nNumero lotto\nScadenza lotto\nMagazzino\n\n\n\nAttenzione tutte le celle\ndevono essere compilate.");
    QMessageBox *box= new QMessageBox(this);
    box->setWindowTitle(qApp->translate("articoli","FabariaGest"));
    box->setText(qApp->translate("articoli","Il file excel\ndeve essere composto:\n"));
    box->setInformativeText(testo);
    box->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box->setDefaultButton(QMessageBox::Ok);
    int ret = box->exec();
    switch(ret) {
    case QMessageBox::Ok:
        //Open file excel
        importa_file_excel();
        box->close();
        break;
    case QMessageBox::Cancel:
        //Close
        box->close();
        break;
    }
}

void articoli::importa_file_excel()
{
    QString filePath = QFileDialog::getOpenFileName(0, qApp->translate("articoli","Apri file excel"), QString(), "*.xlsx");
    if(filePath.isEmpty())
        QMessageBox::warning(this,"FabariaGest",qApp->translate("articoli","Non hai aperto nesssun file"));
    else {
        Document xlsx(filePath);

        for(int i=2; i <= xlsx.dimension().rowCount(); i++)
        {
            QProgressDialog progressDialog(this);
            progressDialog.setRange(0, xlsx.dimension().rowCount());
            progressDialog.setWindowModality(Qt::WindowModal);
            progressDialog.setWindowTitle(qApp->translate("articoli","Inserimento dati da excel"));
            progressDialog.setValue(i);
            progressDialog.setLabelText(qApp->translate("articoli","Inserimento dati %1 di %2...")
                                        .arg(i).arg(xlsx.dimension().rowCount()));
            qApp->processEvents();
            QSqlQuery query,query_up;
            QString req = "INSERT INTO articoli(cod_articolo,codbarre,nome_articolo,descrizione,unita, "
                          "prezzo_unitario,prezzo_s_iva,prezzo_c_iva,iva,quantita,sconto,totale, "
                          "anno_ins,codice_listino,categoria,data_insert,numero_lotto,data_lotto,addMagazzino) VALUES('";
            Cell *codarticolo = xlsx.cellAt(i,1);
            query_up.prepare("select * from articoli where cod_articolo='"+codarticolo->value().toString()+"'");
            query_up.exec();
            if(query_up.next())
            {
                aggiorna_da_excel(filePath);
            }
            else {
                if(Cell *code_articolo = xlsx.cellAt(i,1)) { //cod_articolo
                    req.append(QString(code_articolo->value().toString().toStdString().c_str()).replace("'","''"));
                    req.append("','");
                }
                if(Cell *codbarre = xlsx.cellAt(i,2)) { //codbarre
                    req.append(QString(codbarre->value().toString().toStdString().c_str()).replace("'","''"));
                    req.append("','");
                }
                if(Cell *name_articolo = xlsx.cellAt(i,3)) { //nome_articolo
                    req.append(name_articolo->value().toString());
                    req.append("','");
                }
                if(Cell *desc = xlsx.cellAt(i,4)) { //descrizione
                    req.append(desc->value().toString());
                    req.append("','");
                }
                if(Cell *un = xlsx.cellAt(i,5)) { //unità
                    req.append(un->value().toString());
                    req.append("','");
                }
                if(Cell *pr_unit = xlsx.cellAt(i,6)) { //prezzo_unitario
                    req.append(QString(pr_unit->value().toString().toStdString().c_str()).replace("'","''"));
                    req.append("','");
                }
                if(Cell *pr_s_iva = xlsx.cellAt(i,7)) { //prezzo_s_iva
                    req.append(pr_s_iva->value().toString());
                    req.append("','");
                }
                if(Cell *pr_c_iva = xlsx.cellAt(i,8)) { //prezzo_c_iva
                    req.append(pr_c_iva->value().toString());
                    req.append("','");
                }
                if(Cell *m_iva = xlsx.cellAt(i,9)) { //iva
                    req.append(m_iva->value().toString());
                    req.append("','");
                }
                if(Cell *quan = xlsx.cellAt(i,10)) { //quantita
                    req.append(quan->value().toString());
                    req.append("','");
                }
                if(Cell *sc = xlsx.cellAt(i,11)) { //sconto
                    req.append(sc->value().toString());
                    req.append("','");
                }
                if(Cell *tot = xlsx.cellAt(i,12)) { //totale
                    req.append(tot->value().toString());
                    req.append("','");
                }
                if(Cell *anno = xlsx.cellAt(i,13)) { //anno_ins
                    req.append(anno->value().toString());
                    req.append("','");
                }
                if(Cell *cod_list = xlsx.cellAt(i,14)) { //codice_listino
                    req.append(cod_list->value().toString());
                    req.append("','");
                }
                if(Cell *cat = xlsx.cellAt(i,15)) { //categoria
                    req.append(cat->value().toString());
                    req.append("','");
                }
                if(Cell *date_insert = xlsx.cellAt(i,16)) { //data_insert
                    req.append(date_insert->value().toString());
                    req.append("','");
                }
                if(Cell *num_lotto = xlsx.cellAt(i,17)) { //numero_lotto
                    req.append(num_lotto->value().toString());
                    req.append("','");
                }
                if(Cell *date_lotto = xlsx.cellAt(i,18)) { //data_lotto
                    req.append(date_lotto->value().toString());
                    req.append("','");
                }
                if(Cell *mMag = xlsx.cellAt(i,19)) { //addMagazzino
                    req.append(mMag->value().toString());
                    req.append("','");
                }
                req.chop(2);
                req.append(");");
                query.prepare(req);
                progressDialog.show();
                progressDialog.update();
                if(query.exec())
                    qDebug() <<  qApp->translate("articoli","Inserimento effettuato");
                else
                    QMessageBox::warning(this,"FabariaGest",qApp->translate("articoli","Errore Inserimento:\n ")+query.lastError().text());
            }
        }
    }
    lista();
}

void articoli::aggiorna_da_excel(QString doc)
{
    Document xlsx(doc);

    for(int i=2; i <= xlsx.dimension().rowCount(); i++)
    {
        QProgressDialog progressDialog(this);
        progressDialog.setRange(0, xlsx.dimension().rowCount());
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setWindowTitle(qApp->translate("articoli","Aggionamento dati da excel"));
        progressDialog.setValue(i);
        progressDialog.setLabelText(qApp->translate("articoli","Aggionamento dati %1 di %2...")
                                    .arg(i).arg(xlsx.dimension().rowCount()));
        qApp->processEvents();
        QSqlQuery query,query_up;
        Cell *codarticolo = xlsx.cellAt(i,1);
        query_up.prepare("select * from articoli where cod_articolo='"+codarticolo->value().toString()+"'");
        query_up.exec();
        if(query_up.next())
        {
            QString req = "UPDATE articoli SET codbarre='";
            if(Cell *codbarre = xlsx.cellAt(i,2)) { //codbarre
                req.append(QString(codbarre->value().toString().toStdString().c_str()).replace("'","''"));
                req.append("',nome_articolo='");
            }
            if(Cell *name_articolo = xlsx.cellAt(i,3)) { //nome_articolo
                req.append(name_articolo->value().toString());
                req.append("',descrizione='");
            }
            if(Cell *desc = xlsx.cellAt(i,4)) { //descrizione
                req.append(desc->value().toString());
                req.append("',unita='");
            }
            if(Cell *un = xlsx.cellAt(i,5)) { //unità
                req.append(un->value().toString());
                req.append("',prezzo_unitario='");
            }
            if(Cell *pr_unit = xlsx.cellAt(i,6)) { //prezzo_unitario
                req.append(pr_unit->value().toString());
                req.append("',prezzo_s_iva='");
            }
            if(Cell *pr_s_iva = xlsx.cellAt(i,7)) { //prezzo_s_iva
                double prezzo = pr_s_iva->value().toDouble()+query_up.value(6).toDouble();
                req.append(QString::number(prezzo));
                req.append("',prezzo_c_iva='");
            }
            if(Cell *pr_c_iva = xlsx.cellAt(i,8)) { //prezzo_c_iva
                double prezzo = pr_c_iva->value().toDouble()+query_up.value(7).toDouble();
                req.append(QString::number(prezzo));
                req.append("',iva='");
            }
            if(Cell *m_iva = xlsx.cellAt(i,9)) { //iva
                req.append(m_iva->value().toString());
                req.append("',quantita='");
            }
            if(Cell *quan = xlsx.cellAt(i,10)) { //quantita
                double prezzo = quan->value().toDouble()+query_up.value(9).toDouble();
                req.append(QString::number(prezzo));
                req.append("',sconto='");
            }
            if(Cell *sc = xlsx.cellAt(i,11)) { //sconto
                req.append(sc->value().toString());
                req.append("',totale='");
            }
            if(Cell *tot = xlsx.cellAt(i,12)) { //totale
                double prezzo = tot->value().toDouble()+query_up.value(11).toDouble();
                req.append(QString::number(prezzo));
                req.append("',anno_ins='");
            }
            if(Cell *anno = xlsx.cellAt(i,13)) { //anno_ins
                req.append(anno->value().toString());
                req.append("',codice_listino='");
            }
            if(Cell *cod_list = xlsx.cellAt(i,14)) { //codice_listino
                req.append(cod_list->value().toString());
                req.append("',categoria='");
            }
            if(Cell *cat = xlsx.cellAt(i,15)) { //categoria
                req.append(cat->value().toString());
                req.append("',data_insert='");
            }
            if(Cell *date_insert = xlsx.cellAt(i,16)) { //data_insert
                req.append(date_insert->value().toString());
                req.append("' numero_lotto='");
            }
            if(Cell *num_lotto = xlsx.cellAt(i,17)) { //numero_lotto
                req.append(num_lotto->value().toString());
                req.append("' data_lotto='");
            }
            if(Cell *date_lotto = xlsx.cellAt(i,18)) { //data_lotto
                req.append(date_lotto->value().toString());
                req.append("' addMagazzino='");
            }
            if(Cell *date_insert = xlsx.cellAt(i,19)) { //addMagazzino
                req.append(date_insert->value().toString());
                req.append("' where cod_articolo='");
            }
            if(Cell *code_articolo = xlsx.cellAt(i,1)) { //cod_articolo
                req.append(QString(code_articolo->value().toString().toStdString().c_str()).replace("'","''"));
                req.append("','");
            }
            req.chop(2);
            req.append(";");
            query.prepare(req);
            progressDialog.show();
            progressDialog.update();
            if(query.exec())
                qDebug() <<  qApp->translate("articoli","Aggionamento effettuato");
            else
                QMessageBox::warning(this,"FabariaGest",qApp->translate("articoli","Errore aggiornamento:\n ")+query.lastError().text());
        }
    }
    lista();
}

void articoli::setImporti()
{
    prezzo_unitario->setText("0.00");
    sconto->setText("0.00");
    p_s_iva->setText("0.00");
    p_c_iva->setText("0.00");
    totale->setText("0.00");
}

articoli::~articoli()
{
}