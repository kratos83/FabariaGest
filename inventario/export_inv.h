#include <QtSql/QtSql>
#include <QPainter>
#include <QDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QLocale>
#include <QModelIndex>

#ifndef EXPORT_INV_H
#define EXPORT_INV_H
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
#define DATA "Data inventario"
#define ANNO "Anno"

class export_inv
{
public:
    export_inv();

    void stampa_inventario(QPrinter *printer, QSqlQueryModel *dataset);
    void pagina_inventario(int pagNo);
    QPrinter *printer;
    QPainter *painter;
    QLocale *local_settings;
    QPrintDialog *stampaDlg;
    QDir *settingsDir, *profDir;
    QSettings *general;
};

#endif // EXPORT_INV_H
