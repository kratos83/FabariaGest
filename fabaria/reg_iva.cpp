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
#include "reg_iva.h"
#include "ui_reg_iva.h"

#include "print.h"
#include "defines.h"
#include "queryexception.h"
#include <iostream>
#include <QDebug>

using namespace Defines;

reg_iva::reg_iva(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reg_iva)
{
    ui->setupUi(this);
    
    //Imposto data e id
    data = "data";
    id_fatt = "id";
    
    //Esci dalla finestra di dialogo
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    ui->esci->setShortcut(tr("Ctrl+E"));
    
    /* Imposto il filtro in modo da poter
     * stampare o esportare i sigoli dati
     */
    connect(ui->mensile,SIGNAL(clicked()),this,SLOT(filtro()));
    connect(ui->trim,SIGNAL(clicked()),this,SLOT(filtro()));
    connect(ui->annuale,SIGNAL(clicked()),this,SLOT(filtro()));
    
    //Imposto le icone per i pulsanti
    QIcon icon_print;
    icon_print.addFile(QString::fromUtf8(":/images/document-print.png"));
    
    QIcon icon_excel;
    icon_excel.addFile(QString::fromUtf8(":/images/excel.png"));
    
    QIcon icon_pdf;
    icon_pdf.addFile(QString::fromUtf8(":/images/pdf.png"));
    
    //Imposto i pulsanti
    esporta_excel = new QAction(qApp->translate("reg_iva","Esporta in excel"),this);
    esporta_excel->setIcon(icon_excel);
    esporta_excel->setIconVisibleInMenu(true);
    
    esporta_pdf = new QAction(qApp->translate("reg_iva","Esporta in pdf"),this);
    esporta_pdf->setIcon(icon_pdf);
    esporta_pdf->setIconVisibleInMenu(true);
    
    stampa = new QAction(qApp->translate("reg_iva","Stampa"),this);
    stampa->setIcon(icon_print);
    stampa->setIconVisibleInMenu(true);
    
    //Imposto il menu 
    menu = new QMenu(this);
    menu->addAction(esporta_excel);
    menu->addSeparator();
    menu->addAction(esporta_pdf);
    menu->addSeparator();
    menu->addAction(stampa);
    ui->report->setMenu(menu);
    
    //Setto l'anno
    ui->anno->setText(settingsManager->generalValue("Esercizio/anno",QVariant()).toString());
    
    //Setto come valore decimale ui->iva_prec
    QString iva_cr = QString::number(ui->iva_prec->text().toDouble(),'f',2);
    ui->iva_prec->setText(iva_cr);
}

void reg_iva::filtro()
{
      if(ui->mensile->isChecked() == true)
      {
	connect(esporta_excel,SIGNAL(triggered()),this,SLOT(export_excel()));
	connect(esporta_pdf,SIGNAL(triggered()),this,SLOT(export_pdf()));
	connect(stampa,SIGNAL(triggered()),this,SLOT(stampa_registro()));
	ui->trim->setChecked(false);
	ui->annuale->setChecked(false);
	add_combo_mese();
      }
      else if(ui->trim->isChecked() == true)
      {
	connect(esporta_excel,SIGNAL(triggered()),this,SLOT(export_excel()));
	connect(esporta_pdf,SIGNAL(triggered()),this,SLOT(export_pdf()));
	connect(stampa,SIGNAL(triggered()),this,SLOT(stampa_registro()));
	ui->mensile->setChecked(false);
	ui->annuale->setChecked(false);
	add_combo_three_month();
      }
      else if(ui->annuale->isChecked() == true)
      {
	connect(esporta_excel,SIGNAL(triggered()),this,SLOT(export_excel()));
	connect(esporta_pdf,SIGNAL(triggered()),this,SLOT(export_pdf()));
	connect(stampa,SIGNAL(triggered()),this,SLOT(stampa_registro()));
    ui->mese->addItem(settingsManager->generalValue("Esercizio/anno",QVariant()).toString());
	ui->mensile->setChecked(false);
	ui->trim->setChecked(false);
      }
}

