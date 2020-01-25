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
** qApp->translate("Stampe",DATA), OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/
#include "print.h"
#include <QDir>
#include <qdebug.h>
#include <QPen>
#include <QColor>
#include <QLocale>
#include <QPainter>
#include <QProcess>
#include "qstring.h"
#include "locale.h"
#include "settingsmanager.h"
#include <iostream>

using namespace std;
using namespace Defines;

Stampe::Stampe()
{
   // Creazione oggetto printer (puntatore)
   // ed impostazioni varie sul formato pagina, nome file di appoggio
   // formato file prodotto
   // impostazione margine e risoluzione
    printer = new QPrinter(QPrinter::HighResolution);
        // Formato A4 = 210*297 millimetri
        // Formato A4 = 210*297 millimetri

        printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
        printer->setPaperSize(QPrinter::A4);
        #if defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
        printer->setOutputFormat(QPrinter::PdfFormat);
        #endif
        #ifdef Q_OS_MAC
        printer->setOutputFormat(QPrinter::PdfFormat);
        #endif
        printer->setResolution(254);
            // Formato A4 = millimetri 210*297
            // un pollice = 25,4 millimetri
            // risoluzione posta a 254 punti per pollice
            // quindi 10 punti = 1 millimetro
        stampaDlg = new QPrintDialog();
       stampaDlg->setEnabledOptions(QAbstractPrintDialog::PrintPageRange);
        //QVERIFY(stampaDlg->addEnabledOption());
        local_settings = new QLocale(QLocale::system());

           //Creazione oggetto painter che servir�  per disegnare le cose
            painter = new QPainter();

}

void Stampe::StampaVoci1(QPrinter *printer, QSqlQueryModel *dataset){

     // la funzione stampaVoci accoglie come parametro un oggetto puntatore
     // che rappresenta il dataset da mandare in stampa e che quindi
     // contiene i dati del report

    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
     printer->setPaperSize(QPrinter::A4);
     printer->setOrientation(QPrinter::Landscape);
     printer->setResolution(254);

        //printer->setOrientation(QPrinter::Landscape);
      // inizio fase di stampa. Si attiva il painter sulla printer
      // impostata in precedenza
   painter->begin(printer);

    int pagNo = 1;

    // si richiama la sotto funzione che "disegna" il modulo di stampa
    this->creaPagVoci1(pagNo);
      ++pagNo;

    int recNo = 0;

    // per ogni record del data set si effettua la stampa del dettaglio

    while (recNo < dataset->rowCount())
    {
         // printer->setOrientation(QPrinter::Landscape);
          // Occorre gestire a mano il controllo di quante righe è possibile stampare
          // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
          // Adeguare il numero di righe in base all'altezza del carattere di stampa

        for (int secRow = 0; secRow < 30; ++secRow)
        {
            QSqlRecord record = dataset->record(recNo);

            if (!record.isEmpty())
            {
               // in realt�  il tentativo di controllare se in record è vuoto
               // non funziona nel presente esempio. Non ho capito perché.

                int verticalPos = 200+100*(secRow+1);

                painter->setPen(QPen(Qt::black,1));

                painter->drawText(0,verticalPos,200,100,
                             Qt::AlignVCenter | Qt::AlignHCenter,
                             record.value("id").toString());
                painter->drawText(200,verticalPos,300,100,
                             Qt::AlignVCenter|Qt::AlignHCenter,
                             record.value("nome").toString());
                painter->drawText(400,verticalPos,600,100,
                             Qt::AlignVCenter|Qt::AlignHCenter,
                             record.value("cognome").toString());
                painter->drawText(600,verticalPos,800,100,
                                  Qt::AlignVCenter|Qt::AlignHCenter,
                                  record.value("indirizzo").toString());
                painter->drawText(850,verticalPos,1000,100,
                                  Qt::AlignVCenter|Qt::AlignHCenter,
                                  record.value("telefono").toString());
                painter->drawText(1050,verticalPos,1200,100,
                                  Qt::AlignVCenter|Qt::AlignHCenter,
                                  record.value("email").toString());
                painter->drawText(1300,verticalPos,1400,100,
                                  Qt::AlignVCenter|Qt::AlignHCenter,
                                  record.value("cod_fisc").toString());
                painter->drawText(1550,verticalPos,1600,100,
                                  Qt::AlignVCenter|Qt::AlignHCenter,
                                  record.value("part_iva").toString());
                painter->drawText(1750,verticalPos,1800,100,
                                  Qt::AlignVCenter|Qt::AlignHCenter,
                                  record.value("fax").toString());
                ++recNo;
            }

        }

        // onde evitare di generare una pagina vuota si controlla a mano
        // se si è reggiunto l'ultimo record

        if(recNo < dataset->rowCount())
        {
            printer->newPage();
            this->creaPagVoci1(pagNo);
            ++pagNo;
        }
     }

     // chiusura della fase di inserimento dati. Richiamando end()
     // il file viene fisicamente prodotto o mandato in stampa
    painter->end();

}

