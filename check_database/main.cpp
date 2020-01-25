#include "check_database.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    check_database w;
    w.show();

    return a.exec();
}
