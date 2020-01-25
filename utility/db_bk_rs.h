#ifndef DB_BK_RS_H
#define DB_BK_RS_H

#include <QDialog>
#include <QtSql/QtSql>
#include <QProcess>
#include <QDir>
#include <QSettings>
#include "ui_db_bk_rs.h"

namespace Ui {
    class db_bk_rs;
}

class db_bk_rs : public QDialog,public Ui::db_bk_rs{
    Q_OBJECT

public:
    db_bk_rs(QWidget *parent = 0);
    ~db_bk_rs();

private slots:
    void crea_backup();
    void restore_db();
    void esci();
private:

    QProcess *backup;
    QProcess *restore;
    QString error;
    QSettings *general;
    QDir *settingsDir;
    QString getLineFromCommandOutput( QString command );
    QString Base64ToAscii(QString String);
    QString GetRunCommandMySql();
};

#endif // DB_BK_RS_H
