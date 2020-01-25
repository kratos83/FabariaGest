/****************************************************************************
**
** Copyright (C) 2006-2018 Angelo e Calogero Scarna
** Contact: Angelo Scarnà (angelo.scarna@codelinsoft.it)
**          Calogero Scarnà (calogero.scarna@codelinsoft.it)
**          Team Codelinsoft (info@codelinsoft.it)
**
** This file is part of the project FabariaGest
**
** LGPLv3 License
**
**  You may use this file under the terms of the LGPLv3 license as follows:
*
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Codelinsoft and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/
#include "IconFileProvider.h"

IconFileProvider::IconFileProvider()
{
    extImage.clear();
	QString tmp[] = {"eps", "epsi", "gif", "png", "jpg", "jpeg", "xpm", "tif", "tiff", "bmp", "pbm", "pgm", "ppm", "xbm", "xpm", "psd", "pat"};
	size_t array = sizeof(tmp) / sizeof(*tmp);
	for (uint a = 0; a < array; ++a)
		extImage.append(tmp[a]);
    iconPdf = QIcon(":/images/mimetype/pdf.png"); //File pdf
    iconWord = QIcon(":/images/mimetype/word.png"); //File word
    iconExcel = QIcon(":/images/mimetype/excel.png"); //File excel
    iconPower = QIcon(":/images/mimetype/powerpoint.png"); //File powerpoint
    iconText = QIcon(":/images/mimetype/text.png"); //File text
    iconGeneric = QIcon(":/images/mimetype/null.png");   //File vuoto
    iconArchive = QIcon(":/images/mimetype/generic.png");   //File zip
    iconImg = QIcon(":/images/mimetype/image-generic.png"); //File Immagine
    iconApp = QIcon(":/images/mimetype/application.png");   //File application
    iconSound = QIcon(":/images/mimetype/sound.png");  //File suono
}

QIcon IconFileProvider::getIcon(const QFileInfo& info) const
{
    QString ext = info.suffix().toLower();
	if (ext.isEmpty())
		return iconGeneric;
    if (extImage.contains(ext, Qt::CaseInsensitive))
		return iconImg;
	else
	{
		ext = info.completeSuffix().toLower();
		if (ext.endsWith("pdf", Qt::CaseInsensitive))
			return iconPdf;
		else if (ext.endsWith("doc", Qt::CaseInsensitive) || ext.endsWith("docx", Qt::CaseInsensitive ) || 
                ext.endsWith("odt", Qt::CaseInsensitive ))
			return iconWord;
		else if (ext.endsWith("xls", Qt::CaseInsensitive) || ext.endsWith("xlsx", Qt::CaseInsensitive)
                || ext.endsWith("ods", Qt::CaseInsensitive) )
			return iconExcel;
		else if (ext.endsWith("ppt", Qt::CaseInsensitive) || ext.endsWith("pptx", Qt::CaseInsensitive)
                || ext.endsWith("odp", Qt::CaseInsensitive) )
			return iconPower;
		else if (ext.endsWith("txt", Qt::CaseInsensitive))
			return iconText;
		else if (ext.endsWith("zip", Qt::CaseInsensitive) || ext.endsWith("rar", Qt::CaseInsensitive)
                || ext.endsWith("tar", Qt::CaseInsensitive) || ext.endsWith("tar.gz", Qt::CaseInsensitive)
                || ext.endsWith("tar.bz2", Qt::CaseInsensitive))
			return iconArchive;
		else if (ext.endsWith("exe", Qt::CaseInsensitive) || ext.endsWith("run", Qt::CaseInsensitive))
			return iconApp;
		else if (ext.endsWith("wma", Qt::CaseInsensitive) || ext.endsWith("ogg", Qt::CaseInsensitive)
                || ext.endsWith("mp3", Qt::CaseInsensitive))
			return iconSound;
		else
			return QFileIconProvider::icon(info);
	}
	return QIcon();
}

