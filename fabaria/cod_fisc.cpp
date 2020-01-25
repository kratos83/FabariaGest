/****************************************************************************
**
** Copyright (C) 2006-2018 Angelo e Calogero Scarna
** Contact: Angelo Scarnà (angelo.scarna@codelinsoft.it)
**          Calogero Scarnà (calogero.scarna@codelinsoft.it)
**          Team Codelinsoft (info@codelinsoft.it)
**
** Partial of code
 * author 2007 Francesco Betti Sorbelli <francesco.betti.sorbelli@gmail.com>
 *
 *
** This file is part of the project CodiceFiscale
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

#include "cod_fisc.h"
#include "ui_cod_fisc.h"

#include "about.h"
#include "print.h"
#include "defines.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <iostream>
#include <QCompleter>
#include <QDebug>
#include "databasesql.h"

using namespace DatabaseSql;

cod_fisc::cod_fisc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cod_fisc)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    interfaccia_signal();
}

/*
 * Avvio segnali per l'aggiornamento dell'anteprima
 * della tessera sanitaria.
 */
void cod_fisc::interfaccia_signal()
{
    connect(ui->actionEsci,SIGNAL(triggered()),this,SLOT(on_esci_clicked()));
    connect(ui->actionVisualizza_anteprima,SIGNAL(triggered(bool)),this,SLOT(vis_cod_fisc(bool)));
    connect(ui->actionStampa,SIGNAL(triggered()),this,SLOT(stampa_codice()));
    connect(ui->actionEsporta_in_pdf,SIGNAL(triggered()),this,SLOT(exp_pdf()));
    connect(ui->actionEsporta_in_immagine,SIGNAL(triggered()),this,SLOT(exp_img()));
    connect(ui->comboBox_sel,SIGNAL(currentIndexChanged(int)),this,SLOT(sel_combo_box()));

    /***********************************************************
     * Reload dati immagine
     ***********************************************************/
    connect(ui->lineEditCognome,SIGNAL(textEdited(QString)),this,SLOT(aggiorna()));
    connect(ui->lineEditNome,SIGNAL(textEdited(QString)),this,SLOT(aggiorna()));
    connect(ui->calendarWidget,SIGNAL(dateChanged(QDate)),this,SLOT(aggiorna()));
    connect(ui->radioButtonF,SIGNAL(clicked()),this,SLOT(aggiorna()));
    connect(ui->radioButtonM,SIGNAL(clicked()),this,SLOT(aggiorna()));
    connect(ui->comboBoxComune,SIGNAL(editTextChanged(QString)),this,SLOT(aggiorna()));

    QStringList lis;
    lis << qApp->translate("cod_fisc","Comuni ") << qApp->translate("cod_fisc","Stati");
    ui->comboBox_sel->addItems(lis);
    ui->actionVisualizza_anteprima->setChecked(true);
    ui->radioButtonM->setChecked(true);
    today = ui->calendarWidget->date();
    ui->calendarWidget->setDate(QDate::currentDate());
    img = QImage(":/images/codicefiscale_paint.png");
}

/*
 * La funzione sel_combo_box() serve a selezionare la possibilità di scegliere
 * se calcolare il codice fiscale italiano o estero.
 */
void cod_fisc::sel_combo_box()
{
    if(ui->comboBox_sel->currentIndex() == 0){
        ui->comboBoxComune->clear();
        initComuni();
        calcola_provincia();
        aggiorna();
    }
    else if(ui->comboBox_sel->currentIndex() == 1){
        ui->comboBoxComune->clear();
        initStati();
	calcola_pr_st();
	aggiorna();
    }
}

/*
 * La funzione initStati() serve a visualizzare gli stati all'interno della ComboBox.
 * All'interno troviamo gli stati esteri.
 */
void cod_fisc::initStati(){

    QStringList list;
    list = selectQueryWhile("select * from stati",0);
    ui->comboBoxComune->addItems(list);
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBoxComune->setCompleter(complete);
}

/*
 * La funzione initComuni() serve a visualizzare i comuni all'interno della ComboBox.
 * All'interno troviamo i comuni italiani ma acnhe quelli esteri.
 */
void cod_fisc::initComuni(){

    QStringList list;
    list = selectQueryWhile("select * from codcomune",0);
    ui->comboBoxComune->addItems(list);
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBoxComune->setCompleter(complete);

}

