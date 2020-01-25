#include "Debug.h"

Debug::Debug(TypeDebug type)
{
    m_type = type;
}

Debug::Debug(TypeDebug type, QString msg)
{
    switch (type){
        case INFO:
            fprintf(stderr, FGRN("Info::FabariaGest: %s\n"), msg.toStdString().c_str());
            break;
        case WARNING:
            fprintf(stderr, FYEL("Warning::FabariaGest: %s\n"), msg.toStdString().c_str());
            break;
        case CRITICAL:
            fprintf(stderr, FRED("Critical::FabariaGest: %s\n"), msg.toStdString().c_str());
            break;
        case FATAL:
            fprintf(stderr, FCYN("Fatal::FabariaGest: %s\n"), msg.toStdString().c_str());
            abort();
    }
}

Debug::Debug(TypeDebug type, const QStringList & t)
{
    switch (type) {
        case INFO:{
            foreach(QString a, t){
                fprintf(stderr, FGRN("Info::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            break;
        }
        case WARNING:{
            foreach(QString a, t){
                fprintf(stderr, FYEL("Warning::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            break;
        }
        case CRITICAL:{
            foreach(QString a, t){
                fprintf(stderr, FRED("Critical::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            break;
        }
        case FATAL:{
            foreach(QString a, t){
                fprintf(stderr, FCYN("Fatal::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            abort();
        }
    }
}

Debug::Debug(TypeDebug type, int number)
{
    switch (type) {
        case INFO:
            fprintf(stderr, FGRN("Info::FabariaGest: %d\n"), number);
            break;
        case WARNING:
            fprintf(stderr, FYEL("Warning::FabariaGest: %d\n"), number);
            break;
        case CRITICAL:
            fprintf(stderr, FRED("Critical::FabariaGest: %d\n"), number);
            break;
        case FATAL:
            fprintf(stderr, FCYN("Fatal::FabariaGest: %d\n"), number);
            abort();
    }
}

Debug::Debug(TypeDebug type, double number)
{
     switch (type) {
        case INFO:
            fprintf(stderr, FGRN("Info::FabariaGest: %lf\n"), number);
            break;
        case WARNING:
            fprintf(stderr, FYEL("Warning::FabariaGest: %lf\n"), number);
            break;
        case CRITICAL:
            fprintf(stderr, FRED("Critical::FabariaGest: %lf\n"), number);
            break;
        case FATAL:
            fprintf(stderr, FCYN("Fatal::FabariaGest: %lf\n"), number);
            abort();
    }
}

Debug::Debug(TypeDebug type, float number)
{
     switch (type) {
        case INFO:
            fprintf(stderr, FGRN("Info::FabariaGest: %f\n"), number);
            break;
        case WARNING:
            fprintf(stderr, FYEL("Warning::FabariaGest: %f\n"), number);
            break;
        case CRITICAL:
            fprintf(stderr, FRED("Critical::FabariaGest: %f\n"), number);
            break;
        case FATAL:
            fprintf(stderr, FCYN("Fatal::FabariaGest: %f\n"), number);
            abort();
    }
}

Debug::Debug(TypeDebug type, long number)
{
    switch (type) {
        case INFO:
            fprintf(stderr, FGRN("Info::FabariaGest: %ld\n"), number);
            break;
        case WARNING:
            fprintf(stderr, FYEL("Warning::FabariaGest: %ld\n"), number);
            break;
        case CRITICAL:
            fprintf(stderr, FRED("Critical::FabariaGest: %ld\n"), number);
            break;
        case FATAL:
            fprintf(stderr, FCYN("Fatal::FabariaGest: %ld\n"), number);
            abort();
    }
}


Debug::Debug(TypeDebug type, bool ok)
{
     switch (type) {
        case INFO:
            fprintf(stderr, FGRN("Info::FabariaGest: %s\n"), ok ? "true" : "false");
            break;
        case WARNING:
            fprintf(stderr, FYEL("Warning::FabariaGest: %s\n"), ok ? "true" : "false");
            break;
        case CRITICAL:
            fprintf(stderr, FRED("Critical::FabariaGest: %s\n"), ok ? "true" : "false");
            break;
        case FATAL:
            fprintf(stderr, FCYN("Fatal::FabariaGest: %s\n"), ok ? "true" : "false");
            abort();
    }
}

Debug::Debug(TypeDebug type, const char* msg)
{
    switch (type) {
        case INFO:
            fprintf(stderr, FGRN("Info::FabariaGest: %s\n"), msg);
            break;
        case WARNING:
            fprintf(stderr, FYEL("Warning::FabariaGest: %s\n"), msg);
            break;
        case CRITICAL:
            fprintf(stderr, FRED("Critical::FabariaGest: %s\n"), msg);
            break;
        case FATAL:
            fprintf(stderr, FCYN("Fatal::FabariaGest: %s\n"), msg);
            abort();
    }
}

template <typename D>
Debug::Debug(TypeDebug type, QList<D> & list)
{
    switch (type) {
        case INFO:{
            foreach(QString a, list){
                fprintf(stderr, FGRN("Info::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            break;
        }
        case WARNING:{
            foreach(QString a,list){
                fprintf(stderr, FYEL("Warning::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            break;
        }
        case CRITICAL:{
            foreach(QString a, list){
                fprintf(stderr, FRED("Critical::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            break;
        }
        case FATAL:{
            foreach(QString a, list){
                fprintf(stderr, FCYN("Fatal::FabariaGest: %s\n"), a.toStdString().c_str());
            }
            abort();
        }
    }
}
