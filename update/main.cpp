#include <QApplication>
#include <QStringList>
#include <QTranslator>
#include <QObject>
#include "update.h"
#include "settingsmanager.h"
#include <stdio.h>

static bool parseArguments(const QStringList &argomento)
{
    QStringListIterator arg_cmd(argomento);
    arg_cmd.next();
    while(arg_cmd.hasNext())
    {
        const QString &a_arg = arg_cmd.next().toLower();
        if(a_arg == "-p" || a_arg == "--package"){
            return true;
        }
        else if(a_arg == "-u" || a_arg == "--url"){
            return true;
        }
        else if(a_arg == "-v" || a_arg == "--version"){
            qDebug() << QString::fromUtf8(QObject::tr("La versione del software installato è: ").toStdString().c_str()) << "1.0";
            return false;
        }
        else if(a_arg == "-h" || a_arg == "--help")
        {
            qDebug() << QObject::tr("USA LA LINEA DI COMANDO:\t ") << QObject::tr("VISUALIZZA\n")
                     << QObject::tr("-p or --package \t") << QObject::tr("Selezione del pacchetto da scaricare\n")
                     << QObject::tr("-u or --url \t\t") << QObject::tr("Selezione dell'indirizzo internet:\n"
                                            "\t\t\t ESEMPIO: http://\n\n\n")
                     << QObject::tr("VISUALIZZA LA VERSIONE INSTALLATA DEL SOFTWARE:\n\n")
                     << QObject::tr("-v or --version \t") << QObject::tr("Versione del software\n\n\n")
                     << QObject::tr("VISUALIZZA LE INFORMAZIONI DEL PROGRAMMA:\n\n")
                     << QObject::tr("-h or --help \t\t") << QObject::tr("Informazioni del software.");
            return false;
        }
        else{
            qDebug() << QObject::tr("Comando non trovato: ") << a_arg;
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    SettingsManager *manager = new SettingsManager;

    /*
     * Translate QDialog
     */
    QTranslator translator;
    translator.load(":/language/"+manager->generalValue("Language/language",QVariant()).toString()+".qm");
    app.installTranslator(&translator);


    if(!parseArguments(app.arguments()))
    {
        return -1;
    }

    const QString arg1 = argv[1];

    const QString arg3 = argv[3];

    if((arg1.toLower() == "-u" || arg1.toLower() == "--url") && (arg3.toLower() == "-p" || arg3.toLower() == "--package")){
        QWidget *wid= new QWidget;
        update *manager = new update(wid,argv[2],argv[4]);
        manager->show();
    }
    else
    {
        parseArguments(app.arguments() << "-h");
        exit(0);
    }

    return app.exec();
}
