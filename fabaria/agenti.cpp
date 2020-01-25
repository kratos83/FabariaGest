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
#include "agenti.h"
#include "ui_agenti.h"
#include "itdelegato.h"
#include "defines.h"
#include "queryexception.h"
#include "print.h"

#include <QMessageBox>
#include <QModelIndex>
#include <QCompleter>
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

agenti::agenti(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::agenti)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(qApp->translate("agenti","Agenti"));

    //Esco dalla finestra di dialogo
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    ui->esci->setShortcut(tr("Ctrl+E"));

    connect(ui->cerca,&QLineEdit::textChanged,this,&agenti::cerca_utente);

    //Nuovo agente
    connect(ui->nuovo,SIGNAL(clicked()),this,SLOT(nuovo()));
    ui->nuovo->setShortcut(tr("Ctrl+N"));

    //Elimina agente
    connect(ui->elimina,SIGNAL(clicked()),this,SLOT(elimina()));
    ui->elimina->setShortcut(tr("Ctrl+D"));

    //Inserisci agente
    connect(ui->salva,SIGNAL(clicked()),this,SLOT(inserisci()));
    ui->salva->setShortcut(tr("Ctrl+S"));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(clickgrid()));

    //Aggiora agente
    connect(ui->aggiorna,SIGNAL(clicked()),this,SLOT(aggiorna()));
    ui->aggiorna->setShortcut(tr("Ctrl+M"));

    //Stampa lista agenti
    connect(ui->stampa,SIGNAL(clicked()),this,SLOT(stampa()));
    ui->stampa->setShortcut(tr("Ctrl+P"));

    //Esporta pdf agenti
    connect(ui->pdf,SIGNAL(clicked()),this,SLOT(esporta_pdf()));
    ui->pdf->setShortcut(tr("Ctrl+F"));

    //Leggo il cap
    m_cap = ui->cap->addAction(QIcon(":/images/edit-find.png"),QLineEdit::TrailingPosition);
    m_cap->setToolTip(qApp->translate("agenti","Seleziona cap"));
    m_cap->setCheckable(true);
    m_cap->setChecked(false);
    connect(m_cap,&QAction::toggled,this,&agenti::inst_cap);

    //Leggo la città
    m_comuni = ui->localita->addAction(QIcon(":/images/edit-find.png"),QLineEdit::TrailingPosition);
    m_comuni->setToolTip(qApp->translate("agenti","Seleziona località"));
    m_comuni->setCheckable(true);
    m_comuni->setChecked(false);
    connect(m_comuni,&QAction::toggled,this,&agenti::instPaese);

    //Inizializza la lista:
    mod_grid = new QSqlRelationalTableModel();
    //Disabilito id edit
    ui->id_agenti->setEnabled(false);

    //Setto al 10% la commissione
    ui->commissione->setText("10%");

    //Setto le funzioni da rendere avviabili all'avvio
    lista();
    somma();
    nuovo();
}

void agenti::nuovo()
{
    clear_text();
    if(selectQuery("select count(id)+1 from lista_agenti",0).toString() == "0")
            ui->id_agenti->setText("1");
    else {
        QString text = selectQuery("select count(id)+1 from lista_agenti",0).toString();
        ui->id_agenti->setText(text);
    }
}

void agenti::inserisci()
{
    insertQuery("INSERT INTO lista_agenti "
                  "VALUES ('"+ui->id_agenti->text()+"','"+ui->nome->text().replace("'","''")+"','"+ui->cognome->text().replace("'","''")+"','"+ui->indirizzo->text().replace("'","''")+"',"
                  "'"+ui->localita->text().replace("'","''")+"','"+ui->cap->text()+"','"+ui->prov->text()+"','"+ui->telefono->text()+"',"
                  "'"+ui->email->text()+"','"+ui->note->text().replace("'","''")+"','"+ui->commissione->text().remove("%")+"','"+QDate::currentDate().toString("yyyy/MM/dd")+"')","lista_agenti");
    emit save_agent();
    lista();
    somma();
    nuovo();
}

void agenti::aggiorna()
{
    updateQuery("UPDATE lista_agenti SET nome='"+ui->nome->text().replace("'","''")+"', cognome='"+ui->cognome->text().replace("'","''")+"', note='"+ui->note->text().replace("'","''")+"', "
                  " indirizzo='"+ui->indirizzo->text().replace("'","''")+"', cap='"+ui->cap->text()+"', localita='"+ui->localita->text().replace("'","''")+"', prov='"+ui->prov->text()+"', "
                  " telefono='"+ui->telefono->text()+"', provvigione='"+ui->commissione->text().remove("%")+"', email='"+ui->email->text().replace("'","''")+"', data='"+QDate::currentDate().toString("yyyy/MM/dd")+"' "
                  "WHERE id='"+ui->id_agenti->text()+"'","lista_agenti");
    emit save_agent();
    lista();
    somma();
    nuovo();
}

