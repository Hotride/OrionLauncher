/***********************************************************************************
**
** OrionLauncherWindow.cpp
**
** Copyright (C) December 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "orionlauncherwindow.h"
#include "ui_orionlauncherwindow.h"
#include <QMessageBox>
#include "ServerListItem.h"
#include "ProxyListItem.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDir>
#include <QFileDialog>
#include <QProcess>
#include <windows.h>
#include <Wininet.h>
#include "qzipreader_p.h"
#include <QtConcurrent>
#include <QDebug>

OrionLauncherWindow *g_OrionLauncherWindow = nullptr;
//----------------------------------------------------------------------------------
OrionLauncherWindow::OrionLauncherWindow(QWidget *parent)
: QMainWindow(parent), ui(new Ui::OrionLauncherWindow)
{
	ui->setupUi(this);

	g_OrionLauncherWindow = this;

	qRegisterMetaType<QList<CUpdateInfo>>("QList<CUpdateInfo>");
	qRegisterMetaType<QList<CBackupInfo>>("QList<CBackupInfo>");

	connect(this, SIGNAL(signal_UpdatesListReceived(QList<CUpdateInfo>)), this, SLOT(slot_UpdatesListReceived(QList<CUpdateInfo>)));
	connect(this, SIGNAL(signal_BackupsListReceived(QList<CBackupInfo>)), this, SLOT(slot_BackupsListReceived(QList<CBackupInfo>)));
	connect(this, SIGNAL(signal_FileReceived(QByteArray, QString)), this, SLOT(slot_FileReceived(QByteArray, QString)));
	connect(this, SIGNAL(signal_FileReceivedNotification(QString)), this, SLOT(slot_FileReceivedNotification(QString)));
	connect(&m_UpdatesTimer, SIGNAL(timeout()), this, SLOT(slot_OnUpdatesTimer()));

	setFixedSize(size());

	LoadProxyList();
	LoadServerList();

	ui->tw_Main->setCurrentIndex(0);
	ui->tw_Server->setCurrentIndex(0);

	UpdateOAFecturesCode();

	QString crc32 = "";
	QString version = "";

	CUpdateManager<OrionLauncherWindow>::GetFileInfo(qApp->applicationFilePath(), version, crc32);

	setWindowTitle("Orion launcher " + version);

	m_Loading = false;

	if (!ui->cb_OrionPath->currentText().length())
		on_tb_SetOrionPath_clicked();

	on_cb_OrionPath_currentIndexChanged(ui->cb_OrionPath->currentIndex());

	m_UpdatesTimer.start(15 * 60 * 1000);
}
//----------------------------------------------------------------------------------
OrionLauncherWindow::~OrionLauncherWindow()
{
	g_OrionLauncherWindow = nullptr;

	delete ui;

	if (m_ChangelogForm != nullptr)
	{
		delete m_ChangelogForm;
		m_ChangelogForm = nullptr;
	}

	m_UpdatesTimer.stop();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::slot_OnUpdatesTimer()
{
	if (ui->cb_CheckUpdates->isChecked())
		on_pb_CheckUpdates_clicked();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::closeEvent(QCloseEvent *event)
{
	SaveServerList();
	SaveProxyList();

	if (m_ChangelogForm != nullptr)
		m_ChangelogForm->close();

	event->accept();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
		return;

	if (event->key() == Qt::Key_Delete)
	{
		QWidget *focused = QApplication::focusWidget();

		if (focused == ui->lw_ServerList) //Servers list
			on_pb_ServerRemove_clicked();
		else if (focused == ui->lw_ProxyList) //Proxy list
			on_pb_ProxyRemove_clicked();
	}

	event->accept();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_lw_ServerList_clicked(const QModelIndex &index)
{
	UpdateServerFields(index.row());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::UpdateServerFields(const int &index)
{
	CServerListItem *item = (CServerListItem*)ui->lw_ServerList->item(index);

	if (item != nullptr)
	{
		ui->le_ServerName->setText(item->text());
		ui->le_ServerAddress->setText(item->GetAddress());
		ui->le_ServerAccount->setText(item->GetAccount());
		ui->le_ServerPassword->setText(item->GetPassword());
		ui->le_ServeCharacter->setText(item->GetCharacter());
		ui->cb_ServerEncryptPassword->setChecked(item->GetEncrypted());
		ui->le_CommandLine->setText(item->GetCommand());

		ui->cb_LaunchAutologin->setChecked(item->GetOptionAutologin());
		ui->cb_LaunchSavePassword->setChecked(item->GetOptionSavePassword());
		ui->cb_LaunchSaveAero->setChecked(item->GetOptionSaveAero());
		ui->cb_LaunchFastLogin->setChecked(item->GetOptionFastLogin());
		ui->cb_LaunchRunUOAM->setChecked(item->GetOptionRunUOAM());

		ui->cb_ServerUseProxy->setChecked(item->GetUseProxy());
		ui->cb_ServerProxy->setCurrentText(item->GetProxy());
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_lw_ServerList_doubleClicked(const QModelIndex &index)
{
	Q_UNUSED(index);

	on_pb_Launch_clicked();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_ServerShowPassword_clicked()
{
	if (ui->cb_ServerShowPassword->isChecked())
		ui->le_ServerPassword->setEchoMode(QLineEdit::EchoMode::Normal);
	else
		ui->le_ServerPassword->setEchoMode(QLineEdit::EchoMode::Password);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ServerAdd_clicked()
{
	QString name = ui->le_ServerName->text().toLower();

	if (!name.length())
	{
		QMessageBox::critical(this, "Name is empty", "Enter the server name!");
		return;
	}

	for (int i = 0; i < ui->lw_ServerList->count(); i++)
	{
		QListWidgetItem *item = ui->lw_ServerList->item(i);

		if (item != nullptr && item->text().toLower() == name)
		{
			QMessageBox::critical(this, "Name is already exists", "Server name is already exists!");
			return;
		}
	}

	CServerListItem *item = new CServerListItem(ui->le_ServerName->text(), ui->le_ServerAddress->text(), ui->le_ServerAccount->text(), ui->le_ServerPassword->text(), ui->le_ServeCharacter->text(), ui->cb_ServerEncryptPassword->isChecked());
	item->SetUseProxy(ui->cb_ServerUseProxy->isChecked());
	item->SetProxy(ui->cb_ServerProxy->currentText());

	ui->lw_ServerList->addItem(item);

	ui->lw_ServerList->setCurrentRow(ui->lw_ServerList->count() - 1);

	SaveServerList();

	UpdateServerFields(ui->lw_ServerList->count() - 1);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ServerSave_clicked()
{
	QString name = ui->le_ServerName->text().toLower();

	if (!name.length())
	{
		QMessageBox::critical(this, "Name is empty", "Enter the server name!");
		return;
	}

	CServerListItem *selected = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (selected == nullptr)
	{
		QMessageBox::critical(this, "No selected item", "No selected server!");
		return;
	}

	for (int i = 0; i < ui->lw_ServerList->count(); i++)
	{
		QListWidgetItem *item = ui->lw_ServerList->item(i);

		if (item != nullptr && item->text().toLower() == name)
		{
			if (item != nullptr && item != selected)
			{
				QMessageBox::critical(this, "Name is already exists", "Server name is already exists (not this item)!");
				return;
			}

			break;
		}
	}

	selected->setText(ui->le_ServerName->text());
	selected->SetAddress(ui->le_ServerAddress->text());
	selected->SetAccount(ui->le_ServerAccount->text());
	selected->SetCharacter(ui->le_ServeCharacter->text());
	selected->SetPassword(ui->le_ServerPassword->text());
	selected->SetEncrypted(ui->cb_ServerEncryptPassword->isChecked());
	selected->SetUseProxy(ui->cb_ServerUseProxy->isChecked());
	selected->SetProxy(ui->cb_ServerProxy->currentText());

	SaveServerList();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ServerRemove_clicked()
{
	QListWidgetItem *item = ui->lw_ServerList->currentItem();

	if (item != nullptr)
	{
		item = ui->lw_ServerList->takeItem(ui->lw_ServerList->row(item));

		if (item != nullptr)
		{
			delete item;

			SaveServerList();
		}
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_le_CommandLine_textChanged(const QString &arg1)
{
	CServerListItem *selected = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (selected != nullptr)
		selected->SetCommand(arg1);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_lw_ProxyList_clicked(const QModelIndex &index)
{
	CProxyListItem *item = (CProxyListItem*)ui->lw_ProxyList->item(index.row());

	if (item != nullptr)
	{
		ui->le_ProxyName->setText(item->text());
		ui->le_ProxyAddress->setText(item->GetAddress());
		ui->le_ProxyPort->setText(item->GetProxyPort());
		ui->gb_ProxySocks5->setChecked(item->GetSocks5());
		ui->le_ProxyAccount->setText(item->GetAccount());
		ui->le_ProxyPassword->setText(item->GetPassword());
		ui->cb_ProxyEncryptPassword->setChecked(item->GetEncrypted());
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_ProxyShowPassword_clicked()
{
	if (ui->cb_ProxyShowPassword->isChecked())
		ui->le_ProxyPassword->setEchoMode(QLineEdit::EchoMode::Normal);
	else
		ui->le_ProxyPassword->setEchoMode(QLineEdit::EchoMode::Password);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ProxyAdd_clicked()
{
	QString name = ui->le_ProxyName->text().toLower();

	if (!name.length())
	{
		QMessageBox::critical(this, "Name is empty", "Enter the proxy server name!");
		return;
	}

	for (int i = 0; i < ui->lw_ProxyList->count(); i++)
	{
		QListWidgetItem *item = ui->lw_ProxyList->item(i);

		if (item != nullptr && item->text().toLower() == name)
		{
			QMessageBox::critical(this, "Name is already exists", "Proxy server name is already exists!");
			return;
		}
	}

	ui->lw_ProxyList->addItem(new CProxyListItem(ui->le_ProxyName->text(), ui->le_ProxyAddress->text(), ui->le_ProxyPort->text(), ui->gb_ProxySocks5->isChecked(), ui->le_ProxyAccount->text(), ui->le_ProxyPassword->text(), ui->cb_ProxyEncryptPassword->isChecked()));

	ui->lw_ProxyList->setCurrentRow(ui->lw_ProxyList->count() - 1);

	SaveProxyList();

	ui->cb_ServerProxy->addItem(ui->le_ProxyName->text());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ProxySave_clicked()
{
	QString name = ui->le_ProxyName->text().toLower();

	if (!name.length())
	{
		QMessageBox::critical(this, "Name is empty", "Enter the proxy server name!");
		return;
	}

	CProxyListItem *selected = (CProxyListItem*)ui->lw_ProxyList->currentItem();

	if (selected == nullptr)
	{
		QMessageBox::critical(this, "No selected item", "No selected proxy!");
		return;
	}

	for (int i = 0; i < ui->lw_ProxyList->count(); i++)
	{
		QListWidgetItem *item = ui->lw_ProxyList->item(i);

		if (item != nullptr && item->text().toLower() == name)
		{
			if (item != nullptr && item != selected)
			{
				QMessageBox::critical(this, "Name is already exists", "Proxy server name is already exists (not this item)!");
				return;
			}

			break;
		}
	}

	selected->setText(ui->le_ProxyName->text());
	selected->SetAddress(ui->le_ProxyAddress->text());
	selected->SetProxyPort(ui->le_ProxyPort->text());
	selected->SetSocks5(ui->gb_ProxySocks5->isChecked());
	selected->SetAccount(ui->le_ProxyAccount->text());
	selected->SetPassword(ui->le_ProxyPassword->text());
	selected->SetEncrypted(ui->cb_ProxyEncryptPassword->isChecked());

	SaveProxyList();

	ui->cb_ServerProxy->setItemText(ui->lw_ProxyList->currentRow(), ui->le_ProxyName->text());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ProxyRemove_clicked()
{
	QListWidgetItem *item = ui->lw_ProxyList->currentItem();

	if (item != nullptr)
	{
		int index = ui->lw_ProxyList->row(item);
		item = ui->lw_ProxyList->takeItem(index);

		if (item != nullptr)
		{
			ui->cb_ServerProxy->removeItem(index);

			delete item;

			SaveProxyList();
		}
	}
}
//----------------------------------------------------------------------------------
QString OrionLauncherWindow::BoolToText(const bool &value)
{
	if (value)
		return "true";

	return "false";
}
//----------------------------------------------------------------------------------
bool OrionLauncherWindow::RawStringToBool(QString value)
{
	value = value.toLower();
	bool result = false;

	if (value == "true" || value == "on")
		result = true;
	else
		result = (value.toInt() != 0);

	return result;
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::SaveProxyList()
{
	QFile file(QDir::currentPath() + "/Proxy.xml");

	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QXmlStreamWriter writter(&file);

		writter.setAutoFormatting(true);

		writter.writeStartDocument();

		int count = ui->lw_ProxyList->count();

		writter.writeStartElement("proxylist");
		writter.writeAttribute("version", "0");
		writter.writeAttribute("size", QString::number(count));

		for (int i = 0; i < count; i++)
		{
			CProxyListItem *item = (CProxyListItem*)ui->lw_ProxyList->item(i);

			if (item != nullptr)
			{
				writter.writeStartElement("proxy");

				writter.writeAttribute("name", item->text());
				writter.writeAttribute("address", item->GetAddress());
				writter.writeAttribute("port", item->GetProxyPort());
				writter.writeAttribute("socks5", BoolToText(item->GetSocks5()));
				writter.writeAttribute("account", item->GetAccount());
				writter.writeAttribute("password", item->GetPassword());
				writter.writeAttribute("encrypted", BoolToText(item->GetEncrypted()));

				writter.writeEndElement(); //proxy
			}
		}

		writter.writeEndElement(); //proxylist

		writter.writeEndDocument();

		file.close();
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::SaveServerList()
{
	QFile file(QDir::currentPath() + "/Server.xml");

	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QXmlStreamWriter writter(&file);

		writter.setAutoFormatting(true);

		writter.writeStartDocument();

		int count = ui->lw_ServerList->count();

		writter.writeStartElement("serverlist");
		writter.writeAttribute("version", "0");
		writter.writeAttribute("size", QString::number(count));
		writter.writeAttribute("clientindex", QString::number(ui->cb_OrionPath->currentIndex()));
		writter.writeAttribute("closeafterlaunch", BoolToText(ui->cb_LaunchCloseAfterLaunch->isChecked()));
		writter.writeAttribute("lastserver", QString::number(ui->lw_ServerList->currentRow()));
		writter.writeAttribute("checkupdates", BoolToText(ui->cb_CheckUpdates->isChecked()));
		writter.writeAttribute("changeloglanguage", ui->cb_ChangelogLanguage->currentText());
		writter.writeAttribute("noclientwarnings", BoolToText(ui->cb_NoClientWarnings->isChecked()));

		for (int i = 0; i < ui->cb_OrionPath->count(); i++)
		{
			writter.writeStartElement("clientpath");

			writter.writeAttribute("path", ui->cb_OrionPath->itemText(i));

			writter.writeEndElement(); //clientpath
		}

		for (int i = 0; i < count; i++)
		{
			CServerListItem *item = (CServerListItem*)ui->lw_ServerList->item(i);

			if (item != nullptr)
			{
				writter.writeStartElement("server");

				writter.writeAttribute("name", item->text());

				writter.writeAttribute("address", item->GetAddress());
				writter.writeAttribute("account", item->GetAccount());
				writter.writeAttribute("password", item->GetPassword());
				writter.writeAttribute("character", item->GetCharacter());
				writter.writeAttribute("command", item->GetCommand());
				writter.writeAttribute("encrypted", BoolToText(item->GetEncrypted()));
				writter.writeAttribute("useproxy", BoolToText(item->GetUseProxy()));
				writter.writeAttribute("proxyname", item->GetProxy());

				writter.writeAttribute("optionautologin", BoolToText(item->GetOptionAutologin()));
				writter.writeAttribute("optionsavepassword", BoolToText(item->GetOptionSavePassword()));
				writter.writeAttribute("optionsaveaero", BoolToText(item->GetOptionSaveAero()));
				writter.writeAttribute("optionfastlogin", BoolToText(item->GetOptionFastLogin()));
				writter.writeAttribute("optionrunuoam", BoolToText(item->GetOptionRunUOAM()));

				writter.writeEndElement(); //server
			}
		}

		writter.writeEndElement(); //serverlist

		writter.writeEndDocument();

		file.close();
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::LoadProxyList()
{
	ui->lw_ProxyList->clear();

	QFile file(QDir::currentPath() + "/Proxy.xml");

	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QXmlStreamReader reader(&file);

		int version = 0;
		int count = 0;

		Q_UNUSED(version);
		Q_UNUSED(count);

		while (!reader.atEnd() && !reader.hasError())
		{
			if (reader.isStartElement())
			{
				QXmlStreamAttributes attributes = reader.attributes();

				if (reader.name() == "proxylist")
				{
					if (attributes.hasAttribute("version"))
						version = attributes.value("version").toInt();

					if (attributes.hasAttribute("size"))
						count = attributes.value("size").toInt();
				}
				else if (reader.name() == "proxy")
				{
					if (attributes.hasAttribute("name"))
					{
						CProxyListItem *item = new CProxyListItem(attributes.value("name").toString());

						if (attributes.hasAttribute("address"))
							item->SetAddress(attributes.value("address").toString());

						if (attributes.hasAttribute("port"))
							item->SetProxyPort(attributes.value("port").toString());

						if (attributes.hasAttribute("socks5"))
							item->SetSocks5(RawStringToBool(attributes.value("socks5").toString()));

						if (attributes.hasAttribute("account"))
							item->SetAccount(attributes.value("account").toString());

						if (attributes.hasAttribute("password"))
							item->SetPassword(attributes.value("password").toString());

						if (attributes.hasAttribute("encrypted"))
							item->SetEncrypted(RawStringToBool(attributes.value("encrypted").toString()));

						ui->lw_ProxyList->addItem(item);
						ui->cb_ServerProxy->addItem(attributes.value("name").toString());
					}
				}
			}

			reader.readNext();
		}

		file.close();
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::LoadServerList()
{
	ui->lw_ServerList->clear();

	QFile file(QDir::currentPath() + "/Server.xml");

	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QXmlStreamReader reader(&file);

		int version = 0;
		int count = 0;
		int clientindex = -1;
		int lastServer = -1;

		ui->cb_OrionPath->clear();

		Q_UNUSED(version);
		Q_UNUSED(count);

		auto RemoveFileExtensionFunc =[](QString &str)
		{
			if (str.length() > 4 && str.lastIndexOf(".exe", -4, Qt::CaseInsensitive) == str.length() - 4)
			{
				int pos = qMax(str.lastIndexOf('/'), str.lastIndexOf('\\'));

				if (pos != -1)
					str.resize(pos);
			}
		};

		while (!reader.atEnd() && !reader.hasError())
		{
			if (reader.isStartElement())
			{
				QXmlStreamAttributes attributes = reader.attributes();

				if (reader.name() == "serverlist")
				{
					if (attributes.hasAttribute("version"))
						version = attributes.value("version").toInt();

					if (attributes.hasAttribute("size"))
						count = attributes.value("size").toInt();

					if (attributes.hasAttribute("clientindex"))
						clientindex = attributes.value("clientindex").toInt();

					if (attributes.hasAttribute("closeafterlaunch"))
						ui->cb_LaunchCloseAfterLaunch->setChecked(RawStringToBool(attributes.value("closeafterlaunch").toString()));

					if (attributes.hasAttribute("lastserver"))
						lastServer = attributes.value("lastserver").toInt();

					if (attributes.hasAttribute("path"))
					{
						QString path = attributes.value("path").toString().trimmed();

						RemoveFileExtensionFunc(path);

						ui->cb_OrionPath->addItem(path);
						clientindex = 0;
					}

					if (attributes.hasAttribute("checkupdates"))
						ui->cb_CheckUpdates->setChecked(RawStringToBool(attributes.value("checkupdates").toString()));

					if (attributes.hasAttribute("changeloglanguage"))
						ui->cb_ChangelogLanguage->setCurrentText(attributes.value("changeloglanguage").toString());

					if (attributes.hasAttribute("noclientwarnings"))
						ui->cb_NoClientWarnings->setChecked(RawStringToBool(attributes.value("noclientwarnings").toString()));
				}
				else if (reader.name() == "clientpath")
				{
					if (attributes.hasAttribute("path"))
					{
						QString path = attributes.value("path").toString().trimmed();

						RemoveFileExtensionFunc(path);

						bool found = false;

						for (int i = 0; i < ui->cb_OrionPath->count(); i++)
						{
							if (path == ui->cb_OrionPath->itemText(i))
							{
								found = true;
								break;
							}
						}

						if (!found)
							ui->cb_OrionPath->addItem(path);
					}
				}
				else if (reader.name() == "server")
				{
					if (attributes.hasAttribute("name"))
					{
						CServerListItem *item = new CServerListItem(attributes.value("name").toString());

						if (attributes.hasAttribute("address"))
							item->SetAddress(attributes.value("address").toString());

						if (attributes.hasAttribute("account"))
							item->SetAccount(attributes.value("account").toString());

						if (attributes.hasAttribute("password"))
							item->SetPassword(attributes.value("password").toString());

						if (attributes.hasAttribute("character"))
							item->SetCharacter(attributes.value("character").toString());

						if (attributes.hasAttribute("command"))
							item->SetCommand(attributes.value("command").toString());

						if (attributes.hasAttribute("encrypted"))
							item->SetEncrypted(RawStringToBool(attributes.value("encrypted").toString()));

						if (attributes.hasAttribute("useproxy"))
							item->SetUseProxy(RawStringToBool(attributes.value("useproxy").toString()));

						if (attributes.hasAttribute("proxyname"))
							item->SetProxy(attributes.value("proxyname").toString());

						if (attributes.hasAttribute("optionautologin"))
							item->SetOptionAutologin(RawStringToBool(attributes.value("optionautologin").toString()));

						if (attributes.hasAttribute("optionsavepassword"))
							item->SetOptionSavePassword(RawStringToBool(attributes.value("optionsavepassword").toString()));

						if (attributes.hasAttribute("optionsaveaero"))
							item->SetOptionSaveAero(RawStringToBool(attributes.value("optionsaveaero").toString()));

						if (attributes.hasAttribute("optionfastlogin"))
							item->SetOptionFastLogin(RawStringToBool(attributes.value("optionfastlogin").toString()));

						if (attributes.hasAttribute("optionrunuoam"))
							item->SetOptionRunUOAM(RawStringToBool(attributes.value("optionrunuoam").toString()));

						ui->lw_ServerList->addItem(item);
					}
				}
			}

			reader.readNext();
		}

		if (clientindex >= 0 && clientindex < ui->cb_OrionPath->count())
			ui->cb_OrionPath->setCurrentIndex(clientindex);

		if (lastServer >= 0 && lastServer < ui->lw_ServerList->count())
		{
			ui->lw_ServerList->setCurrentRow(lastServer);
			UpdateServerFields(lastServer);
		}

		file.close();
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_tb_SetOrionPath_clicked()
{
	QString startPath = ui->cb_OrionPath->currentText();

	if (!startPath.length())
		startPath = QDir::currentPath();

	QString path = QFileDialog::getExistingDirectory(nullptr, tr("Select OrionUO directory"), startPath);

	if (path.length())
	{
		for (int i = 0; i < ui->cb_OrionPath->count(); i++)
		{
			if (path == ui->cb_OrionPath->itemText(i))
			{
				ui->cb_OrionPath->setCurrentIndex(i);
				return;
			}
		}

		ui->cb_OrionPath->addItem(path);
		ui->cb_OrionPath->setCurrentIndex(ui->cb_OrionPath->count() - 1);
	}
}
//----------------------------------------------------------------------------------
QString OrionLauncherWindow::DecodeArgumentString(const char *text, const int &length)
{
	QString result = "";

	for (int i = 0; i < length; i += 2)
	{
		char buf[3] = { text[i], text[i + 1], 0 };

		result += (char)QString(buf).toInt(nullptr, 16);
	}

	return result;
}
//----------------------------------------------------------------------------------
QString OrionLauncherWindow::EncodeArgumentString(const char *text, const int &length)
{
	QString result = "";

	for (int i = 0; i < length; i++)
	{
		QString buf = "";
		buf.sprintf("%02X", text[i]);

		result += buf;
	}

	return result;
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::RunProgram(const QString &exePath, const QString &directory)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	memset(&pi, 0, sizeof(pi));

	if (CreateProcess(NULL, (LPWSTR)exePath.toStdWString().c_str(), NULL, NULL, FALSE, 0, NULL, (LPWSTR)directory.toStdWString().c_str(), &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_Launch_clicked()
{
	QString directoryPath = ui->cb_OrionPath->currentText();

	if (!QFile::exists(ui->cb_OrionPath->currentText() + "/Client.cuo"))
	{
		ui->pb_ConfigureClientVersion->setStyleSheet("color: rgb(255, 0, 0);");

		QString configErrorText = "You miss file 'Client.cuo'!\nYou must create it with ConfigurationEditor.exe tool.";

		if (QFile::exists(directoryPath + "/ConfigurationEditor.exe"))
			configErrorText += "\nPress ok to start the tool.";

		QMessageBox::critical(this, "Configuration error!", configErrorText);
		on_pb_ConfigureClientVersion_clicked();
		return;
	}

	CServerListItem *serverItem = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (serverItem == nullptr)
	{
		QMessageBox::critical(this, tr("Launch error"), tr("Server is not selected!"));
		return;
	}

	QString program = ui->cb_OrionPath->currentText() + "/OrionUO.exe";

	QString command = ui->le_CommandLine->text();

	if (ui->cb_LaunchFastLogin->isChecked())
		command += " -fastlogin";

	if (ui->cb_LaunchAutologin->isChecked())
		command += " -autologin";
	else
		command += " -autologin:0";

	if (ui->cb_LaunchSavePassword->isChecked())
		command += " -savepassword";
	else
		command += " -savepassword:0";

	if (ui->cb_LaunchSaveAero->isChecked())
		command += " -aero";

	command += " \"-login:" + serverItem->GetAddress() + "\"";

	QString account = serverItem->GetAccount();
	QString password = serverItem->GetPassword();
	command += " -account:" + EncodeArgumentString(account.toStdString().c_str(), account.length()) + "," + EncodeArgumentString(password.toStdString().c_str(), password.length());

	QString character = serverItem->GetCharacter();

	if (character.length())
		command += " -autologinname:" + EncodeArgumentString(character.toStdString().c_str(), character.length());

	if (ui->cb_NoClientWarnings->isChecked())
		command += "-nowarnings";

	if (serverItem->GetUseProxy())
	{
		QString proxyName = serverItem->GetProxy().toLower();

		for (int i = 0; i < ui->lw_ProxyList->count(); i++)
		{
			CProxyListItem *proxy = (CProxyListItem*)ui->lw_ProxyList->item(i);

			if (proxy != nullptr && proxy->text().toLower() == proxyName)
			{
				command += " -proxyhost:" + proxy->GetAddress() + "," + proxy->GetProxyPort();

				if (proxy->GetSocks5())
				{
					QString proxyAccount = proxy->GetAccount();
					QString proxyPassword = proxy->GetPassword();
					command += " -proxyaccount:" + EncodeArgumentString(proxyAccount.toStdString().c_str(), proxyAccount.length()) + "," + EncodeArgumentString(proxyPassword.toStdString().c_str(), proxyPassword.length());

				}

				break;
			}
		}
	}

	if (command.length())
		program += " " + command;

	RunProgram(program, directoryPath);

	if (ui->cb_LaunchRunUOAM->isChecked())
	{
		directoryPath += "/Map";
		RunProgram(directoryPath + "/EnhancedMap.exe", directoryPath);
	}

	if (ui->cb_LaunchCloseAfterLaunch->isChecked())
	{
		SaveServerList();
		SaveProxyList();

		qApp->exit(0);
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_LaunchAutologin_clicked()
{
	CServerListItem *item = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (item != nullptr)
		item->SetOptionAutologin(ui->cb_LaunchAutologin->isChecked());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_LaunchSavePassword_clicked()
{
	CServerListItem *item = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (item != nullptr)
		item->SetOptionSavePassword(ui->cb_LaunchSavePassword->isChecked());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_LaunchSaveAero_clicked()
{
	CServerListItem *item = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (item != nullptr)
		item->SetOptionSaveAero(ui->cb_LaunchSaveAero->isChecked());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_LaunchFastLogin_clicked()
{
	CServerListItem *item = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (item != nullptr)
		item->SetOptionFastLogin(ui->cb_LaunchFastLogin->isChecked());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_LaunchRunUOAM_clicked()
{
	CServerListItem *item = (CServerListItem*)ui->lw_ServerList->currentItem();

	if (item != nullptr)
		item->SetOptionRunUOAM(ui->cb_LaunchRunUOAM->isChecked());
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_lw_OAFeaturesOptions_clicked(const QModelIndex &index)
{
	Q_UNUSED(index);

	UpdateOAFecturesCode();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_lw_OAFeaturesScripts_clicked(const QModelIndex &index)
{
	Q_UNUSED(index);

	UpdateOAFecturesCode();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_rb_OAFeaturesSphere_clicked()
{
	UpdateOAFecturesCode();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_rb_OAFeaturesRunUO_clicked()
{
	UpdateOAFecturesCode();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_rb_OAFeaturesPOL_clicked()
{
	UpdateOAFecturesCode();
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::UpdateOAFecturesCode()
{
	quint64 featuresFlags = 0;
	quint64 scriptGroupsFlags = 0;

	for (int i = 0; i < ui->lw_OAFeaturesOptions->count(); i++)
	{
		QListWidgetItem *item = ui->lw_OAFeaturesOptions->item(i);

		if (item != nullptr && item->checkState() == Qt::Checked)
			featuresFlags |= 1 << i;
	}

	for (int i = 0; i < ui->lw_OAFeaturesScripts->count(); i++)
	{
		QListWidgetItem *item = ui->lw_OAFeaturesScripts->item(i);

		if (item != nullptr && item->checkState() == Qt::Checked)
			scriptGroupsFlags |= 1 << i;
	}

	QString code = "";

	if (ui->rb_OAFeaturesSphere->isChecked())
	{
		code.sprintf("//data for sendpacket\nB0FC W015 W0A001 D0%08X D0%08X D0%08X D0%08X",
					 (uint)((featuresFlags << 32) & 0xFFFFFFFF), (uint)(featuresFlags & 0xFFFFFFFF),
					 (uint)((scriptGroupsFlags << 32) & 0xFFFFFFFF), (uint)(scriptGroupsFlags & 0xFFFFFFFF));
	}
	else if (ui->rb_OAFeaturesRunUO->isChecked())
	{
		code.sprintf("public sealed class OAFeatures : Packet\n"
					"{\n"
						"public OAFeatures() : base(0xFC)\n"
						"{\n"
							"EnsureCapacity(21);\n"
							"m_Stream.Write((ushort)0xA001);\n"
							"m_Stream.Write((uint)0x%08X);\n"
							"m_Stream.Write((uint)0x%08X);\n"
							"m_Stream.Write((uint)0x%08X);\n"
							"m_Stream.Write((uint)0x%08X);\n"
						"}\n"
					"}",
					 (uint)((featuresFlags << 32) & 0xFFFFFFFF), (uint)(featuresFlags & 0xFFFFFFFF),
					 (uint)((scriptGroupsFlags << 32) & 0xFFFFFFFF), (uint)(scriptGroupsFlags & 0xFFFFFFFF));
	}
	else if (ui->rb_OAFeaturesPOL->isChecked())
	{
		code.sprintf("program oafeatures_sendpacket(who)\n"
					 "var res := SendPacket(who, \"FC0015A001%08X%08X%08X%08X\");\n"
					 "if (!res)\n"
						 "print(\"SendPacket error: \" + res.errortext );\n"
					 "endif\n"
					"endprogram",
					 (uint)((featuresFlags << 32) & 0xFFFFFFFF), (uint)(featuresFlags & 0xFFFFFFFF),
					 (uint)((scriptGroupsFlags << 32) & 0xFFFFFFFF), (uint)(scriptGroupsFlags & 0xFFFFFFFF));
	}

	ui->pte_OAFeaturesCode->setPlainText(code);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_cb_OrionPath_currentIndexChanged(int index)
{
	Q_UNUSED(index);

	if (!m_Loading)
	{
		if (ui->cb_CheckUpdates->isChecked())
			on_pb_CheckUpdates_clicked();

		QString configFilePath = ui->cb_OrionPath->currentText() + "/Client.cuo";

		if (!QFile::exists(configFilePath))
			ui->pb_ConfigureClientVersion->setStyleSheet("color: rgb(255, 0, 0);");
		else
			ui->pb_ConfigureClientVersion->setStyleSheet("color: rgb(0, 0, 0);");
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::slot_UpdatesListReceived(QList<CUpdateInfo> list)
{
	ui->lw_AvailableUpdates->clear();
	QString directoryPath = ui->cb_OrionPath->currentText();

	for (const CUpdateInfo &info : list)
	{
		QString crc32 = "";
		QString version = "";

		bool wantUpdate = !CUpdateManager<OrionLauncherWindow>::GetFileInfo((info.UODir == "yes" ? directoryPath : qApp->applicationDirPath()) + "/" + info.Name, version, crc32);

		if (info.Version.length() && info.Version != version)
			wantUpdate = true;

		if (info.Hash.length() && info.Hash != crc32)
			wantUpdate = true;

		if (wantUpdate)
			ui->lw_AvailableUpdates->addItem(new CUpdateInfoListWidgetItem(info));
	}

	if (ui->lw_AvailableUpdates->count())
		ui->tw_Main->setCurrentIndex(2);

	ui->pb_CheckUpdates->setEnabled(true);
	ui->pb_ApplyUpdates->setEnabled(true);
	ui->lw_Backups->setEnabled(true);
	ui->pb_RestoreSelectedVersion->setEnabled(true);
	ui->pb_ShowChangelog->setEnabled(true);
	ui->pb_UpdateProgress->setValue(100);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::slot_BackupsListReceived(QList<CBackupInfo> list)
{
	ui->lw_Backups->clear();

	for (const CBackupInfo &info : list)
		ui->lw_Backups->addItem(new CBackupInfoListWidgetItem(info));
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::slot_FileReceived(QByteArray array, QString name)
{
	Q_UNUSED(array);
	Q_UNUSED(name);
	//qDebug() << "slot_FileReceived" << array.size() << name;
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::slot_FileReceivedNotification(QString name)
{
	Q_UNUSED(name);
	//qDebug() << "slot_FileReceivedNotification" << name;

	m_FilesToUpdateCount--;

	if (m_FilesToUpdateCount <= 0 || ui->lw_AvailableUpdates->count() <= 0)
	{
		ui->pb_CheckUpdates->setEnabled(true);
		ui->pb_ApplyUpdates->setEnabled(true);
		ui->lw_Backups->setEnabled(true);
		ui->pb_RestoreSelectedVersion->setEnabled(true);
		ui->pb_ShowChangelog->setEnabled(true);
		ui->pb_UpdateProgress->setValue(100);
		m_FilesToUpdateCount = 0;

		if (m_LauncherFoundInUpdates && QFile::exists(qApp->applicationDirPath() + "/olupd.exe"))
		{
			SaveServerList();
			SaveProxyList();

			RunProgram(qApp->applicationDirPath() + "/olupd.exe /OrionLauncher_Update.zip", qApp->applicationDirPath());

			exit(0);
		}
		else
			on_pb_CheckUpdates_clicked();
	}
	else
	{
		ui->pb_UpdateProgress->setValue(((ui->lw_AvailableUpdates->count() - m_FilesToUpdateCount) * 100) / ui->lw_AvailableUpdates->count());
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_CheckUpdates_clicked()
{
	if (!ui->pb_CheckUpdates->isEnabled())
		return;

	ui->pb_CheckUpdates->setEnabled(false);
	ui->pb_ApplyUpdates->setEnabled(false);
	ui->lw_Backups->setEnabled(false);
	ui->pb_RestoreSelectedVersion->setEnabled(false);
	ui->pb_ShowChangelog->setEnabled(false);
	ui->pb_UpdateProgress->setValue(0);

	ui->lw_AvailableUpdates->clear();
	ui->lw_Backups->clear();

	QtConcurrent::run(&CUpdateManager<OrionLauncherWindow>::CheckUpdates, QStringList() << "www.orion-client.online" << "/Downloads/" << "OrionUpdate.html", this);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ApplyUpdates_clicked()
{
	if (!ui->pb_CheckUpdates->isEnabled())
		return;

	if (ui->lw_AvailableUpdates->count() < 1)
	{
		ui->pb_UpdateProgress->setValue(100);
		return;
	}

	ui->pb_UpdateProgress->setValue(0);

	if (QMessageBox::question(this, "Updates notification", "Close all OrionUO windows and press 'Yes'.\nPress 'No' for cancel.") != QMessageBox::Yes)
		return;

	ui->pb_CheckUpdates->setEnabled(false);
	ui->pb_ApplyUpdates->setEnabled(false);
	ui->lw_Backups->setEnabled(false);
	ui->pb_RestoreSelectedVersion->setEnabled(false);
	ui->pb_ShowChangelog->setEnabled(false);

	QString directoryPath = ui->cb_OrionPath->currentText();
	m_LauncherFoundInUpdates = false;
	m_FilesToUpdateCount = 0;

	QList<CUpdateInfoListWidgetItem*> updateList;

	for (int i = 0 ; i < ui->lw_AvailableUpdates->count(); i++)
	{
		CUpdateInfoListWidgetItem *item = (CUpdateInfoListWidgetItem*)ui->lw_AvailableUpdates->item(i);

		if (item == nullptr)
			continue;

		m_FilesToUpdateCount++;

		updateList.push_back(item);
	}

	if (m_FilesToUpdateCount)
	{
		for (CUpdateInfoListWidgetItem *item : updateList)
		{
			bool removeFile = true;
			QString path = directoryPath;

			if (item->m_Info.UODir != "yes")
			{
				if (item->text() == "OrionLauncher.exe")
				{
					removeFile = false;
					m_LauncherFoundInUpdates = true;
				}

				path = qApp->applicationDirPath();
			}

			QtConcurrent::run(&CUpdateManager<OrionLauncherWindow>::DownloadFile, QStringList() << "www.orion-client.online" << "/Downloads/" << item->m_Info.ZipFileName, this, QString(path + "/" + item->m_Info.ZipFileName), removeFile);
		}
	}
	else
	{
		ui->pb_CheckUpdates->setEnabled(true);
		ui->pb_ApplyUpdates->setEnabled(true);
		ui->lw_Backups->setEnabled(true);
		ui->pb_RestoreSelectedVersion->setEnabled(true);
		ui->pb_ShowChangelog->setEnabled(true);
		ui->pb_UpdateProgress->setValue(100);
	}
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ConfigureClientVersion_clicked()
{
	QString directoryPath = ui->cb_OrionPath->currentText();
	QString path = directoryPath + "/ConfigurationEditor.exe";

	if (QFile::exists(path))
		RunProgram(path, directoryPath);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_RestoreSelectedVersion_clicked()
{
	CBackupInfoListWidgetItem *item = (CBackupInfoListWidgetItem*)ui->lw_Backups->currentItem();

	if (item == nullptr)
		return;

	ui->pb_UpdateProgress->setValue(0);

	QtConcurrent::run(&CUpdateManager<OrionLauncherWindow>::DownloadFile, QStringList() << "www.orion-client.online" << "/Downloads/" << item->m_Backup.ZipFileName, this, QString(ui->cb_OrionPath->currentText() + "/" + item->m_Backup.ZipFileName), true);

	QMessageBox::information(this, "Waiting for data...", "Download the backup/test version.\nClick OK and wait until the progress bar is 100%\nThis process can take a long time.");
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_pb_ShowChangelog_clicked()
{
	if (m_ChangelogForm == nullptr)
		m_ChangelogForm = new ChangelogForm(nullptr);

	if (m_ChangelogForm->isVisible())
		m_ChangelogForm->activateWindow();
	else
		m_ChangelogForm->show();

	emit m_ChangelogForm->signal_ChangelogReceived("Loading...");

	QtConcurrent::run(&CUpdateManager<ChangelogForm>::GetChangelog, QStringList() << "www.orion-client.online" << "/Downloads/" << ("OrionChangelog" + ui->cb_ChangelogLanguage->currentText() + ".html"), m_ChangelogForm);
}
//----------------------------------------------------------------------------------
void OrionLauncherWindow::on_lw_Backups_doubleClicked(const QModelIndex &index)
{
	Q_UNUSED(index);
	on_pb_RestoreSelectedVersion_clicked();
}
//----------------------------------------------------------------------------------
