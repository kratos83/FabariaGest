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
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QProgressBar>
#include <QtGui>
#include <QFile>
#include <QObject>
#include <QQueue>
#include <QTime>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class progressbar : public QStyledItemDelegate
{
    Q_OBJECT
public:
    progressbar(int colonna, QWidget* = nullptr);
    progressbar(int colonna, QString url, QString agg, QWidget* = nullptr);
    ~progressbar();

    void startNextDownload(QNetworkRequest& request);
    void install_package();
    void download();
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    static bool setItem( QTreeWidgetItem* item, int column, int progress );
    static void initItem( QTreeWidgetItem* item, int column, int total );
    int downloadedCount;
    qint64 byteRicevuti, byteTotali;
private:
    QString txts;
    QString url_up;
    QNetworkAccessManager *manager;
    QNetworkReply *currentDownload;
    QNetworkRequest mCurrentRequest;
    QFile *output;
    QTime downloadTime;
    QProcess *lin_start,*win_start, *mac_start, *unix_start;
    int totalCount;
    
protected:
    int m_colonna;
    QProgressBar m_bar;

public slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void downloadFinished();
    void downloadReadyRead();
    void display_progress_bar();
signals:
    void finished();
};

class Progress
{
	public:
		Progress() : progress(0), total(0) {}
		Progress(int p, int t ) : progress(p), total(t) {}
		int percent() const
		{
			return static_cast< int >(
					( static_cast<float>( progress ) / static_cast<float>( total ) ) * 100 );
		}
		bool isFinished() const
		{
			return progress == total;
		}
		int progress;
		int total;
};
Q_DECLARE_METATYPE( Progress );
#endif // PROGRESSBAR_H
