#include <QtCore>
#include <QtSql>
#include <iostream>
#include <QtWidgets>

namespace DatabaseSql {

 //Seleziona preferenze dal sql
QVariant getPreferenzeSql(QString m_value);

//Seleziona configurazione dal sql
QVariant getConfigurazioneSql(QString m_value);

//Insert sql preferenze
void insertPreferenze(QString m_preferenze);

//Update sql preferenze
void updatePreferenze(QString m_preferenze);

//Insert sql configurazione
void insertConfigurazione(QString m_configurazione);

//Update sql preferenze
void updateConfigurazione(QString m_configurazione);

//Select Query
QStringList selectQueryWhile(QString query_select, int m_value);
QVariant selectQuery(QString query_select, int m_value);
QVariant selectQueryString(QString query_select, QString m_value);
QSqlRecord selectQueryWhileList(QString query_select);

//Insert query
void insertQuery(QString query_insert, QString m_table);

//InsertQueryLog
void insertQueryLog(QString query_insert);

//Update query
void updateQuery(QString query_update, QString m_table);

//EliminaQuery
void eliminaQuery(QString query_elimina, QString m_table);

//Query exec
void queryExec(QString query, QString table_or_sql);
};