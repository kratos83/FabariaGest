#include "checkdatabase_plugin.h"
#include "../fabaria/databasesql.h"
#include "../fabaria/defines.h"

using namespace Defines;
using namespace DatabaseSql;

checkdatabase_plugin::checkdatabase_plugin(QObject *parent) :
    QObject(parent){}

QWidget* checkdatabase_plugin::creaWidget() {
    return new check_database();
}

QString checkdatabase_plugin::name_menu() const {
    return tr("Plugin");
}
QString checkdatabase_plugin::nameplugin() const {
    return tr("Correggi\ndatabase");
}

QString checkdatabase_plugin::version() const {
    return tr(getConfigurazioneSql("VersioneFabariaGest").toString().toStdString().c_str());
}

QString checkdatabase_plugin::vendor() const {
    return tr("Codelinsoft");
}

QString checkdatabase_plugin::errorText() const {
    return error;
}

QString checkdatabase_plugin::description() const {
    return tr("Strumento per la correzione degli errori del database");
}

QString checkdatabase_plugin::displayName() const {
    return tr("Plugin Check Database");
}

void checkdatabase_plugin::pluginLoad() {
    creaWidget();
}

void checkdatabase_plugin::pluginUnload() {

    delete creaWidget();
}

QCheckBox* checkdatabase_plugin::box() const
{
    return box();
}
QIcon checkdatabase_plugin::icona() {
    QIcon m_icona;
    m_icona.addFile(":/images/svn-commit.png",QSize(48,48));
    return m_icona;
}
