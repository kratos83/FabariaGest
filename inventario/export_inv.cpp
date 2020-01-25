#include "export_inv.h"
#include <QtCore>

export_inv::export_inv()
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

#ifdef Q_OS_WIN
    settingsDir = new QDir(QDir::homePath()+"/fabaria/");
#else
    settingsDir = new QDir(QDir::homePath()+"/.fabaria/");
#endif

    if(!settingsDir->exists())
        settingsDir->mkdir(settingsDir->path());

    if(!settingsDir->exists(settingsDir->path()+"/profiles/"))
        settingsDir->mkdir(settingsDir->path()+"/profiles/");

    if(!settingsDir->exists(settingsDir->path()+"/plugins/"))
        settingsDir->mkdir(settingsDir->path()+"/plugins/");

    profDir = new QDir( settingsDir->path()+"/profiles/" );

    general = new QSettings(settingsDir->path()+"/config.conf",QSettings::IniFormat);
}

void export_inv::stampa_inventario(QPrinter *printer, QSqlQueryModel *dataset){

    // la funzione stampa_inventario accoglie come parametro un oggetto puntatore
        // che rappresenta il dataset da mandare in stampa e che quindi
        // contiene i dati del report
        printer->setResolution(254);
         printer->setPageMargins(5.0,5.0,5.0,5.0,QPrinter::Millimeter);
          printer->setOrientation(QPrinter::Portrait);
        // inizio fase di stampa. Si attiva il painter sulla printer
        // impostata in precedenza
      painter->begin(printer);



      // si richiama la sotto funzione che "disegna" il modulo di stampa
      int pagNo = 0;
      ++pagNo;
      this->pagina_inventario(pagNo);


      int recNo = 0;

      // per ogni record del data set si effettua la stampa del dettaglio

      while (recNo < dataset->rowCount())
      {

            // Occorre gestire a mano il controllo di quante righe è possibile stampare
            // per prova si è impostato un massimo di 4 righe di dettaglio per pagina
            // Adeguare il numero di righe in base all'altezza del carattere di stampa

          for (int secRow = 0; secRow < 36; ++secRow)
          {
              QSqlRecord record = dataset->record(recNo);

              if (!record.isEmpty())
              {
                 // in realtà  il tentativo di controllare se in record è vuoto
                 // non funziona nel presente esempio. Non ho capito perchè.

                  int verticalPos = 200+90*(secRow+1);

                  painter->setPen(QPen(Qt::black,1));
                  painter->drawText(0,verticalPos,200,100,
                                    Qt::AlignVCenter | Qt::AlignHCenter,
                               record.value("codbarre").toString());
                  painter->drawText(200,verticalPos,400,100,
                                    Qt::AlignVCenter|Qt::AlignHCenter,
                               record.value("nome_articolo").toString());
                  painter->drawText(600,verticalPos,600,100,
                               Qt::AlignVCenter|Qt::AlignHCenter,
                               record.value("quantita").toString());
                  painter->drawText(1000,verticalPos,800,100,
                                    Qt::AlignVCenter|Qt::AlignHCenter,
                                    record.value("prezzo_unitario").toString());
                  painter->drawText(1300,verticalPos,1000,100,
                                    Qt::AlignVCenter|Qt::AlignHCenter,
                                    QString::fromUtf8("\u20AC")+ "   " +record.value("totale").toString());


                  ++recNo;
              }

          }

          // onde evitare di generare una pagina vuota si controlla a mano
          // se si Ã¨ reggiunto l'ultimo record

          if(recNo < dataset->rowCount())
          {
              printer->newPage();
              pagina_inventario(pagNo);
              ++pagNo;
          }
       }

       // chiusura della fase di inserimento dati. Richiamando end()
       // il file viene fisicamente prodotto o mandato in stampa

      painter->end();
}

void export_inv::pagina_inventario(int pagNo)
{
    // La funzione disegna gli oggetti "Statici" presenti su ogni pagina
    // di fatto prepara il modulo di stampa per accogliere poi i dati

  painter->setFont(QFont("Arial",12,75));
  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2000,2870);

  painter->setPen(QPen(Qt::blue,1));
  painter->drawRect(0,0,2000,120);
  painter->drawText(0,0,2000,120,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("export_inv","Inventario magazzino in data ")+QDate::currentDate().toString("dd/MM/yyyy"));

  //Valorizzazione header box intestazione colonne

  painter->drawRect(0,120,2000,200);
  painter->drawText(0,200,200,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("export_inv","Barcode"));
  painter->drawText(200,200,400,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   qApp->translate("export_inv","Titolo"));
  painter->drawText(600,200,600,100,
                   Qt::AlignVCenter|Qt::AlignHCenter,
                   QString::fromUtf8(qApp->translate("export_inv","Quantità").toStdString().c_str()));
  painter->drawText(1000,200,800,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("export_inv","Prezzo unitario"));
  painter->drawText(1300,200,1000,100,
                    Qt::AlignVCenter|Qt::AlignHCenter,
                    qApp->translate("export_inv","Totale"));

  painter->drawRect(0,2660,2000,100);
  painter->drawRect(320,2660,320,100);
  painter->drawRect(640,2660,320,100);
  painter->drawRect(960,2660,320,100);
  painter->drawRect(1280,2660,320,100);


  painter->setFont(QFont("Arial",8,50));
  painter->drawText(10,2665,320,100,
                    Qt::AlignVCenter|Qt::AlignLeft,
                    qApp->translate("export_inv","Imponibile"));
  painter->drawText(650,2665,320,100,
                    Qt::AlignVCenter|Qt::AlignLeft,
                    qApp->translate("export_inv","Iva"));
  painter->drawText(1290,2665,320,100,
                    Qt::AlignVCenter|Qt::AlignLeft,
                    qApp->translate("export_inv","Totale"));

  QSqlQuery query;
  query.exec("select sum(prezzo_s_iva),sum(totale) from carico_magazzino");
  if(query.next()){
      double num1 = query.value(0).toDouble();
      painter->drawText(335,2695,310,100,
                        Qt::AlignJustify|Qt::AlignJustify,
                        QString::fromUtf8("\u20AC")+ "   " +QString::number(num1));

      double num_totale1 = query.value(1).toDouble();
      double sum_totale = num_totale1;
      double iva = sum_totale-num1;
      painter->drawText(975,2695,310,100,
                        Qt::AlignJustify|Qt::AlignJustify,
                        QString::fromUtf8("\u20AC")+ "   " +QString::number(iva));

      painter->drawText(1615,2695,310,100,
                        Qt::AlignJustify|Qt::AlignJustify,
                        QString::fromUtf8("\u20AC")+ "   " +QString::number(sum_totale));
  }
  else{
      qDebug() << "Query" << query.lastError().text();
  }

  painter->drawText(0,2770,2000,100,
                    Qt::AlignVCenter|Qt::AlignRight,
                    qApp->translate("export_inv","Pagina: ")+QString("%1").arg(pagNo)+"   ");
  painter->drawText(5,2770,2770,100,
                    Qt::AlignVCenter|Qt::AlignLeft,
                    qApp->translate("export_inv","Fabaria Gest - versione ")+general->value("Version/version",QVariant()).toString());
  painter->drawLine(0,2760,2000,2760);
}

