#ifndef ORDINE_RIGHE_H
#define ORDINE_RIGHE_H

#include <QDialog>
#include <QtSql>
#include "ui_ordine_righe.h"

//Importo excel
#include "../xlsx/xlsxdocument.h"
#include "../xlsx/xlsxformat.h"

using namespace std;
using namespace QXlsx;

namespace Ui {
class ordine_righe;
}

class ordine_righe : public QDialog, public Ui::ordine_righe
{
    Q_OBJECT
    
public:
    explicit ordine_righe(QWidget *parent = 0);
    ~ordine_righe();
    QSqlRelationalTableModel *mod_grid;
    
public slots:
    void formatta_num();
    void cliente_add();
    void open_cliente();
    void open_fornitore();
    void combo_fornitore();
    int open_cl(int index);
    int open_fl(int index);
    void filtro();
    void pagamento_ord();
    void modifica_riga();
    void modifica_riga_one(QModelIndex index);
    void elimina_riga();
    void elimina();
    void sconto_1t(const QString &);
    void sconto_2t(const QString &);
    void sconto_3t(const QString &);
    void spesa_trasp(const QString &);
    void spesa_in(const QString &);
    void ord_aggiorna();
    void tot_imp_iva();
    void totale_impo_iva();
    void ord_inserisci();
    void lista_prodotti();
    void insert_f_prodotti();
    void update_f_prodotti();
    void clickgrid();
    void ord_riga_open();
    void chiudi();
    //Causali di trasporto
    void trasp_causali();
    //Porto
    void porto();
    //Aspetto dei beni
    void aspetto_beni();
    //Consegna
    void consegna_trasp();
    //Combo listino
    void listino();
    //Importo righe da file excel
    void importa();
    void import_file_excel();
    void aggiorna_file_excel(QString doc);
    //Aggiungo riga da tabella
    void addRow();
    bool InserisciRiga();
    void submit();
    //Aggiungo righe
    void insert_scarico_prod();
    void update_articoli(double qua, double _p_siva, double _p_civa, double _tot);
    void update_carico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    void update_scarico_prod( double ca, double _p_siva, double _p_civa, double _tot  );
    //recupero dati dalla QTableView
    QString getORDID();
    QString getCodArt();
    QString getCodBarre();
    QString getNomeArticolo();
    QString getDesc();
    QString getUnita();
    QString getPrezzoUnit();
    QString getPrezzoSIVA();
    QString getPrezzoCIVA();
    QString getIVA();
    QString getQuantita();
    QString getSconto();
    QString getTotale();
    QString getAnnoIns();
    QString getListino();
    QString getCateg();
    QString getDataInsert();
    QString getDataLotto();
    QString getNumeroLotto();

private:
    QLocale *local_settings;
    bool found;
    double prezzos_iva,prezzo_c_iva,perc,totale;

protected:
    bool eventFilter(QObject *o, QEvent *j);
    
signals:
    void salva_ord();
};

#endif // ORDINE_RIGHE_H