void agenti::elimina()
{
    QString flag_controllo = "NO";

    if(!ui->tableView->selectionModel()->currentIndex().isValid()) {
        QMessageBox::warning(this,"Fabaria Gest",qApp->translate("agenti","Selezionare una riga da eliminare..."));
    }
    else if (!ui->id_agenti->text().isEmpty())
    {
        eliminaQuery("DELETE FROM lista_agenti WHERE id='"+ui->id_agenti->text()+"'","lista_agenti");
    }
    emit save_agent();
    lista();
    somma();
    nuovo();
}

void agenti::cerca_utente(const QString &)
{
    QString text;
    text = ui->cerca->text();
    QStringList list_st;
    list_st = selectQueryWhile("select concat(cognome,' ', nome) from lista_agenti",0);
    QCompleter *complete = new QCompleter(list_st,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cerca->setCompleter(complete);
    filtro(text);
}

void agenti::cerca_query()
{
    ui->dettagli->setText(qApp->translate("agenti","ID: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","Nome: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","cognome: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),2),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","Indirizzo: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),3),Qt::DisplayRole).toString()+"\n"+
                          QString::fromUtf8(qApp->translate("agenti","Località: ").toStdString().c_str())+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),4),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","CAP: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),5),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","Provincia: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),6),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","Telefono: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),7),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","E-mail: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),8),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","Note: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),9),Qt::DisplayRole).toString()+"\n"+
                          qApp->translate("agenti","Provvigione: ")+mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),10),Qt::DisplayRole).toString()+"\n");

        ui->nome->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString());
        ui->cognome->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),2),Qt::DisplayRole).toString());
        ui->indirizzo->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),3),Qt::DisplayRole).toString());
        ui->localita->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),4),Qt::DisplayRole).toString());
        ui->cap->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),5),Qt::DisplayRole).toString());
        ui->prov->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),6),Qt::DisplayRole).toString());
        ui->telefono->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),7),Qt::DisplayRole).toString());
        ui->email->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),8),Qt::DisplayRole).toString());
        ui->note->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),9),Qt::DisplayRole).toString());
        ui->commissione->setText(mod_grid->data(mod_grid->index(ui->tableView->selectionModel()->currentIndex().row(),10),Qt::DisplayRole).toString());
}

void agenti::clear_text()
{
    ui->id_agenti->clear();
    ui->nome->clear();
    ui->cognome->clear();
    ui->indirizzo->clear();
    ui->localita->clear();
    ui->cap->clear();
    ui->prov->clear();
    ui->telefono->clear();
    ui->email->clear();
    ui->note->clear();
    ui->cerca->clear();
    ui->dettagli->clear();
}

void agenti::filtro(QString m_text)
{
    qDebug() << m_text;
    if(m_text.length() == 0) {
        lista();
        ui->tabWidget->setCurrentIndex(0);
    }
    else {
        ui->tabWidget->setCurrentIndex(1);
        mod_grid->setFilter(QString("concat(cognome,' ',nome)='" +m_text+"'").arg(m_text));
    }
}

void agenti::stampa()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(anteprima(QPrinter*)));
    preview.exec();
}

