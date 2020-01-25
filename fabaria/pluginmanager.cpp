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
#include "pluginmanager.h"
#include "ui_pluginmanager.h"
#include <QPluginLoader>
#include <QDir>

#include <QDebug>
#include "settingsmanager.h"
#include "mainwindow.h"
#include "luxuryim.h"
#include "defines.h"
#include "databasesql.h"

using namespace Defines;
using namespace DatabaseSql;

pluginmanager *manager =0;

pluginmanager::pluginmanager(QWidget *parent) : QDialog(parent),
  downloadedCount(0), totalCount(0)
{
    if(manager)
        delete manager;
    manager = this;

    setupUi(this);
    settings = new SettingsManager(this);
    m_manager = new QNetworkAccessManager(this);
    //Cambia riga
    connect(treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(readPluginInfo(QTreeWidgetItem*)));
    //Clicca una volta per abilitare o disabilitare un plugin tramite il checkbox
    connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(pluginDoubleClicked(QTreeWidgetItem*,int)));
    //Clicca per abilitare i dettagli dei plugin
    dett_plugin->setShortcut(tr("Ctrl+D"));
    connect(dett_plugin,SIGNAL(clicked()),this,SLOT(add_details()));
    
    //Clicca per aggiornare il plugin
    up_plugin->setShortcut(tr("Ctrl+U"));
    connect(up_plugin,SIGNAL(clicked()),this,SLOT(update_plugin()));

    //Chiudi plugin loader
    chiudi->setShortcut(tr("Ctrl+E"));
    connect(chiudi,SIGNAL(clicked()),this,SLOT(close()));
#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD) || defined Q_OS_WIN
    pluginDir = new QDir( QCoreApplication::applicationDirPath()+"/plugin/" );
#elif defined Q_OS_MACX
    pluginDir = new QDir( QCoreApplication::applicationDirPath()+"/../PlugIns/" );
#endif

    treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
    treeWidget->setItemDelegateForColumn(3, new ProgressBarDelegate());
    treeWidget->setColumnHidden(0,true);
    loader = new QPluginLoader;
    
    dett_plugin->setEnabled(false);
    up_plugin->setEnabled(false);
}

void pluginmanager::leggiplugin(){

#if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD)
QStringList fileNames=pluginDir->entryList( QStringList("*.so"), QDir::Files, QDir::Name);
QString toLoad=getConfigurazioneSql("PluginLoad").toString();
#elif defined Q_OS_WIN
QStringList fileNames=pluginDir->entryList( QStringList("*.dll"), QDir::Files, QDir::Name);
QString toLoad=getConfigurazioneSql("PluginLoad").toString();
#elif defined (Q_OS_MACX)
QStringList fileNames=pluginDir->entryList( QStringList("*.dylib"), QDir::Files, QDir::Name);
QString toLoad=getConfigurazioneSql("PluginLoad").toString();
#endif
    QTreeWidgetItem *item;
    QString cname;

    foreach( QString fileName, fileNames ) {
        loader->setFileName( pluginDir->absoluteFilePath(fileName) );
        QObject *plugin=loader->instance();
        if ( plugin ) {
            cname=QString::fromLatin1( plugin->metaObject()->className() );
            if ( !pluginList.contains( cname ) ) {
                Interface_plugin *plif=qobject_cast<Interface_plugin*>( plugin );
                if(plif) {
                    pluginList.append( cname );
                    pluginLoaders.append(loader);
                    QStringList values;
                    values << fileName << plif->nameplugin() << "" << "" << plif->version() << plif->vendor();
                    item=new QTreeWidgetItem( treeWidget, values );
                    if(toLoad.contains(cname)){
                          item->setText( 2, qApp->translate("pluginmanager","Avviato...") );
			                item->setFlags(item->flags() | Qt::ItemIsUserCheckable); 
			                item->setCheckState(2, Qt::Checked);
                          QString ic = ":/images/dialog-ok-apply.png";
                          QIcon icona(ic);
                          item->setIcon(2,icona);
                              emit pluginLoad( plugin, item );
                    }
                    else{
                        item->setText( 2, qApp->translate("pluginmanager","Disattivato...") );
			            item->setFlags(item->flags() | Qt::ItemIsUserCheckable); 
			            item->setCheckState(2, Qt::Unchecked);
                        QString ic = ":/images/list-remove.png";
                        QIcon icona(ic);
                        item->setIcon(2,icona);
                          emit pluginUnload( plugin, item );
                    }
                }
            }
        }
    }
}

