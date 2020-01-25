/*
 * Copyright (C) 2011-2015  Codelinsoft <info@codelinsoft.it>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <QErrorMessage>
#include <QMessageBox>
#include <QFile>
#include <iostream>


#include "email_send.h"
#include "ui_email_send.h"
#include "defines.h"

using namespace std;
using namespace Defines;

Q_LOGGING_CATEGORY(FabariaGestEmailSend, "fabariagest")

email_send *m_emailSend = 0;

email_send::email_send(QWidget *parent, double id, QString cl_fr, QString data, QString tipo_fattura) : 
    QDialog(parent)
{
    m_emailSend = this;
    setupUi(this);

    id_fat = id;
    cliente_fornitore = cl_fr;
    data_fat = data;
    tipo_doc = tipo_fattura;
    setWindowTitle(qApp->translate("email_send","Invio email"));
 
    //Invio segnali
    connect(invia,SIGNAL(clicked()),this,SLOT(invia_email()));
    connect(vis_al_list,SIGNAL(clicked(bool)),this,SLOT(vis_list_allegati(bool)));
    connect(ricevuta,SIGNAL(clicked(bool)),this,SLOT(visNotifica(bool)));
    connect(rubrica,SIGNAL(clicked()),this,SLOT(visRubrica()));
    
    if(settingsManager->generalValue("EmailServer/vis_allegati_fattura",QVariant()).toBool() == true)
    {
	vis_allegati();
    }
      
     if(tipo_doc == qApp->translate("email_send","Fattura immediata") || tipo_doc == qApp->translate("email_send","Fattura accompagnatoria")
      || tipo_doc == qApp->translate("email_send","Fattura proforma") || tipo_doc == qApp->translate("email_send","Nota di credito"))
    {
      vis_allegati_email();
    }
    else if(tipo_doc == qApp->translate("email_send","DDT")){
      vis_allegati_email_ddt();
    }
    else if(tipo_doc == qApp->translate("email_send","Ordine")){
      vis_allegati_email_ord();
    }
    else if(tipo_doc == qApp->translate("email_send","Preventivo")){
      vis_allegati_email_prev();
    }
    
    vis_list_allegati(false);
    list_file->setViewMode(QListWidget::IconMode);
    list_file->sortItems(Qt::AscendingOrder);
    
    QString testo = QString(qApp->translate("email_send","<p align=\"justify\">Spettabile ")+cliente_fornitore+qApp->translate("email_send",","
		    "in allegato trasmettiamo nostro/a ")+tipo_doc+qApp->translate("email_send"," nr. ")+QString::number(id_fat)+qApp->translate("email_send"," del ")+data_fat+qApp->translate("email_send"," in formato PDF."
		    "Come da precedenti accordi, e nel rispetto delle disposizioni di Legge, non verrà spedita alcuna copia cartacea."
		    "Il/La/L' ")+tipo_doc+qApp->translate("email_send"," allegata/o va stampata/o e conservata/o per tutti i necessari adempimenti di Legge, come disposto dal DPR 633/72 (succ. modifiche) e dalla risoluzione del Ministero delle Finanze PROT.450217 del 30 Luglio 1990."
		    "Per visualizzare l'allegato è necessario utilizzare Acrobat Reader (gratuitamente scaricabile presso questo indirizzo:" 
		    "<a href=\"http://www.adobe.it/products/acrobat/readstep2.html\">http://www.adobe.it/products/acrobat/readstep2.html</a>)."
		    "Porgendo distinti saluti e ringraziando per la fiducia accordata, restiamo a completa disposizione per ogni ulteriore informazione.</p></br></br></br>"
            "<p align=\"justify\">Il presente messaggio e i suoi eventuali allegati contengono informazioni strettamente riservate e destinate esclusivamente alla persona, ente o società sopra indicati."
            "Qualsiasi suo utilizzo, comunicazione o diffusione non autorizzata comporta violazione delle disposizioni di legge sulla tutela dei dati personali (D.L. 30 giugno 2003 n. 196) ed è pertanto rigorosamente vietata."
            "Qualora Lei non fosse il destinatario del messaggio, La invitiamo ad eliminarlo dal Suo Sistema e a distruggere le varie copie o stampe dandocene gentilmente comunicazione.</p>"));
    editor->setHtml(testo);
    QFont font;
    font.setPointSize(10);
    editor->setFont(font);
    a_email->setText(settingsManager->generalValue("EmailServer/user_auth_smtp",QVariant()).toString());
}

EmailAddress *email_send::stringToEmail(const QString &str)
{
    int p1 = str.indexOf("<");
    int p2 = str.indexOf(">");

    if (p1 == -1)
    {
        // no name, only email address
        return new EmailAddress(str);
    }
    else
    {
        return new EmailAddress(str.mid(p1 + 1, p2 - p1 - 1), str.left(p1));
    }
}

void email_send::invia_email()
{
    QString host = settingsManager->generalValue("EmailServer/server_smtp",QVariant()).toString();
    int port = settingsManager->generalValue("EmailServer/porta_smtp",QVariant()).toInt();
    bool ssl = settingsManager->generalValue("EmailServer/ssl_smtp",QVariant()).toBool();
    bool auth = settingsManager->generalValue("EmailServer/auth_smtp",QVariant()).toBool();
    QString user = settingsManager->generalValue("EmailServer/user_auth_smtp",QVariant()).toString();
    QString password = Base64ToAscii(settingsManager->generalValue("EmailServer/pwd_auth_smtp",QVariant()).toString());
    
    //Invio email
    EmailAddress *sender = stringToEmail(a_email->text());
    
    //Invio a più destinatari
    QStringList rcptStringList = cc_email->text().split(';');
    qCDebug(FabariaGestEmailSend) << rcptStringList;
    //Oggetto email
    QString subject = oggetto->text();
    
    //Oggetto ricevuta di lettura
    
    //Corpo del testo
    QString html = editor->toHtml();
    
    //Imposto il client
    SmtpClient smtp (host, port, ssl ? SmtpClient::SslConnection : SmtpClient::TcpConnection);

    //Imposto l'invio del messaggio
    MimeMessage message;

    message.setSender(sender);
    if(ricevuta->isChecked() == true)
        message.setDispositionNotification(notifica);
    message.setSubject(subject);

    for (int i = 0; i < rcptStringList.size(); ++i)
         message.addRecipient(stringToEmail(rcptStringList.at(i)));

    //Formato nel formato html
    MimeHtml content;
    content.setHtml(html);

    message.addPart(&content);

    //Allego file se esistono
    for (int i = 0; i < list_file->count(); ++i)
    {
        message.addPart(new MimeAttachment(new QFile(settingsManager->generalValue("Cartella/destinazione",QVariant()).toString()+"/"+list_file->item(i)->text())));
    }

    /* Se la connessione fallisce
     * visualizza una finestra di errore
     */
    if (!smtp.connectToHost())
    {
        errorMessage(qApp->translate("email_send","Connessione fallita"));
        return;
    }

    /* Se l'autenticazione fallisce
     * visualizza una finestra di errore
     */
    if (auth)
        if (!smtp.login(user, password))
        {
            errorMessage(qApp->translate("email_send","Autenticazione fallita"));
            return;
        }
    
    /* Se l'invio fallisce
     * visualizza una finestra di errore
     */
    if (!smtp.sendMail(message))
    {
        errorMessage(qApp->translate("email_send","Invio fallito"));
        return;
    }
	else{
        QMessageBox okMessage (this);
        okMessage.setWindowTitle("FabariaGest");
        okMessage.setIcon(QMessageBox::Information);
        okMessage.setText(qApp->translate("email_send","Email spedita correttamente"));
        okMessage.setStandardButtons(QMessageBox::Ok);
        okMessage.setDefaultButton(QMessageBox::Ok);
        int ret = okMessage.exec();
        switch(ret)
        {
            case QMessageBox::Ok: 
            {
                for (int i = 0; i < list_file->count(); ++i)
                {
                    QFileInfo fi_st(list_file->item(i)->text());
                    insert_storico_mail(fi_st.fileName(),cliente_fornitore,QDateTime::currentDateTime());
                }
            }
            close();
            break;
        }
    }
    smtp.quit();
}

