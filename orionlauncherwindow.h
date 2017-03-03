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

private:
	Ui::OrionLauncherWindow *ui;

	QString BoolToText(const bool &value);

	bool RawStringToBool(QString value);

	void SaveProxyList();

	void SaveServerList();

	void LoadProxyList();

	void LoadServerList();

	QString DecodeArgumentString(const char *text, const int &length);
	QString EncodeArgumentString(const char *text, const int &length);
};
//----------------------------------------------------------------------------------
extern OrionLauncherWindow *g_OrionLauncherWindow;
//----------------------------------------------------------------------------------
#endif // ORIONLAUNCHERWINDOW_H
//----------------------------------------------------------------------------------
