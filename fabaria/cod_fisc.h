/****************************************************************************
**
** Copyright (C) 2006-2018 Angelo e Calogero Scarna
** Contact: Angelo Scarnà (angelo.scarna@codelinsoft.it)
**          Calogero Scarnà (calogero.scarna@codelinsoft.it)
**          Team Codelinsoft (info@codelinsoft.it)
**
** Partial of code
 * author 2007 Francesco Betti Sorbelli <francesco.betti.sorbelli@gmail.com>
 *
 *
** This file is part of the project CodiceFiscale
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
#ifndef COD_FISC_H
#define COD_FISC_H
/*!
 * \author 2014 Codelinsoft <info@codelinsoft.it>
 *
 * Partial of code
 * \author 2007 Francesco Betti Sorbelli <francesco.betti.sorbelli@gmail.com>
 */
#include "qglobal.h"
#include <QtWidgets/QMainWindow>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>
#include <QDate>
#include <QPainter>


/*!
 * Il namespace Ui, serve a visualizzare
 * l'interfaccia creata da qtcreator
 *
 * \brief <b>namespace Ui{};</b>
 * \param class cod_fisc;
 */
namespace Ui {
    class cod_fisc;
}

/*!
 * La classe cod_fisc estende la classe QMainWindow
 * e eredita tutti gli oggetti.
 *\brief <b>class cod_fisc: public QMainWindow</b>
 *\param QMainWindow
 */
class cod_fisc : public QMainWindow  {

    /*!
     * Macro Qt
     *
     *
     * \brief Q_OBJECT
     */
    Q_OBJECT


public:
    /*!
     * Costruttore <b>cod_fisc(QWidget *parent = 0);</b>
     *
     * Questa classe visualizza il calcolo del codice fiscale,
     * la veridicità del medesimo, la veridicità della partita
     * iva, ecc.
     *
     *
     *
     * \brief <b>cod_fisc(QWidget *parent = 0);</b>
     * \param parent = 0
     */
    cod_fisc(QWidget *parent = 0);

    //!Distruttore  ~cod_fisc();
     /*!
       Il distruttore  ~cod_fisc(); serve a chiudere
       gli eventuali processi attivi.
     */
    ~cod_fisc();

        /*!
         * La funzione calculateCognome(); serve a calcolare il
         * cognome leggendo solo tre lettere.
         *
         *\brief <b>void calculateCognome();</b>
         */
        void calculateCognome();

        /*!
         * La funzione calculateNome(); serve a calcolare il
         * cognome leggendo solo tre lettere.
         *
         *\brief <b>void calculateNome();</b>
         */
        void calculateNome();

        /*!
         * La funzione calculateData(); serve a calcolare la
         * data leggendo la data di nascita.
         *
         *\brief <b>void calculateData();</b>
         */
        void calculateData();

        /*!
         * La funzione calculateComune(); serve a trovare il codice
         * di appartenenza del comune selezionato.
         *
         *\brief <b>void calculateComune();</b>
         */
        void calculateComune();

        /*!
         * La funzione calculateStato(); serve a trovare il codice
         * di appartenenza lo stato selezionato.
         *
         *\brief <b>void calculateStato();</b>
         */
        void calculateStato();

         /*!
         * Calcola la lettera che serve a completare
         * il codice fiscale mediante controllo
         * delle funzioni getEvenCode(QString) se il risultato
         * è pari e getOddCode(QString) se il risultato è dispari.
         *
         *\brief <b>void calculateLetteraControllo();</b>
         */
        void calculateLetteraControllo();

        /*!
         * Calcola codice fiscale mediante la lettura
         * dei 5 blocchi.
         * \brief <b>void merge();</b>
         */
        void merge();

        /*!
        * Controlla se esiste una vocale e se trova uno spazio
        * ritorna -1, invece se trova una consonante ritorna 0,
        * altrimenti 1  se è una vocale.
        *
        * \brief <b>int checkVocale(QString);</b>
        * \return QString v;-------------[<b>Il parametro v ritorna uno spazio se -1, invece se trova una consonante ritorna 0,
        *                                    altrimenti 1  se è una vocale.</b>]
        */
        int checkVocale(QString v);