void email_send::errorMessage(const QString &message)
{
    QErrorMessage err (this);
    err.setWindowTitle("FabariaGest");
    err.showMessage(message);

    err.exec();
}

QString email_send::Base64ToAscii(QString String)
{
    QByteArray ByteArray1=String.toUtf8();

    const char* chArr1=ByteArray1.constData();

    QByteArray ByteArray2=QByteArray::fromBase64(chArr1);

    const char* chArr2=ByteArray2.constData();

    return QString::fromUtf8(chArr2);
}

void email_send::vis_allegati_email()
{
    list_file->clear();
    QStringList list;
    QString m_fileName;
    QSqlQuery qvis_allegati("select * from allegati_email where id='"+QString::number(id_fat)+"'");
    try{
    if(qvis_allegati.exec()){
    while(qvis_allegati.next())
    {	
	list = qvis_allegati.value(1).toStringList();
	foreach(m_fileName,list){
	QListWidgetItem *item = new QListWidgetItem(list_file);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	//File dentro listwidget
	QFile file_size(m_fileName);
	QFileInfo fi(file_size);
	file_size.close();
	//Imposto nome file file allegato
	QString m_name = fi.fileName();
	item->setText(m_name);
	//Imposto icona file allegato
	QIcon icon(":/images/pdf.png");
	item->setIcon(icon);
	item->setFont(f);
	}
    }
    }
    else{
      LOG_FILE(qApp->translate("email_send","Impossibile visualizzare gli allegati_email(email_send::vis_allegati_email())"),LOG_ERROR_ALL,QIODevice::Append);
      LOG_FILE(qApp->translate("email_send","(email_send::vis_allegati_email()): ")+qvis_allegati.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
      throw InvalidQuery(qvis_allegati.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    list_file->update();
}


void email_send::vis_allegati()
{
    list_file->clear();
    QStringList list;
    QString m_fileName;
    QSqlQuery qvis_allegati("select * from allegati_fattura where id='"+QString::number(id_fat)+"'");
    try{
    if(qvis_allegati.exec()){
    while(qvis_allegati.next())
    {	
	list = qvis_allegati.value(1).toStringList();
	foreach(m_fileName,list){
	QListWidgetItem *item = new QListWidgetItem(list_file);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	//File dentro listwidget
	QFile file_size(m_fileName);
	QFileInfo fi(file_size);
	file_size.close();
	//Imposto nome file file allegato
	QString m_name = fi.fileName();
	item->setText(m_name);
	//Imposto icona file allegato
	QIcon icon(":/images/pdf.png");
	item->setIcon(icon);
	item->setFont(f);
	}
    }
    }
    else{
      LOG_FILE(qApp->translate("email_send","Impossibile visualizzare gli allegati_email(email_send::vis_allegati())"),LOG_ERROR_ALL,QIODevice::Append);
      LOG_FILE(qApp->translate("email_send","(email_send::vis_allegati()): ")+qvis_allegati.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
      throw InvalidQuery(qvis_allegati.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    list_file->update();
}

void email_send::vis_allegati_email_ddt()
{
    list_file->clear();
    QStringList list;
    QString m_fileName;
    QSqlQuery qvis_allegati("select * from allegati_email_ddt where id='"+QString::number(id_fat)+"'");
    try{
    if(qvis_allegati.exec()){
    while(qvis_allegati.next())
    {	
	list = qvis_allegati.value(1).toStringList();
	foreach(m_fileName,list){
	QListWidgetItem *item = new QListWidgetItem(list_file);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	//File dentro listwidget
	QFile file_size(m_fileName);
	QFileInfo fi(file_size);
	file_size.close();
	//Imposto nome file file allegato
	QString m_name = file_size.fileName();
	item->setText(m_name);
	//Imposto icona file allegato
	QFileIconProvider provider;
	QIcon icon = provider.icon(fi);
	item->setIcon(icon);
	item->setFont(f);
	}
    }
    }
    else{
      LOG_FILE(qApp->translate("email_send","Impossibile visualizzare gli allegati_email(email_send::vis_allegati_email_ddt())"),LOG_ERROR_ALL,QIODevice::Append);
      LOG_FILE(qApp->translate("email_send","(email_send::vis_allegati_email_ddt()): ")+qvis_allegati.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
      throw InvalidQuery(qvis_allegati.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    list_file->update();
}

void email_send::vis_allegati_email_prev()
{
    list_file->clear();
    QStringList list;
    QString m_fileName;
    QSqlQuery qvis_allegati("select * from allegati_email_prev where id='"+QString::number(id_fat)+"'");
    try{
    if(qvis_allegati.exec()){
    while(qvis_allegati.next())
    {	
	list = qvis_allegati.value(1).toStringList();
	foreach(m_fileName,list){
	QListWidgetItem *item = new QListWidgetItem(list_file);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	//File dentro listwidget
	QFile file_size(m_fileName);
	QFileInfo fi(file_size);
	file_size.close();
	//Imposto nome file file allegato
	QString m_name = file_size.fileName();
	item->setText(m_name);
	//Imposto icona file allegato
	QFileIconProvider provider;
	QIcon icon = provider.icon(fi);
	item->setIcon(icon);
	item->setFont(f);
	}
    }
    }
    else{
      LOG_FILE(qApp->translate("email_send","Impossibile visualizzare gli allegati_email(email_send::vis_allegati_email_prev())"),LOG_ERROR_ALL,QIODevice::Append);
      LOG_FILE(qApp->translate("email_send","(email_send::vis_allegati_email_prev()): ")+qvis_allegati.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
      throw InvalidQuery(qvis_allegati.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    list_file->update();
}

void email_send::vis_allegati_email_ord()
{
    list_file->clear();
    QStringList list;
    QString m_fileName;
    QSqlQuery qvis_allegati("select * from allegati_email_ord where id='"+QString::number(id_fat)+"'");
    try{
    if(qvis_allegati.exec()){
    while(qvis_allegati.next())
    {	
	list = qvis_allegati.value(1).toStringList();
	foreach(m_fileName,list){
	QListWidgetItem *item = new QListWidgetItem(list_file);
	QFont f;
	f.setPointSize(10); // Non deve essere 0
	//File dentro listwidget
	QFile file_size(m_fileName);
	QFileInfo fi(file_size);
	file_size.close();
	//Imposto nome file file allegato
	QString m_name = file_size.fileName();
	item->setText(m_name);
	//Imposto icona file allegato
	QFileIconProvider provider;
	QIcon icon = provider.icon(fi);
	item->setIcon(icon);
	item->setFont(f);
	}
    }
    }
    else{
      LOG_FILE(qApp->translate("email_send","Impossibile visualizzare gli allegati_email(email_send::vis_allegati_email_ord())"),LOG_ERROR_ALL,QIODevice::Append);
      LOG_FILE(qApp->translate("email_send","(email_send::vis_allegati_email_ord()): ")+qvis_allegati.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
      throw InvalidQuery(qvis_allegati.lastError().text().toStdString().c_str());
    }
    }
    catch(FabariaException e)
    {
       LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
    }
    list_file->update();
}

void email_send::vis_list_allegati(bool vero)
{
    if(vero == true){
	list_file->setVisible(true);
    }
    else{
	list_file->setVisible(false);
    }
}

void email_send::visNotifica(bool vero)
{
    if(vero == true)
        notifica = a_email->text();
}

void email_send::insert_storico_mail(QString nome_file, QString forn_cl, QDateTime data_ora)
{
    QSqlQuery query_storico, query_count;
    query_count.exec("select max(id+1) from lista_invio_mail");
    if(query_count.next()){
        query_storico.prepare("INSERT INTO lista_invio_mail"
                " VALUES('"+query_count.value(0).toString()+"','"+nome_file+"','"+forn_cl+"','"+data_ora.toString("yyyy-MM-dd hh:mm:ss")+"')");
        try{
        if(query_storico.exec())
        {
        LOG_FILE(qApp->translate("email_send","Inserimento effettuato con successo(email_send::insert_storico_mail)"),LOG_INFO_ALL,QIODevice::Append);
        }
        else{
        LOG_FILE(qApp->translate("email_send","Impossile inserire(email_send::insert_storico_mail)"),LOG_ERROR_ALL,QIODevice::Append);
        LOG_FILE(qApp->translate("email_send","(email_send::insert_storico_mail): ")+query_storico.lastError().text(),LOG_ERROR_ALL,QIODevice::Append);
        throw QueryException(query_storico.lastError().text().toStdString().c_str());
        }
        }
        catch(FabariaException e)
        {
        LOG_FILE(QString(e.what()),LOG_ERROR_ALL,QIODevice::Append);
        }
    }
}

void email_send::visRubrica()
{
    m_email = new ListaEmail(this);
    m_email->exec();
}

email_send::~email_send()
{
}