void Stampe::creaPagVoci1(int pagNo){
    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

    painter->setFont(QFont("Arial",12,75));
    painter->setPen(QPen(Qt::blue,1));
    painter->drawRect(0,0,2870,2000);

    painter->setPen(QPen(Qt::black,1));
    painter->drawRect(0,0,2870,120);
    painter->drawText(0,0,2870,120,
                     Qt::AlignVCenter|Qt::AlignHCenter,
                     qApp->translate("Stampe","Elenco Clienti"));

    //Valorizzazione header box intestazione colonne

    painter->drawText(0,200,200,100,
                     Qt::AlignVCenter|Qt::AlignHCenter,
                     qApp->translate("Stampe","Codice"));
    painter->drawText(200,200,300,100,
                     Qt::AlignVCenter|Qt::AlignHCenter,
                     qApp->translate("Stampe","Nome"));
    painter->drawText(400,200,600,100,
                     Qt::AlignVCenter|Qt::AlignHCenter,
                     qApp->translate("Stampe","Cognome"));
    painter->drawText(600,200,800,100,
                      Qt::AlignVCenter|Qt::AlignHCenter,
                      qApp->translate("Stampe","Indirizzo"));
    painter->drawText(850,200,1000,100,
                      Qt::AlignVCenter|Qt::AlignHCenter,
                      qApp->translate("Stampe","Telefono"));
    painter->drawText(1050,200,1200,100,
                      Qt::AlignVCenter|Qt::AlignHCenter,
                      qApp->translate("Stampe","E-mail"));
    painter->drawText(1300,200,1400,100,
                      Qt::AlignVCenter|Qt::AlignHCenter,
                      qApp->translate("Stampe","Codice Fiscale"));
    painter->drawText(1550,200,1600,100,
                      Qt::AlignVCenter|Qt::AlignHCenter,
                      qApp->translate("Stampe","Partita Iva"));
    painter->drawText(1750,200,1800,100,
                      Qt::AlignVCenter|Qt::AlignHCenter,
                     qApp->translate("Stampe", "Fax"));

    painter->setFont(QFont("Arial",10,50));
    painter->drawText(0,1900,2770,100,
                         Qt::AlignVCenter|Qt::AlignRight,
                         qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
    painter->drawText(0,1900,2770,100,
                         Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
    painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampaarticoli(QPrinter *printer,QSqlQueryModel *dataset){

    // la funzione stampaarticoli accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report
    printer->setResolution(254);
     printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
      printer->setOrientation(QPrinter::Landscape);
    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);



  // si richiama la sotto funzione che "disegna" il modulo di stampa
  int pagNo = 0;
  ++pagNo;
  this->creapagart(pagNo);


  int recNo = 0;

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {

        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 16; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 200+100*(secRow+1);

              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("cod_articolo").toString());
              painter->drawText(200,verticalPos,400,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("codbarre").toString());
              painter->drawText(600,verticalPos,600,100,
                           Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("nome_articolo").toString());
              painter->drawText(1000,verticalPos,800,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("quantita").toString());
              painter->drawText(1400,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("prezzo_unitario").toString());
              painter->drawText(1600,verticalPos,1400,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("iva").toString());
              painter->drawText(1800,verticalPos,1600,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->creapagart(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::creapagart(int pagNo){

    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Elenco articoli"));

  //Valorizzazione header box intestazione colonne

  painter->drawText(0,200,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Cod. Art."));
  painter->drawText(200,200,400,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Barcode"));
  painter->drawText(600,200,600,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Nome articolo"));
  painter->drawText(1000,200,800,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    QString::fromUtf8(qApp->translate("Stampe",QUANTITA).toStdString().c_str()));
  painter->drawText(1400,200,1000,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",PREZZO_UNITARIO));
  painter->drawText(1600,200,1400,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",IVA));
  painter->drawText(1800,200,1600,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",TOTALE));

      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampabarcode(QPrinter *printer, QSqlQueryModel *dataset, QModelIndex *index, EAN13 *codice){


    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Portrait);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

  painter->setFont(QFont("Arial",8,50));
  painter->setPen(QPen(Qt::black,1));

  if (index->row() < dataset->rowCount())
  {
      for (int secRow = 0; secRow < 1; ++secRow)
      {

          QSqlRecord record = dataset->record(index->row());
          if(!record.isEmpty()){
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.
              painter->setPen(QPen(Qt::black,1));

              /************************************************************
               * Prima riga etichetta codice a barre...
               ************************************************************/
              painter->drawText(40,35,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              qreal scale = painter->device()->logicalDpiX() / 25.4;	// dots per mm
              codice->draw(QRectF(50, 100, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(40,320,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(490,35,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(500, 100, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(490,320,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(940,35,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(950, 100, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(940,320,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(1390,35,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(1400, 100, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(1390,320,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());


              /************************************************************
               * Seconda riga etichetta codice a barre...
               ************************************************************/
              painter->drawText(40,410,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(50, 480, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(40,700,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(490,410,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(500, 480, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(490,700,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(940,410,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(950, 480, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(940,700,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(1390,410,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(1400, 480, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(1390,700,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              /************************************************************
               * Terza riga etichetta codice a barre...
               ************************************************************/
              painter->drawText(40,785,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(50,855 , 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(40,1075,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(490,785,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(500, 855, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(490,1075,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(940,785,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(950, 855, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(940,1075,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(1390,785,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(1400, 855, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(1390,1075,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());


              /************************************************************
               * Quarta riga etichetta codice a barre...
               ************************************************************/
              painter->drawText(40,1160,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(50,1230 , 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(40,1450,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(490,1160,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(500, 1230, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(490,1450,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(940,1160,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(950, 1230, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(940,1450,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(1390,1160,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(1400, 1230, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(1390,1450,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              /************************************************************
               * Quinta riga etichetta codice a barre...
               ************************************************************/
              painter->drawText(40,1535,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(50,1605 , 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(40,1825,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(490,1535,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(500, 1605, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(490,1825,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(940,1535,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(950, 1605, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(940,1825,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(1390,1535,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(1400, 1605, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(1390,1825,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              /************************************************************
               * Sesta riga etichetta codice a barre...
               ************************************************************/
              painter->drawText(40,1910,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(50,1980 , 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(40,2200,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(490,1910,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(500, 1980, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(490,2200,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(940,1910,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(950, 1980, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(940,2200,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(1390,1910,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(1400, 1980, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(1390,2200,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              /************************************************************
               * Settima riga etichetta codice a barre...
               ************************************************************/
              painter->drawText(40,2285,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(50,2355 , 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(40,2575,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(490,2285,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(500, 2355, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(490,2575,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(940,2285,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(950, 2355, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(940,2575,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

              painter->drawText(1390,2285,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->draw(QRectF(1400, 2355, 37.29 * scale, 25.93 * scale), painter,record.value("codbarre").toString());
              painter->drawText(1390,2575,400,120,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());
            }
        }
      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record
  }
  if(index->row() < dataset->rowCount())
  {    
  }
   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::stampabarcodeart(QPrinter *printer, QSqlQueryModel *dataset, QModelIndex *index, EAN13 *codice){
    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Portrait);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

  painter->setFont(QFont("Arial",8,50));
  painter->setPen(QPen(Qt::black,1));

  if (index->row() < dataset->rowCount())
  {
        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa
      for (int secRow = 0; secRow < 1; ++secRow)
      {

          QSqlRecord record = dataset->record(index->row());
          if(!record.isEmpty()){
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.
              int verticalPos = 50*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(140,verticalPos,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           qApp->translate("Stampe","Codice -")+record.value("codbarre").toString());
              codice->makePattern(record.value("codbarre").toString());
              qreal scale = painter->device()->logicalDpiX() / 25.4;	// dots per mm
              codice->draw(QRectF(150, 150, 37.29 * scale, 25.93 * scale), painter);

              painter->drawText(140,verticalPos,400,800,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("nome_articolo").toString());

            }
        }
      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record
  }
  if(index->row() < dataset->rowCount())
  {
  }
   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::stampaforn(QPrinter *printer, QSqlQueryModel *dataset){

    // la funzione st_rma accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report
    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

    int pagNo = 1;
    fornitori(pagNo);
   ++pagNo;
   // si richiama la sotto funzione che "disegna" il modulo di stampa



   int recNo = 0;
   painter->setFont(QFont("Arial",8,50));
           painter->setPen(QPen(Qt::black,1));

   // per ogni record del data set si effettua la stampa del dettaglio

   while (recNo < dataset->rowCount())
   {
         //printer->setOrientation(QPrinter::Landscape);
         // Occorre gestire a mano il controllo di quante righe �? possibile stampare
         // per prova si �? impostato un massimo di 4 righe di dettaglio per pagina
         // Adeguare il numero di righe in base all'altezza del carattere di stampa

       for (int secRow = 0; secRow < 16; ++secRow)
       {
           QSqlRecord record = dataset->record(recNo);

           if (!record.isEmpty())
           {
                int verticalPos = 200+100*(secRow+1);

                painter->setPen(QPen(Qt::black,2));
                painter->drawText(0,verticalPos,200,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("id").toString());
                painter->drawText(150,verticalPos,300,100,
                                 Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("ragione_sociale").toString());
                painter->drawText(400,verticalPos,500,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("part_iva").toString());
                painter->drawText(600,verticalPos,650,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("cod_fiscale").toString());
                painter->drawText(800,verticalPos,800,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("indirizzo").toString());
                painter->drawText(900,verticalPos,950,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("cap").toString());
                painter->drawText(1000,verticalPos,1100,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("localita").toString());
                painter->drawText(1100,verticalPos,1250,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  "("+record.value("prov").toString()+")");
                painter->drawText(1200,verticalPos,1400,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("telefono").toString());
                painter->drawText(1300,verticalPos,1550,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("fax").toString());
                painter->drawText(1500,verticalPos,1700,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("email").toString());
                painter->drawText(1800,verticalPos,1850,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("sito").toString());
                ++recNo;

           }
       }
       if(recNo < dataset->rowCount())
       {
           printer->newPage();
           fornitori(pagNo);
           ++pagNo;
       }
   }
   painter->end();
}

void Stampe::fornitori(int pagNo){

    painter->setFont(QFont("Arial",12,75));
    painter->setPen(QPen(Qt::blue,2));
    painter->drawRect(0,0,2870,2000);
    painter->drawRect(0,0,2870,120);
    painter->drawText(0,0,2870,120,
                      Qt::AlignVCenter | Qt::AlignHCenter,
                      qApp->translate("Stampe","Elenco fornitori"));

    //Valorizzazione header box intestazione colonne
    painter->setPen(QPen(Qt::black,2));
    painter->drawText(0,200,200,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","ID"));
    painter->drawText(150,200,350,100,
                     Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Ragione sociale"));
    painter->drawText(400,200,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Partita iva"));
    painter->drawText(600,200,650,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Codice fiscale"));
    painter->drawText(800,200,800,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Indirizzo"));
    painter->drawText(900,200,950,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","CAP"));
    painter->drawText(1000,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      QString::fromUtf8(qApp->translate("Stampe","Località").toStdString().c_str()));
    painter->drawText(1100,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","PROV"));
    painter->drawText(1200,200,1400,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Telefono"));
    painter->drawText(1300,200,1550,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Fax"));
    painter->drawText(1500,200,1700,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","E-mail"));
    painter->drawText(1800,200,1850,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Sito web"));

    painter->setFont(QFont("Arial",10,50));
    painter->setPen(QPen(Qt::blue,1));
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignRight,
                      qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+" ");
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria Gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
    painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampafattacq(QPrinter *printer, QSqlQueryModel *dataset){

    // la funzione stampaarticoli accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  this->fattacq(pagNo);
  ++pagNo;
  // si richiama la sotto funzione che "disegna" il modulo di stampa



  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
          painter->setPen(QPen(Qt::black,1));

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {
        //printer->setOrientation(QPrinter::Landscape);
        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 20; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 150+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(200,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("data").toString());
              painter->drawText(900,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("fornitore").toString());
              painter->drawText(1500,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->fattacq(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::fattacq(int pagNo){

    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Elenco Fatture d'acquisto"));


  //Valorizzazione header box intestazione colonne

  painter->drawText(0,100,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(250,100,800,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Data"));
  painter->drawText(850,100,1100,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Fornitore"));
  painter->drawText(1500,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Totale"));


      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(20,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampa_fattura(QPrinter *printer, QSqlRelationalTableModel *dataset){
    // la funzione stampa_fattura accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  this->fattura(pagNo);
  ++pagNo;
  // si richiama la sotto funzione che "disegna" il modulo di stampa



  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
          painter->setPen(QPen(Qt::black,1));

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {
        //printer->setOrientation(QPrinter::Landscape);
        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 20; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 150+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(200,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("data").toString());
              painter->drawText(900,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("fornitore").toString());
              painter->drawText(1500,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->fattura(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::fattura(int pagNo){

    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Elenco Fatture di vendita"));


  //Valorizzazione header box intestazione colonne

  painter->drawText(0,100,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(250,100,800,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Data"));
  painter->drawText(850,100,1100,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Fornitore o cliente"));
  painter->drawText(1500,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Totale"));


      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(20,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::esporta_pdf(QPrinter *printer, QSqlRelationalTableModel *dataset)
{
    // la funzione stampaarticoli accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report
    printer->setResolution(254);
     printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
      printer->setOrientation(QPrinter::Landscape);
    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);



  // si richiama la sotto funzione che "disegna" il modulo di stampa
  int pagNo = 0;
  ++pagNo;
  this->pagina(pagNo);


  int recNo = 0;

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {

        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 16; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 200+100*(secRow+1);

              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("cod_articolo").toString());
              painter->drawText(200,verticalPos,400,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("codbarre").toString());
              painter->drawText(600,verticalPos,600,100,
                           Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("nome_articolo").toString());
              painter->drawText(1000,verticalPos,800,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("descrizione").toString());
              painter->drawText(1400,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("prezzo_unitario").toString());
              painter->drawText(1600,verticalPos,1400,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("iva").toString());
              painter->drawText(1800,verticalPos,1600,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->pagina(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::pagina(int pagNo)
{
    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Carico magazzino"));

  //Valorizzazione header box intestazione colonne

  painter->drawText(0,200,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(200,200,400,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe",BARCODE));
  painter->drawText(600,200,600,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe",PRODOTTO));
  painter->drawText(1000,200,800,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",DESCRIZIONE));
  painter->drawText(1400,200,1000,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",PREZZO_UNITARIO));
  painter->drawText(1600,200,1400,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",IVA));
  painter->drawText(1800,200,1600,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",TOTALE));

      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::esporta_scarico(QPrinter *printer, QSqlRelationalTableModel *dataset)
{
    // la funzione stampaarticoli accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report
    printer->setResolution(254);
     printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
      printer->setOrientation(QPrinter::Landscape);
    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);



  // si richiama la sotto funzione che "disegna" il modulo di stampa
  int pagNo = 0;
  ++pagNo;
  this->pagina_scarico(pagNo);


  int recNo = 0;

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {

        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 16; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 200+100*(secRow+1);

              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("cod_articolo").toString());
              painter->drawText(200,verticalPos,400,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("codbarre").toString());
              painter->drawText(600,verticalPos,600,100,
                           Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("nome_articolo").toString());
              painter->drawText(1000,verticalPos,800,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("descrizione").toString());
              painter->drawText(1400,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("prezzo_unitario").toString());
              painter->drawText(1600,verticalPos,1400,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("iva").toString());
              painter->drawText(1800,verticalPos,1600,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->pagina_scarico(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::pagina_scarico(int pagNo)
{
    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Scarico magazzino"));

  //Valorizzazione header box intestazione colonne

  painter->drawText(0,200,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(200,200,400,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe",BARCODE));
  painter->drawText(600,200,600,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe",PRODOTTO));
  painter->drawText(1000,200,800,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",DESCRIZIONE));
  painter->drawText(1400,200,1000,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",PREZZO_UNITARIO));
  painter->drawText(1600,200,1400,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",IVA));
  painter->drawText(1800,200,1600,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe",TOTALE));

      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampa_fatt_vendita_libri(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id)
{
    // la funzione stampa_fatt_vendita_libri accoglie come parametro un oggetto puntatore
        // che rappresenta il dataset da mandare in stampa e che quindi
        // contiene i dati del report
          printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
          printer->setPaperSize(QPrinter::A4);
          printer->setResolution(254);
          printer->setOrientation(QPrinter::Portrait);
        // inizio fase di stampa. Si attiva il painter sulla printer
        // impostata in precedenza
      painter->begin(printer);

      painter->setFont(QFont("Arial",8,50));
      painter->setPen(QPen(Qt::black,1));

      int pagNo = 1;
      pagina_fatt_vendita_libri(pagNo,id);
      ++pagNo;

      int recNo = 0;
      if(index.row() < dataset->rowCount())
      {
          //Instestazione Fattura immediata libri
          intestazione();
          for(int secRow=0; secRow < 1; ++secRow){
              QSqlRecord rec = dataset->record(index.row());
              QSqlQuery query1;
              query1.prepare("select * from fatture_vendita_righe where id='"+QString::number(id)+"'");
              if(!rec.isEmpty()){
                  if(query1.exec()){
                  if(query1.next()){

                      painter->setFont(QFont("Arial",16,50));
                      painter->setPen(QPen(Qt::RoundCap));
                      painter->drawText(1190,65,2000,1500,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        query1.value(13).toString());
                      painter->setFont(QFont("Arial",8,50));
                      painter->drawText(1190,125,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      query1.value(14).toString()+" "+query1.value(15).toString()+"-"+query1.value(16).toString());
                      painter->drawText(1190,160,4000,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    "Tel. "+query1.value(18).toString()+" Fax "+query1.value(19).toString());
                      painter->drawText(1190,205,4000,100,
                                       Qt::AlignJustify | Qt::AlignJustify,
                                      "C.F/P.I: "+query1.value(20).toString());
                  }

                  painter->setFont(QFont("Arial",8,50));
                  painter->drawText(1190,345,4000,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    qApp->translate("Stampe",FATTURA_IMMEDIATA)+" N"+QString::fromUtf8("\u0176")+" "+rec.value("id").toString());
                  painter->drawText(1190,405,4000,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    qApp->translate("Stampe",DATA)+":   "+rec.value("data").toDate().toString("dd-MM-yyyy"));
                  painter->drawText(1190,460,4000,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    qApp->translate("Stampe",TIPO_FATTURA)+":   "+rec.value("tipo_fattura").toString());
              }
              }

          }
      }
      
      QSqlQuery query_ord_ddt;
      query_ord_ddt.prepare("select * from fatture_vendita_righe where id='"+QString::number(id)+"'");
      if(query_ord_ddt.exec())
      {
	if(recNo < query_ord_ddt.record().count())
	{
	  for(int row_sec=0; row_sec < 1; ++row_sec){

              if(query_ord_ddt.next()){
		QSqlRecord rec = query_ord_ddt.record();
		int verticalPos = 625+45*(row_sec);
		
		if(!rec.value("vs_ordine").toString().isEmpty()){

                  painter->setFont(QFont("Arial",8,50));
                  painter->drawText(225,verticalPos,800,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    rec.value("vs_ordine").toString()+" in data "+rec.value("data").toDate().toString("dd/MM/yyyy"));
		}
		else
		{
		  painter->setFont(QFont("Arial",8,50));
                  painter->drawText(20,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    "");
		}
	      }
	  }
	}
      }

      QSqlQuery query;
      query.prepare("select * from fatture_righe_vendita_art where id='"+QString::number(id)+"'");
      if(query.exec()){
      if(recNo < query.record().count()){
          for(int row_sec=0; row_sec < 35; ++row_sec){

              if(query.next()){
              QSqlRecord rec = query.record();
              int verticalPos = 630+45*(row_sec+1);

              if(!rec.isEmpty()){

                  painter->setFont(QFont("Arial",8,50));
                  painter->drawText(20,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    rec.value("cod_art").toString());
                  painter->drawText(225,verticalPos,800,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    rec.value("descrizione").toString());
                  painter->drawText(1020,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    rec.value("unita").toString());
                  painter->drawText(1102,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    rec.value("quantita").toString());
                  painter->drawText(1270,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    QString::number(rec.value("prezzo_unit").toDouble()));
                  painter->drawText(1470,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    QString::number(rec.value("sconto").toDouble()));
                  painter->drawText(1700,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    QString::number(rec.value("prezzo_s_iva").toDouble()));
                  painter->drawText(1930,verticalPos,200,100,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    QString::number(rec.value("iva").toDouble())+"%");

              }
              }
              ++recNo;
          }
      }
      }

      if(recNo < query.record().count()){
          painter->drawText(1600,2590,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     qApp->translate("Stampe","Segue ----->"));
                   printer->newPage();
                   pagina_fatt_vendita_libri(pagNo,id);
                   ++pagNo;
      }
        painter->end();
}

void Stampe::pagina_fatt_vendita_libri(int pagNo,int id)
{
    //Instestazione Fattura immediata libri
        intestazione();
        //Impaginazione fattura
        layout_fattura();
        int recNo_1=0;
        //Impostazione degli ultimi dati
        QSqlQuery query2;
        query2.exec("select * from fatture_vendita_righe where id='"+QString::number(id)+"'");
        if(recNo_1 < query2.record().count()){
            for(int x=0; x < 1; ++x){
                if(query2.next()){
                QSqlRecord rec = query2.record();
                //int verticalPos = 2500+1*(x+1);

                if(!rec.isEmpty()){

                    //Visualizza le scadenze
                    painter->drawText(820,2555,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("scadenza_1").toDate().toString("dd-MM-yyyy"));

                    painter->drawText(820,2580,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("scadenza_2").toDate().toString("dd-MM-yyyy"));

                    painter->drawText(820,2605,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("scadenza_3").toDate().toString("dd-MM-yyyy"));

                    //Visualizza importi
                    painter->drawText(1075,2555,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("importo_1").toString());
                    painter->drawText(1075,2580,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("importo_2").toString());
                    painter->drawText(1075,2605,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("importo_3").toString());

                    //Visualizza spese di trasporto
                    painter->drawText(355,2260,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("spese_trasporto").toString());

                    //Visualizza spese di incasso
                    painter->drawText(975,2260,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("spese_incasso").toString());

                    //Visualizza tipo di pagamento
                    painter->drawText(160,2503,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("tipo_pagamento").toString());

                    //Visualizza Banca
                    painter->drawText(110,2553,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("banca").toString());

                    //Visualizza Iban
                    painter->drawText(90,2608,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("iban").toString());

                    //Visualizza note fattura
                    painter->drawText(160,2503,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("annotazioni").toString());


                    //Setto i caratteri in grassetto
                    painter->setFont(QFont("Arial",6,QFont::Bold,true));
                    //Visualizza imponibile
                    painter->drawText(1670,2290,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("prezzo_s_iva").toString());

                    //Visualizza iva
                    painter->drawText(1670,2370,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("iva").toString());

                    //Visualizzo totale
                    painter->drawText(1670,2450,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::fromUtf8("\u20AC")+ "   " +rec.value("totale").toString());

                    //Setto i caratteri in corsivo

                    QFont *font = new QFont("Arial");
                    font->setItalic(true);
                    font->setPixelSize(24);
                    painter->setFont(*font);

                    //Visualizzo descizione iva
                    QSqlQuery query3;
                    query3.exec("select A.iva,A.descrizione as aliquota, B.iva as fatture_righe_vendita_art from aliquota A,fatture_righe_vendita_art B where A.iva=B.iva and B.id='"+QString::number(id)+"'");
                    if(query3.next()){

                        //Visualizzo codice iva
                        painter->drawText(20,2360,4000,100,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          query3.value(0).toString());

                        //Visualizzo descrizione iva
                        painter->drawText(280,2360,4000,100,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          query3.value(1).toString());


                        //Visualizza imponibile
                        painter->drawText(540,2360,4000,100,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          QString::fromUtf8("\u20AC")+ "   " +rec.value("prezzo_s_iva").toString());


                        //Visualizzo codice iva
                        painter->drawText(800,2360,4000,100,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          query3.value(0).toString()+"%");



                        //Visualizza iva
                        painter->drawText(1060,2360,4000,100,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          QString::fromUtf8("\u20AC")+ "   " +rec.value("iva").toString());

                    }


                }
                }
                ++recNo_1;
            }
        }
        //Numero di pagina
        painter->setFont(QFont("Arial",6,50));
        painter->drawText(1800,2700,2000,100,
                         Qt::AlignVCenter|Qt::AlignLeft,
                          qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"/"+QString("%1").arg(pagNo));

}

void Stampe::stampa_prev_prod(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id)
{
    // la funzione stampa_fatt_vendita_libri accoglie come parametro un oggetto puntatore
          // che rappresenta il dataset da mandare in stampa e che quindi
          // contiene i dati del report
            printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
            printer->setPaperSize(QPrinter::A4);
            printer->setResolution(254);
            printer->setOrientation(QPrinter::Portrait);
          // inizio fase di stampa. Si attiva il painter sulla printer
          // impostata in precedenza
        painter->begin(printer);

        painter->setFont(QFont("Arial",8,50));
        painter->setPen(QPen(Qt::black,1));

        int pagNo = 1;
        pagina_prev_prod(pagNo,id);
        ++pagNo;

        int recNo = 0;
        if(index.row() < dataset->rowCount())
        {

            for(int secRow=0; secRow < 1; ++secRow){
                QSqlRecord rec = dataset->record(index.row());
                QSqlQuery query1;
                query1.exec("select * from prev_righe where id='"+QString::number(id)+"'");
                if(!rec.isEmpty()){

                    if(query1.next()){

                        painter->setFont(QFont("Arial",16,50));
                        painter->setPen(QPen(Qt::RoundCap));
                        painter->drawText(1190,65,2000,1500,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          query1.value(13).toString());
                        painter->setFont(QFont("Arial",8,50));
                        painter->drawText(1190,125,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        query1.value(14).toString()+" "+query1.value(15).toString()+"-"+query1.value(16).toString());
                        painter->drawText(1190,160,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      "Tel. "+query1.value(18).toString()+" Fax "+query1.value(19).toString());
                        painter->drawText(1190,205,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                        "C.F/P.I: "+query1.value(20).toString());
                    }

                    painter->setFont(QFont("Arial",8,50));
                    painter->drawText(1190,345,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",PREVENTIVO)+"  N"+QString::fromUtf8("\u0176")+" "+rec.value("id").toString());
                    painter->drawText(1190,405,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",DATA_PREV)+":   "+rec.value("data").toDate().toString("dd-MM-yyyy"));
                    painter->drawText(1190,460,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",TIPO_PREVENTIVO)+":   "+rec.value("tipo_preventivo").toString());
                }

            }
        }
        QSqlQuery query;
        query.exec("select * from prev_righe_art where id='"+QString::number(id)+"'");
        if(recNo < query.record().count()){
            for(int row_sec=0; row_sec < 36; ++row_sec){

                if(query.next()){
                QSqlRecord rec = query.record();
                int verticalPos = 595+45*(row_sec+1);

                if(!rec.isEmpty()){

                    painter->setFont(QFont("Arial",8,50));
                    painter->drawText(20,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("cod_art").toString());

                    QString desc = rec.value("descrizione").toString();
                    if(desc.length() == 100)
                    {
                        desc.append("\n");
                    }
                    painter->drawText(225,verticalPos,800,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      desc);


                    painter->drawText(1020,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("unita").toString());
                    painter->drawText(1102,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("quantita").toString());

                    painter->drawText(1270,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("prezzo_unit").toDouble()));
                    painter->drawText(1470,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("sconto").toDouble()));
                    painter->drawText(1700,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("prezzo_s_iva").toDouble()));
                    painter->drawText(1930,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("iva").toDouble())+"%");

                }
                }
                ++recNo;
            }
        }

        if(recNo < query.record().count()){
            painter->drawText(1600,2590,4000,100,
                              Qt::AlignJustify | Qt::AlignJustify,
                              qApp->translate("Stampe","Segue ----->"));
            printer->newPage();
            pagina_prev_prod(pagNo,id);
            ++pagNo;
        }


          painter->end();
}

void Stampe::pagina_prev_prod(int pagNo, int id)
{
    //Instestazione preventivi
       intestazione();
       //Impaginazione preventivi
       layout_preventivi();
       int recNo_1=0;
       //Impostazione degli ultimi dati
       QSqlQuery query2;
       query2.exec("select * from prev_righe where id='"+QString::number(id)+"'");
       if(recNo_1 < query2.record().count()){
           for(int x=0; x < 1; ++x){
               if(query2.next()){
               QSqlRecord rec = query2.record();

               if(!rec.isEmpty()){

                   //Visualizza spese di trasporto
                   painter->drawText(355,2260,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("spese_trasporto").toString());

                   //Visualizza spese di incasso
                   painter->drawText(975,2260,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("spese_incasso").toString());

                   //Visualizza tipo di pagamento
                   painter->drawText(160,2503,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("tipo_pagamento").toString());

                   //Visualizza Banca
                   painter->drawText(110,2553,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("banca").toString());

                   //Visualizza Iban
                   painter->drawText(90,2608,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("iban").toString());

                   //Visualizza note fattura
                   painter->drawText(160,2503,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("annotazioni").toString());


                   //Setto i caratteri in grassetto
                   painter->setFont(QFont("Arial",6,QFont::Bold,true ));
                   //Visualizza imponibile
                   painter->drawText(1670,2290,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("prezzo_s_iva").toString());

                   //Visualizza iva
                   painter->drawText(1670,2370,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("iva").toString());

                   //Visualizzo totale
                   painter->drawText(1670,2450,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("totale").toString());

                   //Setto i caratteri in corsivo

                   QFont *font = new QFont("Arial");
                   font->setItalic(true);
                   font->setPixelSize(24);
                   painter->setFont(*font);

                   //Visualizzo descizione iva
                   QSqlQuery query3;
                   query3.exec("select A.iva,A.descrizione as aliquota, B.iva as prev_righe_art from aliquota A,prev_righe_art B where A.iva=B.iva and B.id='"+QString::number(id)+"'");
                   if(query3.next()){

                       //Visualizzo codice iva
                       painter->drawText(20,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         query3.value(0).toString());

                       //Visualizzo descrizione iva
                       painter->drawText(280,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         query3.value(1).toString());


                       //Visualizza imponibile
                       painter->drawText(540,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         QString::fromUtf8("\u20AC")+ "   " +rec.value("prezzo_s_iva").toString());


                       //Visualizzo codice iva
                       painter->drawText(800,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         query3.value(0).toString()+"%");



                       //Visualizza iva
                       painter->drawText(1060,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         QString::fromUtf8("\u20AC")+ "   " +rec.value("iva").toString());

                   }


               }
               }
               ++recNo_1;
           }
       }
       //Numero di pagina
       painter->setFont(QFont("Arial",6,50));
       painter->drawText(1800,2700,2000,100,
                        Qt::AlignVCenter|Qt::AlignLeft,
                         qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"/"+QString("%1").arg(pagNo));
}

void Stampe::intestazione()
{
    //Instestazione
    QRect target(150,10,500,240);
    QImage img(settingsManager->generalValue("Image_resize/dir_image",QVariant()).toString());
    painter->drawImage(target,img);

    QSqlQuery query;
    query.exec("select * from azienda where id='"+settingsManager->generalValue("Azienda/id",QVariant()).toString()+"' and predefinito='"+qApp->translate("Stampe","Predefinito")+"'");
    if(query.next())
    {
        painter->setFont(QFont("Arial",16,50));
        painter->setPen(QPen(Qt::RoundCap));
        painter->drawText(55,250,2000,100,
                          Qt::AlignJustify | Qt::AlignJustify,
                          query.value(1).toString());
        painter->setFont(QFont("Arial",8,50));
        painter->drawText(55,315,4000,100,
                        Qt::AlignJustify | Qt::AlignJustify,
                        query.value(4).toString()+" "+query.value(5).toString()+"-"+query.value(6).toString()+
                        " "+"("+query.value(7).toString()+")");
        painter->drawText(55,360,4000,100,
                      Qt::AlignJustify | Qt::AlignJustify,
                      "Tel. "+query.value(8).toString()+" Fax "+query.value(9).toString());
        painter->drawText(55,405,4000,100,
                         Qt::AlignJustify | Qt::AlignJustify,
                        "e-mail: "+query.value(11).toString()+" Internet: "+query.value(10).toString());
        painter->drawText(55,445,4000,100,
                         Qt::AlignJustify | Qt::AlignJustify,
                        "C.F: "+query.value(3).toString()+" P.Iva: "+query.value(2).toString());
    }
}

void Stampe::layout_fattura()
{

    //Rettangolo intestazione azienda
    QRect rett(32,220,800,305);
    painter->drawRoundRect(rett,25,25);
    painter->drawLine(55,310,780,310);

    //Rettangolo intestazione cliente-fornitore
    QRect rett1(1170,45,725,205);
    painter->drawRoundRect(rett1,25,25);
    painter->drawLine(1190,123,1815,123);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,10,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Cliente"));

    //Rettangolo dati fattura
    QRect rett2(1170,325,725,205);
    painter->drawRoundRect(rett2,25,25);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,280,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Dati Fattura"));


    //Rettangolo fattura da riempire
    painter->drawRect(5,550,2000,2140);
    painter->drawRect(5,610,2000,0);

    painter->setFont(QFont("Arial",6,50));
    //Rettangolo Codice
    painter->drawRect(5,550,200,1700);
    painter->drawText(12,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",CODICE));
    //Rettangolo Descrizione
    painter->drawRect(205,550,800,1700);
    painter->drawText(225,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));

    //Rettangolo qApp->translate("Stampe",UM)
    painter->drawRect(1005,550,80,1700);
    painter->drawText(1020,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",UM));

    //Rettangolo Quantit�
    painter->drawRect(1085,550,160,1700);
    painter->drawText(1102,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      QString::fromUtf8(qApp->translate("Stampe",QUANTITA).toStdString().c_str()));

    //Rettangolo Prezzo
    painter->drawRect(1245,550,200,1700);
    painter->drawText(1290,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PREZZO_UNITARIO));

    //Rettangolo Sconto
    painter->drawRect(1445,550,200,1700);
    painter->drawText(1490,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SCONTO));

    //Rettangolo importo
    painter->drawRect(1645,550,255,1700);
    painter->drawText(1720,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPORTO));

    //Rettangolo Iva
    painter->drawRect(1900,550,105,1700);
    painter->drawText(1930,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA));

    //Rettangolo spese di trasporto
    painter->drawRect(5,2250,1300,50);
    painter->drawRect(325,2250,320,50);
    painter->drawRect(645,2250,320,50);
    painter->drawText(20,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SPESE_DI_TRASPORTO));

    painter->drawText(655,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SPESE_DI_INCASSO));

    //Dettaglio iva
    painter->drawRect(5,2300,1300,50);
    painter->drawRect(5,2300,1300,190);
    painter->drawRect(5,2300,260,190);
    painter->drawRect(265,2300,260,190);
    painter->drawRect(525,2300,260,190);
    painter->drawRect(785,2300,260,190);

    painter->drawText(20,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DETTAGLIO_IVA));

    painter->drawText(280,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));

    painter->drawText(540,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPONIBILE));

    painter->drawText(800,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA_PERC));

    painter->drawText(1060,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPOSTA));


    //Tipo pagamento
    painter->drawRect(5,2490,800,50);
    painter->drawText(20,2465,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PAGAMENTO)+": ");

    //Banca
    painter->drawRect(5,2540,800,50);
    painter->drawText(20,2515,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",BANCA)+": ");

    //Iban
    painter->drawRect(5,2590,800,50);
    painter->drawText(20,2570,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IBAN)+": ");

    //Note fattura
    painter->drawText(20,2620,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",NOTE_FATTURA)+": ");
    //Scadenze
    painter->drawRect(805,2490,500,200);
    painter->drawRect(805,2515,250,175);
    painter->drawRect(805,2490,500,25);
    painter->drawRect(805,2515,500,25);

    painter->drawText(1015,2454,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SCADENZE));

    painter->drawText(820,2477,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DATA_SC));

    painter->drawText(1075,2477,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPORTO));

    //Rettangolo imponibile-iva-totale
    painter->drawRect(1305,2250,700,240);
    painter->drawRect(1305,2250,350,240);
    painter->drawText(1320,2250,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPONIBILE));
    //Riga imponibile
    painter->drawRect(1305,2250,700,80);
    painter->drawText(1320,2330,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA));
    //Riga iva
    painter->drawRect(1305,2330.6,700,80);
    painter->drawText(1320,2410,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",TOTALE));

    //Versione Fabaria Gest sulla pagina di stampa
    painter->setFont(QFont("Arial",6,50));
    painter->drawText(10,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString()+" Powered by Codelinsoft");

}

void Stampe::layout_preventivi()
{
    //Rettangolo intestazione azienda
    QRect rett(32,220,800,305);
    painter->drawRoundRect(rett,25,25);
    painter->drawLine(55,310,780,310);

    //Rettangolo intestazione cliente-fornitore
    QRect rett1(1170,45,725,205);
    painter->drawRoundRect(rett1,25,25);
    painter->drawLine(1190,123,1815,123);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,10,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Cliente"));

    //Rettangolo dati fattura
    QRect rett2(1170,325,725,205);
    painter->drawRoundRect(rett2,25,25);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,280,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Dati preventivo"));


    //Rettangolo fattura da riempire
    painter->drawRect(5,550,2000,2140);
    painter->drawRect(5,610,2000,0);

    painter->setFont(QFont("Arial",6,50));
    //Rettangolo Codice
    painter->drawRect(5,550,200,1700);
    painter->drawText(12,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",CODICE));
    //Rettangolo Descrizione
    painter->drawRect(205,550,800,1700);
    painter->drawText(225,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));

    //Rettangolo qApp->translate("Stampe",UM)
    painter->drawRect(1005,550,80,1700);
    painter->drawText(1020,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",UM));

    //Rettangolo Quantit�
    painter->drawRect(1085,550,160,1700);
    painter->drawText(1102,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      QString::fromUtf8(qApp->translate("Stampe",QUANTITA).toStdString().c_str()));

    //Rettangolo Prezzo
    painter->drawRect(1245,550,200,1700);
    painter->drawText(1290,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PREZZO_UNITARIO));

    //Rettangolo Sconto
    painter->drawRect(1445,550,200,1700);
    painter->drawText(1490,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SCONTO));

    //Rettangolo importo
    painter->drawRect(1645,550,255,1700);
    painter->drawText(1720,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPORTO));

    //Rettangolo Iva
    painter->drawRect(1900,550,105,1700);
    painter->drawText(1930,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA));

    //Rettangolo spese di trasporto
    painter->drawRect(5,2250,1300,50);
    painter->drawRect(325,2250,320,50);
    painter->drawRect(645,2250,320,50);
    painter->drawText(20,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SPESE_DI_TRASPORTO));

    painter->drawText(655,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SPESE_DI_INCASSO));

    //Dettaglio iva
    painter->drawRect(5,2300,1300,50);
    painter->drawRect(5,2300,1300,190);
    painter->drawRect(5,2300,260,190);
    painter->drawRect(265,2300,260,190);
    painter->drawRect(525,2300,260,190);
    painter->drawRect(785,2300,260,190);

    painter->drawText(20,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DETTAGLIO_IVA));

    painter->drawText(280,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));

    painter->drawText(540,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPONIBILE));

    painter->drawText(800,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA_PERC));

    painter->drawText(1060,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPOSTA));


    //Tipo pagamento
    painter->drawRect(5,2490,800,50);
    painter->drawText(20,2465,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PAGAMENTO)+": ");

    //Banca
    painter->drawRect(5,2540,800,50);
    painter->drawText(20,2515,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",BANCA)+": ");

    //Iban
    painter->drawRect(5,2590,800,50);
    painter->drawText(20,2570,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IBAN)+": ");

    //Note fattura
    painter->drawText(20,2620,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",NOTE_PREVENTIVO)+": ");


    //Rettangolo imponibile-iva-totale
    painter->drawRect(1305,2250,700,240);
    painter->drawRect(1305,2250,350,240);
    painter->drawText(1320,2250,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPONIBILE));
    //Riga imponibile
    painter->drawRect(1305,2250,700,80);
    painter->drawText(1320,2330,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA));
    //Riga iva
    painter->drawRect(1305,2330.6,700,80);
    painter->drawText(1320,2410,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",TOTALE));

    //Versione Fabaria Gest sulla pagina di stampa
    painter->setFont(QFont("Arial",6,50));
    painter->drawText(10,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString()+" Powered by Codelinsoft");

}

void Stampe::layout_ordini()
{
    //Rettangolo intestazione azienda
    QRect rett(32,220,800,305);
    painter->drawRoundRect(rett,25,25);
    painter->drawLine(55,310,780,310);

    //Rettangolo intestazione cliente-fornitore
    QRect rett1(1170,45,725,205);
    painter->drawRoundRect(rett1,25,25);
    painter->drawLine(1190,123,1815,123);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,10,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Cliente"));

    //Rettangolo dati fattura
    QRect rett2(1170,325,725,205);
    painter->drawRoundRect(rett2,25,25);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,280,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Dati ordine"));


    //Rettangolo fattura da riempire
    painter->drawRect(5,550,2000,2140);
    painter->drawRect(5,610,2000,0);

    painter->setFont(QFont("Arial",6,50));
    //Rettangolo Codice
    painter->drawRect(5,550,200,1700);
    painter->drawText(12,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",CODICE));
    //Rettangolo Descrizione
    painter->drawRect(205,550,800,1700);
    painter->drawText(225,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));

    //Rettangolo qApp->translate("Stampe",UM)
    painter->drawRect(1005,550,80,1700);
    painter->drawText(1020,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",UM));

    //Rettangolo Quantità
    painter->drawRect(1085,550,160,1700);
    painter->drawText(1102,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      QString::fromUtf8(qApp->translate("Stampe",QUANTITA).toStdString().c_str()));

    //Rettangolo Prezzo
    painter->drawRect(1245,550,200,1700);
    painter->drawText(1290,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PREZZO_UNITARIO));

    //Rettangolo Sconto
    painter->drawRect(1445,550,200,1700);
    painter->drawText(1490,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SCONTO));

    //Rettangolo importo
    painter->drawRect(1645,550,255,1700);
    painter->drawText(1720,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPORTO));

    //Rettangolo Iva
    painter->drawRect(1900,550,105,1700);
    painter->drawText(1930,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA));

    //Rettangolo spese di trasporto
    painter->drawRect(5,2250,1300,50);
    painter->drawRect(325,2250,320,50);
    painter->drawRect(645,2250,320,50);
    painter->drawText(20,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SPESE_DI_TRASPORTO));

    painter->drawText(655,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SPESE_DI_INCASSO));

    //Dettaglio iva
    painter->drawRect(5,2300,1300,50);
    painter->drawRect(5,2300,1300,190);
    painter->drawRect(5,2300,260,190);
    painter->drawRect(265,2300,260,190);
    painter->drawRect(525,2300,260,190);
    painter->drawRect(785,2300,260,190);

    painter->drawText(20,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DETTAGLIO_IVA));

    painter->drawText(280,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));

    painter->drawText(540,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPONIBILE));

    painter->drawText(800,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA_PERC));

    painter->drawText(1060,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPOSTA));


    //Tipo pagamento
    painter->drawRect(5,2490,800,50);
    painter->drawText(20,2465,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PAGAMENTO)+": ");

    //Banca
    painter->drawRect(5,2540,800,50);
    painter->drawText(20,2515,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",BANCA)+": ");

    //Iban
    painter->drawRect(5,2590,800,50);
    painter->drawText(20,2570,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IBAN)+": ");

    //Note fattura
    painter->drawText(20,2620,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",NOTE_ORDINE)+": ");
    //Scadenze
    painter->drawRect(805,2490,500,200);
    painter->drawRect(805,2515,250,175);
    painter->drawRect(805,2490,500,25);
    painter->drawRect(805,2515,500,25);

    painter->drawText(1015,2454,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",SCADENZE));

    painter->drawText(820,2477,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DATA_SC));

    painter->drawText(1075,2477,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPORTO));

    //Rettangolo imponibile-iva-totale
    painter->drawRect(1305,2250,700,240);
    painter->drawRect(1305,2250,350,240);
    painter->drawText(1320,2250,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IMPONIBILE));
    //Riga imponibile
    painter->drawRect(1305,2250,700,80);
    painter->drawText(1320,2330,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA));
    //Riga iva
    painter->drawRect(1305,2330.6,700,80);
    painter->drawText(1320,2410,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",TOTALE));

    //Versione Fabaria Gest sulla pagina di stampa
    painter->setFont(QFont("Arial",6,50));
    painter->drawText(10,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString()+" Powered by Codelinsoft");

}

//Stampa lista scadenze
void Stampe::stampa_lista_scadenze(QPrinter *printer, QSqlRelationalTableModel *model){

    // la funzione stampa_lista_scadenze accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report
    printer->setResolution(254);
     printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
      printer->setOrientation(QPrinter::Landscape);
    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);



  // si richiama la sotto funzione che "disegna" il modulo di stampa
  int pagNo = 0;
  ++pagNo;
  this->pagina_lista_scadenze(pagNo);


  int recNo = 0;

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < model->rowCount())
  {

        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 16; ++secRow)
      {
          QSqlRecord record = model->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 220+100*(secRow+1);

              painter->setPen(QPen(Qt::black,1));
              painter->drawText(20,verticalPos,100,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(100,verticalPos,400,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("cliente").toString());
              painter->drawText(400,verticalPos,580,100,
                           Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("tipo_doc").toString());
              painter->drawText(680,verticalPos,850,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("num_doc").toString());
              painter->drawText(850,verticalPos,1200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("data_scadenza").toString());
              /*
               * Tipo di pagamento
               * Se supera i 100 caratteri automaticamente
               * va a capo...
               */
              QString pag_fatt = record.value("pag_fatt").toString();
              if(pag_fatt.length() == 100)
              {
                  pag_fatt.append("\n");
              }
              painter->drawText(1200,verticalPos,1600,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                pag_fatt);

              if(record.value("importo").toInt() == 0){
                  painter->drawText(1600,verticalPos,1750,100,
                                    Qt::AlignVCenter | Qt::AlignHCenter,
                                    "");
              }
              else{
              painter->drawText(1580,verticalPos,1750,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                QString::fromUtf8("\u20AC")+ "   " +record.value("importo").toString());
              }
              painter->drawText(1750,verticalPos,1900,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                                record.value("flag_sc").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < model->rowCount())
      {
          printer->newPage();
          this->pagina_lista_scadenze(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::pagina_lista_scadenze(int pagNo){
    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::black,1));
  painter->drawRect(0,0,2870,2000);

  //Quadrato indicazioni lista
  painter->drawRect(0,120,2870,170);

  //Disegno i quadrati dove visulizzo i dati
  //Quadrato ID
  painter->drawRect(0,120,130,1780);
  //Quadrato Cliente
  painter->drawRect(130,120,350,1780);
  //Quadrato tipo documento
  painter->drawRect(480,120,450,1780);
  //Quadrato numero documento
  painter->drawRect(930,120,340,1780);
  //Quadrato scadenza documento
  painter->drawRect(1270,120,350,1780);
  //Quadrato tipo di pagamento
  painter->drawRect(1620,120,730,1780);
  //Quadrato importo
  painter->drawRect(2350,120,210,1780);

  //Quadrato lista scadenze
  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Lista scadenze"));

  painter->setPen(QPen(Qt::black,1));
  //Valorizzazione header box intestazione colonne
  painter->drawText(30,155,60,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  QString cli_for = qApp->translate("Stampe","Cliente ");
  cli_for = cli_for.append("\n");
  cli_for += qApp->translate("Stampe","o fornitore");
  painter->drawText(120,155,340,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                    cli_for);
  painter->drawText(340,155,700,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Tipo documento"));

  QString num_doc = qApp->translate("Stampe","Numero ");
  num_doc = num_doc.append("\n");
  num_doc += qApp->translate("Stampe","documento");
  painter->drawText(700,155,800,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    num_doc);

  QString scad_doc = qApp->translate("Stampe","Scadenza ");
  scad_doc = scad_doc.append("\n");
  scad_doc += qApp->translate("Stampe","documento");
  painter->drawText(800,155,1300,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    scad_doc);
  painter->drawText(1300,155,1400,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Tipo di pagamento"));
  painter->drawText(1610,155,1700,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Importo"));

  QString pag_nonpag = qApp->translate("Stampe","Pagata(P) o");
  pag_nonpag = pag_nonpag.append("\n");
  pag_nonpag += qApp->translate("Stampe"," non pagata(N)");
  painter->drawText(1770,155,1900,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    pag_nonpag);

      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampa_distinta(QPrinter *printer, QSqlRelationalTableModel *model, QModelIndexList index, 
			     QModelIndex *index_row, int cli_forn){


    // la funzione stampa_distinta accoglie come parametro un oggetto puntatore
        // che rappresenta il dataset da mandare in stampa e che quindi
        // contiene i dati del report
          printer->setResolution(254);
          printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
          printer->setOrientation(QPrinter::Portrait);
        // inizio fase di stampa. Si attiva il painter sulla printer
        // impostata in precedenza
      painter->begin(printer);
      painter->setFont(QFont("Arial",8,10));
      int pagNo=0;
      ++pagNo;
      pagina_riba(pagNo);
      
      // per ogni record del data set si effettua la stampa del dettaglio
      if ( index.count() <= model->rowCount())
      {
	  
            // Occorre gestire a mano il controllo di quante righe �? possibile stampare
            // per prova si �? impostato un massimo di 4 righe di dettaglio per pagina
            // Adeguare il numero di righe in base all'altezza del carattere di stampa
          for (int secRow = 0; secRow < index.size(); ++secRow)
          {

              QSqlRecord record = model->record(index.at(secRow).row());

              if(!record.isEmpty()){
                 // in realt�  il tentativo di controllare se in record �? vuoto
                 // non funziona nel presente esempio. Non ho capito perché.

                  //Data emissione
                  painter->setPen(QPen(Qt::black,1));
                  painter->setFont(QFont("Arial",8,10));
                  painter->drawText(290,740,300,300,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    QString::number(QDate::currentDate().day())+"/"+
                                    QString::number(QDate::currentDate().month())+"/"+
                                    QString::number(QDate::currentDate().year()));
                  QSqlQuery query;
                  query.prepare("select nome_azienda from azienda where predefinito='"+qApp->translate("Stampe","Predefinito")+"'");
                  query.exec();
                  //Ditta emittente riba
                  if(query.next()){
                  painter->drawText(960,740,300,300,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    query.value(0).toString());
                   }
                  painter->drawText(800,800,1000,1000,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    record.value("banca").toString()+" - "+record.value("iban").toString());

                  if(cli_forn == 0){
                      QSqlQuery query_cli;
                      query_cli.prepare("select * from clienti where id="+record.value("idcliente").toString());
                      query_cli.exec();
                      if(query_cli.next()){
                          painter->drawText(60,1050,500,500,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            query_cli.value(1).toString()+" "+query_cli.value(2).toString().append("\n\n")+
                                            query_cli.value(3).toString().append("\n\n")+
                                            "P.IVA: "+query_cli.value(7).toString().append("\n\n")+
                                            "C.FISC: "+query_cli.value(6).toString());
                  }
		  }
                  else if(cli_forn==1){
                      QSqlQuery query_cli;
                      query_cli.prepare("select * from fornitori where id="+record.value("idfornitore").toString());
                      query_cli.exec();
                      if(query_cli.next()){
                          painter->drawText(60,1050,500,500,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            query_cli.value(1).toString().append("\n\n")+
                                            query_cli.value(4).toString().append("\n\n")+
                                            query_cli.value(5).toString()+" "+query_cli.value(6).toString()+" "+query_cli.value(7).toString().append("\n\n")+
                                            "P.IVA: "+query_cli.value(2).toString().append("\n\n")+
                                            "C.FISC: "+query_cli.value(3).toString());
                  }
                }

                 QSqlQuery query_banca;
                 query_banca.prepare("select * from banca where nome_banca='"+record.value("banca").toString()+"'");
                 query_banca.exec();

                 if(query.next()){
                 painter->drawText(1300,1050,500,500,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   query_banca.value(2).toString()+
                                   query_banca.value(3).toString()+" "+
                                   query_banca.value(4).toString()+" "+
                                   query_banca.value(6).toString().append("\n\n")
                                   /*
                                    * query_banca.value(1).toString() deve essere l'ABI E CAB della
                                    * banca domiciliata e non l'qApp->translate("Stampe",IBAN)
                                    **/
                                   +query.value(1).toString());
                 }

                 int verticalPos = 1500+50*(secRow+1);
                 QSqlQuery query_fatt;
                 query_fatt.prepare("select * from fattura_vendita where anno_ins='"+record.value("doc_anno").toString()+"' and id='"+record.value("num_doc").toString()+"'");
                 query_fatt.exec();
                 if(query_fatt.next()){

                 painter->drawText(60,verticalPos,1000,1000,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   record.value("tipo_doc").toString()+" "+record.value("num_doc").toString()
                                   +" del "+QString::number(query_fatt.value(1).toDate().day())+"/"+
                                   QString::number(query_fatt.value(1).toDate().month())+"/"+
                                   QString::number(query_fatt.value(1).toDate().year()));
                 }

                 if(record.value("data_scadenza").toString() == ""){
                     painter->drawText(1000,verticalPos,400,400,
                                       Qt::AlignJustify | Qt::AlignJustify,
                                       "");
                 }
                 else{
                 painter->drawText(1000,verticalPos,400,400,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   QString::number(record.value("data_scadenza").toDate().day())+"/"+
                                   QString::number(record.value("data_scadenza").toDate().month())+"/"+
                                   QString::number(record.value("data_scadenza").toDate().year()));
                 }

                 if(record.value("importo").toInt() == 0){
                     painter->drawText(1400,verticalPos,400,400,
                                       Qt::AlignJustify | Qt::AlignJustify,
                                       "");
                 }
                 else{
                 painter->drawText(1400,verticalPos,400,400,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   QString::fromUtf8("\u20AC")+ "   " +record.value("importo").toString());


                 QString importo = record.value(record.indexOf("importo")).toString();
                 QFile file("temp.txt");
                 file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
                 QTextStream stream(&file);
                 stream << importo;
                 stream << "\n";
                 file.close();
                 }
            }


          // onde evitare di generare una pagina vuota si controlla a mano
          // se si �? reggiunto l'ultimo record
      }
          totale_riba();
      }
      if(index_row->row() < model->rowCount())
      {
      }
       // chiusura della fase di inserimento dati. Richiamando end()
       // il file viene fisicamente prodotto o mandato in stampa

      painter->end();
}

void Stampe::totale_riba()
{
    int verticale = 2575+50;
    QFile file("temp.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString linea;
    double sum = 0;
    while(!file.atEnd()){
        linea = file.readLine();
        sum = sum +linea.toDouble();
    }
    painter->setFont(QFont("Arial",16,50));
    painter->setPen(QPen(Qt::RoundCap));
    QRect rect(32,2600,1900,100);
    painter->drawRect(rect);
    QRect rect1(32,2600,850,100);
    painter->drawRect(rect1);
    painter->drawText(550,verticale,1900,1900,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Totale"));
    painter->drawText(1400,verticale,1900,1900,
                      Qt::AlignJustify | Qt::AlignJustify,
                      QString::fromUtf8("\u20AC")+ "   " +QString::number(sum));
    file.close();
    file.remove();
}

void Stampe::pagina_riba(int pagNo)
{
    //Visualizzo intestazione
    intestazione();

    //Rettangolo intestazione azienda
    QRect rett(32,220,800,305);
    painter->drawRoundRect(rett,25,25);
    painter->drawLine(55,310,780,310);

    //Rettangolo intestazione riba
    QRect rett2(32,700,1900,200);
    painter->drawRect(rett2);
    painter->setFont(QFont("Arial",25,50));
    painter->drawText(40,605,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Distinta emissione RIBA"));

    //Dati all'interno del rettangolo riba
    QRect rett3(32,900,1900,1900);
    painter->drawRect(rett3);
    painter->setFont(QFont("Arial",8,50));
    painter->drawText(60,740,300,200,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Data emissione:"));

    painter->drawText(800,740,300,200,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Emittente:"));

    //Destinatario riba
    painter->drawText(60,1000,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Destinatario:"));

    //Banca ricevente riba
    painter->drawText(1300,1000,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Banca domiciliata:"));

    //Causale della riba
    painter->drawText(60,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Causale"));

    painter->drawText(1000,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Scadenza"));

    painter->drawText(1400,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Importo"));


    //Numero di pagina
    painter->setFont(QFont("Arial",6,50));
    painter->drawLine(32,2700,1925,2700);
    painter->drawText(1800,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"/"+QString("%1").arg(pagNo));
    //Versione Fabaria Gest sulla pagina di stampa
    painter->drawText(100,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString()+" Powered by Codelinsoft");
}

void Stampe::stampa_riep_agenti(QPrinter* printer, QSqlRelationalTableModel* model, 
				QModelIndexList index, QModelIndex* index_row, QString mese)
{
          printer->setResolution(254);
          printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
          printer->setOrientation(QPrinter::Portrait);
        // inizio fase di stampa. Si attiva il painter sulla printer
        // impostata in precedenza
      painter->begin(printer);
      painter->setFont(QFont("Arial",8,10));
      int pagNo=0;
      ++pagNo;
      pagina_riep_agenti(pagNo);
      
      // per ogni record del data set si effettua la stampa del dettaglio
      if ( index.count() <= model->rowCount())
      {
          for (int secRow = 0; secRow < index.size(); ++secRow)
          {

              QSqlRecord record = model->record(index.at(secRow).row());

              if(!record.isEmpty()){
                  //Data emissione
                  painter->setPen(QPen(Qt::black,1));
                  painter->setFont(QFont("Arial",8,10));
                  painter->drawText(290,740,300,300,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    QString::number(QDate::currentDate().day())+"/"+
                                    QString::number(QDate::currentDate().month())+"/"+
                                    QString::number(QDate::currentDate().year()));
                  QSqlQuery query;
                  query.prepare("select nome_azienda from azienda");
                  query.exec();
                  //Ditta emittente riepilogo agenti
                  if(query.next()){
                  painter->drawText(960,740,300,300,
                                    Qt::AlignJustify | Qt::AlignJustify,
                                    query.value(0).toString());
                   }

                      QSqlQuery query_cli;
                      query_cli.prepare("select * from lista_agenti where id="+record.value("id_agente").toString());
                      query_cli.exec();
                      if(query_cli.next()){
                          painter->drawText(60,1050,500,500,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            query_cli.value(1).toString()+" "+query_cli.value(2).toString().append("\n\n")+
                                            query_cli.value(7).toString().append("\n\n")+
                                            query_cli.value(5).toString()+" "+query_cli.value(4).toString()+" "+query_cli.value(6).toString().append("\n\n"));
		      }

		      if(mese == ""){
			//Periodo di appartenenza
		      painter->drawText(1300,1050,500,500,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   QString::number(record.value("data").toDate().year()));
		      }
		      else{
		      //Periodo di appartenenza
		      painter->drawText(1300,1050,500,500,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   mese+"  -  "+QString::number(record.value("data").toDate().year()));
		      }
		      
                 int verticalPos = 1500+50*(secRow+1);
		 QSqlQuery query_fatt;
                 query_fatt.prepare("select * from fattura_vendita where anno_ins='"+record.value("data").toDate().toString("yyyy")+"' and id='"+record.value("id_fattura").toString()+"'");
                 query_fatt.exec();
                 if(query_fatt.next()){
		  
		 //Visualizzo la fattura
                 painter->drawText(60,verticalPos,1000,1000,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   query_fatt.value(3).toString()+" "+record.value("id_fattura").toString()
                                   +qApp->translate("Stampe"," del ")+QString::number(query_fatt.value(1).toDate().day())+"/"+
                                   QString::number(query_fatt.value(1).toDate().month())+"/"+
                                   QString::number(query_fatt.value(1).toDate().year()));
		 
		 //visulizzo l'importo della fattura
		 painter->drawText(1350,verticalPos,400,400,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   QString::fromUtf8("\u20AC")+ "   " +query_fatt.value(4).toString());
                 }
                 
		 painter->drawText(700,verticalPos,1000,1000,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   record.value("agente").toString());
		 
                 painter->drawText(1100,verticalPos,400,400,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   record.value("provvigione").toString()+" "+QString::fromUtf8("%"));
                 

                 if(record.value("importo").toInt() == 0){
                     painter->drawText(1600,verticalPos,400,400,
                                       Qt::AlignJustify | Qt::AlignJustify,
                                       "");
                 }
                 else{
                 painter->drawText(1600,verticalPos,400,400,
                                   Qt::AlignJustify | Qt::AlignJustify,
                                   QString::fromUtf8("\u20AC")+ "   " +record.value("importo").toString());


                 QString importo = record.value(record.indexOf("importo")).toString();
                 QFile file("temp.txt");
                 file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
                 QTextStream stream(&file);
                 stream << importo;
                 stream << "\n";
                 file.close();
                 }
            }


          // onde evitare di generare una pagina vuota si controlla a mano
          // se si �? reggiunto l'ultimo record
      }
          totale_riep_agenti();
      }
      if(index_row->row() < model->rowCount())
      {
	    printer->newPage();
	    pagina_riep_agenti(pagNo);
	    ++pagNo;
      }
       // chiusura della fase di inserimento dati. Richiamando end()
       // il file viene fisicamente prodotto o mandato in stampa

      painter->end();
}

void Stampe::totale_riep_agenti()
{
    int verticale = 2575+50;
    QFile file("temp.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString linea;
    double sum = 0;
    while(!file.atEnd()){
        linea = file.readLine();
        sum = sum +linea.toDouble();
    }
    painter->setFont(QFont("Arial",16,50));
    painter->setPen(QPen(Qt::RoundCap));
    QRect rect(32,2600,1900,100);
    painter->drawRect(rect);
    QRect rect1(32,2600,850,100);
    painter->drawRect(rect1);
    painter->drawText(550,verticale,1900,1900,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Totale"));
    painter->drawText(1400,verticale,1900,1900,
                      Qt::AlignJustify | Qt::AlignJustify,
                      QString::fromUtf8("\u20AC")+ "   " +QString::number(sum));
    file.close();
    file.remove();
}

void Stampe::pagina_riep_agenti(int pagNo)
{
    //Visualizzo intestazione
    intestazione();

    //Rettangolo intestazione azienda
    QRect rett(32,220,800,305);
    painter->drawRoundRect(rett,25,25);
    painter->drawLine(55,310,780,310);

    //Rettangolo intestazione riepilogo agente
    QRect rett2(32,700,1900,200);
    painter->drawRect(rett2);
    painter->setFont(QFont("Arial",25,50));
    painter->drawText(40,605,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Riepilogo agenti"));

    //Dati all'interno del rettangolo agenti
    QRect rett3(32,900,1900,1900);
    painter->drawRect(rett3);
    painter->setFont(QFont("Arial",8,50));
    painter->drawText(60,740,300,200,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Data emissione:"));

    painter->drawText(800,740,300,200,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Emittente:"));

    //Destinatario riepilogo agente
    painter->drawText(60,1000,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Destinatario:"));

    //Periodo di appartenenza
    painter->drawText(1300,1000,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Periodo:"));
    
    //Fattura
    painter->drawText(60,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Fattura"));

    //Fattura
    painter->drawText(700,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Agente"));
    
    painter->drawText(1100,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Provvigione"));

    painter->drawText(1350,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Importo fattura"));
    
    painter->drawText(1600,1500,300,300,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Importo provvigione"));


    //Numero di pagina
    painter->setFont(QFont("Arial",6,50));
    painter->drawLine(32,2700,1925,2700);
    painter->drawText(1800,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"/"+QString("%1").arg(pagNo));
    //Versione Fabaria Gest sulla pagina di stampa
    painter->drawText(100,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString()+" Powered by Codelinsoft");
}

void Stampe::stampa_preventivi(QPrinter *printer, QSqlRelationalTableModel *dataset){
    // la funzione stampa_fattura accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  this->preventivi(pagNo);
  ++pagNo;
  // si richiama la sotto funzione che "disegna" il modulo di stampa



  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
          painter->setPen(QPen(Qt::black,1));

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {
        //printer->setOrientation(QPrinter::Landscape);
        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 20; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 150+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(200,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("data").toString());
              painter->drawText(900,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("fornitore").toString());
              painter->drawText(1500,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->preventivi(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::preventivi(int pagNo){

    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Elenco preventivi"));


  //Valorizzazione header box intestazione colonne

  painter->drawText(0,100,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(250,100,800,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Data"));
  painter->drawText(850,100,1100,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Fornitore o cliente"));
  painter->drawText(1500,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Totale"));


      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(20,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}


void Stampe::stampa_ordini(QPrinter *printer, QSqlRelationalTableModel *dataset){
    // la funzione stampa_fattura accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  this->ordini(pagNo);
  ++pagNo;
  // si richiama la sotto funzione che "disegna" il modulo di stampa



  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
          painter->setPen(QPen(Qt::black,1));

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {
        //printer->setOrientation(QPrinter::Landscape);
        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 20; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 150+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(200,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("data").toString());
              painter->drawText(900,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("fornitore").toString());
              painter->drawText(1500,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->ordini(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::ordini(int pagNo){

    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Elenco ordini"));


  //Valorizzazione header box intestazione colonne

  painter->drawText(0,100,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(250,100,800,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Data"));
  painter->drawText(850,100,1100,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Fornitore o cliente"));
  painter->drawText(1500,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Totale"));


      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(20,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampa_ord_prod(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id)
{
    // la funzione stampa_fatt_vendita_libri accoglie come parametro un oggetto puntatore
          // che rappresenta il dataset da mandare in stampa e che quindi
          // contiene i dati del report
            printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
            printer->setPaperSize(QPrinter::A4);
            printer->setResolution(254);
            printer->setOrientation(QPrinter::Portrait);
          // inizio fase di stampa. Si attiva il painter sulla printer
          // impostata in precedenza
        painter->begin(printer);

        painter->setFont(QFont("Arial",8,50));
        painter->setPen(QPen(Qt::black,1));

        int pagNo = 1;
        pagina_ord_prod(pagNo,id);
        ++pagNo;

        int recNo = 0;
        if(index.row() < dataset->rowCount())
        {

            for(int secRow=0; secRow < 1; ++secRow){
                QSqlRecord rec = dataset->record(index.row());
                QSqlQuery query1;
                query1.exec("select * from ordine_righe where id='"+QString::number(id)+"'");
                if(!rec.isEmpty()){

                    if(query1.next()){

                        painter->setFont(QFont("Arial",16,50));
                        painter->setPen(QPen(Qt::RoundCap));
                        painter->drawText(1190,65,2000,1500,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          query1.value(13).toString());
                        painter->setFont(QFont("Arial",8,50));
                        painter->drawText(1190,125,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        query1.value(14).toString()+" "+query1.value(15).toString()+"-"+query1.value(16).toString());
                        painter->drawText(1190,160,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      "Tel. "+query1.value(18).toString()+" Fax "+query1.value(19).toString());
                        painter->drawText(1190,205,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                        "C.F/P.I: "+query1.value(20).toString());
                    }

                    painter->setFont(QFont("Arial",8,50));
                    painter->drawText(1190,345,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",ORDINE)+"  N"+QString::fromUtf8("\u0176")+":"+rec.value("id").toString());
                    painter->drawText(1190,405,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",DATA_ORDINE)+":   "+rec.value("data").toDate().toString("dd-MM-yyyy"));
                    painter->drawText(1190,460,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",TIPO_ORDINE)+":   "+rec.value("tipo_ordine").toString());
                }

            }
        }
        QSqlQuery query;
        query.exec("select * from ordine_righe_art where id='"+QString::number(id)+"'");
        if(recNo < query.record().count()){
            for(int row_sec=0; row_sec < 36; ++row_sec){

                if(query.next()){
                QSqlRecord rec = query.record();
                int verticalPos = 595+45*(row_sec+1);

                if(!rec.isEmpty()){

                    painter->setFont(QFont("Arial",8,50));
                    painter->drawText(20,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("cod_art").toString());

                    QString desc = rec.value("descrizione").toString();
                    if(desc.length() == 100)
                    {
                        desc.append("\n");
                    }
                    painter->drawText(225,verticalPos,800,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      desc);


                    painter->drawText(1020,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("unita").toString());
                    painter->drawText(1102,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("quantita").toString());

                    painter->drawText(1270,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("prezzo_unit").toDouble()));
                    painter->drawText(1470,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("sconto").toDouble()));
                    painter->drawText(1700,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("prezzo_s_iva").toDouble()));
                    painter->drawText(1930,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("iva").toDouble())+"%");

                }
                }
                ++recNo;
            }
        }

        if(recNo < query.record().count()){
            painter->drawText(1600,2590,4000,100,
                              Qt::AlignJustify | Qt::AlignJustify,
                              qApp->translate("Stampe","Segue ----->"));
            printer->newPage();
            pagina_ord_prod(pagNo,id);
            ++pagNo;
        }


          painter->end();
}

void Stampe::pagina_ord_prod(int pagNo, int id)
{
    //Instestazione ordini
       intestazione();
       //Impaginazione ordini
       layout_ordini();
       int recNo_1=0;
       //Impostazione degli ultimi dati
       QSqlQuery query2;
       query2.exec("select * from ordine_righe where id='"+QString::number(id)+"'");
       if(recNo_1 < query2.record().count()){
           for(int x=0; x < 1; ++x){
               if(query2.next()){
               QSqlRecord rec = query2.record();

               if(!rec.isEmpty()){

                   //Visualizza spese di trasporto
                   painter->drawText(355,2260,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("spese_trasporto").toString());

                   //Visualizza spese di incasso
                   painter->drawText(975,2260,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("spese_incasso").toString());

                   //Visualizza tipo di pagamento
                   painter->drawText(160,2503,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("tipo_pagamento").toString());

                   //Visualizza Banca
                   painter->drawText(110,2553,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("banca").toString());

                   //Visualizza Iban
                   painter->drawText(90,2608,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("iban").toString());

                   //Visualizza note fattura
                   painter->drawText(160,2503,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     rec.value("annotazioni").toString());


                   //Setto i caratteri in grassetto
                   painter->setFont(QFont("Arial",6,QFont::Bold,true ));
                   //Visualizza imponibile
                   painter->drawText(1670,2290,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("prezzo_s_iva").toString());

                   //Visualizza iva
                   painter->drawText(1670,2370,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("iva").toString());

                   //Visualizzo totale
                   painter->drawText(1670,2450,4000,100,
                                     Qt::AlignJustify | Qt::AlignJustify,
                                     QString::fromUtf8("\u20AC")+ "   " +rec.value("totale").toString());

                   //Setto i caratteri in corsivo

                   QFont *font = new QFont("Arial");
                   font->setItalic(true);
                   font->setPixelSize(24);
                   painter->setFont(*font);

                   //Visualizzo descizione iva
                   QSqlQuery query3;
                   query3.exec("select A.iva,A.descrizione as aliquota, B.iva as ordine_righe_art from aliquota A,ordine_righe_art B where A.iva=B.iva and B.id='"+QString::number(id)+"'");
                   if(query3.next()){

                       //Visualizzo codice iva
                       painter->drawText(20,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         query3.value(0).toString());

                       //Visualizzo descrizione iva
                       painter->drawText(280,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         query3.value(1).toString());


                       //Visualizza imponibile
                       painter->drawText(540,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         QString::fromUtf8("\u20AC")+ "   " +rec.value("prezzo_s_iva").toString());


                       //Visualizzo codice iva
                       painter->drawText(800,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         query3.value(0).toString()+"%");



                       //Visualizza iva
                       painter->drawText(1060,2360,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                         QString::fromUtf8("\u20AC")+ "   " +rec.value("iva").toString());

                   }
                   else{
                       qDebug() << query3.lastError().text();
                   }


               }
               }
               ++recNo_1;
           }
       }
       //Numero di pagina
       painter->setFont(QFont("Arial",6,50));
       painter->drawText(1800,2700,2000,100,
                        Qt::AlignVCenter|Qt::AlignLeft,
                         qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"/"+QString("%1").arg(pagNo));
}
/*
 * Impostazione stampa ddt
 */
void Stampe::layout_ddt()
{
    //Rettangolo intestazione azienda
    QRect rett(32,220,800,305);
    painter->drawRoundRect(rett,25,25);
    painter->drawLine(55,310,780,310);

    //Rettangolo intestazione cliente-fornitore
    QRect rett1(1170,45,725,205);
    painter->drawRoundRect(rett1,25,25);
    painter->drawLine(1190,123,1815,123);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,10,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Cliente"));

    //Rettangolo dati fattura
    QRect rett2(1170,325,725,205);
    painter->drawRoundRect(rett2,25,25);
    painter->setFont(QFont("Arial",10,50));
    painter->drawText(1235,280,2000,1500,
                      Qt::AlignJustify | Qt::AlignJustify,
                      qApp->translate("Stampe","Dati DDT"));


    //Rettangolo fattura da riempire
    painter->drawRect(5,550,2000,2140);
    painter->drawRect(5,610,2000,0);

    painter->setFont(QFont("Arial",6,50));
    //Rettangolo Codice
    painter->drawRect(5,550,200,1700);
    painter->drawText(12,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",CODICE));
    //Rettangolo Descrizione
    painter->drawRect(205,550,1240,1700);
    painter->drawText(225,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));

    //Rettangolo Sconto
    painter->drawRect(1445,550,200,1700);
    painter->drawText(1490,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",UM));

    //Rettangolo importo
    painter->drawRect(1645,550,255,1700);
    painter->drawText(1720,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      QString::fromUtf8(qApp->translate("Stampe",QUANTITA).toStdString().c_str()));

    //Rettangolo Iva
    painter->drawRect(1900,550,105,1700);
    painter->drawText(1930,540,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",IVA));

    //Rettangolo spese di trasporto
    painter->drawRect(5,2250,1300,50);
    painter->drawRect(325,2250,320,50);
    painter->drawRect(645,2250,320,50);
    painter->drawText(20,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",ASPETTO_ESTERIORE));

    painter->drawText(655,2225,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",NUMERO_COLLI));

    //Dettaglio consegna
    painter->drawRect(5,2300,1300,50);
    painter->drawRect(5,2300,1300,190);
    painter->drawRect(5,2300,260,190);
    painter->drawRect(265,2300,260,190);
    painter->drawRect(525,2300,260,190);
    painter->drawRect(785,2300,260,190);

    painter->drawText(20,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PESO_LORDO));

    painter->drawText(280,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PESO_NETTO));

    painter->drawText(540,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",CONSEGNA));

    painter->drawText(800,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DATA_ORA_DDT));

    painter->drawText(1060,2275,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",DESCRIZIONE));


    //Tipo vettore
    painter->drawRect(5,2490,800,50);
    painter->drawText(20,2465,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",VETTORE)+": ");

    //Firma vettore
    painter->drawRect(5,2540,800,50);
    painter->drawText(20,2515,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",FIRMA_VETTORE)+": ");

    //conducente
    painter->drawRect(5,2590,800,50);
    painter->drawText(20,2570,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      QString::fromUtf8(qApp->translate("Stampe",GENERALITA).toStdString().c_str())+": ");

    //Note fattura
    painter->drawRect(5,2640,800,50);
    painter->drawText(20,2620,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",NOTE_DDT)+": ");

    //Rettangolo porto-firma conducente-firma destinatario
    painter->drawRect(1305,2250,700,240);
    painter->drawRect(1305,2250,350,240);
    painter->drawText(1320,2250,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",PORTO));
    //Riga imponibile
    painter->drawRect(1305,2250,700,80);
    painter->drawText(1320,2330,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",FIRMA_COND));
    //Riga iva
    painter->drawRect(1305,2330.6,700,80);
    painter->drawText(1320,2410,2000,100,
                      Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe",FIRMA_DEST));

    //Versione Fabaria Gest sulla pagina di stampa
    painter->setFont(QFont("Arial",6,50));
    painter->drawText(10,2700,2000,100,
                     Qt::AlignVCenter|Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString()+" Powered by Codelinsoft");

}

void Stampe::stampa_ddt(QPrinter *printer, QSqlRelationalTableModel *dataset)
{
    // la funzione stampa_fattura accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  this->ddt(pagNo);
  ++pagNo;
  // si richiama la sotto funzione che "disegna" il modulo di stampa



  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
          painter->setPen(QPen(Qt::black,1));

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {
        //printer->setOrientation(QPrinter::Landscape);
        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 20; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 150+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(200,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("data").toString());
              painter->drawText(900,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("fornitore").toString());
              painter->drawText(1500,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("totale").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->ddt(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::ddt(int pagNo)
{
    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Elenco ddt"));


  //Valorizzazione header box intestazione colonne

  painter->drawText(0,100,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(250,100,800,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Data"));
  painter->drawText(850,100,1100,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Fornitore o cliente"));
  painter->drawText(1500,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Totale"));


      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(20,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampa_ddt_prod(QPrinter *printer, QSqlRelationalTableModel *dataset, QModelIndex index, int id)
{
    // la funzione stampa_fatt_vendita_libri accoglie come parametro un oggetto puntatore
          // che rappresenta il dataset da mandare in stampa e che quindi
          // contiene i dati del report
            printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
            printer->setPaperSize(QPrinter::A4);
            printer->setResolution(254);
            printer->setOrientation(QPrinter::Portrait);
          // inizio fase di stampa. Si attiva il painter sulla printer
          // impostata in precedenza
        painter->begin(printer);

        painter->setFont(QFont("Arial",8,50));
        painter->setPen(QPen(Qt::black,1));

        int pagNo = 1;
        pagina_ddt_prod(pagNo,id);
        ++pagNo;

        int recNo = 0;
        if(index.row() < dataset->rowCount())
        {

            for(int secRow=0; secRow < 1; ++secRow){
                QSqlRecord rec = dataset->record(index.row());
                QSqlQuery query1;
                query1.exec("select * from ddt_righe where id='"+QString::number(id)+"'");
                if(!rec.isEmpty()){

                    if(query1.next()){

                        painter->setFont(QFont("Arial",16,50));
                        painter->setPen(QPen(Qt::RoundCap));
                        painter->drawText(1190,65,2000,1500,
                                          Qt::AlignJustify | Qt::AlignJustify,
                                          query1.value(13).toString());
                        painter->setFont(QFont("Arial",8,50));
                        painter->drawText(1190,125,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        query1.value(14).toString()+" "+query1.value(15).toString()+"-"+query1.value(16).toString());
                        painter->drawText(1190,160,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      "Tel. "+query1.value(18).toString()+" Fax "+query1.value(19).toString());
                        painter->drawText(1190,205,4000,100,
                                         Qt::AlignJustify | Qt::AlignJustify,
                                        "C.F/P.I: "+query1.value(20).toString());
                    }

                    painter->setFont(QFont("Arial",8,50));
                    painter->drawText(1190,345,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",DDT)+"  N"+QString::fromUtf8("\u0176")+" "+rec.value("id").toString());
                    painter->drawText(1190,405,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",DATA_DDT)+":   "+rec.value("data").toDate().toString("dd-MM-yyyy"));
                    painter->drawText(1190,460,4000,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      qApp->translate("Stampe",TIPO_DDT)+":   "+rec.value("tipo_ddt").toString());
                }

            }
        }
        QSqlQuery query;
        query.exec("select * from ddt_righe_art where id='"+QString::number(id)+"'");
        if(recNo < query.record().count()){
            for(int row_sec=0; row_sec < 36; ++row_sec){

                if(query.next()){
                QSqlRecord rec = query.record();
                int verticalPos = 595+45*(row_sec+1);

                if(!rec.isEmpty()){

                    painter->setFont(QFont("Arial",8,50));
                    painter->drawText(20,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("cod_art").toString());

                    QString desc = rec.value("descrizione").toString();
                    if(desc.length() == 100)
                    {
                        desc.append("\n");
                    }
                    painter->drawText(225,verticalPos,800,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      desc);


                    painter->drawText(1470,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      rec.value("unita").toString());
                    painter->drawText(1700,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("quantita").toDouble()));
                    painter->drawText(1930,verticalPos,200,100,
                                      Qt::AlignJustify | Qt::AlignJustify,
                                      QString::number(rec.value("iva").toDouble())+"%");

                }
                }
                ++recNo;
            }
        }

        if(recNo < query.record().count()){
            painter->drawText(1600,2590,4000,100,
                              Qt::AlignJustify | Qt::AlignJustify,
                              qApp->translate("Stampe","Segue ----->"));
            printer->newPage();
            pagina_ddt_prod(pagNo,id);
            ++pagNo;
        }


          painter->end();
}

void Stampe::pagina_ddt_prod(int pagNo, int id)
{
    //Instestazione ddt
       intestazione();
       //Impaginazione ddt
       layout_ddt();
          int recNo_1=0;
          //Impostazione degli ultimi dati
          QSqlQuery query2;
          query2.exec("select * from ddt_righe where id='"+QString::number(id)+"'");
          if(recNo_1 < query2.record().count()){
              for(int x=0; x < 1; ++x){
                  if(query2.next()){
                  QSqlRecord rec = query2.record();

                  if(!rec.isEmpty()){

                      //Visualizza spese di trasporto
                      painter->drawText(355,2260,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        rec.value("asp_est").toString());

                      //Visualizza spese di incasso
                      painter->drawText(975,2260,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        "N"+QString::fromUtf8("\u0176") +" "+rec.value("n_colli").toString());

                      //Visualizza tipo di pagamento
                      painter->drawText(160,2503,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        rec.value("vettore").toString());

                      //Visualizza Iban
                      painter->drawText(300,2608,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        rec.value("vettore").toString());

                      //Visualizza note fattura
                      painter->drawText(160,2503,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        rec.value("annotazioni").toString());

                      painter->drawText(1670,2290,4000,100,
                                        Qt::AlignJustify | Qt::AlignJustify,
                                        rec.value("porto").toString());

                      //Setto i caratteri in corsivo

                      QFont *font = new QFont("Arial");
                      font->setItalic(true);
                      font->setPixelSize(24);
                      painter->setFont(*font);

                      //Visualizzo descizione iva
                      QSqlQuery query3;
                      query3.exec("select A.iva,A.descrizione as aliquota, B.iva as ddt_righe_art from aliquota A,ddt_righe_art B where A.iva=B.iva and B.id='"+QString::number(id)+"'");
                      if(query3.next()){

                          //Visualizzo codice iva
                          painter->drawText(20,2360,4000,100,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            rec.value("peso_lordo").toString()+" Kg");

                          //Visualizzo descrizione iva
                          painter->drawText(280,2360,4000,100,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            rec.value("peso_netto").toString()+" Kg");


                          //Visualizza imponibile
                          painter->drawText(540,2360,4000,100,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            rec.value("cons_trasp").toString());


                          //Visualizzo codice iva
                          painter->drawText(800,2360,4000,100,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            rec.value("data_ora").toDateTime().toString(QString("dd/MM/yyyy-hh:mm:ss")));



                          //Visualizza iva
                          painter->drawText(1060,2360,4000,100,
                                            Qt::AlignJustify | Qt::AlignJustify,
                                            query3.value(1).toString()+"%");

                      }
                      else{
                          qDebug() << query3.lastError().text();
                      }


                  }
                  }
                  ++recNo_1;
              }
          }
          //Numero di pagina
          painter->setFont(QFont("Arial",6,50));
          painter->drawText(1800,2700,2000,100,
                           Qt::AlignVCenter|Qt::AlignLeft,
                            qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"/"+QString("%1").arg(pagNo));
}

void Stampe::print_codice(QPrinter *printer, QImage img, QString cognome, QString nome,
                         bool maschio, bool femmina, QString com_st, QString pr_st, QString codice, QDate date)
{
        printer->setPaperSize(QPrinter::A7);
        printer->setOrientation(QPrinter::Landscape);
        printer->setResolution(254);

           //printer->setOrientation(QPrinter::Landscape);
         // inizio fase di stampa. Si attiva il painter sulla printer
         // impostata in precedenza
      painter->begin(printer);

      painter->drawImage(5,5,img);
      painter->setFont(QFont("Arial",10,50));
      painter->setPen(QPen(Qt::black, 10, Qt::SolidLine, Qt::RoundCap));
      painter->drawText(60,190,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,codice);
      painter->drawText(30,240,480,100, Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,cognome);
      painter->drawText(30,290,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap, nome);
      painter->drawText(40,340,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap, com_st);
      painter->drawText(40,395,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,pr_st);
      painter->drawText(30,450,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,date.toString(QString("dd/MM/yyyy")));

      if(maschio){
          painter->drawText(480,335,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"M");
      }
      else if(femmina){
          painter->drawText(480,335,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"F");
      }

      painter->setFont(QFont("Arial",12,50));
      painter->drawText(400,440,480,100,Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,"FAC SIMILE");
      painter->end();
}

void Stampe::stampa_lista_email(QPrinter *printer, QSqlRelationalTableModel *dataset){
    // la funzione stampa_fattura accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  pagina_email(pagNo);
  ++pagNo;
  // si richiama la sotto funzione che "disegna" il modulo di stampa



  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
          painter->setPen(QPen(Qt::black,1));

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {
        //printer->setOrientation(QPrinter::Landscape);
        // Occorre gestire a mano il controllo di quante righe è possibile stampare
        // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 20; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realt�  il tentativo di controllare se in record è vuoto
             // non funziona nel presente esempio. Non ho capito perché.

              int verticalPos = 150+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(200,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("nome_file").toString());
              painter->drawText(900,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("forn_cl").toString());
              painter->drawText(1500,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("data").toDateTime().toString("dd/MM/yyyy-hh:mm:ss"));
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si è reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          pagina_email(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void Stampe::pagina_email(int pagNo){

    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Elenco Storico email"));


  //Valorizzazione header box intestazione colonne

  painter->drawText(0,100,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","ID"));
  painter->drawText(250,100,800,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("Stampe","Nome file"));
  painter->drawText(850,100,1100,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Fornitore o cliente"));
  painter->drawText(1500,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("Stampe","Data invio"));


      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(20,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("Stampe","Fabaria gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampa_lista_iva_acq(QPrinter* printer, QSqlQuery query_acq, QString mese, QString iva_cr)
{
    // la funzione stampa_fattura accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  pagina_lista_iva_acq(pagNo);
  ++pagNo;
  
  // si richiama la sotto funzione che "disegna" il modulo di stampa
  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
  painter->setPen(QPen(Qt::black,1));
  
  if(recNo < query_acq.record().count())
  {
      for (int secRow = 0; secRow < 15; ++secRow)
      {   
	  if(query_acq.next()){
          QSqlRecord record_acq = query_acq.record();
	  
	  if(!record_acq.isEmpty())
	  {
              int verticalPos = 250+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("data").toString());
	      painter->drawText(200,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("data_fattura").toString());
	      painter->drawText(400,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("id").toString());
	      painter->drawText(580,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("numero_fattura").toString());
	      painter->drawText(930,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("fornitore").toString());
	      painter->drawText(1250,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("totale").toString());
	      painter->drawText(1480,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("prezzo_s_iva").toString());
	      painter->drawText(1650,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_acq.value("iva").toString());

	      painter->drawText(120,1270,500,200,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           mese+" - "+record_acq.value("anno_ins").toString());
	      
	      //Setto l'azienda
	      QSqlQuery azienda;
	      azienda.exec("select * from azienda where id='"+settingsManager->generalValue("Azienda/id",QVariant()).toString()+"' and predefinito='"+qApp->translate("Stampe","Predefinito")+"'");
	      if(azienda.next())
	      {
		  painter->drawText(200,1480,2000,400,
                          Qt::AlignJustify | Qt::AlignJustify,
                          azienda.value(1).toString());
		  painter->drawText(200,1540,5000,400,
                        Qt::AlignJustify | Qt::AlignJustify,
                        azienda.value(4).toString()+" "+azienda.value(5).toString()+"-"+azienda.value(6).toString()+
                        " "+"("+azienda.value(7).toString()+")");
		  painter->drawText(200,1600,5000,400,
                      Qt::AlignJustify | Qt::AlignJustify,
                      "Tel. "+azienda.value(8).toString()+" Fax "+azienda.value(9).toString());
		  painter->drawText(200,1660,5000,400,
                         Qt::AlignJustify | Qt::AlignJustify,
                        "e-mail: "+azienda.value(11).toString()+" Internet: "+azienda.value(10).toString());
		  painter->drawText(200,1720,5000,400,
                         Qt::AlignJustify | Qt::AlignJustify,
                        "C.F: "+azienda.value(3).toString()+" P.Iva: "+azienda.value(2).toString());
	      }
	      
	      QSqlQuery m_fatt_acq;
	      m_fatt_acq.exec("select sum(iva) from fatture_acq_righe");
	      if(m_fatt_acq.next())
	      {
		  double somma = iva_cr.toDouble()+m_fatt_acq.value(0).toDouble();
		  //Iva per acquisti
		  painter->drawText(2000,1450,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +QString::number(somma));
		  //Iva a credito
		  painter->drawText(2000,1550,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +QString::number(somma));
		  //Iva a credito(Dal periodo precedente)
		  painter->drawText(2000,1650,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +iva_cr);
		  //Totale iva a credito
		  painter->drawText(2000,1750,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +QString::number(somma));
	      }
	      
	      ++recNo;
	  }
	  }
      }
      
      if(recNo < query_acq.record().count())
      {
          printer->newPage();
          pagina_lista_iva_acq(pagNo);
          ++pagNo;
      }
  }
   
  painter->end();
}

void Stampe::pagina_lista_iva_acq(int pagNo)
{
    painter->setFont(QFont("Arial",10,75));
    painter->setPen(QPen(Qt::black,1));
    painter->drawRect(0,0,2870,2000);
    painter->drawRect(0,0,2870,120);
    painter->drawText(0,0,2870,120,
                      Qt::AlignVCenter | Qt::AlignHCenter,
                      qApp->translate("Stampe","Registro IVA - Fatture di acquisto"));

    //Valorizzazione header box intestazione colonne
    painter->drawRect(0,120,2870,200);
    
    //Disegno rettangolo dati
    painter->drawRect(0,120,400,1200);
    painter->drawText(0,200,200,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Data\nReg."));
    painter->drawText(100,200,350,100,
                     Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Data\nDoc"));
    //Disegno rettangolo fattura
    painter->drawRect(400,120,400,1200);
    painter->drawText(325,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Fattura"));
    painter->drawText(250,200,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Num.\nProg."));
    painter->drawText(350,200,650,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Num.\nFatt."));
    
    //Rettangolo ragione sociale
    painter->drawRect(800,120,450,1200);
    painter->drawText(620,200,800,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Ragione sociale/\nNome e cognome"));
    
    //Rettangolo importo
    painter->drawRect(1250,120,200,1200);
    painter->drawText(880,200,950,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Importo"));
    
    //Rettangolo aliquota
    painter->drawRect(1450,120,380,1200);
    painter->drawText(1400,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Aliquota ")+settingsManager->generalValue("ListaIva/Iva",QVariant()).toString()+" %");
    painter->drawText(1030,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Imponibile"));
    painter->drawText(1130,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe",IVA));
    
    //Rettangolo aliquota
    painter->drawRect(1830,120,380,1200);
    painter->drawText(1780,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Aliquota    %"));
    painter->drawText(1410,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Imponibile"));
    painter->drawText(1520,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe",IVA));
    
    //Rettangolo Costi
    painter->drawRect(2210,120,380,1200);
    painter->drawText(2130,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Costi"));
    painter->drawText(1790,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Imponibile"));
    painter->drawText(1900,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe",IVA));
    //Rettangolo esenti/non esenti
    painter->drawRect(2590,120,280,1200);
    painter->drawText(2100,120,1250,200,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","EsentI/Non\nImponibili/\nFuori campo"));

    //Realizzo rettangolo dati
    painter->drawRect(0,1320,2870,580);
    painter->drawRect(0,1320,1435,580);
    //Rettangolo mese
    painter->drawRect(0,1320,1435,100);
    painter->drawText(0,1320,200,100,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Periodo: "));
    //Rettangolo azienda
    painter->drawRect(0,1420,1435,480);
    painter->drawText(0,1440,200,100,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Azienda: "));
    
    //Rettangolo IVA
    painter->drawText(1100,1320,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Iva per acquisti: "));
    painter->drawText(1100,1420,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Iva a credito: "));
    painter->drawText(1100,1520,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Iva a credito\n(Dal periodo precedente): "));
    painter->drawText(1100,1620,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      "Iva a credito: ");
    
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignRight,
                      qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+" ");
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria Gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
    painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampa_lista_iva_vnd(QPrinter* printer, QSqlQuery query_vnd, QString mese, QString iva_cr)
{
     // la funzione stampa_fattura accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report

    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

   int pagNo = 1;
  pagina_lista_iva_vnd(pagNo);
  ++pagNo;
  
  // si richiama la sotto funzione che "disegna" il modulo di stampa
  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
  painter->setPen(QPen(Qt::black,1));
  
  
  if( recNo < query_vnd.record().count())
  {
      for (int secRow = 0; secRow < 15; ++secRow)
      {   
	if(query_vnd.next()){
	  QSqlRecord record_vnd = query_vnd.record();
	  
	  if(!record_vnd.isEmpty())
	  {
              int verticalPos = 250+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record_vnd.value("data").toString());
	      painter->drawText(400,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_vnd.value("id").toString());
	      painter->drawText(930,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_vnd.value("fornitore").toString());
	      painter->drawText(1250,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_vnd.value("totale").toString());
	      painter->drawText(1480,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_vnd.value("prezzo_s_iva").toString());
	      painter->drawText(1650,verticalPos,200,100,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           record_vnd.value("iva").toString());

	      painter->drawText(120,1270,500,200,
				Qt::AlignVCenter | Qt::AlignHCenter,
                           mese+" - "+record_vnd.value("anno_ins").toString());
	      
	      //Setto l'azienda
	      QSqlQuery azienda;
	      azienda.exec("select * from azienda where id='"+settingsManager->generalValue("Azienda/id",QVariant()).toString()+"' and predefinito='"+qApp->translate("Stampe","Predefinito")+"'");
	      if(azienda.next())
	      {
		  painter->drawText(200,1480,2000,400,
                          Qt::AlignJustify | Qt::AlignJustify,
                          azienda.value(1).toString());
		  painter->drawText(200,1540,5000,400,
                        Qt::AlignJustify | Qt::AlignJustify,
                        azienda.value(4).toString()+" "+azienda.value(5).toString()+"-"+azienda.value(6).toString()+
                        " "+"("+azienda.value(7).toString()+")");
		  painter->drawText(200,1600,5000,400,
                      Qt::AlignJustify | Qt::AlignJustify,
                      "Tel. "+azienda.value(8).toString()+" Fax "+azienda.value(9).toString());
		  painter->drawText(200,1660,5000,400,
                         Qt::AlignJustify | Qt::AlignJustify,
                        "e-mail: "+azienda.value(11).toString()+" Internet: "+azienda.value(10).toString());
		  painter->drawText(200,1720,5000,400,
                         Qt::AlignJustify | Qt::AlignJustify,
                        "C.F: "+azienda.value(3).toString()+" P.Iva: "+azienda.value(2).toString());
	      }
	      
	      QSqlQuery m_fatt_acq;
	      m_fatt_acq.exec("select sum(iva) from fatture_vendita_righe");
	      if(m_fatt_acq.next())
	      {
		  double somma = iva_cr.toDouble()+m_fatt_acq.value(0).toDouble();
		  //Iva per acquisti
		  painter->drawText(2000,1450,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +QString::number(somma));
		  //Iva a credito
		  painter->drawText(2000,1550,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +QString::number(somma));
		  //Iva a credito(Dal periodo precedente)
		  painter->drawText(2000,1650,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +iva_cr);
		  //Totale iva a credito
		  painter->drawText(2000,1750,300,300,
			    Qt::AlignJustify | Qt::AlignJustify,
			    QString::fromUtf8("\u20AC")+ "   " +QString::number(somma));
	      }
	      
	      ++recNo;
	  }
	  }
      }
      
      if(recNo < query_vnd.record().count())
      {
          printer->newPage();
          pagina_lista_iva_vnd(pagNo);
          ++pagNo;
      }
  }
  painter->end();
}

void Stampe::pagina_lista_iva_vnd(int pagNo)
{
    painter->setFont(QFont("Arial",10,75));
    painter->setPen(QPen(Qt::black,1));
    painter->drawRect(0,0,2870,2000);
    painter->drawRect(0,0,2870,120);
    painter->drawText(0,0,2870,120,
                      Qt::AlignVCenter | Qt::AlignHCenter,
                      qApp->translate("Stampe","Registro IVA - Fatture di vendita"));

    //Valorizzazione header box intestazione colonne
    painter->drawRect(0,120,2870,200);
    
    //Disegno rettangolo dati
    painter->drawRect(0,120,400,1200);
    painter->drawText(0,200,200,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Data\nReg."));
    painter->drawText(100,200,350,100,
                     Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Data\nDoc"));
    //Disegno rettangolo fattura
    painter->drawRect(400,120,400,1200);
    painter->drawText(325,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Fattura"));
    painter->drawText(250,200,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Num.\nProg."));
    painter->drawText(350,200,650,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Num.\nFatt."));
    
    //Rettangolo ragione sociale
    painter->drawRect(800,120,450,1200);
    painter->drawText(620,200,800,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Ragione sociale/\nNome e cognome"));
    
    //Rettangolo importo
    painter->drawRect(1250,120,200,1200);
    painter->drawText(880,200,950,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Importo"));
    
    //Rettangolo aliquota
    painter->drawRect(1450,120,380,1200);
    painter->drawText(1400,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Aliquota ")+settingsManager->generalValue("ListaIva/Iva",QVariant()).toString()+" %");
    painter->drawText(1030,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Imponibile"));
    painter->drawText(1130,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe",IVA));
    
    //Rettangolo aliquota
    painter->drawRect(1830,120,380,1200);
    painter->drawText(1780,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Aliquota    %"));
    painter->drawText(1410,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Imponibile"));
    painter->drawText(1520,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe",IVA));
    
    //Rettangolo Costi
    painter->drawRect(2210,120,380,1200);
    painter->drawText(2130,120,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Costi"));
    painter->drawText(1790,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Imponibile"));
    painter->drawText(1900,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe",IVA));
    //Rettangolo esenti/non esenti
    painter->drawRect(2590,120,280,1200);
    painter->drawText(2100,120,1250,200,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Esenti/Non\nImponibili/\nFuori campo"));

    //Realizzo rettangolo dati
    painter->drawRect(0,1320,2870,580);
    painter->drawRect(0,1320,1435,580);
    //Rettangolo mese
    painter->drawRect(0,1320,1435,100);
    painter->drawText(0,1320,200,100,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Periodo: "));
    //Rettangolo azienda
    painter->drawRect(0,1420,1435,480);
    painter->drawText(0,1440,200,100,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Azienda: "));
    
    //Rettangolo qApp->translate("Stampe",IVA)
    painter->drawText(1100,1320,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Iva per le vendite: "));
    painter->drawText(1100,1420,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Iva a debito: "));
    painter->drawText(1100,1520,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Iva a debito\n(Dal periodo precedente): "));
    painter->drawText(1100,1620,1200,300,
		      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Iva a debito: "));
    
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignRight,
                      qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+" ");
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria Gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
    painter->drawLine(0,1900,2870,1900);
}

void Stampe::stampaAgenti ( QPrinter* printer, QSqlQueryModel* dataset )
{
    // la funzione st_rma accoglie come parametro un oggetto puntatore
    // che rappresenta il dataset da mandare in stampa e che quindi
    // contiene i dati del report
    printer->setResolution(254);
    printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setPaperSize(QPrinter::A4);
    printer->setOrientation(QPrinter::Landscape);

    // inizio fase di stampa. Si attiva il painter sulla printer
    // impostata in precedenza
  painter->begin(printer);

    int pagNo = 1;
    pagina_agenti(pagNo);
   ++pagNo;
   // si richiama la sotto funzione che "disegna" il modulo di stampa



   int recNo = 0;
   painter->setFont(QFont("Arial",8,50));
           painter->setPen(QPen(Qt::black,1));

   // per ogni record del data set si effettua la stampa del dettaglio

   while (recNo < dataset->rowCount())
   {
         //printer->setOrientation(QPrinter::Landscape);
         // Occorre gestire a mano il controllo di quante righe �? possibile stampare
         // per prova si �? impostato un massimo di 4 righe di dettaglio per pagina
         // Adeguare il numero di righe in base all'altezza del carattere di stampa

       for (int secRow = 0; secRow < 16; ++secRow)
       {
           QSqlRecord record = dataset->record(recNo);

           if (!record.isEmpty())
           {
                int verticalPos = 200+100*(secRow+1);

                painter->setPen(QPen(Qt::black,2));
                painter->drawText(0,verticalPos,200,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("id").toString());
                painter->drawText(150,verticalPos,300,100,
                                 Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("nome").toString());
                painter->drawText(400,verticalPos,500,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("cognome").toString());
                painter->drawText(600,verticalPos,650,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("indirizzo").toString());
                painter->drawText(800,verticalPos,800,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("localita").toString());
                painter->drawText(900,verticalPos,950,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("cap").toString());
                painter->drawText(1000,verticalPos,1100,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  "("+record.value("prov").toString()+")");
                painter->drawText(1100,verticalPos,1250,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("telefono").toString());
                painter->drawText(1300,verticalPos,1400,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("provvigione").toString());
                painter->drawText(1500,verticalPos,1700,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("email").toString());
                painter->drawText(1800,verticalPos,1850,100,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  record.value("data").toString());
                ++recNo;

           }
       }
       if(recNo < dataset->rowCount())
       {
           printer->newPage();
           pagina_agenti(pagNo);
           ++pagNo;
       }
   }
   painter->end();
}

void Stampe::pagina_agenti ( int pagNo )
{
    painter->setFont(QFont("Arial",12,75));
    painter->setPen(QPen(Qt::blue,2));
    painter->drawRect(0,0,2870,2000);
    painter->drawRect(0,0,2870,120);
    painter->drawText(0,0,2870,120,
                      Qt::AlignVCenter | Qt::AlignHCenter,
                      qApp->translate("Stampe","Elenco agenti"));

    //Valorizzazione header box intestazione colonne
    painter->setPen(QPen(Qt::black,2));
    painter->drawText(0,200,200,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","ID"));
    painter->drawText(150,200,350,100,
                     Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Nome"));
    painter->drawText(400,200,500,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Cognome"));
    painter->drawText(600,200,650,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Indirizzo"));
    painter->drawText(800,200,800,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      QString::fromUtf8(qApp->translate("Stampe","Località").toStdString().c_str()));
    painter->drawText(900,200,950,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","CAP"));
    painter->drawText(1000,200,1100,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","PROV"));
    painter->drawText(1100,200,1250,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Telefono"));
    painter->drawText(1300,200,1400,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Provvigione"));
    painter->drawText(1500,200,1700,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","E-mail"));
    painter->drawText(1800,200,1850,100,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      qApp->translate("Stampe","Data"));

    painter->setFont(QFont("Arial",10,50));
    painter->setPen(QPen(Qt::blue,1));
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignRight,
                      qApp->translate("Stampe","Pagina: ")+QString("%1").arg(pagNo)+" ");
    painter->drawText(0,1900,2700,100,
                      Qt::AlignVCenter | Qt::AlignLeft,
                      qApp->translate("Stampe","Fabaria Gest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
    painter->drawLine(0,1900,2870,1900);
}