/*
 * La funzione on_pushButtonCalcola_clicked() serve a dire al programma di
 * calcolare il codice fiscale.
 */
void cod_fisc::on_pushButtonCalcola_clicked(){


    /*
     * Inizializzo per la scrittura del codice fiscale
     */
     cognome = ui->lineEditCognome->displayText().toUpper();
     nome = ui->lineEditNome->displayText().toUpper();
     sesso = (ui->radioButtonM->isChecked() == true);
     comune = ui->comboBoxComune->currentText().toUpper();

     QDate date = ui->calendarWidget->date();
     dataGiorno = date.day();
     dataMese = date.month();
     dataAnno = date.year();

     if (!checkInput())
             return;

     /*
      * Avvio delle seguenti funzioni per calcolare
      * il codice fiscale
      */

     calculateCognome();
     calculateNome();
     calculateData();
     if(ui->comboBox_sel->currentIndex() == 0){
        calculateComune();
        calcola_provincia();
     }
     else{
         calculateStato();
     }
     calculateLetteraControllo();

     merge();
     aggiorna();
}

/*
 * La funzione on_pushButtonCancella_clicked() serve a cancella ciò che è scritto
 * all'interno delle QLineEdit.
 */
void cod_fisc::on_pushButtonCancella_clicked(){

    /*
     * La fuonzione on_pushButtonCancella_clicked();
     * serve a pulire le qlineedit.
     */
    ui->lineEditCognome->setText("");
    ui->lineEditNome->setText("");
    ui->radioButtonM->setChecked(true);
    ui->comboBoxComune->setCurrentIndex(0);
    ui->calendarWidget->setDate(QDate::currentDate());
    ui->lineEditCodiceFiscale->setText("");
}

/*
 * Calcola il cognome dell'utente
 */
void cod_fisc::calculateCognome(){

    block1 = "";

            /*
             * Il ciclo for in questione serve a
             * calcolare le consonanti.
             */
            for (int i = 0; i < cognome.length(); i++)
            {
                    QString tmp = QString(cognome[i]);

                    if (checkVocale(tmp) == 0)
                    {
                            block1 += tmp;

                            if (block1.length() == 3)
                                    return;
                    }
            }

            /*
             * Il ciclo for in questione serve a
             * calcolare le vocali.
             */
            for (int i = 0; i < cognome.length(); i++)
            {
                    QString tmp = QString(cognome[i]);

                    if (checkVocale(tmp) == 1)
                    {
                            block1 += tmp;

                            if (block1.length() == 3)
                                    return;
                    }
            }

            /*
             * Il ciclo if in questione serve a
             * calcolare le eventuali X
             */
            if (block1.length() == 1)
                    block1 += "XX";
            else if (block1.length() == 2)
                    block1 += "X";
}

/*
 * Calcola il nome dell'utente
 */
void cod_fisc::calculateNome(){

    block2 = "";
            bool ok = false;

            /*
             * Il ciclo for in questione serve a
             * calcolare le consonanti, test sullo scarto della
             * 2° consonante.
             */
            for (int i = 0; i < nome.length(); i++)
            {
                    QString tmp = QString(nome[i]);

                    if (checkVocale(tmp) == 0)
                    {
                            if ((ok == false) && (block2.length() == 1))
                                    ok = true;
                            else
                                    block2 += tmp;

                            if (block2.length() == 3)
                                    return;
                    }
            }

            /*
             * Se non bastano le consonanti, riprovo
             * nuovamente a rifare daccapo.
             */
            block2 = "";

            /*
             * Il ciclo for in questione serve a
             * calcolare le consonanti.
             */
            for (int i = 0; i < nome.length(); i++)
            {
                    QString tmp = QString(nome[i]);

                    if (checkVocale(tmp) == 0)
                    {
                            block2 += tmp;

                            if (block2.length() == 3)
                                    return;
                    }
            }

            /*
             * Il ciclo for in questione serve a
             * calcolare le vocali.
             */
            for (int i = 0; i < nome.length(); i++)
            {
                    QString tmp = QString(nome[i]);

                    if (checkVocale(tmp) == 1)
                    {
                            block2 += tmp;

                            if (block2.length() == 3)
                                    return;
                    }
            }

            /*
             * Il ciclo if in questione serve a
             * calcolare le eventuali X
             */
            if (block2.length() == 1)
                    block2 += "XX";
            else if (block2.length() == 2)
                    block2 += "X";
}

