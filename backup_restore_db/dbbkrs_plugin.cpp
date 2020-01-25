#include "dbbkrs_plugin.h"
#include "../fabaria/databasesql.h"
#include "../fabaria/defines.h"

using namespace Defines;
using namespace DatabaseSql;

dbbkrs_plugin::dbbkrs_plugin(QObject *parent) :
    QObject(parent)
{}

QWidget* dbbkrs_plugin::creaWidget() {
    return new db_bk_rs();
}

QString dbbkrs_plugin::name_menu() const {
    return tr("Plugin");
}
QString dbbkrs_plugin::nameplugin() const {
    return tr("Backup and\nrestore database");
}

QString dbbkrs_plugin::version() const {
    return tr(getConfigurazioneSql("VersioneFabariaGest").toString().toStdString().c_str());
}

QString dbbkrs_plugin::vendor() const {
    return tr("Codelinsoft");
}

QString dbbkrs_plugin::errorText() const {
    return error;
}

QString dbbkrs_plugin::description() const {
    return tr("Strumento per il salvataggio e ripristino del database");
}

QString dbbkrs_plugin::displayName() const {
    return tr("Plugin Backup and restore");
}

void dbbkrs_plugin::pluginLoad() {
    creaWidget();
}

void dbbkrs_plugin::pluginUnload() {
    delete creaWidget();
}

QCheckBox* dbbkrs_plugin::box() const
{
    return box();
}

QIcon dbbkrs_plugin::icona() {
    QIcon m_icona;
    m_icona.addFile(":/images/svn-commit.png",QSize(48,48));
    return m_icona;
}