void reg_iva::stampa_registro()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer,this);
    preview.setWindowFlags(Qt::Window);
    preview.setWindowTitle(qApp->translate("reg_iva","Anteprima di stampa."));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/document-preview.png"), QSize(), QIcon::Normal, QIcon::Off);
    preview.setWindowIcon(icon);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(preview_view(QPrinter*)));
    preview.exec();
}

void reg_iva::preview_view(QPrinter *printer)
{
      Q_UNUSED(printer);
      if(ui->mensile->isChecked() == true)
      {
	if(ui->comboBox->currentIndex() == 0 && ui->tipo_fattura->currentIndex() == 0)
	{
	  stampa_mese_print(printer,data,ui->tipo_fattura->currentIndex());
	}
	else{
	  stampa_mese_print(printer,id_fatt,ui->tipo_fattura->currentIndex());
	}
      }
      else if(ui->trim->isChecked() == true)
      {
	 if(ui->comboBox->currentIndex() == 0 && ui->tipo_fattura->currentIndex() == 0)
	 {
	    stampa_three_month_print(printer,data,ui->tipo_fattura->currentIndex());
	 }
	 else
	 {
	   stampa_three_month_print(printer,id_fatt,ui->tipo_fattura->currentIndex());
	 }
      }
      else if(ui->annuale->isChecked() == true)
      {
	 if(ui->comboBox->currentIndex() == 0 && ui->tipo_fattura->currentIndex() == 0)
	 {
	    stampa_year_print(printer,data,ui->tipo_fattura->currentIndex());
	 }
	 else
	 {
	   stampa_year_print(printer,id_fatt,ui->tipo_fattura->currentIndex());
	 }
      }
}

void reg_iva::stampa_mese_print(QPrinter* print,QString numero,int id)
{
    Q_UNUSED(print);
    Stampe *pr = new Stampe();
    QSqlQuery query_mese_acq;
    query_mese_acq.prepare("select DISTINCT * from fatture_acq_righe "
			   "where MONTH(data)='"+QString::number(1+ui->mese->currentIndex())+"' AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'; ");
    query_mese_acq.exec();
    
    QSqlQuery query_mese_vnd;
    query_mese_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
			   "where MONTH(data)='"+QString::number(1+ui->mese->currentIndex())+"' AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"';");
    query_mese_vnd.exec();    
    if(id == 0)
    {
	pr->stampa_lista_iva_acq(print,query_mese_acq,ui->mese->currentText(),ui->iva_prec->text());
    }
    else
    {
      pr->stampa_lista_iva_vnd(print,query_mese_vnd,ui->mese->currentText(),ui->iva_prec->text());
    }
}