/*
 * Calcola la data di nascita
 */
void cod_fisc::calculateData(){

            /*
             * Calcolo l'anno per il calcolo
             * del codice fiscale.
             */
            QString anno;
            anno.setNum(dataAnno);
            block3 = anno.right(2);

            /*
             * Calcolo il mese per il calcolo
             * del codice fiscale.
             */
            switch (dataMese)
            {
                    case 1:
                            block3 += "A"; // Gennaio = A
                            break;
                    case 2:
                            block3 += "B"; // Febbraio = B
                            break;
                    case 3:
                            block3 += "C"; // Marzo = C
                            break;
                    case 4:
                            block3 += "D"; // Aprile = D
                            break;
                    case 5:
                            block3 += "E"; // Maggio = E
                            break;
                    case 6:
                            block3 += "H"; // Giugno = H
                            break;
                    case 7:
                            block3 += "L"; // Luglio = L
                            break;
                    case 8:
                            block3 += "M"; // Agosto = M
                            break;
                    case 9:
                            block3 += "P"; // Settembre = P
                            break;
                    case 10:
                            block3 += "R"; // Ottobre = R
                            break;
                    case 11:
                            block3 += "S"; // Novembre = S
                            break;
                    case 12:
                            block3 += "T"; // Dicembre = T
                            break;
            }

            /*
             * Calcolo del giorno per il calcolo
             * del codice fiscale.
             */
            QString giorno;
            if (ui->radioButtonM->isChecked())
            {
                    giorno.setNum(dataGiorno);
                    if (giorno.length() == 1)
                    {
                            QString tmp = giorno;
                            giorno = "0" + tmp;
                    }
            }
            else if (ui->radioButtonF->isChecked())
                    giorno.setNum(dataGiorno+40);

            block3 += giorno;
}

/*
 * Seleziona il comune di
 * appartenenza.
 */
void cod_fisc::calculateComune()
{
    QStringList list;
    list = selectQueryWhile("select * from codcomune where comune='"+ui->comboBoxComune->currentText()+"'",1);
    block4 = list.at(0).left(4);
}

/*
 * Calcola la provincia adatta
 */

void cod_fisc::calcola_provincia()
{ 
    QStringList list;
    list = selectQueryWhile("select * from codcomune where comune='"+ui->comboBoxComune->currentText()+"'",2);
    prov = list.at(0);
}

/*
 * Calcola la provincia adatta
 */

void cod_fisc::calcola_pr_st()
{
    QStringList list;
    list << selectQueryWhile("select * from stati where stato='"+ui->comboBoxComune->currentText()+"'",2);
    pr_st = list.at(0);
}

/*
 * Seleziona lo stato di
 * appartenenza.
 */
void cod_fisc::calculateStato()
{
    QStringList list;
    list << selectQueryWhile("select * from stati where stato='"+ui->comboBoxComune->currentText()+"'",1);
    block4 = list.at(0).left(4);
}

/*
 * Calcola la lettera che serve a completare
 * il codice fiscale mediante controllo
 * delle funzioni getEvenCode(QString) se il risultato
 * è pari e getOddCode(QString) se il risultato è dispari.
 */
void cod_fisc::calculateLetteraControllo(){

    int sum = 0;
            QString CFtmp = block1+block2+block3+block4;

            for (int i = 0; i < 15; i++)
            {
                    if ((i % 2) == 1)
                            sum += getEvenCode(QString(CFtmp[i]));
                    else
                            sum += getOddCode(QString(CFtmp[i]));
            }

            int total = sum % 26;
            block5 = mapNumber(total);
}

/*
 * Calcola codice fiscale mediante la lettura
 * dei 5 blocchi.
 */
void cod_fisc::merge(){

    CF = block1+block2+block3+block4+block5;

            ui->lineEditCodiceFiscale->setText(CF);
}

/*
* Controlla se esiste una vocale e se trova uno spazio
* ritorna -1, invece se trova una consonante ritorna 0,
* altrimenti 1  se è una vocale.
*/
int cod_fisc::checkVocale(QString v){

            if (v.toUpper() == " ")
                    return -1;

            if ((v.toUpper() == "A") || (v.toUpper() == "E") || (v.toUpper() == "I") || (v.toUpper() == "O") || (v.toUpper() == "U"))
                    return 1;
            return 0;
}

