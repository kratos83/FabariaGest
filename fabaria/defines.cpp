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
#include "defines.h"
#include "queryexception.h"
#include "databasesql.h"

using namespace QXlsx;
using namespace DatabaseSql;

QString Defines::getSystemInfo()
{
  QString os_type = "";
#if defined(Q_OS_LINUX)
    QString com_lsb = "lsb_release -d ";
    QString com_uname = "uname -a";
    QProcess *add_rel = new QProcess;
    add_rel->setReadChannel(QProcess::StandardOutput);
    add_rel->setProcessChannelMode(QProcess::MergedChannels);
    add_rel->start(com_lsb);
    add_rel->closeWriteChannel();
    if(add_rel->state()!=QProcess::NotRunning){
        add_rel->waitForReadyRead();
        QByteArray result = add_rel->readAll();
    QStringList list = QString(result).split(":");
    QString text = list.at(1);
        os_type += text;
    }

    os_type += " -- ";

    QProcess *add_uname = new QProcess;
    add_uname->setReadChannel(QProcess::StandardOutput);
    add_uname->setProcessChannelMode(QProcess::MergedChannels);
    add_uname->start(com_uname);
    add_uname->closeWriteChannel();
    if(add_uname->state()!=QProcess::NotRunning){
        add_uname->waitForReadyRead();
        QByteArray result = add_uname->readAll();
        os_type += QString(result);
    }

#elif defined(Q_OS_WIN)
    switch(QSysInfo::windowsVersion())
    {
      case QSysInfo::WV_XP:
    os_type += WINDOWS_XP;
      break;
      case QSysInfo::WV_VISTA:
    os_type += WINDOWS_VISTA;
      break;
      case QSysInfo::WV_WINDOWS7:
    os_type += WINDOWS_SEVEN;
      break;
      case QSysInfo::WV_WINDOWS8:
    os_type += WINDOWS_8;
      break;
      case QSysInfo::WV_WINDOWS8_1:
    os_type += WINDOWS_8_1;
      break;
      case QSysInfo::WV_WINDOWS10:
    os_type += WINDOWS_10;
      break;
      case QSysInfo::WV_NT_based:
    os_type += _WINNT;
      break;
      default:
      break;
    }
#elif defined(Q_OS_MACX)
    QString com_uname = "uname -a";
  QProcess *add_uname = new QProcess;
  add_uname->setReadChannel(QProcess::StandardOutput);
  add_uname->setProcessChannelMode(QProcess::MergedChannels);
  add_uname->start(com_uname);
  add_uname->closeWriteChannel();
  if(add_uname->state()!=QProcess::NotRunning){
      add_uname->waitForReadyRead();
      QByteArray result = add_uname->readAll();
      os_type += QString(result);
  }
#elif defined(Q_OS_FREEBSD)
    QString com_uname = "uname -a";
    QProcess *add_uname = new QProcess;
    add_uname->setReadChannel(QProcess::StandardOutput);
    add_uname->setProcessChannelMode(QProcess::MergedChannels);
    add_uname->start(com_uname);
    add_uname->closeWriteChannel();
    if(add_uname->state()!=QProcess::NotRunning){
        add_uname->waitForReadyRead();
        QByteArray result = add_uname->readAll();
        os_type += QString(result);
    }
#endif

    return os_type;
}

