/***********************************************************************************
**
** ServerListItem.h
**
** Copyright (C) October 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef SERVERLISTITEM_H
#define SERVERLISTITEM_H
//----------------------------------------------------------------------------------
#include <QListWidgetItem>
//----------------------------------------------------------------------------------
class CServerListItem : public QListWidgetItem
{
private:
	QString m_Address{""};
	QString m_Account{""};
	QString m_Password{""};
	QString m_Command{""};
	bool m_Encrypted{false};
	bool m_UseProxy{false};
	QString m_Proxy{""};

public:
	CServerListItem(const QString &name);
	CServerListItem(const QString &name, const QString &address, const QString &account, const QString &password, const bool &encrypted);
	virtual ~CServerListItem() {}

	QString GetAddress() const { return m_Address; }
	void SetAddress(const QString &address) { m_Address = address; }

	QString GetAccount() const { return m_Account; }
	void SetAccount(const QString &account) { m_Account = account; }

	QString GetPassword() const { return m_Password; }
	void SetPassword(const QString &password) { m_Password = password; }

	QString GetCommand() const { return m_Command; }
	void SetCommand(const QString &command) { m_Command = command; }

	bool GetEncrypted() const { return m_Encrypted; }
	void SetEncrypted(const bool &encrypted) { m_Encrypted = encrypted; }

	bool GetUseProxy() const { return m_UseProxy; }
	void SetUseProxy(const bool &useProxy) { m_UseProxy = useProxy; }

	QString GetProxy() const { return m_Proxy; }
	void SetProxy(const QString &proxy) { m_Proxy = proxy; }
};
//----------------------------------------------------------------------------------
#endif // SERVERLISTITEM_H
//----------------------------------------------------------------------------------
