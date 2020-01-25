#include "databasesql.h"
#include "defines.h"
#include "queryexception.h"

using namespace Defines;

QVariant DatabaseSql::getPreferenzeSql(QString m_value)
{
   QVariant sql_value;
   QSqlQuery query;
   query.prepare("select * from preferenze where id = 1;");
   if(query.exec() && query.next())
   {
       if(m_value == "id")
         sql_value = query.value("id");
       else if(m_value == "menu" )
         sql_value = query.value("menu");
       else if(m_value == "ApplicationFont" )
         sql_value = query.value("ApplicationFont");
       else if(m_value == "ApplicationFont2" )
         sql_value = query.value("ApplicationFont2");
       else if(m_value == "AggEn" )
         sql_value = query.value("AggEn");
       else if(m_value == "AggDis" )
         sql_value = query.value("AggDis");
       else if(m_value == "AutoEn" )
         sql_value = query.value("AutoEn");
       else if(m_value == "AutoDis" )
         sql_value = query.value("AutoDis");
       else if(m_value == "SelTema" )
         sql_value = query.value("SelTema");
       else if(m_value == "EmailServerSmtp" )
         sql_value = query.value("EmailServerSmtp");
       else if(m_value == "EmailServerPortaSmtp" )
         sql_value = query.value("EmailServerPortaSmtp");
       else if(m_value == "EmailServerSSlSmtp" )
         sql_value = query.value("EmailServerSSlSmtp");
       else if(m_value == "EmailServerAuthSmtp" )
         sql_value = query.value("EmailServerAuthSmtp");
       else if(m_value == "EmailServerUserAuthSmtp" )
         sql_value = query.value("EmailServerUserAuthSmtp");
       else if(m_value == "EmailServerPwdAuthSmtp" )
         sql_value = query.value("EmailServerPwdAuthSmtp");
       else if(m_value == "EmailServerAllegatiFattura" )
         sql_value = query.value("EmailServerAllegatiFattura");
       else if(m_value == "EmailServerMail" )
         sql_value = query.value("EmailServerMail");
       else if(m_value == "CartellaDestinazione" )
         sql_value = query.value("CartellaDestinazione");
       else if(m_value == "ListaBancaCheck" )
         sql_value = query.value("ListaBancaCheck");
       else if(m_value == "ListaNomeBanca" )
         sql_value = query.value("ListaNomeBanca");
       else if(m_value == "ListaSelectIva" )
         sql_value = query.value("ListaSelectIva");
       else if(m_value == "ListaIvaCheck" )
         sql_value = query.value("ListaIvaCheck");
       else if(m_value == "LinguaMadre" )
         sql_value = query.value("LinguaMadre");
       else if(m_value == "ImageResizeDirImage" )
         sql_value = query.value("ImageResizeDirImage");
       else if(m_value == "ImageResizeLarghezza" )
         sql_value = query.value("ImageResizeLarghezza");
       else if(m_value == "ImageResizeAltezza" )
         sql_value = query.value("ImageResizeAltezza");
   }
   else 
        sql_value = query.lastError().text();
   return sql_value;
}

