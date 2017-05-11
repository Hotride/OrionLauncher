/***********************************************************************************
**
** UpdateListItem.h
**
** Copyright (C) May 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef UPDATELISTITEM_H
#define UPDATELISTITEM_H
//----------------------------------------------------------------------------------
#include <QListWidgetItem>
#include <QListWidget>
#include <QList>
//----------------------------------------------------------------------------------
class CUpdateListItem : public QListWidgetItem
{
public:
	CUpdateListItem();
	virtual ~CUpdateListItem() {}

	QString Name{ "" };
	QString Version{ "" };
	QString Hash{ "" };
	QString ZipFileName{ "" };
	QString Notes{ "" };
	bool InUODir{ true };
};
//----------------------------------------------------------------------------------
#endif // UPDATELISTITEM_H
//----------------------------------------------------------------------------------
