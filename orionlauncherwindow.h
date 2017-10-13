/***********************************************************************************
**
** OrionLauncherWindow.h
**
** Copyright (C) December 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef ORIONLAUNCHERWINDOW_H
#define ORIONLAUNCHERWINDOW_H
//----------------------------------------------------------------------------------
#include <QMainWindow>
#include <QCloseEvent>
#include <QKeyEvent>
#include "UpdateManager/updatemanager.hpp"
#include "changelogform.h"
//----------------------------------------------------------------------------------
namespace Ui
{
	class OrionLauncherWindow;
}
//----------------------------------------------------------------------------------
class OrionLauncherWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit OrionLauncherWindow(QWidget *parent = 0);
	~OrionLauncherWindow();

protected:
	void closeEvent(QCloseEvent *event);

	void keyPressEvent(QKeyEvent *event);

private slots:
	void on_lw_ServerList_clicked(const QModelIndex &index);

	void on_lw_ServerList_doubleClicked(const QModelIndex &index);

	void on_cb_ServerShowPassword_clicked();

	void on_pb_ServerAdd_clicked();

	void on_pb_ServerSave_clicked();

	void on_pb_ServerRemove_clicked();

	void on_le_CommandLine_textChanged(const QString &arg1);

	void on_lw_ProxyList_clicked(const QModelIndex &index);

	void on_cb_ProxyShowPassword_clicked();

	void on_pb_ProxyAdd_clicked();

	void on_pb_ProxySave_clicked();

	void on_pb_ProxyRemove_clicked();

	void on_tb_SetOrionPath_clicked();

	void on_pb_Launch_clicked();

	void on_cb_LaunchAutologin_clicked();

	void on_cb_LaunchSavePassword_clicked();

	void on_cb_LaunchSaveAero_clicked();

	void on_cb_LaunchFastLogin_clicked();

	void on_cb_LaunchRunUOAM_clicked();

	void on_lw_OAFeaturesOptions_clicked(const QModelIndex &index);

	void on_lw_OAFeaturesScripts_clicked(const QModelIndex &index);

	void on_rb_OAFeaturesSphere_clicked();

	void on_rb_OAFeaturesRunUO_clicked();

	void on_rb_OAFeaturesPOL_clicked();

	void on_pb_CheckUpdates_clicked();

	void on_cb_OrionPath_currentIndexChanged(int index);

	void on_pb_ApplyUpdates_clicked();

	void on_pb_ConfigureClientVersion_clicked();

	void slot_UpdatesListReceived(QList<CUpdateInfo> list);
	void slot_BackupsListReceived(QList<CBackupInfo> list);
	void slot_FileReceived(QByteArray array, QString name);
	void slot_FileReceivedNotification(QString name);

	void on_pb_RestoreSelectedVersion_clicked();

	void on_pb_ShowChangelog_clicked();

	void on_lw_Backups_doubleClicked(const QModelIndex &index);

signals:
	void signal_UpdatesListReceived(QList<CUpdateInfo>);
	void signal_BackupsListReceived(QList<CBackupInfo>);
	void signal_ChangelogReceived(QString);
	void signal_FileReceived(QByteArray, QString);
	void signal_FileReceivedNotification(QString);
	void signal_AutoUpdateProgress(int);
	void signal_AutoUpdateNotification();

private:
	Ui::OrionLauncherWindow *ui;

	bool m_Loading{ true };

	int m_FilesToUpdateCount{ 0 };

	bool m_LauncherFoundInUpdates{ false };

	ChangelogForm *m_ChangelogForm{ nullptr };

	void UpdateServerFields(const int &index);

	QString BoolToText(const bool &value);

	bool RawStringToBool(QString value);

	void SaveProxyList();

	void SaveServerList();

	void LoadProxyList();

	void LoadServerList();

	QString DecodeArgumentString(const char *text, const int &length);
	QString EncodeArgumentString(const char *text, const int &length);

	void RunProgram(const QString &exePath, const QString &directory);

	void UpdateOAFecturesCode();
};
//----------------------------------------------------------------------------------
extern OrionLauncherWindow *g_OrionLauncherWindow;
//----------------------------------------------------------------------------------
#endif // ORIONLAUNCHERWINDOW_H
//----------------------------------------------------------------------------------
