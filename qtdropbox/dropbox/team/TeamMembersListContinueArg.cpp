/**********************************************************
 DO NOT EDIT
 This file was generated from stone specification "team"
 www.prokarpaty.net
***********************************************************/

#include "dropbox/team/TeamMembersListContinueArg.h"
using namespace dropboxQt;

namespace dropboxQt{

namespace team{
///MembersListContinueArg

MembersListContinueArg::operator QJsonObject()const{
    QJsonObject js;
    this->toJson(js);
    return js;
}


void MembersListContinueArg::toJson(QJsonObject& js)const{

    if(!m_cursor.isEmpty())
        js["cursor"] = QString(m_cursor);
}

void MembersListContinueArg::fromJson(const QJsonObject& js){

    m_cursor = js["cursor"].toString();
}

QString MembersListContinueArg::toString(bool multiline)const
{
    QJsonObject js;
    toJson(js);
    QJsonDocument doc(js);
    QString s(doc.toJson(multiline ? QJsonDocument::Indented : QJsonDocument::Compact));
    return s;
}


std::unique_ptr<MembersListContinueArg>  MembersListContinueArg::factory::create(const QByteArray& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject js = doc.object();
    return create(js);
}


std::unique_ptr<MembersListContinueArg>  MembersListContinueArg::factory::create(const QJsonObject& js)
{
    std::unique_ptr<MembersListContinueArg> rv;
    rv = std::unique_ptr<MembersListContinueArg>(new MembersListContinueArg);
    rv->fromJson(js);
    return rv;
}

}//team
}//dropboxQt