QString Defines::getSystemRam()
{
      QString os_ram = "";
#if defined(Q_OS_LINUX)
    QString com_ram = "head -3 /proc/meminfo";
    QProcess *add_mem = new QProcess;
    add_mem->setReadChannel(QProcess::StandardOutput);
    add_mem->setProcessChannelMode(QProcess::MergedChannels);
    add_mem->start(com_ram);
    add_mem->closeWriteChannel();
    if(add_mem->state()!=QProcess::NotRunning){
        add_mem->waitForReadyRead();
        QByteArray result = add_mem->readAll();
    QStringList list = QString(result).split(":");
    //Visualizzo la memoria totale
    QStringList ram_tot = list.at(1).split("MemFree");
    QStringList ram_tot_t = ram_tot.at(0).split("kB");
    double ram_totale = ram_tot_t.at(0).toDouble()/1024/1024;
    //Visualizzo la memoria libera
    QStringList ram_av = list.at(3).split("kB");
    double ram_libera = ram_av.at(0).toDouble()/1024/1024;
    //Visualizzo la memoria utilizzata
    double ram_utilizzata = ram_totale-ram_libera;
    os_ram += qApp->translate("Defines","Memoria totale: ")+QString::number(ram_totale,'f',2)+" GB\n"+
          qApp->translate("Defines","Memoria utilizzata: ")+QString::number(ram_utilizzata,'f',2)+" GB\n"+
          qApp->translate("Defines","Memoria libera: ")+QString::number(ram_libera,'f',2)+" GB";
    }
#elif defined(Q_OS_WIN)
  MEMORYSTATUSEX memory_status;
  ZeroMemory(&memory_status, sizeof(MEMORYSTATUSEX));
  memory_status.dwLength = sizeof(MEMORYSTATUSEX);
  if (GlobalMemoryStatusEx(&memory_status)) {
    os_ram += QString(qApp->translate("Defines","\nMemoria totale: %1 GB")).arg(memory_status.ullTotalPhys / (1024 * 1024 *1024))+
              QString(qApp->translate("Defines","\nMemoria utilizzata: %1 GB")).arg(memory_status.ullAvailVirtual / (1024 * 1024 *1024))+
             QString(qApp->translate("Defines","\nMemoria libera: %1 GB")).arg(memory_status.ullAvailPhys / (1024 * 1024 *1024));
  } else {
    os_ram += qApp->translate("Defines","Memoria sconosciuta");
  }
#elif defined(Q_OS_MACX)
  QString com_ram_freebsd = "sysctl hw.physmem hw.usermem";
  QProcess *add_mem = new QProcess;
  add_mem->setReadChannel(QProcess::StandardOutput);
  add_mem->setProcessChannelMode(QProcess::MergedChannels);
  add_mem->start(com_ram_freebsd);
  add_mem->closeWriteChannel();
  if(add_mem->state()!=QProcess::NotRunning){
      add_mem->waitForReadyRead();
      QByteArray result = add_mem->readAll();
  QStringList list = QString(result).split(":");
  //Visualizzo memoria totale
  QStringList ram_tot = list.at(1).split("hw.usermem");
  double ram_totale = (ram_tot.at(0).toDouble()/1024)/1024/1024;
  //Visualizzo la memoria libera
  double ram_libera = (list.at(2).toDouble()/1024)/1024/1024;
  //Visualizzo la ram utilizzata
  double ram_utilizzata = ram_totale-ram_libera;
  os_ram += qApp->translate("Defines","Memoria totale: ")+QString::number(ram_totale,'f',2)+" GB\n"+
        qApp->translate("Defines","Memoria utilizzata: ")+QString::number(ram_utilizzata,'f',2)+" GB\n"+
        qApp->translate("Defines","Memoria libera: ")+QString::number(ram_libera,'f',2)+" GB";
  }
#elif defined(Q_OS_FREEBSD)
  QString com_ram_freebsd = "sysctl hw.physmem hw.usermem";
  QProcess *add_mem = new QProcess;
  add_mem->setReadChannel(QProcess::StandardOutput);
  add_mem->setProcessChannelMode(QProcess::MergedChannels);
  add_mem->start(com_ram_freebsd);
  add_mem->closeWriteChannel();
  if(add_mem->state()!=QProcess::NotRunning){
      add_mem->waitForReadyRead();
      QByteArray result = add_mem->readAll();
    QStringList list = QString(result).split(":");
    //Visualizzo memoria totale
    QStringList ram_tot = list.at(1).split("hw.usermem");
    double ram_totale = (ram_tot.at(0).toDouble()/1024)/1024/1024;
    //Visualizzo la memoria libera
    double ram_libera = (list.at(2).toDouble()/1024)/1024/1024;
    //Visualizzo la ram utilizzata
    double ram_utilizzata = ram_totale-ram_libera;
    os_ram += qApp->translate("Defines","Memoria totale: ")+QString::number(ram_totale,'f',2)+" GB\n"+
            qApp->translate("Defines","Memoria utilizzata: ")+QString::number(ram_utilizzata,'f',2)+" GB\n"+
            qApp->translate("Defines","Memoria libera: ")+QString::number(ram_libera,'f',2)+" GB";
  }

#endif

  return os_ram;
}

QString Defines::Base64ToAscii(QString String)
{

    QByteArray ByteArray1=String.toUtf8();

    const char* chArr1=ByteArray1.constData();

    QByteArray ByteArray2=QByteArray::fromBase64(chArr1);

    const char* chArr2=ByteArray2.constData();

    return QString::fromUtf8(chArr2);
}