void reg_iva::stampa_three_month_print(QPrinter* print, QString numero, int id)
{
    Q_UNUSED(print);
    Stampe *pr = new Stampe();
    QSqlQuery query_three_month_acq, query_three_month_vnd;
    if(ui->mese->currentText().contains(qApp->translate("reg_iva","Gennaio/Febbraio/Marzo")))
    {
        if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(3+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    pr->stampa_lista_iva_acq(print,query_three_month_acq,ui->mese->currentText(),ui->iva_prec->text());
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(3+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  pr->stampa_lista_iva_vnd(print,query_three_month_vnd,ui->mese->currentText(),ui->iva_prec->text());
	}
    }
    else if(ui->mese->currentText().contains(qApp->translate("reg_iva","Aprile/Maggio/Giugno")))
    {
	if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(6+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    pr->stampa_lista_iva_acq(print,query_three_month_acq,ui->mese->currentText(),ui->iva_prec->text());
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(6+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  pr->stampa_lista_iva_vnd(print,query_three_month_vnd,ui->mese->currentText(),ui->iva_prec->text());
	}
    }
    else if(ui->mese->currentText().contains(qApp->translate("reg_iva","Luglio/Agosto/Settembre")))
    {
       if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(9+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    pr->stampa_lista_iva_acq(print,query_three_month_acq,ui->mese->currentText(),ui->iva_prec->text());
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(9+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  pr->stampa_lista_iva_vnd(print,query_three_month_vnd,ui->mese->currentText(),ui->iva_prec->text());
	}
    }
    else if(ui->mese->currentText().contains(qApp->translate("reg_iva","Ottobre/Novembre/Dicembre")))
    {
       if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(12+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    pr->stampa_lista_iva_acq(print,query_three_month_acq,ui->mese->currentText(),ui->iva_prec->text());
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(12+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  pr->stampa_lista_iva_vnd(print,query_three_month_vnd,ui->mese->currentText(),ui->iva_prec->text());
	}
    }
}

void reg_iva::stampa_year_print(QPrinter* print, QString numero, int id)
{
    Q_UNUSED(print);
    Stampe *pr = new Stampe();
    QSqlQuery query_mese_acq;
    query_mese_acq.prepare("select DISTINCT * from fatture_acq_righe "
			   "where anno_ins='"+ui->anno->text()+"' order by '"+numero+"'; ");
    query_mese_acq.exec();
    
    QSqlQuery query_mese_vnd;
    query_mese_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
			   "where anno_ins='"+ui->anno->text()+"' order by '"+numero+"';");
    query_mese_vnd.exec();    
    if(id == 0)
    {
	pr->stampa_lista_iva_acq(print,query_mese_acq,ui->mese->currentText(),ui->iva_prec->text());
    }
    else
    {
      pr->stampa_lista_iva_vnd(print,query_mese_vnd,ui->mese->currentText(),ui->iva_prec->text());
    }
}

void reg_iva::export_pdf()
{
      QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("reg_iva","Esporta PDF"),
                                                        "*.pdf", qApp->translate("reg_iva","Pdf Files(*.pdf);;Tutti i file(*.*)"));

        if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".pdf") < 0) {
                    fileName += ".pdf";
                }
            }

     if(!fileName.isEmpty()){
        QPrinter *printer = new QPrinter(QPrinter::HighResolution);
        printer->setPageSize(QPrinter::A4);
        printer->setOrientation(QPrinter::Landscape);
	Q_UNUSED(printer);
        printer->setOutputFileName(fileName);;
      if(ui->mensile->isChecked() == true)
      {
	if(ui->comboBox->currentIndex() == 0)
	{
	  stampa_mese_print(printer,data,ui->tipo_fattura->currentIndex());
	}
	else{
	  stampa_mese_print(printer,id_fatt,ui->tipo_fattura->currentIndex());
	}
      }
      else if(ui->trim->isChecked() == true)
      {
	 if(ui->comboBox->currentIndex() == 0)
	 {
	    stampa_three_month_print(printer,data,ui->tipo_fattura->currentIndex());
	 }
	 else
	 {
	   stampa_three_month_print(printer,id_fatt,ui->tipo_fattura->currentIndex());
	 }
      }
      else if(ui->annuale->isChecked() == true)
      {
	 if(ui->comboBox->currentIndex() == 0)
	 {
	    stampa_year_print(printer,data,ui->tipo_fattura->currentIndex());
	 }
	 else
	 {
	   stampa_year_print(printer,id_fatt,ui->tipo_fattura->currentIndex());
	 }
      }
    }//fine isEmpty()
}

