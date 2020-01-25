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
#include "pdf_export.h"
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
#include "defines.h"

using namespace Defines;

pdf_export::pdf_export()
{
    // Creazione oggetto printer (puntatore)
    // ed impostazioni varie sul formato pagina, nome file di appoggio
    // formato file prodotto
    // impostazione margine e risoluzione
    printer = new QPrinter();
         // Formato A4 = 210*297 millimetri
         // Formato A4 = 210*297 millimetri

         printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
         printer->setPaperSize(QPrinter::A4);
         #ifdef Q_OS_WIN
         printer->setOutputFormat(QPrinter::PdfFormat);
         #endif
         #if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
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

            //Creazione oggetto painter che servirÃ  per disegnare le cose
             painter = new QPainter();

}

void pdf_export::StampaVoci1(QPrinter *printer, QSqlQueryModel *dataset){

     // la funzione stampaVoci accoglie come parametro un oggetto puntatore
     // che rappresenta il dataset da mandare in stampa e che quindi
     // contiene i dati del report

   printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
    printer->setPaperSize(QPrinter::A4);
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
         // Occorre gestire a mano il controllo di quante righe Ã¨ possibile stampare
         // per prova si Ã¨ impostato un massimo di 4 righe di dettaglio per pagina
         // Adeguare il numero di righe in base all'altezza del carattere di stampa

       for (int secRow = 0; secRow < 30; ++secRow)
       {
           QSqlRecord record = dataset->record(recNo);

           if (!record.isEmpty())
           {
              // in realtÃ  il tentativo di controllare se in record Ã¨ vuoto
              // non funziona nel presente esempio. Non ho capito perchÃ©.

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
       // se si Ã¨ reggiunto l'ultimo record

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

void pdf_export::creaPagVoci1(int pagNo){
    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->setPen(QPen(Qt::black,1));
  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("pdf_export","Elenco Clienti"));

  //Valorizzazione header box intestazione colonne

  painter->drawText(0,200,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("pdf_export","Codice"));
  painter->drawText(200,200,300,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("pdf_export","Nome"));
  painter->drawText(400,200,600,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("pdf_export","Cognome"));
  painter->drawText(600,200,800,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Indirizzo"));
  painter->drawText(850,200,1000,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Telefono"));
  painter->drawText(1050,200,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("pdf_export","E-mail"));
  painter->drawText(1300,200,1400,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Codice Fiscale"));
  painter->drawText(1550,200,1600,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Partita Iva"));
  painter->drawText(1750,200,1800,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Fax"));

  painter->setFont(QFont("Arial",10,50));
  painter->drawText(0,1900,2770,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("pdf_export","Pagina: ")+QString("%1").arg(pagNo)+"   ");
  painter->drawText(0,1900,2770,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                    qApp->translate("pdf_export","FabariaGest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
  painter->drawLine(0,1900,2870,1900);
}

void pdf_export::stampa_banca(QPrinter *printer, QSqlRelationalTableModel *dataset){
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
  this->banca(pagNo);
  ++pagNo;
  // si richiama la sotto funzione che "disegna" il modulo di stampa



  int recNo = 0;
  painter->setFont(QFont("Arial",8,50));
          painter->setPen(QPen(Qt::black,1));

  // per ogni record del data set si effettua la stampa del dettaglio

  while (recNo < dataset->rowCount())
  {
        //printer->setOrientation(QPrinter::Landscape);
        // Occorre gestire a mano il controllo di quante righe Ã¨ possibile stampare
        // per prova si Ã¨ impostato un massimo di 4 righe di dettaglio per pagina
        // Adeguare il numero di righe in base all'altezza del carattere di stampa

      for (int secRow = 0; secRow < 20; ++secRow)
      {
          QSqlRecord record = dataset->record(recNo);

          if (!record.isEmpty())
          {
             // in realtÃ  il tentativo di controllare se in record Ã¨ vuoto
             // non funziona nel presente esempio. Non ho capito perchÃ©.

              int verticalPos = 150+60*(secRow+1);
              painter->setPen(QPen(Qt::black,1));
              painter->drawText(0,verticalPos,200,100,
                                Qt::AlignVCenter | Qt::AlignHCenter,
                           record.value("id").toString());
              painter->drawText(100,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                           record.value("nome_banca").toString());
              painter->drawText(550,verticalPos,1000,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("iban").toString());
              painter->drawText(1100,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("indirizzo").toString());
              painter->drawText(1500,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("cap").toString());
              painter->drawText(1900,verticalPos,1200,100,
                                Qt::AlignVCenter|Qt::AlignHCenter,
                                record.value("comune").toString());
              ++recNo;
          }

      }

      // onde evitare di generare una pagina vuota si controlla a mano
      // se si Ã¨ reggiunto l'ultimo record

      if(recNo < dataset->rowCount())
      {
          printer->newPage();
          this->banca(pagNo);
          ++pagNo;
      }
   }

   // chiusura della fase di inserimento dati. Richiamando end()
   // il file viene fisicamente prodotto o mandato in stampa

  painter->end();
}

void pdf_export::banca(int pagNo){

    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2870,2000);

  painter->drawRect(0,0,2870,120);
  painter->drawText(0,0,2870,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("pdf_export","Elenco banche"));


  //Valorizzazione header box intestazione colonne

  painter->drawText(0,100,200,100,
                    Qt::AlignVCenter | Qt::AlignHCenter,
                    qApp->translate("pdf_export","ID"));
  painter->drawText(100,100,1000,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Banca"));
  painter->drawText(550,100,1000,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","IBAN"));
  painter->drawText(1100,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Indirizzo"));
  painter->drawText(1500,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Cap"));
  painter->drawText(1900,100,1200,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("pdf_export","Comune"));


      painter->setFont(QFont("Arial",10,50));
      painter->drawText(0,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignRight,
                       qApp->translate("pdf_export","Pagina: ")+QString("%1").arg(pagNo)+"   ");
      painter->drawText(20,1900,2700,100,
                       Qt::AlignVCenter|Qt::AlignLeft,
                       qApp->translate("pdf_export","FabariaGest - versione ")+settingsManager->generalValue("Version/version",QVariant()).toString());
      painter->drawLine(0,1900,2870,1900);
}
