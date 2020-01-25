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
#include "esistenza_articoli.h"
#include "ui_esistenza_articoli.h"
#include "defines.h"

#include <QDebug>

const int MAX_SIZE = 15;

esistenza_articoli::esistenza_articoli(QWidget *parent ):
    QDialog(parent),
    ui(new Ui::esistenza_articoli)
{
    ui->setupUi(this);
    
    setWindowTitle(qApp->translate("esistenza_articoli","Esistenza prodotti"));
    
    //Imposto il grafico
    ui->esistenza->setTitle(qApp->translate("esistenza_articoli","Esistenza prodotti"));
    ui->esistenza->setAxisTitle(QwtPlot::xBottom,qApp->translate("esistenza_articoli","Visualizzo anno"));
    ui->esistenza->setAxisTitle(QwtPlot::yLeft,QString::fromUtf8(qApp->translate("esistenza_articoli","Quantità prodotti").toStdString().c_str()));
    
    //Scrivo i mesi dell'anno
    ui->esistenza->setAxisScale(QwtPlot::xBottom,0.0,12.0);
    //ui->esistenza->setAxisScaleDraw(QwtPlot::xBottom, new YearScaleDraw());
    
    //Visualizzo la Quantità
    ui->esistenza->setAxisScale(QwtPlot::yLeft,-10000.0,10000.0);
    
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
    connect(ui->esporta,SIGNAL(clicked()),this,SLOT(esporta()));
    ui->esporta->setShortcut(tr("Ctrl+P"));    
}

void esistenza_articoli::vis_quantita(QModelIndex *index, QSqlQueryModel *dataset)
{
      if(index->row() < dataset->rowCount())
      {
    double x[MAX_SIZE];
    double y[MAX_SIZE];
    
	for(int i=0; i < 1; i++){	  
	    QSqlRecord record = dataset->record(index->row());
	    if(!record.isEmpty())
	    {
	        x[i] = record.value("quantita").toDouble();
		y[i] = record.value("data_insert").toDate().month();
		QString testo = qApp->translate("esistenza_articoli","L'articolo '")+record.value("cod_articolo").toString()+QString::fromUtf8(qApp->translate("esistenza_articoli","' è prezzato: ").toStdString().c_str());
		QString txt = QString::fromUtf8(tr(testo.toStdString().c_str()).toStdString().c_str())+
				    QString::fromUtf8("\u20AC")+" "+record.value("prezzo_unitario").toString();
		add_text(txt+"\n");
	 
	    }
	}
	
       //Inserisco i dati 
       m_curve = new QwtPlotCurve();
       m_curve->setTitle("Articoli");
       m_curve->setPen(Qt::gray, 4);
       m_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
       
       //Visualizzo ellisse
       QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
       m_curve->setSymbol(symbol);
       
       m_curve->setSamples(y,x,MAX_SIZE);
       m_curve->attach(ui->esistenza);
       ui->esistenza->replot();
      }
}

void esistenza_articoli::esporta()
{
    QString fileName = QFileDialog::getSaveFileName(this, qApp->translate("esistenza_articoli","Esporta PDF"),
                                                    "*.pdf", qApp->translate("esistenza_articoli","Pdf Files(*.pdf);;Tutti i file(*.*)"));

    if (fileName.length() != 0) {
            // Aggiunge estensione alla fine del file se non c'è
            if (fileName.indexOf(".pdf") < 0) {
                fileName += ".pdf";
            }
        }
        
    QwtPlotRenderer renderer;
    renderer.renderDocument(ui->esistenza,fileName, "pdf",QSizeF( 300, 200 ), 85 );
}

void esistenza_articoli::add_text(QString testo)
{
    ui->labe_test->setText(ui->labe_test->text().append(testo));
}

esistenza_articoli::~esistenza_articoli()
{
    delete ui;
}
