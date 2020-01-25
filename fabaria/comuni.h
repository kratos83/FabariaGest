#ifndef COMUNI_H
#define COMUNI_H

#include <QtCore>
#include <QtSql>
#include <QtWidgets>
#include "defines.h"
#include "databasesql.h"

namespace Ui {
class Comuni;
}

class Comuni : public QDialog
{
    Q_OBJECT

public:
    explicit Comuni(QString _comune_,QWidget *parent = nullptr);
    ~Comuni();

private slots:
    void insertComuni();
    void cerca(QString m_comune);
    void findComuni();
    void lista();
private:
    Ui::Comuni *ui;
    QSqlRelationalTableModel *model;
    QString __comune;

signals:
    void addComuni(QString,QString,QString);
};

#endif //COMUNI_H