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
#include "image_resize.h"

image_resize::image_resize()
{
}

image_resize::image_resize(QImage qImage)
{
    image = qImage;
}

QPixmap image_resize::getPixmap()
{
    return QPixmap::fromImage(image);
}

image_resize::image_resize(QString fileName):image(fileName)
{
}

image_resize image_resize::resizeAverage(int newwidth, int newheight)
{

    if(newwidth < 1) newwidth=1;
    if(newheight < 1) newheight=1;
    QImage newmi(newwidth,newheight,image.format());
    int width=image.width();
    int height=image.height();

    float scaley=((float)height-1)/(newheight-1);
    float scalex=((float)width-1)/(newwidth-1);

    for(int newy=0;newy<newheight;newy++)
        for(int newx=0;newx<newwidth;newx++)
        {
            float x=newx*scalex;
            float y=newy*scaley;
            float xn= x+1<width? x+1:x; //x+1
            float yn= y+1<height? y+1:y; //y+1
            int ne=image.pixel(x,y);
            int no=image.pixel(xn,y);
            int se=image.pixel(x,yn);
            int so=image.pixel(xn,yn);

            //Average
            int red=round(qRed(ne)*.25f+qRed(no)*.25f+qRed(se)*.25f+qRed(so)*.25f);
            int green=round(qGreen(ne)*.25f+qGreen(no)*.25f+qGreen(se)*.25f+qGreen(so)*.25f);
            int blue=round(qBlue(ne)*.25f+qBlue(no)*.25f+qBlue(se)*.25f+qBlue(so)*.25f);
            int black = round(qAlpha(ne)*.25f+qAlpha(no)*.25f+qAlpha(se)*.25f+qAlpha(so)*.25f);
            newmi.setPixel(newx,newy,qRgba(red,green,blue,black));

        }
    return image_resize(newmi);
}

image_resize image_resize::resizeBilinear(int newwidth, int newheight)
{
    if(newwidth < 1) newwidth=1;
    if(newheight < 1) newheight=1;
    QImage newmi(newwidth,newheight,image.format());
    int width=image.width();
    int height=image.height();

    float scaley=((float)height-1)/(newheight-1);
    float scalex=((float)width-1)/(newwidth-1);

    for(int newy=0;newy<newheight;newy++)
        for(int newx=0;newx<newwidth;newx++)
        {
            float x=newx*scalex;
            float y=newy*scaley;
            float xn= x+1<width? x+1:x; //x+1
            float yn= y+1<height? y+1:y; //y+1
            float dx=x-floor(x);
            float dy=y-floor(y);
            int ne=image.pixel(x,y);
            int no=image.pixel(xn,y);
            int se=image.pixel(x,yn);
            int so=image.pixel(xn,yn);

            //Weights
            float wne=dx*dy;
            float wno=(1-dx)*dy;
            float wse=dx*(1-dy);
            float wso=(1-dx)*(1-dy);

            //Calculate RGB Values
            int red=round(qRed(ne)*wne+qRed(no)*wno+qRed(se)*wse+qRed(so)*wso);
            int green=round(qGreen(ne)*wne+qGreen(no)*wno+qGreen(se)*wse+qGreen(so)*wso);
            int blue=round(qBlue(ne)*wne+qBlue(no)*wno+qBlue(se)*wse+qBlue(so)*wso);
            int black = round(qAlpha(ne)*.25f+qAlpha(no)*.25f+qAlpha(se)*.25f+qAlpha(so)*.25f);
            newmi.setPixel(newx,newy,qRgba(red,green,blue,black));
        }

    return image_resize(newmi);
}