void pluginmanager::readPluginInfo(QTreeWidgetItem *item){

    if(!item)
        return;

    loader->setFileName( pluginDir->absoluteFilePath( item->text(0) ) );
    QObject *plugin=loader->instance();
    if(plugin) {
        Interface_plugin *pi=qobject_cast<Interface_plugin*>( plugin );
        if(!pi)
            return;

        nome->setText(pi->nameplugin());
        autore->setText(pi->vendor());
        versione->setText(pi->version());
        descrizione->setText(pi->description());

        dett_plugin->setEnabled(true);
	up_plugin->setEnabled(true);
    }
}

void pluginmanager::add_details()
{

    if(!treeWidget->currentItem())
        return;

    loader->setFileName( pluginDir->absoluteFilePath( treeWidget->currentItem()->text(0) ) );
    QObject *plugin=loader->instance();
    if(plugin) {
        Interface_plugin *pi=qobject_cast<Interface_plugin*>( plugin );

        if(!pi)
            return;

        dettagli = new details_plugin(this,pi->nameplugin(),treeWidget->currentItem()->text(1),pi->vendor(),
                                      pi->version(),pi->description(),pi->name_menu(),pi->displayName(),
                                      pi->icona().pixmap(24,24));
        dettagli->exec();

    }
}

void pluginmanager::update_plugin()
{
    if(!treeWidget->currentItem())
        return;

    loader->setFileName( pluginDir->absoluteFilePath( treeWidget->currentItem()->text(0) ) );
    QObject *plugin=loader->instance();
    if(plugin) {
        Interface_plugin *pi=qobject_cast<Interface_plugin*>( plugin );

        if(!pi)
            return;
	self_update(QString(plugin->metaObject()->className()));
    }
}

void pluginmanager::pluginDoubleClicked(QTreeWidgetItem *item, int column){

  if(column == 2){
    if(!item)
        return;

    loader->setFileName( pluginDir->absoluteFilePath( item->text(0) ) );
    QObject *plugin = loader->instance();
    if(plugin) {
        QString name=plugin->metaObject()->className();

        if( loadedList.contains(name) )
            emit pluginUnload(plugin,item);
        else
            emit pluginLoad(plugin,item);
    }
  }
}

void pluginmanager::pluginLoaded(const QString &plugin, bool loaded){

    if( loaded )
        loadedList.append( plugin );
    else
       loadedList.removeAll( plugin );
       
    updateConfigurazione("PluginLoad='"+loadedList.join(",")+"'");
}

void pluginmanager::self_update(QString nameplugin){

#if defined(Q_OS_LINUX) && defined(__x86_64__)
    QUrl url("http://www.codelinsoft.it/package/plugin/"+nameplugin+"-lin64.xml");
    m_manager->get(QNetworkRequest(QUrl(url)));
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
    qDebug() << "Indirizzo" << url << "\n" << "Nome plugin" << nameplugin;
#elif defined(Q_OS_LINUX) && defined(__x86)
    QUrl url("http://www.codelinsoft.it/package/plugin/"+nameplugin+"-lin32.xml");
    m_manager->get(QNetworkRequest(QUrl(url)));
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_WIN64)
    QUrl url("http://www.codelinsoft.it/package/plugin/"+nameplugin+"-win64.xml");
    m_manager->get(QNetworkRequest(QUrl(url)));
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_WIN32)
    QUrl url("http://www.codelinsoft.it/package/plugin/"+nameplugin+"-win32.xml");
    m_manager->get(QNetworkRequest(QUrl(url)));
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_MACX)
    QUrl url("http://www.codelinsoft.it/package/plugin/"+nameplugin+"-macx.xml");
    m_manager->get(QNetworkRequest(QUrl(url)));
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#elif defined(Q_OS_FREEBSD)
    QUrl url("http://www.codelinsoft.it/package/plugin/"+nameplugin+"-unix.xml");
    m_manager->get(QNetworkRequest(QUrl(url)));
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(self_update_parse(QNetworkReply*)));
#endif
}

