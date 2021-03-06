/**********************************************************
 DO NOT EDIT
 This file was generated from stone specification "team"
 www.prokarpaty.net
***********************************************************/

#include "dropbox/team/TeamGroupMemberSelectorError.h"

namespace dropboxQt{

namespace team{
///GroupMemberSelectorError

GroupMemberSelectorError::operator QJsonObject()const{
    QJsonObject js;
    this->toJson(js, "");
    return js;
}


void GroupMemberSelectorError::toJson(QJsonObject& js, QString name)const{

    switch(m_tag){
        case GroupSelectorError_GROUP_NOT_FOUND:{
            if(!name.isEmpty())
                js[name] = QString("group_not_found");
        }break;
        case GroupSelectorError_OTHER:{
            if(!name.isEmpty())
                js[name] = QString("other");
        }break;
        case GroupMemberSelectorError_MEMBER_NOT_IN_GROUP:{
            if(!name.isEmpty())
                js[name] = QString("member_not_in_group");
        }break;
    }//switch
}

void GroupMemberSelectorError::fromJson(const QJsonObject& js){

    QString s = js[".tag"].toString();
    if(s.compare("group_not_found") == 0){
        m_tag = GroupSelectorError_GROUP_NOT_FOUND;

    }
    else if(s.compare("other") == 0){
        m_tag = GroupSelectorError_OTHER;

    }
    if(s.compare("member_not_in_group") == 0){
        m_tag = GroupMemberSelectorError_MEMBER_NOT_IN_GROUP;

    }
}

QString GroupMemberSelectorError::toString(bool multiline)const
{
    QJsonObject js;
    toJson(js, "GroupMemberSelectorError");
    QJsonDocument doc(js);
    QString s(doc.toJson(multiline ? QJsonDocument::Indented : QJsonDocument::Compact));
    return s;
}

std::unique_ptr<GroupMemberSelectorError>  GroupMemberSelectorError::factory::create(const QByteArray& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject js = doc.object();
    std::unique_ptr<GroupMemberSelectorError> rv = std::unique_ptr<GroupMemberSelectorError>(new GroupMemberSelectorError);
    rv->fromJson(js);
    return rv;
}

}//team
}//dropboxQt
