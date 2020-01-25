#include "inventario_plugin.h"
#include <QtGui>
#include "../fabaria/databasesql.h"
#include "../fabaria/defines.h"

using namespace Defines;
using namespace DatabaseSql;

inventario_plugin::inventario_plugin(QObject *parent) :
    QObject(parent)
{
}

QWidget* inventario_plugin::creaWidget(){
    return new inventario();
}

QString inventario_plugin::name_menu() const{
    return tr("Plugin");
}
QString inventario_plugin::nameplugin() const{
    return tr("Inventario\ndei prodotti");
}

QString inventario_plugin::version() const{
    return tr(getConfigurazioneSql("VersioneFabariaGest").toString().toStdString().c_str());
}

QString inventario_plugin::vendor() const{
    return tr("Codelinsoft");
}

QString inventario_plugin::errorText() const{
    return error;
}

QString inventario_plugin::description() const{
    return tr("Inventario dei prodotti presenti in magazzino");
}

QString inventario_plugin::displayName() const{
    return tr("Plugin Inventario");
}

void inventario_plugin::pluginLoad(){
    creaWidget();
}

void inventario_plugin::pluginUnload(){
    delete creaWidget();
}

QCheckBox* inventario_plugin::box() const
{
    return box();
}


QIcon inventario_plugin::icona(){
    QIcon m_icona;
    m_icona.addFile(":/images/documentation.png",QSize(48,48));
    return m_icona;
}