void Defines::LOG_FILE(QString linea, const Defines::SOFT_LOG soft, QIODevice::OpenModeFlag flags)
{
    Q_UNUSED(flags)
    if(soft == LOG_ERROR_ALL)
        insertQueryLog("INSERT INTO fabarialog(log_error,logo_linea,dataLog) VALUES('"+qApp->translate("Defines","[ERRORE]: ")+"','"+linea+"','"+QDate::currentDate().toString("yyyy-MM-dd")+"')");
    else if(soft == LOG_WARNING_ALL)
        insertQueryLog("INSERT INTO fabarialog(log_error,logo_linea,dataLog) VALUES('"+qApp->translate("Defines","[ATTENZIONE]: ")+"','"+linea+"','"+QDate::currentDate().toString("yyyy-MM-dd")+"')");
    else if(soft == LOG_INFO_ALL)
        insertQueryLog("INSERT INTO fabarialog(log_error,logo_linea,dataLog) VALUES('"+qApp->translate("Defines","[INFO]: ")+"','"+linea+"','"+QDate::currentDate().toString("yyyy-MM-dd")+"')");
}

QString Defines::GetCartellaDatabase()
{
    QString cartella_db="";
    QSqlQuery query("SHOW VARIABLES WHERE Variable_Name LIKE 'datadir'");
    query.exec();
    if(query.next()){
#if defined(Q_OS_LINUX)
    cartella_db = query.value(1).toString()+"fabaria";
#elif defined(Q_OS_WIN)
    cartella_db = query.value(1).toString()+"fabaria";
#elif defined(Q_OS_MACX)
    cartella_db = query.value(1).toString()+"fabaria";
#elif defined(Q_OS_FREEBSD)
    cartella_db = query.value(1).toString()+"fabaria";
#endif
    }
    else{
    }
    return cartella_db;
}

QString Defines::GetRunCommandMySql()
{
    QString cartella_db="";
    QSqlQuery query("SHOW VARIABLES WHERE Variable_Name LIKE 'basedir'");
    query.exec();
    if(query.next()){
#if defined(Q_OS_LINUX)
    cartella_db = query.value(1).toString()+"/bin";
#elif defined(Q_OS_WIN)
    cartella_db = query.value(1).toString()+"\\bin";
#elif defined(Q_OS_MACX)
    cartella_db = query.value(1).toString()+"/bin";
#elif defined(Q_OS_FREEBSD)
    cartella_db = query.value(1).toString()+"/bin";
#endif
    }
    else{
    }
    return cartella_db;
}


QString Defines::getVerificaCodiceFiscale(QString code, QLabel* label1)
{
  int s, i, c;
    int setdisp[] = { 1, 0, 5, 7, 9, 13, 15, 17, 19, 21, 2, 4, 18, 20,
            11, 3, 6, 8, 12, 14, 16, 10, 22, 25, 24, 23 };


    if(code.length() == 0){
        label1->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-close.png")).scaled(16,16));
    }

    if(code.length() != 16){
        label1->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-close.png")).scaled(16,16));
    }

    for( i=0; i<16; i++ ){
        c = toupper( code[i].toLatin1() );
            if( !isdigit(c) && !( 'A'<=c && c<='Z' ) )
                label1->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-close.png")).scaled(16,16));
    }

    s = 0;
    for( i=1; i<=13; i+=2 ){
        c = toupper( code[i].toLatin1() );
           if( isdigit(c) )
                s += c - '0';
            else
                s += c - 'A';
    }

    for( i=0; i<=14; i+=2 ){
        c = toupper( code[i].toLatin1() );
            if( isdigit(c) )  c = c - '0' + 'A';
            s += setdisp[c - 'A'];
    }

    if( s%26 + 'A' != toupper( code[15].toLatin1() ) )
    {
        label1->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-close.png")).scaled(16,16));
    }
    else{
        label1->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-ok-apply.png")).scaled(16,16));
    }
    
    return code;
}

QString Defines::getPartitaIva(QString code, QLabel *label)
{
  int i;
    int p_pari = 0;
    int p_dispari = 0;
    int num_sup = 0;


    if(code.length() == 0){
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-close.png")).scaled(16,16));
    }

    if(code.length() != 11 || code == "00000000000"){
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-close.png")).scaled(16,16));
    }
    else{
        for(i=0; i<code.length();i++){
            if(((i+1)%2)==0){
                int k = toascii(code[i].toLatin1()) - toascii('0');
                p_pari += k;
                if(k>5)
                    num_sup++;
            }
            else
                p_dispari += toascii(code[i].toLatin1())- toascii('0');
        }
    }

    int tot = p_pari + p_dispari + num_sup;
    p_pari = tot % 10;
    if(tot == ((10 -p_pari) %10) ){
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-close.png")).scaled(16,16));
    }
    else{
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/dialog-ok-apply.png")).scaled(16,16));
    }
    
    return code;
}