/*
 * La funzione checkInput() serve a visualizzare
 * una finestra di errore se non viene digitato niente
 * all'interno delle QLineEdit.
 */
bool cod_fisc::checkInput(){

    QString errors = "";

            if (ui->lineEditCognome->displayText() == "")
                    errors += qApp->translate("cod_fisc","Inserisci il cognome\n");
            if (ui->lineEditNome->displayText() == "")
                    errors += qApp->translate("cod_fisc","Inserisci il nome\n");
            if ((!ui->radioButtonM->isChecked()) && (!ui->radioButtonF->isChecked()))
                    errors += qApp->translate("cod_fisc","Inserisci il sesso\n");

            if (errors != "")
            {
                    errors += qApp->translate("cod_fisc","\nFinisci di inserire i dati correttamente");
                    QMessageBox::critical(this, qApp->translate("cod_fisc","Errore"), errors);
                    return false;
            }

            return true;
}

/*
 * La funzione getEvenCode(QString) ritorna un numero
 * pari se (i%2 = 1).
 */
int cod_fisc::getEvenCode(QString n){

    // calcolo del carattere
            if ((n == "A") || (n == "0"))
                    return 0;
            if ((n == "B") || (n == "1"))
                    return 1;
            if ((n == "C") || (n == "2"))
                    return 2;
            if ((n == "D") || (n == "3"))
                    return 3;
            if ((n == "E") || (n == "4"))
                    return 4;
            if ((n == "F") || (n == "5"))
                    return 5;
            if ((n == "G") || (n == "6"))
                    return 6;
            if ((n == "H") || (n == "7"))
                    return 7;
            if ((n == "I") || (n == "8"))
                    return 8;
            if ((n == "J") || (n == "9"))
                    return 9;
            if (n == "K")
                    return 10;
            if (n == "L")
                    return 11;
            if (n == "M")
                    return 12;
            if (n == "N")
                    return 13;
            if (n == "O")
                    return 14;
            if (n == "P")
                    return 15;
            if (n == "Q")
                    return 16;
            if (n == "R")
                    return 17;
            if (n == "S")
                    return 18;
            if (n == "T")
                    return 19;
            if (n == "U")
                    return 20;
            if (n == "V")
                    return 21;
            if (n == "W")
                    return 22;
            if (n == "X")
                    return 23;
            if (n == "Y")
                    return 24;
            if (n == "Z")
                    return 25;

            return -1;
}

/*
 * La funzione getEvenCode(QString) ritorna un numero
 * dispari se (i%2 != 1).
 */
int cod_fisc::getOddCode(QString c){

    // calcolo del carattere
            if ((c == "A") || (c == "0"))
                    return 1;
            if ((c == "B") || (c == "1"))
                    return 0;
            if ((c == "C") || (c == "2"))
                    return 5;
            if ((c == "D") || (c == "3"))
                    return 7;
            if ((c == "E") || (c == "4"))
                    return 9;
            if ((c == "F") || (c == "5"))
                    return 13;
            if ((c == "G") || (c == "6"))
                    return 15;
            if ((c == "H") || (c == "7"))
                    return 17;
            if ((c == "I") || (c == "8"))
                    return 19;
            if ((c == "J") || (c == "9"))
                    return 21;
            if (c == "K")
                    return 2;
            if (c == "L")
                    return 4;
            if (c == "M")
                    return 18;
            if (c == "N")
                    return 20;
            if (c == "O")
                    return 11;
            if (c == "P")
                    return 3;
            if (c == "Q")
                    return 6;
            if (c == "R")
                    return 8;
            if (c == "S")
                    return 12;
            if (c == "T")
                    return 14;
            if (c == "U")
                    return 16;
            if (c == "V")
                    return 10;
            if (c == "W")
                    return 22;
            if (c == "X")
                    return 25;
            if (c == "Y")
                    return 24;
            if (c == "Z")
                    return 23;

            return -1;
}

/*
 * La funzione mapNumber(int) trova l'ultima lettera
 * per poter completare il codice fiscale.
 */
