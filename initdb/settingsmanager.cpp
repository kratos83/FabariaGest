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
