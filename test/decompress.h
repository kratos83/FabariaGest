#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <QtCore/QtCore>
#include "../quazip/JlCompress.h"

class decompress : public QObject
{
  Q_OBJECT
  
public:
    explicit decompress(QObject *parent=0);
    ~decompress();

public slots:
  void exttractFile(QString zipFile, QString destDir);
};

#endif // DECOMPRESS_H
