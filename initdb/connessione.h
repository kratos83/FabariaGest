#ifndef CONNESSIONE_H
#define CONNESSIONE_H

#include <QtWidgets>
#include <QDialog>
#include <QProcess>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtWidgets/QDesktopWidget>
#include <QTimer>
#include <QDir>
#include <QSettings>
#include "ui_connessione.h"
#include "settingsmanager.h"

namespace Ui {
    class connessione;
}

class connessione : public QDialog, public Ui::connessione
{
    Q_OBJECT

public:
    explicit connessione(QString inizio_update,QWidget *parent,Qt::WindowFlags f = 0);
    ~connessione();
    void load_style(const QString &sheetName);

private:
    QSqlDatabase db;
    QTimer *timer;
    int prog;
    SettingsManager *manager;
    QString m_inizio;

public slots:
    void creadb(QSqlDatabase db, QString db_fabaria, int year);
    void barra_progresso();
    void crea_tabelle(const QString &sql) const;
    void connettidatabase();
    void confread();
    bool add_vis_groupbox(bool vis);
    void vis_main();
    void messaggio();
    bool isNull();
    QSqlDatabase connetti();


protected:
    void closeEvent(QCloseEvent *);
    QString Base64ToAscii(QString String);
};

#endif // CONNESSIONE_H