void Defines::extractFileZip(QString zipFile, QString destDir)
{
    //Extract file into plugin
    if(!zipFile.isNull())
      JlCompress::extractDir(zipFile,destDir);
    else
      LOG_FILE(qApp->translate("Defines","File zip non trovato"),LOG_WARNING_ALL,QIODevice::Append);
}

QString Defines::getLineFromCommandOutput(QString command)
{
    FILE *file = popen(command.toLatin1(),"r");

                char buffer[100];
                QString line = "";
                char firstChar;

                if ((firstChar = fgetc(file)) != -1){
                        line += firstChar;
                        line += fgets(buffer,100,file);
                }

                pclose(file);


                return line;
}

QString Defines::cartellaDoc()
{
    QString cartella;
#ifdef Q_OS_WIN
    cartella = QDir::homePath()+"/fabaria";
#else
    cartella = QDir::homePath()+"/.fabaria/";
#endif
    return cartella;
}

void Defines::esportaCSV(QString m_tarns, QTableView *table, QSqlQueryModel *model)
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, qApp->translate(m_tarns.toStdString().c_str(),"Esporta CSV"),
                                                    "*.csv", qApp->translate(m_tarns.toStdString().c_str(),"CSV(*.csv);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".csv") < 0) {
                fileName += ".csv";
            }
        }

    int x = 0;
    QString exportdata;

    int counthidden=0, jint = 0;

    while (jint < model->columnCount()) {

    counthidden+=table->isColumnHidden(jint);
    jint++;
    }


    int w = 1;
    while (x < model->columnCount()){

    if (!table->isColumnHidden(x)) {


    exportdata.append(model->headerData(x,Qt::Horizontal,Qt::DisplayRole).toString());


    if (model->columnCount() - w > counthidden)
    exportdata.append(";");
    else {
    exportdata.append("\n");

    }
    w++;
    }
    x++;

    }

    int z = 0;

    w = 1;
    while (z < model->rowCount()) {

    x = 0;

    w = 1;
    while (x < model->columnCount()) {
    if (!table->isColumnHidden(x)) {


    exportdata.append(model->data(model->index(z,x),Qt::DisplayRole).toString());

    if (model->columnCount() - w > counthidden)
    exportdata.append(";");
    else
    exportdata.append("\n");

    w++;
    }
    x++;

    }

    z++;
    }

    QFile file;
    if (!fileName.isEmpty()) {
     file.setFileName(fileName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return;
    }
    QByteArray ttext;
    ttext.append(exportdata);
    file.write(ttext);
}

void Defines::esportaExcelDefine(QString m_trans, QSqlQueryModel *model)
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, qApp->translate(m_trans.toStdString().c_str(),"Esporta Excel"),
                                                            "*.xlsx", qApp->translate(m_trans.toStdString().c_str(),"XLSX(*.xlsx);;Tutti i file(*.*)"));


            if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".xlsx") < 0) {
                    fileName += ".xlsx";
                }
            }

            Document excel;
            for(int i= 0; i < model->columnCount();++i){
                    //Imposto header label per la descrizione
		    Format fm;
		    fm.setFontName("OpenSans");
		    fm.setFontBold(true);
		    fm.setFontSize(14);
		    excel.setRowHeight(1,30.0);
                    excel.write(1,1+i,model->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString(),fm);
                for(int row = 0; row < model->rowCount(); row++){
                    //Imposto le righe
                    excel.write(2+row,1+i,model->data(model->index(row,i),Qt::DisplayRole).toString());
                }
            }

         try{
            int ris = excel.saveAs(fileName);
            if(ris )
		QMessageBox::information(nullptr,"FabariaGest",qApp->translate(m_trans.toStdString().c_str(),"File salvato correttamente"));
            else
                throw InvalidFile(qApp->translate(m_trans.toStdString().c_str(),"File non salvato").toStdString(),fileName);
	  }
	  catch(exception e)
	  {
	      throw InvalidFile(e.what(),fileName);
	  }
}