void reg_iva::export_excel()
{
      QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("reg_iva","Esporta Excel"),
                                                            "*.xlsx", qApp->translate("reg_iva","XLSX(*.xlsx);;Tutti i file(*.*)"));


            if (fileName.length() != 0) {
                // Aggiunge estensione alla fine del file se non c'è
                if (fileName.indexOf(".xlsx") < 0) {
                    fileName += ".xlsx";
                }
            }

            //workbook excel;
            Document excel;
	    
	    if(ui->mensile->isChecked() == true)
	    {
	       if(ui->comboBox->currentIndex() == 0)
	       {
		  stampa_mese_excel(excel,data,ui->tipo_fattura->currentIndex());
	       }
	       else{
		  stampa_mese_excel(excel,id_fatt,ui->tipo_fattura->currentIndex());
	       }
	    }
	    else if(ui->trim->isChecked() == true)
	    {
	      if(ui->comboBox->currentIndex() == 0)
	       {
		  stampa_three_month_excel(excel,data,ui->tipo_fattura->currentIndex());
	       }
	       else{
		  stampa_three_month_excel(excel,id_fatt,ui->tipo_fattura->currentIndex());
	       }
	    }
	    else if(ui->annuale->isChecked() == true)
	    {
		if(ui->comboBox->currentIndex() == 0)
	       {
		  stampa_year_excel(excel,data,ui->tipo_fattura->currentIndex());
	       }
	       else{
		  stampa_year_excel(excel,id_fatt,ui->tipo_fattura->currentIndex());
	       }
	    }

            try{
                    int ris = excel.saveAs(fileName);
                    if(ris )
                        QMessageBox::information(this,"FabariaGest",qApp->translate("reg_iva","File salvato correttamente"));
                    else
                        throw InvalidFile(qApp->translate("reg_iva","File non salvato").toStdString(),fileName);
            }
            catch(exception e)
            {
                throw InvalidFile(e.what(),fileName);
            }
      
}

void reg_iva::stampa_mese_excel(Document &excel, QString numero, int id)
{
    QSqlQuery query_mese_acq;
    query_mese_acq.prepare("select DISTINCT * from fatture_acq_righe "
			   "where MONTH(data)='"+QString::number(1+ui->mese->currentIndex())+"' AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'; ");
    query_mese_acq.exec();
    
    QSqlQuery query_mese_vnd;
    query_mese_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
			   "where MONTH(data)='"+QString::number(1+ui->mese->currentIndex())+"' AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"';");
    query_mese_vnd.exec();  
      
      if(id == 0)
      {	    
	 excel_fattura_acq(excel,query_mese_acq,qApp->translate("reg_iva","Fattura di acquisto"));
      }
      else
      {
	 excel_fattura_vnd(excel,query_mese_vnd,qApp->translate("reg_iva","Fattura di vendita"));
      }
}