void agenti::anteprima(QPrinter* printer)
{
    Q_UNUSED(printer);
    printer->setResolution(90);
    printer->setPageMargins(10.0,10.0,10.0,10.0,QPrinter::Millimeter);
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);
    QString id,nome,cognome,indirizzo,localita,cap,provincia,telefono,email,note,provvigione,tabella;
    QFile file(":/stampa/agenti.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    while(!file.atEnd())
    {
        tabella = file.readAll();
        for(int i = 0; i < mod_grid->rowCount(); ++i)
        {
            QSqlRecord record = mod_grid->record(i);
            if(!record.isEmpty())
            {
                id = record.value("id").toString();
                nome = record.value("nome").toString();
                cognome = record.value("cognome").toString();
                indirizzo = record.value("indirizzo").toString();
                localita = record.value("localita").toString();
                cap = record.value("cap").toString();
                provincia = record.value("prov").toString();
                telefono = record.value("telefono").toString();
                email = record.value("email").toString();
                note = record.value("note").toString();
                provvigione = record.value("provvigione").toString();
                tabella.replace("LIST AGENT",qApp->translate("agenti","LISTA AGENTI"));
                tabella.replace("ID",qApp->translate("agenti","Id"));
                tabella.replace("NAME",qApp->translate("agenti","Nome"));
                tabella.replace("COGNOME",qApp->translate("agenti","Cognome"));
                tabella.replace("ADDRESS",qApp->translate("agenti","Indirizzo"));
                tabella.replace("CITY",qApp->translate("agenti","Città"));
                tabella.replace("CAP",qApp->translate("agenti","C.A.P."));
                tabella.replace("PROVINCIA",qApp->translate("agenti","Provincia"));
                tabella.replace("TELEFONO",qApp->translate("agenti","Telefono"));
                tabella.replace("NOTE",qApp->translate("agenti","Note"));
                tabella.replace("PROVVIGIONE",qApp->translate("agenti","Provvigione"));
                tabella.replace("EMAIL",qApp->translate("agenti","email"));
                tabella.replace("<td id=\"id\" valign=\"top\" align=\"center\">","<td id=\"id\" valign=\"top\" align=\"center\">"+id+"<br>");
                tabella.replace("<td id=\"name\" valign=\"top\" align=\"center\">","<td id=\"name\" valign=\"top\" align=\"center\">"+nome+"<br>");
                tabella.replace("<td id=\"surname\" valign=\"top\" align=\"center\">","<td id=\"surname\" valign=\"top\" align=\"center\">"+cognome+"<br>");
                tabella.replace("<td id=\"address\" valign=\"top\" align=\"center\">","<td id=\"address\" valign=\"top\" align=\"center\">"+indirizzo+"<br>");
                tabella.replace("<td id=\"city\" valign=\"top\" align=\"center\">","<td id=\"city\" valign=\"top\" align=\"center\">"+localita+"<br>");
                tabella.replace("<td id=\"cap\" valign=\"top\" align=\"center\">","<td id=\"cap\" valign=\"top\" align=\"center\">"+cap+"<br>");
                tabella.replace("<td id=\"provincia\" valign=\"top\" align=\"center\">","<td id=\"provincia\" valign=\"top\" align=\"center\">"+provincia+"<br>");
                tabella.replace("<td id=\"telefono\" valign=\"top\" align=\"center\">","<td id=\"telefono\" valign=\"top\" align=\"center\">"+telefono+"<br>");
                tabella.replace("<td id=\"note\" valign=\"top\" align=\"center\">","<td id=\"note\" valign=\"top\" align=\"center\">"+note+"<br>");
                tabella.replace("<td id=\"provvigione\" valign=\"top\" align=\"center\">","<td id=\"provvigione\" valign=\"top\" align=\"center\">"+provvigione+"%"+"<br>");
                tabella.replace("<td id=\"email\" valign=\"top\" align=\"center\">","<td id=\"email\" valign=\"top\" align=\"center\">"+email+"<br>");
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

void agenti::esporta_pdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("agenti","Esporta PDF"),
                       "*.pdf", qApp->translate("agenti","Pdf Files(*.pdf);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
        // Aggiunge estensione alla fine del file se non c'è
        if (fileName.indexOf(".pdf") < 0) {
            fileName += ".pdf";
        }
    }
    if(!fileName.isEmpty()){
        QPrinter *printer = new QPrinter(QPrinter::HighResolution);
        printer->setPageSize(QPrinter::A4);
        printer->setOrientation(QPrinter::Landscape);
        Q_UNUSED(printer);
        printer->setOutputFileName(fileName);
        anteprima(printer);
    }
}

void agenti::clickgrid()
{
    QString id_new;
    int riga = ui->tableView->selectionModel()->currentIndex().row();
    QModelIndex elemento = mod_grid->index(riga,0,QModelIndex());

    id_new = mod_grid->data(elemento).toString();
    ui->id_agenti->setText(id_new);

    cerca_query();
}

void agenti::somma()
{
    ui->totale_list->clear();
    ui->totale_list->setText(QString::fromUtf8("N°  ")+QString::number(mod_grid->rowCount()));
}

void agenti::lista()
{
    mod_grid->setTable("lista_agenti");
    mod_grid->select();
    mod_grid->setHeaderData(0,Qt::Horizontal,qApp->translate("agenti","ID"));
    mod_grid->setHeaderData(1,Qt::Horizontal,qApp->translate("agenti","Nome"));
    mod_grid->setHeaderData(2,Qt::Horizontal,qApp->translate("agenti","Cognome"));
    mod_grid->setHeaderData(3,Qt::Horizontal,qApp->translate("agenti","Indirizzo"));
    mod_grid->setHeaderData(4,Qt::Horizontal,QString::fromUtf8(qApp->translate("agenti","Località").toStdString().c_str()));
    mod_grid->setHeaderData(5,Qt::Horizontal,qApp->translate("agenti","CAP"));
    mod_grid->setHeaderData(6,Qt::Horizontal,qApp->translate("agenti","PROV"));
    mod_grid->setHeaderData(7,Qt::Horizontal,qApp->translate("agenti","Telefono"));
    mod_grid->setHeaderData(8,Qt::Horizontal,qApp->translate("agenti","E-mail"));
    mod_grid->setHeaderData(9,Qt::Horizontal,qApp->translate("agenti","Note"));
    mod_grid->setHeaderData(10,Qt::Horizontal,qApp->translate("agenti","Provvigione"));

    ui->tableView->setModel(mod_grid);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setColumnHidden(11,true);
    ui->tableView->setModel(mod_grid);

    ui->tableView->setItemDelegateForColumn(12,new coldxdelegato(this));
}

void agenti::inst_cap()
{
    _comune = new Comuni("cap",this);
    connect(_comune,&Comuni::addComuni,this,&agenti::inst_paese);
    _comune->exec();
}

void agenti::instPaese()
{
    _comune = new Comuni("comune",this);
    connect(_comune,&Comuni::addComuni,this,&agenti::inst_paese);
    _comune->exec();
}

void agenti::inst_paese(QString m_com, QString m_pr, QString m_cp)
{
    ui->localita->setText(m_com);
    ui->prov->setText(m_pr);
    ui->cap->setText(m_cp);
}

agenti::~agenti()
{
    delete ui;
}