        /*!
         * La funzione getEvenCode(QString) ritorna un numero
         * dispari se (i%2 != 1).
         *
         *\brief <b>int getOddCode(QString n);</b>
         *\return -1;-------------------[<b>Ritorna uno spazio</b>]
         */
        int getOddCode(QString c);

        /*!
         * La funzione getEvenCode(QString) ritorna un numero
         * pari se (i%2 = 1).
         *
         *\brief <b>int getEvenCode(QString n);</b>
         *\return -1;-------------------[<b>Ritorna uno spazio</b>]
         */
        int getEvenCode(QString n);

        /*!
         * La funzione mapNumber(int) trova l'ultima lettera
         * per poter completare il codice fiscale.
         *
         *\brief <b>QString mapNumber(int);</b>
         *\param int n;
         *
         *\return "_"
         */
        QString mapNumber(int n);

        /*!
         * La funzione checkInput() serve a visualizzare
         * una finestra di errore se non viene digitato niente
         * all'interno delle QLineEdit.
         *
         *\brief <b>bool checkInput();</b>
         */
        bool checkInput();

private slots:

    /*!
     * La funzione on_esci_clicked() serve a dire al programma di uscire
     * dal calcolo del codice fiscale.
     *
     *\brief <b>void on_esci_clicked();</b>
     */
    void on_esci_clicked();

    /*!
     * La funzione on_pushButtonCalcola_clicked() serve a dire al programma di
     * calcolare il codice fiscale.
     *
     *\brief <b>void on_pushButtonCalcola_clicked();</b>
     */
    void on_pushButtonCalcola_clicked();

    /*!
     * La funzione on_pushButtonCancella_clicked() serve a cancella ciò che è scritto
     * all'interno delle QLineEdit.
     *
     *\brief <b>void on_pushButtonCancella_clicked();</b>
     */
    void on_pushButtonCancella_clicked();

    /*!
     * La funzione initComuni() serve a visualizzare i comuni all'interno della ComboBox.
     * All'interno troviamo i comuni italiani ma acnhe quelli esteri.
     *
     *\brief <b>void initComuni();</b>
     */
    void initComuni();

    /*!
     * La funzione calcola_provincia(); serve a calcolare la
     * provincia di nascita.
     *
     *\brief <b>void calcola_provincia();</b>
     */
    void calcola_provincia();

    /*!
     * La funzione calcola_pr_st(); serve a calcolare la
     * provincia di nascita estera.
     *
     *\brief <b>void calcola_pr_st();<b>
     */
    void calcola_pr_st();

    /*!
     * La funzione initStati() serve a visualizzare gli stati all'interno della ComboBox.
     * All'interno troviamo gli stati esteri.
     *
     *\brief <b>void initStati();</b>
     */
    void initStati();

    /*!
     * La funzione vis_cod_fisc(bool) serve a visualizzre l'anteprima della tessera
     * sanitaria.
     *
     *\brief <b>int vis_cod_fisc(bool click)</b>;
     *\param bool click;--------------[<b>Il parametro click serve a visualizzare l'anteprima del codice fiscale</b>]
     *\retun click;
     */
    bool vis_cod_fisc(bool click);

    /*!
     * La funzione stampa_codice() serve a stampare il codice fiscale sulla tessera
     * sanitaria.
     *
     *\brief <b>void stampa_codice();</b>
     */
    void stampa_codice();

    /*!
     * La funzione anteprima(Qprinter *printer) visualizza
     * un'anteprima del file da stampare.
     * \brief <b>void anteprima(QPrinter *printer);</b>
     * \param QPrinter *printer;
     *
     *
     * Il paramatro printer serve a poter visualizzare
     * la stampa grazie alla classe QPrinter,QPainter.
     */
    void anteprima(QPrinter *printer);

    /*!
     * La funzione exp_pdf() serve a esportare la tessera sanitaria in formato
     * pdf.
     *
     *\brief <b>void exp_pdf();</b>
     */
    void exp_pdf();

    /*!
     * La funzione exp_img() serve a esportare la tessera sanitaria in formato
     * png.
     *
     *\brief <b>void exp_img();</b>
     */
    void exp_img();

