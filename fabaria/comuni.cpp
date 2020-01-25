#include "comuni.h"
#include "itdelegato.h"
#include "ui_comuni.h"

using namespace Defines;
using namespace DatabaseSql;

Comuni::Comuni(QString _comune_, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Comuni),
        model(new QSqlRelationalTableModel),
        __comune(_comune_)
{
    ui->setupUi(this);
    if(__comune == "comune")
        setWindowTitle("Seleziona comune");
    else if(__comune == "cap")
        setWindowTitle("Seleziona C.A.P.");
    connect(ui->chiudi,&QPushButton::clicked,this,&Comuni::close);
    connect(ui->lineEditCerca,&QLineEdit::textChanged,this,&Comuni::cerca);
    connect(ui->lineEditCerca,&QLineEdit::textChanged,this,&Comuni::findComuni);
    connect(ui->insert,&QPushButton::clicked,this,&Comuni::insertComuni);
    connect(ui->tableView,&QTableView::doubleClicked,this,&Comuni::insertComuni);
    lista();
}

void Comuni::lista()
{
    model->setTable("comuni");
    model->select();
    model->setHeaderData(0,Qt::Horizontal,qApp->translate("Comuni","Località"));
    model->setHeaderData(1,Qt::Horizontal,qApp->translate("Comuni","Provincia"));
    model->setHeaderData(2,Qt::Horizontal,qApp->translate("Comuni","C.A.P."));
    
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->setItemDelegateForColumn(3, new coldxdelegato(this));
}

void Comuni::insertComuni()
{
    QString comune = model->data(model->index(ui->tableView->selectionModel()->currentIndex().row(),0),Qt::DisplayRole).toString();
    QString provincia = model->data(model->index(ui->tableView->selectionModel()->currentIndex().row(),1),Qt::DisplayRole).toString();
    QString cap = model->data(model->index(ui->tableView->selectionModel()->currentIndex().row(),2),Qt::DisplayRole).toString();
    emit addComuni(comune,provincia,cap);
    close();
}

void Comuni::cerca(QString m_comune)
{
    if(m_comune.isEmpty())
        lista();
    else if(__comune == "comune")
        model->setFilter(QString("comune = '" +m_comune+"'").arg(m_comune));
    else if(__comune == "cap")
        model->setFilter(QString("codice = '" +m_comune+"'").arg(m_comune));
}

void Comuni::findComuni()
{
    QStringList list;
    if(__comune == "comune")
        list = selectQueryWhile("select comune from comuni",0);
    else if(__comune == "cap")
        list = selectQueryWhile("select codice from comuni",0);
    QCompleter *complete = new QCompleter(list,this);
    complete->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEditCerca->setCompleter(complete);
}

Comuni::~Comuni()
{
    delete ui;
}