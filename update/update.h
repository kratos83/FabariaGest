#ifndef UPDATE_H
#define UPDATE_H

#include <QDialog>
#include <QtGui>
#include <QFile>
#include <QObject>
#include <QQueue>
#include <QTime>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "ui_update.h"

namespace Ui {
class update;
}

class update : public QDialog, public Ui::update
{
    Q_OBJECT
    
public:
    explicit update(QWidget *parent = 0);
    update(QWidget *wig,QString url, QString agg);
    ~update();
    QString txts;
    QString url_up;
private:
    QNetworkAccessManager *manager;
    QNetworkReply *currentDownload;
    QNetworkRequest mCurrentRequest;
    QFile *output;
    QTime downloadTime;
    QProcess *lin_start,*win_start, *mac_start, *unix_start;

    int downloadedCount;
    int totalCount;
    int sizepause;

    bool stop,riavvio;
public slots:
    QString download(QString av_dw);
    void startNextDownload(QNetworkRequest& request);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void downloadFinished();
    void downloadReadyRead();
    void pause();
    void install_package();
    void download();
    void display_progress_bar();

protected:
    void changeEvent(QEvent *e);
signals:
    void finished();
};

#endif // UPDATE_H