void reg_iva::stampa_three_month_excel(Document &excel, QString numero, int id)
{
    QSqlQuery query_three_month_acq, query_three_month_vnd;
    if(ui->mese->currentText().contains(qApp->translate("reg_iva","Gennaio/Febbraio/Marzo")))
    {
        if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(3+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    excel_fattura_acq(excel,query_three_month_acq,qApp->translate("reg_iva","Fattura di acquisto"));
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(3+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  excel_fattura_vnd(excel,query_three_month_vnd,qApp->translate("reg_iva","Fattura di vendita"));
	}
    }
    else if(ui->mese->currentText().contains(qApp->translate("reg_iva","Aprile/Maggio/Giugno")))
    {
	if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(6+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    excel_fattura_acq(excel,query_three_month_acq,qApp->translate("reg_iva","Fattura di acquisto"));
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(6+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  excel_fattura_vnd(excel,query_three_month_vnd,qApp->translate("reg_iva","Fattura di vendita"));
	}
    }
    else if(ui->mese->currentText().contains(qApp->translate("reg_iva","Luglio/Agosto/Settembre")))
    {
       if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(9+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    excel_fattura_acq(excel,query_three_month_acq,qApp->translate("reg_iva","Fattura di acquisto"));
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(9+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  excel_fattura_vnd(excel,query_three_month_vnd,qApp->translate("reg_iva","Fattura di vendita"));
	}
    }
    else if(ui->mese->currentText().contains(qApp->translate("reg_iva","Ottobre/Novembre/Dicembre")))
    {
       if(id == 0){
	    query_three_month_acq.prepare("select DISTINCT * from fatture_acq_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(12+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	    query_three_month_acq.exec();
	    excel_fattura_acq(excel,query_three_month_acq,qApp->translate("reg_iva","Fattura di acquisto"));
	}
	else{
	  query_three_month_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
				  "where data>=DATE(NOW() - INTERVAL '"+QString::number(12+ui->mese->currentIndex())+"' MONTH) AND anno_ins='"+ui->anno->text()+"' order by '"+numero+"'" );
	  query_three_month_vnd.exec();
	  excel_fattura_vnd(excel,query_three_month_vnd,qApp->translate("reg_iva","Fattura di vendita"));
	}
    }
}

void reg_iva::stampa_year_excel(Document &excel, QString numero, int id)
{
    QSqlQuery query_mese_acq;
    query_mese_acq.prepare("select DISTINCT * from fatture_acq_righe "
			   "where anno_ins='"+ui->anno->text()+"' order by '"+numero+"'; ");
    query_mese_acq.exec();
    
    QSqlQuery query_mese_vnd;
    query_mese_vnd.prepare("select DISTINCT * from fatture_vendita_righe "
			   "where anno_ins='"+ui->anno->text()+"' order by '"+numero+"';");
    query_mese_vnd.exec(); 
    if(id == 0)
      {	    
	    excel_fattura_acq(excel,query_mese_acq,qApp->translate("reg_iva","Fattura di acquisto"));
      }
      else if(id==1)
      {
	    excel_fattura_vnd(excel,query_mese_vnd,qApp->translate("reg_iva","Fattura di vendita"));
      }
}

void reg_iva::excel_fattura_acq(Document &excel, QSqlQuery query_mese_acq, QString fattura)
{
	    //File excel Fattura di acquisto
	    excel.addSheet(fattura);
        Format fm;
        fm.setFontName("OpenSans");
        fm.setFontBold(true);
        fm.setFontSize(14);
        excel.setRowHeight(1,30.0);
	    for(int row=0; row < query_mese_acq.record().count(); row++){
	    if(query_mese_acq.next()){
	      QSqlRecord record_acq = query_mese_acq.record();
	    if(!record_acq.isEmpty()){
		 //Imposto header label per la descrizione
		 excel.write(0,4,QString(qApp->translate("reg_iva","Registro IVA - ")+fattura));
		 if(ui->mese->currentText() == ui->anno->text())
            excel.write(2,1,QString(qApp->translate("reg_iva","Periodo:   ")+ui->anno->text()));
         else
            excel.write(2,1,QString("Periodo:   "+ui->mese->currentText()+ "  "+ui->anno->text()));
		 excel.write(7,1,qApp->translate("reg_iva","Data Reg."));
		 excel.write(7,2,qApp->translate("reg_iva","Data Doc."));
		 excel.write(6,3,qApp->translate("reg_iva","Fattura"));
		 excel.write(7,3,qApp->translate("reg_iva","Num. Prog."));
		 excel.write(7,4,qApp->translate("reg_iva","Num. Fatt."));
		 excel.write(7,5,qApp->translate("reg_iva","Ragione sociale/ Nome e cognome"));
		 excel.write(7,6,qApp->translate("reg_iva","Importo"));
		 excel.write(6,7,QString(qApp->translate("reg_iva","Aliquota ")+settingsManager->generalValue("ListaIva/iva").toString()+" %"));
		 excel.write(7,7,qApp->translate("reg_iva","Imponibile"));
		 excel.write(7,8,qApp->translate("reg_iva","IVA"));
		 excel.write(6,9,QString(qApp->translate("reg_iva","Aliquota  %")));
		 excel.write(7,9,qApp->translate("reg_iva","Imponibile"));
		 excel.write(7,10,qApp->translate("reg_iva","IVA"));
		 excel.write(6,11,QString(qApp->translate("reg_iva","Costi")));
		 excel.write(7,11,qApp->translate("reg_iva","Imponibile"));
		 excel.write(7,12,qApp->translate("reg_iva","IVA"));
		 excel.write(7,13,qApp->translate("reg_iva","Esenti/Non imponibili/Fuori campo"));
		 //Righe excel
		 excel.write(8+row,1,record_acq.value("data").toString());
		 excel.write(8+row,2,record_acq.value("data_fattura").toString());
		 excel.write(8+row,3,record_acq.value("id").toString());
		 excel.write(8+row,4,record_acq.value("numero_fattura").toString());
		 excel.write(8+row,5,record_acq.value("fornitore").toString());
		 excel.write(8+row,6,record_acq.value("totale").toString());
		 excel.write(8+row,7,record_acq.value("prezzo_s_iva").toString());
		 excel.write(8+row,8,record_acq.value("iva").toString());
		 excel.write(8+row,9,"0.00");
		 excel.write(8+row,10,"0.00");
		 excel.write(8+row,11,"0.00");
		 excel.write(8+row,12,"0.00");
		 excel.write(8+row,13,"0.00");
		 //Riepilogo dati
         excel.addSheet(qApp->translate("reg_iva","Riepilogo"));
		 excel.write(3,5,qApp->translate("reg_iva","Riepilogo fattura di acquisto"));
		 QSqlQuery m_fatt_acq;
		 m_fatt_acq.exec("select sum(iva) from fatture_acq_righe");
		 if(m_fatt_acq.next())
		 {
            double somma = ui->iva_prec->text().toDouble()+m_fatt_acq.value(0).toDouble();  
            excel.setRowHeight(5,50);
            excel.setRowHeight(6,50);
            excel.setRowHeight(7,50);
            excel.setRowHeight(8,50);
            excel.write(5,2,QString(qApp->translate("reg_iva","Iva per acquisti: € ")+QString::number(somma)));
            excel.write(6,2,QString(qApp->translate("reg_iva","Iva a credito: € ")+QString::number(somma)));
            excel.write(7,2,QString(qApp->translate("reg_iva","Iva a credito\n(Dal periodo precedente): € ")+ui->iva_prec->text()));
            excel.write(8,2,QString(qApp->translate("reg_iva","Iva a credito: € ")+QString::number(somma)));
		 }
		}
	     }
	   }
}

void reg_iva::excel_fattura_vnd(Document &excel, QSqlQuery query_mese_vnd, QString fattura)
{
	   //File excel Fattura di di vendita
	    excel.addSheet(fattura);
        Format fm;
        fm.setFontName("OpenSans");
        fm.setFontBold(true);
        fm.setFontSize(14);
        excel.setRowHeight(1,30.0);
	    for(int row=0; row < query_mese_vnd.record().count(); row++){
	    if(query_mese_vnd.next()){
	      QSqlRecord record_vnd = query_mese_vnd.record();
	    if(!record_vnd.isEmpty()){
		 //Imposto header label per la descrizione
		 excel.write(0,4,QString(qApp->translate("reg_iva","Registro IVA - ")+fattura));
         if(ui->mese->currentText() == ui->anno->text())
            excel.write(2,1,QString(qApp->translate("reg_iva","Periodo:   ")+ui->anno->text()));
         else
            excel.write(2,1,QString(qApp->translate("reg_iva","Periodo:   ")+ui->mese->currentText()+ "  "+ui->anno->text()));
		 excel.write(7,1,qApp->translate("reg_iva","Data Reg."));
		 excel.write(7,2,qApp->translate("reg_iva","Data Doc."));
		 excel.write(6,3,qApp->translate("reg_iva","Fattura"));
		 excel.write(7,3,qApp->translate("reg_iva","Num. Prog."));
		 excel.write(7,4,qApp->translate("reg_iva","Num. Fatt."));
		 excel.write(7,5,qApp->translate("reg_iva","Ragione sociale/ Nome e cognome"));
		 excel.write(7,6,qApp->translate("reg_iva","Importo"));
		 excel.write(6,7,QString(qApp->translate("reg_iva","Aliquota ")+settingsManager->generalValue("ListaIva/iva").toString()+" %"));
		 excel.write(7,7,qApp->translate("reg_iva","Imponibile"));
		 excel.write(7,8,qApp->translate("reg_iva","IVA"));
		 excel.write(6,9,QString(qApp->translate("reg_iva","Aliquota  %")));
		 excel.write(7,9,qApp->translate("reg_iva","Imponibile"));
		 excel.write(7,10,qApp->translate("reg_iva","IVA"));
		 excel.write(6,11,QString(qApp->translate("reg_iva","Costi")));
		 excel.write(7,11,qApp->translate("reg_iva","Imponibile"));
		 excel.write(7,12,qApp->translate("reg_iva","IVA"));
		 excel.write(7,13,qApp->translate("reg_iva","Esenti/Non imponibili/Fuori campo"));
		 //Righe excel
		 excel.write(8+row,1,record_vnd.value("data").toString());
		 excel.write(8+row,2,record_vnd.value("data_fattura").toString());
		 excel.write(8+row,3,record_vnd.value("id").toString());
		 excel.write(8+row,4,record_vnd.value("numero_fattura").toString());
		 excel.write(8+row,5,record_vnd.value("fornitore").toString());
		 excel.write(8+row,6,record_vnd.value("totale").toString());
		 excel.write(8+row,7,record_vnd.value("prezzo_s_iva").toString());
		 excel.write(8+row,8,record_vnd.value("iva").toString());
		 excel.write(8+row,9,"0.00");
		 excel.write(8+row,10,"0.00");
		 excel.write(8+row,11,"0.00");
		 excel.write(8+row,12,"0.00");
		 excel.write(8+row,13,"0.00");
		 //Riepilogo dati
        excel.addSheet(qApp->translate("reg_iva","Riepilogo"));
		excel.write(3,5,qApp->translate("reg_iva","Riepilogo fattura di vendita"));
		 QSqlQuery m_fatt_acq;
		 m_fatt_acq.exec("select sum(iva) from fatture_vendita_righe");
		 if(m_fatt_acq.next())
		 {
		 double somma = ui->iva_prec->text().toDouble()+m_fatt_acq.value(0).toDouble();  
		excel.setRowHeight(5,50);
		excel.setRowHeight(6,50);
		excel.setRowHeight(7,50);
		excel.setRowHeight(8,50);
		excel.write(5,2,QString(qApp->translate("reg_iva","Iva per vendita: € ")+QString::number(somma)));
		excel.write(6,2,QString(qApp->translate("reg_iva","Iva a debito: € ")+QString::number(somma)));
		excel.write(7,2,QString(qApp->translate("reg_iva","Iva a debito\n(Dal periodo precedente): € ")+ui->iva_prec->text()));
		excel.write(8,2,QString(qApp->translate("reg_iva","Iva a debito: € ")+QString::number(somma)));
		 }
		}
	     }
	   }
}

void reg_iva::add_combo_mese()
{
    ui->mese->clear();
    QStringList list_mese;
    list_mese << qApp->translate("reg_iva","Gennaio") << qApp->translate("reg_iva","Febbraio") << qApp->translate("reg_iva","Marzo") << qApp->translate("reg_iva","Aprile") << qApp->translate("reg_iva","Maggio") << qApp->translate("reg_iva","Giugno") << qApp->translate("reg_iva","Luglio") << qApp->translate("reg_iva","Agosto") << qApp->translate("reg_iva","Settembre") << qApp->translate("reg_iva","Ottobre") << qApp->translate("reg_iva","Novembre") << qApp->translate("reg_iva","Dicembre");
    ui->mese->addItems(list_mese);
}

void reg_iva::add_combo_three_month()
{
    ui->mese->clear();
    QStringList list_three_month;
    list_three_month << qApp->translate("reg_iva","Gennaio/Febbraio/Marzo") << qApp->translate("reg_iva","Aprile/Maggio/Giugno") << qApp->translate("reg_iva","Luglio/Agosto/Settembre")  << qApp->translate("reg_iva","Ottobre/Novembre/Dicembre");
    ui->mese->addItems(list_three_month);
}

reg_iva::~reg_iva()
{
    delete ui;
}
