#include "decompress.h"

decompress::decompress(QObject* parent): QObject(parent)
{

}

decompress::~decompress()
{
  
}

void decompress::exttractFile(QString zipFile, QString destDir)
{
  //Estraggo i file
  JlCompress::extractDir(zipFile,destDir);
}


