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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
  Definizioni delle classi FabariaGest
  */
#include "ui_mainwindow.h"
#include "../analogclock/analogclock.h"
#include "../log_man/license.h"
#include "../initdb/update_db.h"
#include "about.h"
#include "esci.h"
#include "user.h"
#include "articoli.h"
#include "categoria.h"
#include "classe.h"
#include "pref.h"
#include "cod_fisc.h"
#include "pag.h"
#include "causali.h"
#include "iva.h"
#include "azienda.h"
#include "banche.h"
#include "fatt_acq.h"
#include "fornitori.h"
#include "carico_mag.h"
#include "scarico_mag.h"
#include "misure.h"
#include "scadenze.h"
#include "fattura.h"
#include "distinta_riba.h"
#include "chiudi_anno_contabile.h"
#include "imposta_anno.h"
#include "preventivi.h"
#include "listino.h"
#include "ordine.h"
#include "ddt.h"
#include "settingsmanager.h"
#include "lista_invio_mail.h"
#include "esistenza_all.h"
#include "reg_iva.h"
#include "agenti.h"
#include "riep_agent.h"
#include "inputdialogcombo.h"
#include "magazzino.h"
#include "selezionamag.h"
/*
  Definizioni delle librerie QT
  */

#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets>
#include <QMdiArea>
#include <QtGui>
#include <QtCore>
#include <QProcess>
#include <QMainWindow>
#include <QPluginLoader>
#include <QDir>
#include <QPluginLoader>
#include <QSystemTrayIcon>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void load_style(const QString &sheetName);
    void enable_menu();
    void disable_menu();
    void ex();
    void setVisible(bool visible);
    void createActions();
    void createTrayIcon();
    QMdiArea *mdiArea;
    QString versione;
    QSystemTrayIcon *trayIcon;
    classe *cl1,*cl2,*cl3,*cl4,*cl5,*label_donate;
    about *aboutUi;
    esci *chiudi;
    user *userUi;
    articoli *art;
    porto *po;
    pref *sh1;
    license *lic;
    AnalogClock *cl;
    QDockWidget *dock;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *impo;
    QAction *quitAction;
    QAction *ag_tray;
    QComboBox *box;
    QMenu *trayMenu;
    cod_fisc *fisc;
    pag *pagamento;
    causali *caus;
    iva *iv;
    azienda *azUi;
    banche *banca;
    fatt_acq *fatt;
    fornitori *forn;
    carico_mag *car_mag;
    misure *un;
    scadenze *sc_fatt;
    fattura *fatt_v;
    distinta_riba *riba;
    chiudi_anno_contabile *anno_cont;
    imposta_anno *sel_anno;
    preventivi *prev;
    listino *listino_gen;
    ordine *ord_acq_ven;
    ddt *m_ddt;
    update_db *agg_db;
    lista_invio_mail *lista_email;
    esistenza_all *es_all;
    reg_iva *iva_reg;
    agenti *prov_agent;
    riep_agent *riepilogo_agenti;
    InputDialogCombo *m_input;
    Magazzino *m_magazzino;
    SelezionaMag *m_mag;
    
protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *e);
    bool loadImPlugin( QObject* );
    bool unloadImPlugin( QObject* );

private:
    QDir *pluginDir;
    QPluginLoader *loader;
    QMap<QPushButton*,QObject*> rimuovi_bottoni;

public slots:
        void onclose();
        void onactionAbout();
        void onarticoli();
        void createStatusBar();
        void onactiondatabasetriggered();
        void onclienti();
        void titolo(QString testo);
        void categorie();
        void settings();
        void interfaccia();
        void onwid();
        void read();
        void messageClicked(QSystemTrayIcon::ActivationReason reason);
        void reload_all();
        void reload_data();
        void reload_lista_cmq();
        void reload_data_anno();
        void reload_data_mese_succ();
        void reload_data_mese_prec();
        void core();
        void gestioneplugin();
        QString getLineFromCommandOutput( QString command );
        void agg_automatico();
        void dis_agg_automatico();
        void avvio_licenza();
        void orologio();
        void gest_pagamento();
        void gest_causali();
        void gest_codfisc();
        void gest_iva();
        void azienda_cod();
        void azienda_ok();
        void gest_banca();
        void gest_fatt_acq();
        void gest_fornitori();
        void gest_fattura_v();
        void populateMenus(QObject *plugin);
        void removePopulateMenus(QObject *plugin);
        void addToMenu(QObject *plugin, QIcon ico, const QString &name, QMenu *menu_add,
                       const char *member);
        void addToButton(QObject *plugin, QIcon ico, const QString &name, QWidget *wid_add,
                         const char *member);
        void removeToButton(QObject *plugin, QIcon ico, const QString &name, QWidget *wid_add,
                            const char *member);
        void gest_misure();
        void vis_carico_magazzino();
        void vis_scarico_magazzino();
        void installa_plugin();
        void gest_scadenze();
        void gest_scadenze_1();
        void gest_scadenze_2();
        void gest_scadenze_3();
        void gest_scadenze_4();
        void gest_dist_riba();
        void gest_anno_cont();
        void gest_sel_anno();
        void gest_preventivi();
        void gest_listini();
        void gest_ordini();
        void gest_ddt();
        void scrivi_posizione();
        void leggi_posizione();
        void aggiorna_database();
        void conf_link();
        void open_link();
        void vis_storico_mail();
        void gest_update();
        void gest_esistenza();
        void gest_registro_iva();
        void gest_agenti();
        void gest_riep_agenti();
        void gest_dropbox();
        void animationIn(QWidget *wid);
        void selAzienda();
        void createMagazzino();
        void setMagazzino();
        
protected slots:
        void pluginLoad(QObject *plugin, QTreeWidgetItem *item);
        void pluginUnload(QObject *plugin, QTreeWidgetItem *item);
        void avvia_widget();
        void avvia_button_widget(QObject *plugin);
        void avvia_backup();
        void moveEvent(QMoveEvent *);
        void resizeEvent(QResizeEvent *);

Q_SIGNALS:
        void pluginLoaded(const QString&, bool);
};

extern MainWindow *form;
#endif