QString cod_fisc::mapNumber(int n){

    // calcolo del carattere
            if (n == 0)
                    return "A";
            if (n == 1)
                    return "B";
            if (n == 2)
                    return "C";
            if (n == 3)
                    return "D";
            if (n == 4)
                    return "E";
            if (n == 5)
                    return "F";
            if (n == 6)
                    return "G";
            if (n == 7)
                    return "H";
            if (n == 8)
                    return "I";
            if (n == 9)
                    return "J";
            if (n == 10)
                    return "K";
            if (n == 11)
                    return "L";
            if (n == 12)
                    return "M";
            if (n == 13)
                    return "N";
            if (n == 14)
                    return "O";
            if (n == 15)
                    return "P";
            if (n == 16)
                    return "Q";
            if (n == 17)
                    return "R";
            if (n == 18)
                    return "S";
            if (n == 19)
                    return "T";
            if (n == 20)
                    return "U";
            if (n == 21)
                    return "V";
            if (n == 22)
                    return "W";
            if (n == 23)
                    return "X";
            if (n == 24)
                    return "Y";
            if (n == 25)
                    return "Z";

            return "_";
}

/*
 * Serve a eliminare la ui, per poterla
 * riaprire in un secondo momento.
 */
cod_fisc::~cod_fisc()
{
    delete ui;
}


/*
 * La funzione vis_cod_fisc(bool) serve a visualizzre l'anteprima della tessera
 * sanitaria.
 *
 * return click;
 */
bool cod_fisc::vis_cod_fisc(bool click)
{

    if(click == true)
    {
        ui->groupBox_2->setVisible(true);
        setFixedSize(856,454);
    }
    else if(click == false)
       {
        ui->groupBox_2->setVisible(false);
        setFixedSize(427,454);
       }

    return click;
}

/*
 * La funzione paintEvent(QPaintEvent *event) serve disegnare
 * il codice fiscale in immagine.
 */
void cod_fisc::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    painter = new QPainter(this);
    disegna(painter);
}

/*
 * Visualizza i dati nella schermata della tessera
 * sanitaria.
 *
 * Risolto bug uso intensivo cpu.
 */
void cod_fisc::disegna(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->drawImage(435,60,img);

    painter->setPen(QPen(Qt::black, 12, Qt::SolidLine, Qt::RoundCap));
    painter->drawText(330,130,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,CF);
    painter->drawText(300,155,480,100, Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,ui->lineEditCognome->text());
    painter->drawText(300,180,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap, ui->lineEditNome->text());
    painter->drawText(330,210,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap, ui->comboBoxComune->currentText());
    if(ui->comboBox_sel->currentIndex() == 0){
        painter->drawText(330,235,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,prov);
    }
    else{
        painter->drawText(330,235,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,pr_st);
    }
    painter->drawText(330,260,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,ui->calendarWidget->date().toString(QString("dd/MM/yyyy")));
    if(ui->radioButtonM->isChecked())
    {
        painter->drawText(550,195,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"M");
    }
    else if(ui->radioButtonF->isChecked())
    {
        painter->drawText(550,195,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"F");
    }
    painter->setFont(QFont("Arial",18,50));
    painter->drawText(513,250,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"FAC SIMILE");
    painter->end();
}

/*
 * Aggiorna i dati indolore nella schermata
 * della tessera sanitaria
 */
void cod_fisc::aggiorna()
{
    update();
}

/*
 * La funzione stampa_codice() serve a stampare il codice fiscale sulla tessera
 * sanitaria.
 */
void cod_fisc::stampa_codice()
{
    QPrinter printer(QPrinter::HighResolution);

    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("cod_fisc","Anteprima di stampa."));
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(anteprima(QPrinter*)));
    preview.exec();
}

/*
 * La funzione anteprima(QPrinter *printer) serve a visualizzare l'anteprima
 * di stampa della tessera sanitaria.
 */
void cod_fisc::anteprima(QPrinter *printer)
{
    Q_UNUSED(printer);
    Stampe *stampa = new Stampe();
    QImage image_800x600 = QImage(":/images/codicefiscale_800x600.png");
    if(ui->comboBox_sel->currentIndex() == 0){
       stampa->print_codice(printer,image_800x600,ui->lineEditCognome->text(),ui->lineEditNome->text(),ui->radioButtonM->isChecked(),ui->radioButtonF->isChecked(),ui->comboBoxComune->currentText(),prov,CF,ui->calendarWidget->date());
    }
    else{
        stampa->print_codice(printer,image_800x600,ui->lineEditCognome->text(),ui->lineEditNome->text(),ui->radioButtonM->isChecked(),ui->radioButtonF->isChecked(),ui->comboBoxComune->currentText(),pr_st,CF,ui->calendarWidget->date());
    }

}

