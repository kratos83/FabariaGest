#include "completer.h"

Completer::Completer(const QStringList& stringa, QObject *parent) :
    QCompleter(parent),list(stringa), model()
{
    setModel(&model);    
}
    
//Aggiorno
void Completer::update(QString word)
{
    QStringList filtro = list.filter(word,caseSensitivity());
    model.setStringList(filtro);
    m_stringa = word;
    complete();
}

//Aggiorno il testo
QString Completer::testo()
{
    return m_stringa;
}
