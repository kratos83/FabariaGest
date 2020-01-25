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
#include "progressbar.h"
#include <QByteArray>
#include <stdio.h>
#include <iostream>

using namespace std;

progressbar::progressbar(int colonna, QWidget*)
: QStyledItemDelegate(), m_colonna(colonna)
{
}


progressbar::progressbar(int colonna, QString url, QString agg, QWidget *p)
: QStyledItemDelegate(p), m_colonna(colonna)
{
  url_up = url;
  txts = agg;
  manager = new QNetworkAccessManager(this);
  download();
}

progressbar::~progressbar()
{
}

void progressbar::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if( index.column() == m_colonna ){
		auto p = index.data().value<Progress>();
		if( p.total > 0 ){
			int percent = p.percent();

			QStyleOptionProgressBar progressBarOption;
			progressBarOption.initFrom( &m_bar );
			progressBarOption.rect = option.rect;
			progressBarOption.minimum = 0;
			progressBarOption.maximum = 100;
			progressBarOption.progress = percent;
			progressBarOption.text = QString::number( percent ) + "%";
			progressBarOption.textVisible = true;

			painter->save();
			QStyle *drawStyle = m_bar.style();
			drawStyle->drawControl( QStyle::CE_ProgressBar, &progressBarOption, painter, &m_bar );
			painter->restore();
			return;
		}
	}
    QStyledItemDelegate::paint(painter, option, index);
}

void progressbar::initItem(QTreeWidgetItem* item, int column, int total)
{
    QVariant v;
    v.setValue<Progress>( Progress( 0, total ) );
    item->setData( column, Qt::DisplayRole, v );
}

bool progressbar::setItem(QTreeWidgetItem* item, int column, int progress)
{
	auto v = item->data( column, Qt::DisplayRole );
	auto p = v.value<Progress>();

	p.progress = progress;
	if( p.progress > p.total ){
		p.progress = p.total;
	}
	v.setValue<Progress>( p );
	item->setData( column, Qt::DisplayRole, v );
	return p.isFinished();
}

void progressbar::download()
{
  QProcess *lin_start = new QProcess(this);
#if defined (Q_OS_LINUX)
    lin_start->start("pkexec chmod 777 /opt/fabaria_gest");
#elif defined (Q_OS_FREEBSD)
    lin_start->start("pkexec chmod 777 /opt/fabaria_gest");
#endif
    QUrl url(url_up+txts);
    QString filename = QFileInfo(url.path()).fileName();
    mCurrentRequest = QNetworkRequest(url);
    output = new QFile(filename+".part");
    output->open(QIODevice::ReadWrite);
    startNextDownload(mCurrentRequest);
}

void progressbar::startNextDownload(QNetworkRequest& request)
{
	currentDownload = manager->get(request);

        connect(currentDownload, SIGNAL(finished()),SLOT(downloadFinished()));
        connect(currentDownload, SIGNAL(readyRead()),SLOT(downloadReadyRead()));
        connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),SLOT(downloadProgress(qint64,qint64)));
        connect(currentDownload, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));


        // prepare the output
        downloadTime.start();
}

void progressbar::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    byteRicevuti = bytesReceived;
    byteTotali = bytesTotal;
    QVariant v;
    v.setValue<Progress>( Progress( byteRicevuti, byteTotali ) );
}

void progressbar::downloadFinished()
{
		output->close();
                QUrl url(txts);
                QString filename = QFileInfo(url.path()).fileName();
                QDir *direct = new QDir( QCoreApplication::applicationDirPath() );
#if defined(Q_OS_LINUX)
                QString fileNames=direct->currentPath()+"/"+filename+".part";
#elif defined(Q_OS_MACX)
                QString fileNames=direct->currentPath()+"/"+filename+".part";
#elif defined(Q_OS_WIN)
                QString fileNames=direct->currentPath()+"\\"+filename+".part";
#elif defined (Q_OS_FREEBSD)
                QString fileNames=direct->currentPath()+"/"+filename+".part";
#endif
                

                if(currentDownload->error()){
                    output->remove(fileNames);
                }
                else{
                output->rename(fileNames,filename);
                downloadReadyRead();
                ++downloadedCount;
		emit finished();
                install_package();
                }

}

void progressbar::downloadReadyRead()
{
  output->write(currentDownload->readAll());
}

void progressbar::install_package()
{
   QUrl url(txts);
   QString filename = QFileInfo(url.path()).fileName();

   QDir *direct = new QDir( QCoreApplication::applicationDirPath() );
   QString file_dir=direct->currentPath()+"/"+filename;
   QStringList fileNames=direct->entryList( QStringList("*.zip"), QDir::Files, QDir::Name);
   fileNames << filename;
#if defined(Q_OS_LINUX)
    lin_start = new QProcess(this);
    connect(lin_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
    lin_start->start("pkexec unzip -o "+file_dir+" -d /opt/fabaria_gest/");
#elif defined(Q_OS_WIN)
    win_start = new QProcess(this);
    connect(win_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
    win_start->start(QDir::currentPath()+"\\unzip -o "+file_dir+" -d "+QDir::currentPath()+"\\FabariaGest");
#elif defined(Q_OS_MAC64)
    mac_start = new QProcess(this);
    connect(mac_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
    mac_start->start("unzip -o "+file_dir+" -d /Application/FabariaGest/");
#elif defined (Q_OS_FREEBSD)
   unix_start = new QProcess(this);
   connect(unix_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
   unix_start->start("pkexec unzip -o "+file_dir+" -d /opt/FabariaGest/");
#endif
}

void progressbar::error(QNetworkReply::NetworkError code)
{
}

void progressbar::display_progress_bar()
{
#if defined (Q_OS_LINUX)
    int val = lin_start->readLine().toInt();
#elif defined(Q_OS_WIN)
    int val = win_start->readLine().toInt();
#elif defined(Q_OS_MACX)
    int val = mac_start->readLine().toInt();
#elif defined (Q_OS_FREEBSD)
    int val = unix_start->readLine().toInt();
#endif
    QVariant v;
    for(val=0;val <= 100; val++){
        v.setValue<Progress>( Progress( 0, val ) );
    }
}
