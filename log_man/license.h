#ifndef LICENSE_H
#define LICENSE_H

#include <QDialog>
#include <QSettings>
#include <QtGui>
#include <QDir>

namespace Ui {
    class license;
}

class license : public QDialog
{
    Q_OBJECT

public:
    explicit license(QWidget *parent = 0);
    ~license();
    void load_style(const QString &sheetName);

public slots:
    void button();
    void esci();
    void page_stack();
    void page_fine();
    void indietro();
    void avanti();
    void read_conf();

private:
    Ui::license *ui;
    QDir *settingsDir;
    QSettings *general;

protected:
    void closeEvent(QCloseEvent *);
signals:
    void load_license();
};

#endif // LICENSE_H
