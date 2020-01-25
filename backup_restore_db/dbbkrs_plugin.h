#ifndef DBBKRS_PLUGIN_H
#define DBBKRS_PLUGIN_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include "db_bk_rs.h"
#include "../fabaria/luxuryim.h"

class dbbkrs_plugin : public QObject, public Interface_plugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.importdbbkrs_plugin" FILE "dbbkrs_plugin.json")
    Q_INTERFACES(Interface_plugin)


public:
    explicit dbbkrs_plugin(QObject *parent = 0);
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
    //QIcon seticona();
private:
    QString error;
    QDir *settingsDir, *profDir;
    QSettings *general;
    QIcon icon;
};

#endif // DBBKRS_PLUGIN_H