    /*!
     * La funzione sel_combo_box(int) serve a selezionare la possibilità di scegliere
     * se calcolare il codice fiscale italiano o estero.
     *
     * \brief <b>void sel_combo_box();</b>
     */
    void sel_combo_box();

    /*!
     * Visualizza i dati nella schermata della tessera
     * sanitaria.
     *
     * Risolto bug uso intensivo cpu.
     *
     * \brief void disegna(QPainter *painter);
     */
    void disegna(QPainter *painter);

    /*!
     * Aggiorna i dati indolore nella schermata
     * della tessera sanitaria
     *
     * \brief void aggiorna(QString code);
     */
    void aggiorna();

    /*!
     * Avvio segnali per l'aggiornamento dell'anteprima
     * della tessera sanitaria.
     *
     * \brief void interfaccia_signal();
     */
    void interfaccia_signal();

private:
    /*!
     * Ui::cod_fisc *ui è l'interfaccia realizzata da qtcreator
     * mediante ui.
     *
     * \brief <b>Ui::cod_fisc *ui</b>
     * \param ui;
     */
    Ui::cod_fisc *ui;

    /*!
     * Serve a inizializzare la data
     *
     * \brief <b>QDate today</b>
     */
    QDate today;

    /*!
     * Serve visualizzare il cognome
     *
     * \brief <b>QString cognome</b>
     */
    QString cognome;

    /*!
     * Serve a visualizzare il nome
     *
     * \brief <b>QString nome</b>
     */
    QString nome;

    /*!
     * Serve a scegliere il sesso
     *
     * \brief <b>bool sesso</b>
     */
    bool sesso;

    /*!
     * Serve a visualizzare il comune
     *
     * \brief <b>QString comune</b>
     */
    QString comune;

    /*!
     * Serve a visualizzare il giorno
     * \brief <b>int dataGiorno</b>
     */
    int dataGiorno;

    /*!
     * Serve a visualizzare il mese
     * \brief <b>int dataMese</b>
     */
    int dataMese;

    /*!
     * Serve a visualizzare l'anno
     * \brief <b>int dataAnno</b>
     */
    int dataAnno;

    /*!
     * Serve a visualizzare il blocco 1
     * per la prima parte del codice fiscale.
     *
     * \brief <b>QString block1</b>
     */
    QString block1;

    /*!
     * Serve a visualizzare il blocco 2
     * per la seconda parte del codice fiscale.
     *
     *\brief <b>QString block2</b>
     */
    QString block2;

    /*!
     * Serve a visualizzare il blocco 3
     * per la terza parte del codice fiscale.
     *
     * \brief <b>QString block3</b>
     */
    QString block3;

    /*!
     * Serve a visualizzare il blocco 4
     * per la quarta parte del codice fiscale.
     *
     * \brief <b>QString block4</b>
     */
    QString block4;

    /*!
     * Serve a visualizzare il blocco 5
     * per l'ultima parte del codice fiscale.
     *
     * \brief <b>QString block5</b>
     */
    QString block5;

    /*!
     * Serve a visualizzre il codice fiscale intero.
     *
     * \brief <b>QString CF</b>
     */
    QString CF;

    /*!
     * Serve a visualizzare la provincia di nascita
     *
     * \brief <b>QString prov</b>
     */
    QString prov;

    /*!
     * Serve a visualizzare la provincia estera
     *
     * \brief <b>QString pr_st</b>
     */
    QString pr_st;

    /*!
     * Serve a visualizzare l'immagine del codice fiscale.
     * \brief <b>QImage img</b>
     */
    QImage img;

    /*!
     * Serve a disegnare gli oggetti presenti nell'area della tessere sanitaria.
     *
     * \brief <b>QPainter *painter;</b>
     */
    QPainter *painter;

protected:
    /*!
     * La funzione paintEvent(QPaintEvent *event) serve disegnare
     * il codice fiscale in immagine.
     *
     * \brief <b>paintEvent(QPaintEvent *event);
     *\param QPaintEvent *event;------------------[<b>Il parametro event ritorna un evento nel widget desiderato.</b>]
     */
    void paintEvent(QPaintEvent *event);
};

#endif // COD_FISC_H
