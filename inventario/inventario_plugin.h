#ifndef INVENTARIO_PLUGIN_H
#define INVENTARIO_PLUGIN_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QtGui>
#include <QSettings>
#include <QDir>
#include "inventario.h"
#include "../fabaria/luxuryim.h"

class inventario_plugin : public QObject, public Interface_plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.importinventario_plugin" FILE "inventario_plugin.json")
    Q_INTERFACES(Interface_plugin)

public:
    explicit inventario_plugin(QObject *parent = 0);
    QWidget *creaWidget();
    QString nameplugin() const;
        QString version() const;
        QString vendor() const;
        QString errorText() const;
        QString description() const;
        QString displayName() const;
        QIcon icona();
        QString name_menu() const;
	QCheckBox *box() const;
        void pluginLoad();
        void pluginUnload();
    
signals:
    
public slots:

private:
        QString error;
        QDir *settingsDir, *profDir;
        QSettings *general;
};

#endif // INVENTARIO_PLUGIN_H