void pluginmanager::self_update_parse(QNetworkReply* reply){

	
            // Check file type
            QDomDocument doc( "XML" );
            if (!doc.setContent(reply->readAll(), true)) {
            return;
            }

            QDomElement root = doc.documentElement();
            if( root.tagName() != "appname" ){
                    return;
            }

            // Loop over main nodes
            latest_version.clear();
            filelist.clear();
            QDomNode mainnode = root.firstChild();
            while( !mainnode.isNull() ){
                    QDomNode subnode = mainnode.firstChild();
            if( !subnode.isNull() ) {
                // Loop over each elements in subnodes
                while(! subnode.isNull()){
                    QDomElement e = subnode.toElement();
                    if(!e.isNull()){
                        // Start parsing, finally
                                            if( e.tagName() == "version" ){
                                                    latest_version = e.attribute("value","");

                                            }
                                            else if( e.tagName() == "file" ){
                                                    filelist.push_back(e.attribute("value",""));
                                            }
                                    }
                                    subnode = subnode.nextSibling();
                            }
                    }
                    mainnode = mainnode.nextSibling();
            }

            // Compare current version with the one on the server
            if( getConfigurazioneSql("VersioneFabariaGest") < latest_version){
                    std::string htmlText =
                            "<html>"+
                            qApp->translate("pluginmanager","<b>E' disponibile la nuova versione:</b> ").toStdString() + latest_version.toStdString() + "<br><br>"
                            "<br><br>";
                    // Add the list of files to download list
		    qDebug() << qApp->translate("pluginmanager","E' disponibile la nuova versione:") << latest_version;
                    QString txtt= "http://www.codelinsoft.it/package/plugin/";
		    download(txtt,filelist.data()->toStdString().c_str());
            }
            else if(reply->error()){
                qDebug() << qApp->translate("pluginmanager","Errore: ") << reply->errorString();
            }
            else{
                qDebug() << qApp->translate("pluginmanager","Plugin aggiornato");
            }
}

void pluginmanager::download(QString url, QString pkg)
{
    lin_start = new QProcess(this);
#if defined (Q_OS_LINUX)
    lin_start->start("pkexec chmod 777 /opt/FabariaGest");
#elif defined (Q_OS_FREEBSD)
    lin_start->start("pkexec chmod 777 /opt/FabariaGest");
#endif
    QUrl url_pkg(url+pkg);
    package = url_pkg.toString();
    QString filename = QFileInfo(url_pkg.path()).fileName();
    mCurrentRequest = QNetworkRequest(url_pkg);
    output = new QFile(filename+".part");
    output->open(QIODevice::ReadWrite);
    startNextDownload(mCurrentRequest);
}

void pluginmanager::startNextDownload(QNetworkRequest& request)
{
	currentDownload = m_manager->get(request);

        connect(currentDownload, SIGNAL(finished()),SLOT(downloadFinished()));
        connect(currentDownload, SIGNAL(readyRead()),SLOT(downloadReadyRead()));
        connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),SLOT(downloadProgress(qint64,qint64)));
        connect(currentDownload, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

        // prepare the output
        downloadTime.start();
}

void pluginmanager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int percent = (qint64)((bytesReceived * 100) / bytesTotal);
    treeWidget->currentItem()->setData(3,Qt::DisplayRole,QString::number(percent));
}

void pluginmanager::downloadFinished()
{
		output->close();
                QUrl url(package);
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

void pluginmanager::downloadReadyRead()
{
    output->write(currentDownload->readAll());
}

void pluginmanager::error(QNetworkReply::NetworkError code)
{
}

void pluginmanager::install_package()
{
   QUrl url(package);
   QString filename = QFileInfo(url.path()).fileName();

   QDir *direct = new QDir( QCoreApplication::applicationDirPath() );
   QString file_dir=direct->currentPath()+"/"+filename;
   QStringList fileNames=direct->entryList( QStringList("*.zip"), QDir::Files, QDir::Name);
   fileNames << filename;
#if defined(Q_OS_LINUX)
    lin_start = new QProcess(this);
    connect(lin_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
    lin_start->start("pkexec unzip -o "+file_dir+" -d /opt/FabariaGest/plugin/");
#elif defined(Q_OS_WIN)
    win_start = new QProcess(this);
    connect(win_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
    win_start->start(QDir::currentPath()+"\\unzip -o "+file_dir+" -d "+QDir::currentPath()+"\\FabariaGest\\plugin");
#elif defined(Q_OS_MAC64)
    mac_start = new QProcess(this);
    connect(mac_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
    mac_start->start("unzip -o "+file_dir+" -d /Application/FabariaGest/FabariaGest/PlugIns");
#elif defined (Q_OS_FREEBSD)
   unix_start = new QProcess(this);
   connect(unix_start,SIGNAL(readyReadStandardOutput()),this,SLOT(display_progress_bar()));
   unix_start->start("pkexec unzip -o "+file_dir+" -d /opt/FabariaGest/plugin");
#endif
}

void pluginmanager::display_progress_bar()
{
    QTreeWidgetItem *item = treeWidget->topLevelItem(treeWidget->currentIndex().row());
#if defined (Q_OS_LINUX)
    int val = lin_start->readLine().toInt();
#elif defined(Q_OS_WIN)
    int val = win_start->readLine().toInt();
#elif defined(Q_OS_MACX)
    int val = mac_start->readLine().toInt();
#elif defined (Q_OS_FREEBSD)
    int val = unix_start->readLine().toInt();
#endif
    for(val=0;val <= 100; val++){
        item->setData(3,Qt::DisplayRole,val);
    }
    item->setData(3,Qt::DisplayRole,100);
}

pluginmanager::~pluginmanager(){
}
