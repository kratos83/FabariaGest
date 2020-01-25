#ifndef UPDATE_DB_H
#define UPDATE_DB_H

#include <QtGui>
#include <QDialog>
#include <QProcess>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QTimer>
#include <QDir>
#include <QSettings>
#include <QMessageBox>

namespace Ui {
class update_db;
}

class update_db : public QDialog
{
    Q_OBJECT

public:
    explicit update_db(QWidget *parent = 0);
    ~update_db();
    void load_style(const QString &sheetName);

private:
    Ui::update_db *ui;
    QSqlDatabase db;
    QDir *settingsDir;
    QSettings *general;
    QString Base64ToAscii(QString String);

public slots:
    QSqlDatabase connetti();
    void aggiorna_db(QSqlDatabase db,QString db_fabaria);
    void aggiorna_tabelle(const QString &sql) const;
    void messaggio();
};

#endif // UPDATE_DB_H
