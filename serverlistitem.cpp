/***********************************************************************************
**
** ServerListItem.cpp
**
** Copyright (C) December 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "ServerListItem.h"
//----------------------------------------------------------------------------------
CServerListItem::CServerListItem(const QString &name)
: QListWidgetItem(name)
{
}
//----------------------------------------------------------------------------------
CServerListItem::CServerListItem(const QString &name, const QString &address, const QString &account, const QString &password, const bool &encrypted)
: QListWidgetItem(name), m_Address(address), m_Account(account), m_Password(password),
m_Command(""), m_Encrypted(encrypted)
{
}
//----------------------------------------------------------------------------------
