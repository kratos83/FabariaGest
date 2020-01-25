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
#ifndef DEFINES_H
#define DEFINES_H


#include <QtGui>
#include <QtSql>
#include <QSysInfo>
#include <QDebug>
#include <QProcess>
#include <QLabel>
#include <QtCore>
#include <QtWidgets>
#include <QLoggingCategory>
#include <iostream>
#if defined(Q_OS_WIN)
#include <windows.h>
#include <cstdio>
#endif

#include "settingsmanager.h"
#include "../quazip/JlCompress.h"

#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

using namespace std;
#define DATA "Data fattura"
#define DATA_PREV "Data preventivo"
#define DATA_ORDINE "Data ordine"
#define DATA_DDT "Data DDT"
#define TIPO_FATTURA "Tipo di Fattura"
#define FATTURA_IMMEDIATA "Fattura immediata"
#define FATTURA_ACCOMMAGNATORIA "Fattura accompagnatoria"
#define FATTURA_PRO_FORMA "Fattura pro forma"
#define SCONTRINO "Scontrino"
#define TIPO_SCONTRINO "Tipo di scontrino"
#define NOTA_DI_CREDITO "Nota di credito"
#define TIPO_NT_CREDIT "Tipo nota di credito"
#define TIPO_DDT "Tipo DDT"
#define DDT "Documento di trasporto"
#define ORDINE "Ordine"
#define TIPO_ORDINE "Tipo ordine"
#define PREVENTIVO "Preventivo"
#define TIPO_PREVENTIVO "Tipo preventivo"
#define PAGAMENTO "Pagamento"
#define NOTE_PAGAMENTO "Note Pagamento"
#define NOTE_FATTURA "Note fattura"
#define NOTE_PREVENTIVO "Note preventivo"
#define NOTE_ORDINE "Note ordine"
#define NOTE_DDT "Note ddt"
#define COORDINATE_BC "Coordinate Bancarie"
#define AGENTE "Agente"
#define CODICE "Codice"
#define DESCRIZIONE "Descrizione"
#define UM "UM"
#define QUANTITA "Quantità"
#define PREZZO_UNITARIO "Prezzo"
#define SCONTO "Sconto"
#define IMPORTO "Importo"
#define IVA "Iva"
#define SPESE_DI_TRASPORTO "Spese di trasporto"
#define SPESE_DI_INCASSO "Spese di incasso"
#define DETTAGLIO_IVA "C.IVA"
#define IMPONIBILE "Imponibile"
#define IVA_PERC "Aliquota"
#define IMPOSTA "Imposta"
#define TOTALE "Totale"
#define CONTRIBUTO "Contributo conai assolto ove dovuto"
#define SCADENZE "Scadenze"
#define DATA_SC "Data"
#define BANCA "Banca"
#define IBAN "Iban"
#define ASPETTO_ESTERIORE "Aspetto esteriore dei beni"
#define NUMERO_COLLI "Numero colli"
#define PESO_LORDO "Peso lordo"
#define PESO_NETTO "Peso netto"
#define CONSEGNA "Consegna o inizo trasporto"
#define DATA_ORA_DDT "Data e Ora"
#define GENERALITA "Generalità del conducente"
#define VETTORE "Vettore"
#define FIRMA_VETTORE "Firma vettore"
#define PORTO "Porto"
#define FIRMA_COND "Firma coducente"
#define FIRMA_DEST "Firma destinatario"
#define PRODOTTO "Prodotto"
#define BARCODE "Barcode"
/********************************************/
/********Definizizioni sistemi operativi*****/
#define WINDOWS_XP "Windows Xp"
#define WINDOWS_VISTA "Windows Vista"
#define WINDOWS_SEVEN "Windows 7"
#define WINDOWS_8 "Windows 8"
#define WINDOWS_8_1 "Windows 8.1"
#define WINDOWS_10 "Windows 10"
#define _WINNT "OS basato su NT"
/*********************************************/
/***Definizione dei colori per il debug*******/
#define RESET  "\x1B[0m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYANO  "\x1B[36m"
#define WHITE  "\x1B[37m"

#define FRED(x) RED x RESET
#define FGRN(x) GREEN x RESET
#define FYEL(x) YELLOW x RESET
#define FBLU(x) BLUE x RESET
#define FMAG(x) MAGENTA x RESET
#define FCYN(x) CYANO x RESET
#define FWHT(x) WHITE x RESET

#define BOLD(x) "\x1B[1m" x RESET
#define UNDL(x) "\x1B[4m" x RESET
                
namespace Defines {

/*
 * Istanzio gli errori
 */

enum SOFT_LOG{
    LOG_ERROR_ALL,
    LOG_WARNING_ALL,
    LOG_INFO_ALL,
    LOG_DEFAULT_ALL
};

//Visualizzo i sistemi utilizzati
QString getSystemInfo();

//Visualizzo la ram installata e utilizzata
QString getSystemRam();

/*
 * Converto stringa base64 a stringa
 */
QString Base64ToAscii(QString String);

/*
 * Scrivo il log del file
 */
void LOG_FILE(QString linea ,const SOFT_LOG soft, QIODevice::OpenModeFlag flags);


//Leggo cartella database
QString GetCartellaDatabase();

//Lancio i comandi mysql
QString GetRunCommandMySql();

//Verifico il codice fiscale
QString getVerificaCodiceFiscale(QString code,QLabel *label1);

//Verifico la partita iva
QString getPartitaIva(QString code, QLabel *label);

//Estraggo i file
void extractFileZip(QString zipFile, QString destDir);

//Lancio i comandi da terminale
QString getLineFromCommandOutput( QString command );

//Cartella configurazione
QString cartellaDoc();

//Imposta al centro una finestra
static QWindow *windowFromWidget(const QWidget *widget)
{
    QWindow *windowHandle = widget->windowHandle();
    if (windowHandle) {
        return windowHandle;
    }

    const QWidget *nativeParent = widget->nativeParentWidget();
    if (nativeParent) {
        return nativeParent->windowHandle();
    }

    return nullptr;
}

static QScreen *screenFromWidget(const QWidget *widget)
{
    const QWindow *windowHandle = windowFromWidget(widget);
    if (windowHandle && windowHandle->screen()) {
        return windowHandle->screen();
    }

    return QGuiApplication::primaryScreen();
}

void esportaCSV(QString m_tarns, QTableView *table, QSqlQueryModel *model);

void esportaExcelDefine(QString m_trans, QSqlQueryModel *model);
};


#endif // DEFINES_H
