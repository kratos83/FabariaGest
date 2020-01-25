#ifndef COMPLETER_H
#define COMPLETER_H

#include <QtCore>
#include <QCompleter>

class Completer : public QCompleter
{
    Q_OBJECT
    
public:
    //Costruttore
    Completer(const QStringList& stringa, QObject *parent=0);
    
    //Aggiorno
    void update(QString word);
    
    
    //Aggiorno il testo
    inline QString testo();
private:
    QStringList list;
    QStringListModel model;
    QString m_stringa;
};

#endif // COMPLETER_H
