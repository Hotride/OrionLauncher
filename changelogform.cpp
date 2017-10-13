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
//----------------------------------------------------------------------------------
ChangelogForm::ChangelogForm(QWidget *parent)
: QMainWindow(parent), ui(new Ui::ChangelogForm)
{
	ui->setupUi(this);

	qRegisterMetaType<QList<CChangelogInfo>>("QList<CChangelogInfo>");

	connect(this, SIGNAL(signal_ChangelogReceived(QList<CChangelogInfo>)), this, SLOT(slot_ChangelogReceived(QList<CChangelogInfo>)));
}
//----------------------------------------------------------------------------------
ChangelogForm::~ChangelogForm()
{
	delete ui;
}
//----------------------------------------------------------------------------------
void ChangelogForm::slot_ChangelogReceived(QList<CChangelogInfo> list)
{
	ui->pte_Log->clear();
	ui->pte_Log->setPlainText("Changelog Received!!!");
}
//----------------------------------------------------------------------------------
