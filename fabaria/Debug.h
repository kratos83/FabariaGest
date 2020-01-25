#ifndef DEBUG_H
#define DEBUG_H

#include "defines.h"
#include <QtCore>

class Debug 
{
public:
    /*
    * Instanzio il debug
    */
        enum TypeDebug
        {
                INFO,
                WARNING,
                CRITICAL,
                FATAL
        };
        Debug(TypeDebug type);
        //Debug output 
        Debug(TypeDebug type, QString msg);
        Debug(TypeDebug type, const QStringList & t);
        Debug(TypeDebug type, int number);
        Debug(TypeDebug type, double number);
        Debug(TypeDebug type, float number);
        Debug(TypeDebug type, long number);
        Debug(TypeDebug type, bool ok);
        Debug(TypeDebug type, const char* msg);
        template <typename D>
        Debug(TypeDebug type, QList<D> & list);
    
private:
        TypeDebug m_type;
};
#endif // DEBUG_H
