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
#include <QDir>
#include <QSettings>
#include <QDebug>
#include "settingsmanager.h"

SettingsManager *settingsManager=0;

SettingsManager::SettingsManager(QObject *parent): QObject(parent) {

	settingsManager = this;

#ifdef Q_OS_WIN
    settingsDir = new QDir(QDir::homePath()+"/fabaria/");
#else
    settingsDir = new QDir(QDir::homePath()+"/.fabaria/");
#endif

	if(!settingsDir->exists())
		settingsDir->mkdir(settingsDir->path());

	if(!settingsDir->exists(settingsDir->path()+"/profiles/"))
		settingsDir->mkdir(settingsDir->path()+"/profiles/");

	if(!settingsDir->exists(settingsDir->path()+"/plugins/"))
		settingsDir->mkdir(settingsDir->path()+"/plugins/");

	if(!settingsDir->exists(settingsDir->path()+"/log/"))
		settingsDir->mkdir(settingsDir->path()+"/log/");
	
	profDir = new QDir( settingsDir->path()+"/profiles/" );
	
	logDir = new QDir( settingsDir->path()+"/log/" );
	if(!logDir->exists(logDir->path()+"/backup/"))
	  logDir->mkdir(logDir->path()+"/backup/");

        general = new QSettings(settingsDir->path()+"/config.conf",QSettings::IniFormat);
}

QVariant SettingsManager::generalValue( const QString &key, const QVariant &defaultValue ) const
{
    return general->value(key,defaultValue);
}

void SettingsManager::setGeneralValue(QString const& key,  QVariant const& value ) {
	general->setValue(key,value);
	general->sync();
}

QDir* SettingsManager::profilesDir() const {
	return profDir;
}

void SettingsManager::createPluginSettings( const QString& plugin ) {
	if( !existsPluginSettings( plugin ) )
        pluginSettings.insert( plugin, new QSettings( settingsDir->path()+"/plugins/"+plugin ) );

}

bool SettingsManager::existsPluginSettings( const QString& plugin ) const {
	return pluginSettings.contains(plugin);
}

void SettingsManager::deletePluginSettings( const QString& plugin ) {
	if( existsPluginSettings( plugin ) ) {
		delete pluginSettings[plugin];
		pluginSettings.remove( plugin );
	}
}

void SettingsManager::setPluginValue( const QString& plugin, const QString& key, const QVariant& value ) {
	if( existsPluginSettings( plugin ) )
		pluginSettings[plugin]->setValue(key,value);
}

QVariant SettingsManager::pluginValue( const QString& plugin, const QString& key, const QVariant& value ) const {
	if( existsPluginSettings( plugin ) )
		return pluginSettings[plugin]->value(key,value);

	return QVariant();
}

QStringList SettingsManager::profilesList( const QString& plugin ) const {
	return profDir->entryList( QStringList( "*."+plugin ), QDir::Files, QDir::Name);
}
