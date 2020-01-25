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
#ifndef PREF_H
#define PREF_H
/******************************
 *Classi pref
 *************************/
#include "ui_pref.h"
#include "settingsmanager.h"
#include "imagescene.h"
#include "image_resize.h"
#include "defines.h"

/******************************
 *Classi QT
 *************************/
#include <QBuffer>
#include <QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui>
#include <QVector>
#include <QtSql>


class QPluginLoader;

using namespace std;

class pref : public QDialog, public Ui::pref
{
    Q_OBJECT
//static const QString VERSION; // Current version

public:
    explicit pref(QWidget *parent = 0);
    void readsettings();
    // Self Update
    QBuffer xmldata;
    int getRequestId;
    int filedownloaded;
    QString latest_version;
    QString versione;
    QVector<QString> filelist; // The list of files to be downloaded
    QFile *file;
    QNetworkAccessManager *manager;
    QCheckBox *box,*box1,*box2;
    QList<QCheckBox*> box3;
    QGraphicsItem * getPixmapItem();
    QListWidgetItem *configButton,*plugins_button,*databaseButton,
		    *emailButton,*updateButton,*fatturaButton,*bancaButton;
    SettingsManager *settings;
    QDir *dir;
    QSqlDatabase db;
    QProcess *process;

protected:
    void changeEvent(QEvent *e);

signals:
    void load_plugins();
    void menu_mod();

private:
    image_resize images;
    void updateDisplay();
    void pdateuButton(int arg1);
    QGraphicsPixmapItem *imagePixmapItem;
    QGraphicsView *graphicsView;
    ImageScene *imageScene;
    QLabel *alt_larg;

public slots:
    void applica();
    void menu_ex();
    void self_update();
    void self_update_parse(QNetworkReply *reply);
    void agg_en();
    void agg_dis();
    void visagg();
    bool dir_server();
    void up_dw(QString package, QString url);
    void apri();
    void save_file();
    void showResizeDialog();
    void resizeSwitch(QSize size, int filter);
    void seleziona_tema(const QString &sheetName);
    void label_file();
    void connetti_database();
    void messaggio();
    void salva_cartella();
    void clicca_per_eliminare();
    void cancella_riga();
    void vis_banca();
    void vis_iva();
     void visEnAuth(bool vero);
};

#endif // PREF_H
