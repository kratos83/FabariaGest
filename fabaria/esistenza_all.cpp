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
#include "esistenza_all.h"
#include "ui_esistenza_all.h"
#include "defines.h"

const int MAX_SIZE = 15;

esistenza_all::esistenza_all(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::esistenza_all)
{
    ui->setupUi(this);
    setWindowTitle(qApp->translate("esistenza_all","Statistica fatturato/ordinato/preventivato"));
    
    //Imposto il grafico
    ui->esistenza->setTitle(qApp->translate("esistenza_all","Statistica fatturato/ordinato/preventivato"));
    ui->esistenza->setAxisTitle(QwtPlot::xBottom,qApp->translate("esistenza_all","Visualizzo anno"));
    ui->esistenza->setAxisTitle(QwtPlot::yLeft,QString::fromUtf8(qApp->translate("esistenza_all","Quantità fatture/ordini/preventivi").toStdString().c_str()));
    ui->esistenza->setAxisScale(QwtPlot::xBottom,0.0,12.0);
    ui->esistenza->setAxisScale(QwtPlot::yLeft,0.0,10000.0);
    
    //Visualizzo la legenda
    ui->esistenza->insertLegend(new QwtLegend());
    
    //Inizializzo il layout
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin( true );
    grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
    grid->setMinorPen( Qt::darkGray, 0, Qt::DotLine );
    grid->attach(ui->esistenza);
    
    //Imposto lo zoom
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    ui->esistenza->setCanvas(canvas);
    canvas->setPalette( Qt::darkGray );
    canvas->setBorderRadius( 10 );
    

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( canvas );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas );
    panner->setMouseButton( Qt::MidButton );
    
    //Connessione pulsanti
    //Esco dalla finestra di dialogo
    connect(ui->esci,SIGNAL(clicked()),this,SLOT(close()));
    ui->esci->setShortcut(tr("Ctrl+E"));
    
    //Esporto il pdf del grafico
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(esporta()));
    ui->pushButton->setShortcut(tr("Ctrl+P"));
    
    connect(ui->ft_acq,SIGNAL(clicked()),this,SLOT(vis_fatture_acq()));
    connect(ui->ft_vn,SIGNAL(clicked()),this,SLOT(vis_fatture()));
    connect(ui->es_prvev,SIGNAL(clicked()),this,SLOT(vis_preventivi()));
    connect(ui->es_ord,SIGNAL(clicked()),this,SLOT(vis_ordini()));
    
    //Visualizzo anno
    combo_anno();
}


void esistenza_all::vis_fatture_acq()
{
    QSqlQuery query_acq;
    query_acq.prepare("select count(id),sum(totale),data from fatture_acq where MONTH(data)='"+QString::number(ui->mese->currentIndex())+"' and YEAR(data)='"+ui->anno->currentText()+"'");
    query_acq.exec();
    double x[MAX_SIZE];
    double y[MAX_SIZE];
    
    for(int i=0; i < query_acq.record().count(); i++){	  
	 while(query_acq.next())
	 {
	 x[i] = query_acq.value(0).toDouble();
	 y[i] = query_acq.value(2).toDate().month();
	 QString testo = qApp->translate("esistenza_all","Hai acquistato per il mese di ")+ui->mese->currentText()+qApp->translate("esistenza_all"," un totale di: ");
	 QString txt = QString::fromUtf8(tr(testo.toStdString().c_str()).toStdString().c_str())+
			       QString::fromUtf8("\u20AC")+" "+query_acq.value(1).toString();
	  add_text(txt+"\n");
	 }
    }
       m_curve = new QwtPlotCurve();
       m_curve->setTitle(qApp->translate("esistenza_all","Fattura di acquisto"));
       m_curve->setPen(Qt::blue, 4);
       m_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
       
       //Visualizzo ellisse
       QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
       m_curve->setSymbol(symbol);
       
       m_curve->setSamples(y,x,MAX_SIZE);
       m_curve->attach(ui->esistenza);
       ui->esistenza->replot();
}

