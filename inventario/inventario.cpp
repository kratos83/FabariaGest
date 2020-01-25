#include "inventario.h"
#include "ui_inventario.h"
#include "export_inv.h"

inventario::inventario(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inventario)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->print_inv,SIGNAL(clicked()),this,SLOT(stampa_inv()));
    connect(ui->print_pdf,SIGNAL(clicked()),this,SLOT(esporta_pdf()));
}

void inventario::stampa_inv()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer);
    preview.setWindowFlags(Qt::Window);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_prev(QPrinter*)));
    preview.exec();
}

void inventario::preview_prev(QPrinter *printer)
{
    Q_UNUSED(printer);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery("select * from carico_magazzino");

    export_inv *inv = new export_inv();
    inv->stampa_inventario(printer,model);

}

void inventario::esporta_pdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("export_inv","Esporta PDF"),
                                                    "*.pdf", qApp->translate("export_inv","Pdf Files(*.pdf);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".pdf") < 0) {
                fileName += ".pdf";
            }
        }
       QPrinter *printer = new QPrinter(QPrinter::HighResolution);
       Q_UNUSED(printer);
       printer->setPageSize(QPrinter::A4);
       printer->setOrientation(QPrinter::Landscape);
       printer->setOutputFileName(fileName);
       QSqlQueryModel *model = new QSqlQueryModel(this);
       model->setQuery("select * from carico_magazzino");
       export_inv *in = new export_inv();
       in->stampa_inventario(printer,model);
}

inventario::~inventario()
{
    delete ui;
}
