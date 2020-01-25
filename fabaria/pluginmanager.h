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
#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QBuffer>
#include <QFile>
#include <QtWidgets>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui>
#include <QVector>
#include <QtSql>
#include "details_plugin.h"
#include "settingsmanager.h"
#include "ui_pluginmanager.h"

class QDir;
class QPluginLoader;

namespace Ui {
class pluginmanager;
}

class pluginmanager : public QDialog, public Ui::pluginmanager
{
    Q_OBJECT
    
public:
    explicit pluginmanager(QWidget *parent = 0);
    ~pluginmanager();

signals:
    void pluginLoad( QObject*, QTreeWidgetItem* );
    void pluginUnload( QObject*, QTreeWidgetItem* );
    void pluginLoader(const QString&, bool);
    void finished();

public slots:
    void pluginLoaded( const QString&, bool );
    void leggiplugin();
    void add_details();
    void update_plugin();
    void self_update(QString namePlugin);
    void self_update_parse(QNetworkReply *reply);
    void startNextDownload(QNetworkRequest& request);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void downloadFinished();
    void downloadReadyRead();
    void install_package();
    void download(QString url, QString pkg);
    void display_progress_bar();

protected slots:
    void readPluginInfo(QTreeWidgetItem*);
    void pluginDoubleClicked(QTreeWidgetItem*,int);

private:
    QDir *pluginDir;
    QPluginLoader *loader;
    QStringList pluginList;
    QStringList loadedList;
    QList<QPluginLoader*> pluginLoaders;
    details_plugin *dettagli;
    // Self Update
    QBuffer xmldata;
    int getRequestId;
    int filedownloaded;
    QString latest_version;
    QString m_versione,package;
    QVector<QString> filelist; // The list of files to be downloaded
    QFile *file;
    QNetworkAccessManager *m_manager;
    SettingsManager *settings;
    QProcess *process,*lin_start,*win_start, *mac_start, *unix_start;
    QNetworkReply *currentDownload;
    QNetworkRequest mCurrentRequest;
    QFile *output;
    QTime downloadTime;
    int downloadedCount;
    int totalCount;
};

extern pluginmanager *manager;

class ProgressBarDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    inline ProgressBarDelegate(QObject *plugin = 0) : QItemDelegate(plugin){}
    inline void paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const
{
        if(index.column() == 3){
	 QProgressBar progressbar;
         // Set up a QStyleOptionProgressBar to precisely mimic the
         // environment of a progress bar.
         QStyleOptionProgressBar progressBarOption;
         
         progressBarOption.state = QStyle::State_Enabled;
         progressBarOption.direction = QApplication::layoutDirection();
         progressBarOption.rect =option.rect;
         progressBarOption.fontMetrics = QApplication::fontMetrics();
  
         progressBarOption.type =  QStyleOption::SO_ProgressBar;
         progressBarOption.version = 2 ;
         
         progressBarOption.minimum = 0;
         progressBarOption.maximum = 100;
         progressBarOption.textAlignment = Qt::AlignCenter;       
         int progress = index.data(Qt::DisplayRole).toInt(); 
          
         // Set the progress and text values of the style option.
         progressBarOption.progress = progress < 0 ? 0 : progress;
         progressBarOption.text = QString("%1%").arg(progressBarOption.progress);
	 progressBarOption.textVisible = true; 
	 painter->save();
         // Draw the progress bar onto the view.
         QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter,0);
	 painter->restore();
	}
	else{
	 QItemDelegate::paint(painter, option, index);
	}
     }
};
#endif // PLUGINMANAGER_H