void esistenza_all::vis_fatture()
{
    QSqlQuery query_vnd;
    query_vnd.prepare("select count(id),sum(totale),data from fattura_vendita where MONTH(data)='"+QString::number(ui->mese->currentIndex())+"' and YEAR(data)='"+ui->anno->currentText()+"'");
    query_vnd.exec();
    double x[MAX_SIZE];
    double y[MAX_SIZE];
    
    for(int i=0; i < query_vnd.record().count(); i++){	  
	 while(query_vnd.next())
	 {
	 x[i] = query_vnd.value(0).toDouble();
	 y[i] = query_vnd.value(2).toDate().month();
	 QString testo = qApp->translate("esistenza_all","Hai venduto per il mese di ")+ui->mese->currentText()+qApp->translate("esistenza_all"," un totale di: ");
	 QString txt = QString::fromUtf8(tr(testo.toStdString().c_str()).toStdString().c_str())+
			       QString::fromUtf8("\u20AC")+" "+query_vnd.value(1).toString();
	 add_text(txt+"\n");
	 
	 }
    }
       m_curve = new QwtPlotCurve();
       m_curve->setTitle(qApp->translate("esistenza_all","Fattura di vendita"));
       m_curve->setPen(Qt::yellow, 4);
       m_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
       
       //Visualizzo ellisse
       QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
       m_curve->setSymbol(symbol);
       
       m_curve->setSamples(y,x,MAX_SIZE);
       m_curve->attach(ui->esistenza);
       ui->esistenza->replot();
}

void esistenza_all::vis_preventivi()
{
    QSqlQuery query_prev;
    query_prev.prepare("select count(id),sum(totale),data from preventivi where MONTH(data)='"+QString::number(ui->mese->currentIndex())+"' and YEAR(data)='"+ui->anno->currentText()+"'");
    query_prev.exec();
    double x[MAX_SIZE];
    double y[MAX_SIZE];
    
    for(int i=0; i < query_prev.record().count(); i++){	  
	 while(query_prev.next())
	 {
	 x[i] = query_prev.value(0).toDouble();
	 y[i] = query_prev.value(2).toDate().month();
	 QString testo = qApp->translate("esistenza_all","Hai preventivato per il mese di ")+ui->mese->currentText()+qApp->translate("esistenza_all"," un totale di: ");
	 QString txt = QString::fromUtf8(tr(testo.toStdString().c_str()).toStdString().c_str())+
			       QString::fromUtf8("\u20AC")+" "+query_prev.value(1).toString();
	 add_text(txt+"\n");
	 
	 }
    }
       m_curve = new QwtPlotCurve();
       m_curve->setTitle(qApp->translate("esistenza_all","Preventivi"));
       m_curve->setPen(Qt::cyan, 4);
       m_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
       
       //Visualizzo ellisse
       QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
       m_curve->setSymbol(symbol);
       
       m_curve->setSamples(y,x,MAX_SIZE);
       m_curve->attach(ui->esistenza);
       ui->esistenza->replot();
}

void esistenza_all::vis_ordini()
{
    QSqlQuery query_ord;
    query_ord.prepare("select count(id),sum(totale),data from ordine where MONTH(data)='"+QString::number(ui->mese->currentIndex())+"' and YEAR(data)='"+ui->anno->currentText()+"'");
    query_ord.exec();
    double x[MAX_SIZE];
    double y[MAX_SIZE];
    
    for(int i=0; i < query_ord.record().count(); i++){	  
	 while(query_ord.next())
	 {
	 x[i] = query_ord.value(0).toDouble();
	 y[i] = query_ord.value(2).toDate().month();
	 QString testo = qApp->translate("esistenza_all","Hai ordinato per il mese di ")+ui->mese->currentText()+qApp->translate("esistenza_all"," un totale di: ");
	 QString txt = QString::fromUtf8(tr(testo.toStdString().c_str()).toStdString().c_str())+
			       QString::fromUtf8("\u20AC")+" "+query_ord.value(1).toString();
	 add_text(txt+"\n");
	 
	 }
    }
       m_curve = new QwtPlotCurve();
       m_curve->setTitle("Ordini");
       m_curve->setPen(Qt::lightGray, 4);
       m_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
       
       //Visualizzo ellisse
       QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
       m_curve->setSymbol(symbol);
       
       m_curve->setSamples(y,x,MAX_SIZE);
       m_curve->attach(ui->esistenza);
       ui->esistenza->replot();
}

void esistenza_all::combo_anno()
{
    ui->anno->clear();
    QStringList list;
    QSqlQuery query("select anno_ins from anno");
    if(query.exec()){
    while(query.next()){
        list << query.value(0).toString();
    }
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    complete->setCompletionMode(QCompleter::PopupCompletion);
    ui->anno->setCompleter(complete);
    ui->anno->addItems(list);
    }
}

void esistenza_all::esporta()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("esistenza_all","Esporta PDF"),
                                                    "*.pdf", qApp->translate("esistenza_all","Pdf Files(*.pdf);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".pdf") < 0) {
                fileName += ".pdf";
            }
        }
        
    QwtPlotRenderer renderer;
    renderer.renderDocument(ui->esistenza,fileName, "pdf",QSizeF( 300, 200 ), 85 );
}

void esistenza_all::add_text(QString testo)
{
    ui->label_stat->setText(ui->label_stat->text().append(testo));
}

esistenza_all::~esistenza_all()
{
    delete ui;
}
