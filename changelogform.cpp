/***********************************************************************************
**
** ChangelogForm.cpp
**
** Copyright (C) October 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "changelogform.h"
#include "ui_changelogform.h"
#include <QDesktopServices>
//----------------------------------------------------------------------------------
ChangelogForm::ChangelogForm(QWidget *parent)
: QMainWindow(parent), ui(new Ui::ChangelogForm)
{
	ui->setupUi(this);

	connect(this, SIGNAL(signal_ChangelogReceived(QString)), this, SLOT(slot_ChangelogReceived(QString)));
	connect(ui->tb_Log, SIGNAL(anchorClicked(QUrl)), this, SLOT(slot_StartLink(QUrl)));
}
//----------------------------------------------------------------------------------
ChangelogForm::~ChangelogForm()
{
	delete ui;
}
//----------------------------------------------------------------------------------
void ChangelogForm::slot_ChangelogReceived(QString str)
{
	ui->tb_Log->setHtml(str);
}
//----------------------------------------------------------------------------------
void ChangelogForm::slot_StartLink(QUrl url)
{
	QDesktopServices::openUrl(url);
}
//----------------------------------------------------------------------------------
