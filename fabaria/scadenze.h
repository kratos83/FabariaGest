#ifndef SCADENZE_H
#define SCADENZE_H

#include <QDialog>
#include <QtSql>
#include <QtCore>
#include <QtGui>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>
#include "ui_scadenze.h"

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

namespace Ui {
class scadenze;
}

class scadenze : public QDialog, public Ui::scadenze
{
    Q_OBJECT
    
public:
    explicit scadenze(QWidget *parent = 0);
    ~scadenze();

public slots:
    void lista();
    void lista_comp();
    void check_list();
    void clickgrid();
    void aggiorna();
    void elimina();
    void cerca();
    void lista_fatture_pagate();
    void clicca_fatture_pagate();
    void cerca_fatture(const QString &);
    void filtro();
    void agg_riavvia_count();
    void el_riavvia_count();
    void scadenza_m_precedente();
    void scadenza_m_successivo();
    void scadenza_a_corrente();
    void situazione_fornitori();
    void situazione_clienti();
    void anteprima_di_stampa();
    void stampa(QPrinter *print);
    void stampa_scadenze();
    void esporta_pdf_scadenze();
    void esporta_excel_scadenze();
    
private:
    QSqlRelationalTableModel *model;
    QLocale *local_settings;

protected:

signals:
    void riavvia();
};

#endif // SCADENZE_H
