#include "decompress.h"
#include <QtCore/QtCore>

int main(int argc, char * argv[])
{
   decompress dec;
   dec.exttractFile("file.zip",QDir::currentPath());
   return 0;
}
