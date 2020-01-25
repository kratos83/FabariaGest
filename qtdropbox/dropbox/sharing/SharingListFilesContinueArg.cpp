/**********************************************************
 DO NOT EDIT
 This file was generated from stone specification "sharing"
 www.prokarpaty.net
***********************************************************/

#include "dropbox/sharing/SharingListFilesContinueArg.h"
using namespace dropboxQt;

namespace dropboxQt{

namespace sharing{
///ListFilesContinueArg

ListFilesContinueArg::operator QJsonObject()const{
    QJsonObject js;
    this->toJson(js);
    return js;
}


void ListFilesContinueArg::toJson(QJsonObject& js)const{

    if(!m_cursor.isEmpty())
        js["cursor"] = QString(m_cursor);
}

void ListFilesContinueArg::fromJson(const QJsonObject& js){

    m_cursor = js["cursor"].toString();
}

QString ListFilesContinueArg::toString(bool multiline)const
{
    QJsonObject js;
    toJson(js);
    QJsonDocument doc(js);
    QString s(doc.toJson(multiline ? QJsonDocument::Indented : QJsonDocument::Compact));
    return s;
}


std::unique_ptr<ListFilesContinueArg>  ListFilesContinueArg::factory::create(const QByteArray& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject js = doc.object();
    return create(js);
}


std::unique_ptr<ListFilesContinueArg>  ListFilesContinueArg::factory::create(const QJsonObject& js)
{
    std::unique_ptr<ListFilesContinueArg> rv;
    rv = std::unique_ptr<ListFilesContinueArg>(new ListFilesContinueArg);
    rv->fromJson(js);
    return rv;
}

}//sharing
}//dropboxQt
