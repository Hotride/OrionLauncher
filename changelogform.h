/***********************************************************************************
**
** ChangelogForm.h
**
** Copyright (C) October 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef CHANGELOGFORM_H
#define CHANGELOGFORM_H
//----------------------------------------------------------------------------------
#include <QMainWindow>
#include <QUrl>
#include "UpdateManager/updatemanager.hpp"
//----------------------------------------------------------------------------------
namespace Ui
{
	class ChangelogForm;
}
//----------------------------------------------------------------------------------
class ChangelogForm : public QMainWindow
{
	Q_OBJECT

public:
	explicit ChangelogForm(QWidget *parent = 0);
	~ChangelogForm();

private slots:
	void slot_ChangelogReceived(QString str);
	void slot_StartLink(QUrl url);

signals:
	void signal_UpdatesListReceived(QList<CUpdateInfo>);
	void signal_BackupsListReceived(QList<CBackupInfo>);
	void signal_ChangelogReceived(QString);
	void signal_FileReceived(QByteArray, QString);
	void signal_FileReceivedNotification(QString);
	void signal_AutoUpdateProgress(int);
	void signal_AutoUpdateNotification();

private:
	Ui::ChangelogForm *ui;
};
//----------------------------------------------------------------------------------
#endif // CHANGELOGFORM_H
//----------------------------------------------------------------------------------