void DatabaseSql::insertPreferenze(QString m_preferenze)
{
    QSqlQuery query;
    qDebug() << m_preferenze;
    query.prepare("INSERT INTO preferenze VALUES('1',"+m_preferenze+");");
    try{
        if(query.exec())
        {
            LOG_FILE("Inserimento effettuato con successo in preferenze",LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE("Impossibile inserire in preferenze",LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void DatabaseSql::updatePreferenze(QString m_preferenze)
{
    QSqlQuery query;
    qDebug() << m_preferenze;
    query.prepare("UPDATE preferenze SET "+m_preferenze+" WHERE id='1'");
    try{
        if(query.exec())
        {
            LOG_FILE("Aggiornamento effettuato con successo in preferenze",LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE("Impossibile aggiornare in preferenze",LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

QVariant DatabaseSql::getConfigurazioneSql(QString m_value)
{
   QVariant sql_value;
   QSqlQuery query;
   query.prepare("select * from configurazione where id=1");
   if(query.exec() && query.next())
   {
       if(m_value == "id")
         sql_value = query.value("id");
       else if(m_value == "DatabaseServerIP" )
         sql_value = query.value("DatabaseServerIP");
       else if(m_value == "DatabaseServerPortaDB" )
         sql_value = query.value("DatabaseServerPortaDB");
       else if(m_value == "DatabaseServerNomeDB" )
         sql_value = query.value("DatabaseServerNomeDB");
       else if(m_value == "DatabaseServerUserDB" )
         sql_value = query.value("DatabaseServerUserDB");
       else if(m_value == "DatabaseServerPWD" )
         sql_value = query.value("DatabaseServerPWD");
       else if(m_value == "AziendaID" )
         sql_value = query.value("AziendaID");
       else if(m_value == "Esercizioconfigurazione" )
         sql_value = query.value("Esercizioconfigurazione");
       else if(m_value == "CartellaLOG" )
         sql_value = query.value("CartellaLOG");
       else if(m_value == "MagazzinoNome" )
         sql_value = query.value("MagazzinoNome");
       else if(m_value == "MagazzinoID" )
         sql_value = query.value("MagazzinoID");
       else if(m_value == "BackupDate" )
         sql_value = query.value("BackupDate");
       else if(m_value == "PluginLoad" )
         sql_value = query.value("PluginLoad");
       else if(m_value == "DatabaseLocale" )
         sql_value = query.value("DatabaseLocale");
       else if(m_value == "DatabaseLocaleHost" )
         sql_value = query.value("DatabaseLocaleHost");
       else if(m_value == "DatabaseLocaleDBNome" )
         sql_value = query.value("DatabaseLocaleDBNome");
       else if(m_value == "DatabaseLocaleUserDB" )
         sql_value = query.value("DatabaseLocaleUserDB");
       else if(m_value == "DatabaseLocalePWDDB" )
         sql_value = query.value("DatabaseLocalePWDDB");
       else if(m_value == "DatabaseInitialize" )
         sql_value = query.value("DatabaseInitialize");
       else if(m_value == "VersioneMain" )
         sql_value = query.value("VersioneMain");
       else if(m_value == "MainLicenza" )
         sql_value = query.value("MainLicenza");
       else if(m_value == "VersioneFabariaGest" )
         sql_value = query.value("VersioneFabariaGest");
       else if(m_value == "PosizioneGeometry" )
         sql_value = query.value("PosizioneGeometry");
       else if(m_value == "PosizioneStato" )
         sql_value = query.value("PosizioneStato");
       else if(m_value == "PosizioneMassimizza" )
         sql_value = query.value("PosizioneMassimizza");
       else if(m_value == "PosizionePOS" )
         sql_value = query.value("PosizionePOS");
       else if(m_value == "PosizioneSize" )
         sql_value = query.value("PosizioneSize");
       else if(m_value == "AnnoIns" )
         sql_value = query.value("AnnoIns");
       else if(m_value == "Tutorial")
         sql_value = query.value("Tutorial");
   }
   else 
        sql_value = query.lastError().text();

   return sql_value;
}

void DatabaseSql::insertConfigurazione(QString m_configurazione)
{
    QSqlQuery query;
    query.prepare("INSERT INTO configurazione VALUES("+m_configurazione+")");
    try{
        if(query.exec())
        {
            LOG_FILE("Inserimento effettuato con successo in configurazione",LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE("Impossibile inserire in configurazione",LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		    throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void DatabaseSql::updateConfigurazione(QString m_configurazione)
{
     QSqlQuery query;
    query.prepare("UPDATE configurazione SET "+m_configurazione+" WHERE id='1'");
    try{
        if(query.exec())
        {
            LOG_FILE("Aggiornamento effettuato con successo in configurazione",LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE("Impossibile aggiornare in configurazione",LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		    throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

QStringList DatabaseSql::selectQueryWhile(QString query_select, int m_value)
{
    QSqlQuery query;
    QStringList m_select;
    query.prepare(query_select);
    try{
        if(query.exec())
        {
          while(query.next()){
            m_select << query.value(m_value).toString();
          }
        }
        else{
            LOG_FILE("Impossibile selezionare",LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    return m_select;
}

QSqlRecord DatabaseSql::selectQueryWhileList(QString query_select)
{
    QSqlQueryModel query;
    QSqlRecord m_select;
    query.setQuery(query_select);
    try{
        for(int i=0; i < query.rowCount(); i++)
        {
            m_select = query.record(i);
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    return m_select;
}

QVariant DatabaseSql::selectQuery(QString query_select, int m_value)
{
    QSqlQuery query;
    QVariant m_select;
    query.prepare(query_select);
    try{
        if(query.exec())
        {   if(query.next())
              m_select = query.value(m_value);
            else return m_select == "";
        }          
        else{
            return m_select = query.lastError().text();
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    return m_select;
}

QVariant DatabaseSql::selectQueryString(QString query_select, QString m_value)
{
    QSqlQuery query;
    QVariant m_select;
    query.prepare(query_select);
    try{
        if(query.exec())
        {   
            if(query.next())
              m_select = query.value(m_value);
            else return m_select == "";
        }
        else{
            return m_select = query.lastError().text();
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    return m_select;
}

void DatabaseSql::insertQuery(QString query_insert, QString m_table)
{
   QSqlQuery query;
    query.prepare(query_insert);
    try{
        if(query.exec())
        {
            LOG_FILE(QString("Inserimento effettuato con successo in "+m_table),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE(QString("Impossibile inserire in "+m_table),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void DatabaseSql::insertQueryLog(QString query_insert)
{
  QSqlQuery query;
    query.prepare(query_insert);
    try{
        if(query.exec())
        {
            qDebug() << QString("Inserimento effettuato con successo in "+query_insert);
        }
        else{
            qDebug() << QString("Impossibile inserire in "+query_insert);
            qDebug() << query.lastError().text();
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        qDebug() << QString(e.what());
    }
}

void DatabaseSql::updateQuery(QString query_update, QString m_table)
{
   QSqlQuery query;
    query.prepare(query_update);
    try{
        if(query.exec())
        {
            LOG_FILE(QString("Aggiornamento effettuato con successo in "+m_table),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE(QString("Aggiornamento inserire in "+m_table),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void DatabaseSql::eliminaQuery(QString query_elimina, QString m_table)
{
    QSqlQuery query;
    query.prepare(query_elimina);
    try{
        if(query.exec())
        {
            LOG_FILE(QString("Aggiornamento effettuato con successo in "+m_table),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
            LOG_FILE(QString("Aggiornamento inserire in "+m_table),LOG_ERROR_ALL,QIODevice::Append);
            LOG_FILE(query.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
		        throw InvalidQuery(query.lastError().text().toStdString().c_str());   
        }
    }
    catch(FabariaException e)
    {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
}

void DatabaseSql::queryExec(QString query, QString table_or_sql)
{
   QSqlQuery query_ex;
   query_ex.prepare(query);
   try{
      if(query_ex.exec())
          qDebug() << QString("Query exec effettuata correttamente: "+table_or_sql);
      else{
          qDebug() << QString("Query exec non effettuata correttamente: "+table_or_sql);
          qDebug() << query_ex.lastError().text();
		      throw InvalidQuery(query_ex.lastError().text().toStdString().c_str()); 
      }
   }
   catch(FabariaException e)
   {
      LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
   }
}