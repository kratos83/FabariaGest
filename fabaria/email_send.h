/*
 * Copyright (C) 2011-2015  Codelinsoft <info@codelinsoft.it>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef EMAIL_SEND_H
#define EMAIL_SEND_H

/******************************
 * Classi QT
 *************************/
#include <QBuffer>
#include <QFile>
#include <QtNetwork>
#include <QtGui>
#include <QtSql>
#include <QDialog>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>
#include <QLoggingCategory>

#include "settingsmanager.h"
#include "listaemail.h"
#include "ui_email_send.h"

//Invio email
#include "../smtp/SmtpMime"

//Gestione eccezioni
#include "queryexception.h"

Q_DECLARE_LOGGING_CATEGORY(FastAssEmailSend)

namespace Ui
{
class email_send;
}

class email_send : public QDialog, public Ui::email_send
{
    Q_OBJECT
    
public:
     explicit email_send(QWidget *parent,double id, QString cl_fr, QString data, QString tipo_fattura);
    ~email_send();
    
    //Visualizzazione in html
    static EmailAddress * stringToEmail(const QString & str);

private:
    
   /* Visualizzazione errori in caso di
    * non invio email
    */
   void errorMessage(const QString & message);
   
   /* Converte la password nel formato
    * stringa...
    */
   QString Base64ToAscii(QString String);
   
   //Visualizzo i file in allegato
   double id_fat;
   
   //Imposto i parametri
   SettingsManager *settings;
   
   //Cliente-fornitore
   QString cliente_fornitore;
   
   //Data fattura-preventivo-ordine-ddt
   QString data_fat;
   
   //Tipo di documento
   QString tipo_doc;
   
   //Notifica
   QString notifica;
   
   //Lista email
   ListaEmail *m_email;
   
public slots:
  //Invio la mail
  void invia_email();
  //Visulizzo allegati esportati in pdf
  void vis_allegati_email();
  //Visualizzo allegati inseriti nella fattura
  void vis_allegati();
   //Visualizzo allegati esportati in pdf ddt
  void vis_allegati_email_ddt();
  //Visualizzo allegati esportati in pdf ordini
  void vis_allegati_email_ord();
  //Visualizzo allegati esportati in pdf preventivi
  void vis_allegati_email_prev();
  //Visualizzazione lista allegati
   void vis_list_allegati(bool vero);
   //Visualizzazione della notifica
   void visNotifica(bool vero);
   //Visualizzo rubrica email
   void visRubrica();
  //Inserimento storico email
  void insert_storico_mail(QString nome_file, QString forn_cl, QDateTime data_ora);
};

extern email_send *m_emailSend;
#endif // EMAIL_SEND_H
