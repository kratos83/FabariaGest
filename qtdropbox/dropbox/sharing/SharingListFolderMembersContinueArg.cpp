/**********************************************************
 DO NOT EDIT
 This file was generated from stone specification "sharing"
 www.prokarpaty.net
***********************************************************/

#include "dropbox/sharing/SharingListFolderMembersContinueArg.h"
using namespace dropboxQt;

namespace dropboxQt{

namespace sharing{
///ListFolderMembersContinueArg

ListFolderMembersContinueArg::operator QJsonObject()const{
    QJsonObject js;
    this->toJson(js);
    return js;
}


void ListFolderMembersContinueArg::toJson(QJsonObject& js)const{

    if(!m_cursor.isEmpty())
        js["cursor"] = QString(m_cursor);
}

void ListFolderMembersContinueArg::fromJson(const QJsonObject& js){

    m_cursor = js["cursor"].toString();
}

QString ListFolderMembersContinueArg::toString(bool multiline)const
{
    QJsonObject js;
    toJson(js);
    QJsonDocument doc(js);
    QString s(doc.toJson(multiline ? QJsonDocument::Indented : QJsonDocument::Compact));
    return s;
}


std::unique_ptr<ListFolderMembersContinueArg>  ListFolderMembersContinueArg::factory::create(const QByteArray& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject js = doc.object();
    return create(js);
}


std::unique_ptr<ListFolderMembersContinueArg>  ListFolderMembersContinueArg::factory::create(const QJsonObject& js)
{
    std::unique_ptr<ListFolderMembersContinueArg> rv;
    rv = std::unique_ptr<ListFolderMembersContinueArg>(new ListFolderMembersContinueArg);
    rv->fromJson(js);
    return rv;
}

}//sharing
}//dropboxQt
