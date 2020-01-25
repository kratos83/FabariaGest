#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <QDialog>
#include <QtGui>
#include <QFileDialog>
#if QT_VERSION < 0x050000
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QAbstractPrintDialog>
#else
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#endif

namespace Ui {
class inventario;
}

class inventario : public QDialog
{
    Q_OBJECT
    
public:
    explicit inventario(QWidget *parent = 0);
    ~inventario();
    
private:
    Ui::inventario *ui;
public slots:
    void stampa_inv();
    void esporta_pdf();
    void preview_prev(QPrinter *printer);
};

#endif // INVENTARIO_H
