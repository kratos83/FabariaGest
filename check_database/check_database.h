#ifndef CHECK_DATABASE_H
#define CHECK_DATABASE_H

#include <QDialog>
#include <QtGui>
#include <QtWidgets/QDesktopWidget>
#include <QProcess>
#include <QtSql>


namespace Ui {
class check_database;
}

class check_database : public QDialog
{
    Q_OBJECT

public:
    explicit check_database(QWidget *parent = 0);
    ~check_database();

public slots:
    void analizza_database();
    void ripristina_database();
    void lista();
private:
    Ui::check_database *ui;
    QProcess *analizza,*ripristina;
    QStandardItemModel *model;
    void read_output(QString nameFile);
    QDesktopWidget *desk;
};

#endif // CHECK_DATABASE_H