/*
 * La funzione on_esci_clicked() serve a dire al programma di uscire
 * dal calcolo del codice fiscale.
 */
void cod_fisc::on_esci_clicked()
{
    close();
}

/*
 * La funzione exp_img() serve a esportare la tessera sanitaria in formato
 * png.
 */
void cod_fisc::exp_img()
{
    //FIXME
    /*
     * Bug presente nell'apertura della finestra di dialogo
     * su ubuntu e kubuntu
     */
    QString fileName;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setWindowTitle(qApp->translate("cod_fisc","Esporta immagine"));
    dialog.setNameFilter(qApp->translate("cod_fisc","Images (*.png);;Tutti i file(*.*);;PNG(*.png)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
      if(dialog.exec())
        dialog.selectFile(fileName);


      if (fileName.length() != 0) {
         // Aggiunge estensione alla fine del file se non c'è
          if (fileName.indexOf(".png") < 0) {
              fileName += ".png";
          }
      }


                QImage image_800x600 = QImage(":/images/codicefiscale_800x600.png");
                painter = new QPainter(&image_800x600);
                painter->setFont(QFont("Arial",18,50));
                painter->setPen(QPen(Qt::black, 12, Qt::SolidLine, Qt::RoundCap));
                painter->drawText(60,190,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,CF);
                painter->drawText(30,240,480,100, Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,ui->lineEditCognome->text());
                painter->drawText(30,290,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap, ui->lineEditNome->text());
                painter->drawText(40,340,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap, ui->comboBoxComune->currentText());
                if(ui->comboBox_sel->currentIndex() == 0)
                {
                        painter->drawText(40,395,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,prov);
                }
                else{
                        painter->drawText(40,395,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,pr_st);
                }
                painter->drawText(30,450,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,ui->calendarWidget->date().toString(QString("dd/MM/yyyy")));
                if(ui->radioButtonM->isChecked())
                {
                    painter->drawText(480,335,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"M");
                }
                else if(ui->radioButtonF->isChecked())
                {
                    painter->drawText(480,335,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"F");
                }
                painter->setFont(QFont("Arial",18,50));
                painter->drawText(400,440,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"FAC SIMILE");
                painter->end();
                image_800x600.save(fileName);
}

/*
 * La funzione exp_pdf() serve a esportare la tessera sanitaria in formato
 * pdf.
 */
void cod_fisc::exp_pdf()
{
        QString fileName;
        //FIXME
        /*
         * Bug presente nell'apertura della finestra di dialogo
         * su ubuntu e kubuntu
         */
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setWindowTitle(qApp->translate("cod_fisc","Esporta in PDF"));
        dialog.setNameFilter(qApp->translate("cod_fisc","Pdf Files(*.pdf);;Tutti i file(*.*)"));
        dialog.setViewMode(QFileDialog::Detail);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        if(dialog.exec())
            dialog.selectFile(fileName);

        if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".pdf") < 0) {
                    fileName += ".pdf";
                }
            }
           QPrinter *printer = new QPrinter(QPrinter::HighResolution);
           printer->setPageSize(QPrinter::A0);
	   Q_UNUSED(printer);
           printer->setOutputFileName(fileName);
           QImage image_800x600 = QImage(":/images/codicefiscale_800x600.png");
           Stampe *stampa = new Stampe();
           if(ui->comboBox_sel->currentIndex() == 0){
                    stampa->print_codice(printer,image_800x600,ui->lineEditCognome->text(),ui->lineEditNome->text(),ui->radioButtonM->isChecked(),ui->radioButtonF->isChecked(),ui->comboBoxComune->currentText(),prov,CF,ui->calendarWidget->date());
           }
           else{
                    stampa->print_codice(printer,image_800x600,ui->lineEditCognome->text(),ui->lineEditNome->text(),ui->radioButtonM->isChecked(),ui->radioButtonF->isChecked(),ui->comboBoxComune->currentText(),pr_st,CF,ui->calendarWidget->date());
           }
}
