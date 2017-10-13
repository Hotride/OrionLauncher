/**
@file UpdateInfo.hpp

@brief Класс с информацией о обновлении

@author Мустакимов Т.Р.
**/
//----------------------------------------------------------------------------------
#ifndef UPDATEINFO_H
#define UPDATEINFO_H
//----------------------------------------------------------------------------------
#include <QListWidgetItem>
//----------------------------------------------------------------------------------
/**
 * @brief The CUpdateInfo class
 * Информация о файле
 */
class CUpdateInfo
{
public:
	CUpdateInfo() {}
	~CUpdateInfo() {}

	//! Название файла для проверки
	QString Name{ "" };

	//! CRC32 файла
	QString Hash{ "" };

	//! Версия файла
	QString Version{ "" };

	//! Название архива с файлом (как называется на сервере)
	QString ZipFileName{ "" };

	//! Примечание к обновлению
	QString Notes{ "" };

	//! В корневой директории
	QString UODir{ "" };
};
//----------------------------------------------------------------------------------
/**
 * @brief The CUpdateInfo class
 * Информация о файле
 */
class CBackupInfo
{
public:
	CBackupInfo() {}
	~CBackupInfo() {}

	//! Название
	QString Name{ "" };

	//! Название архива с файлом/файлами (как называется на сервере)
	QString ZipFileName{ "" };
};
//----------------------------------------------------------------------------------
/**
 * @brief The CUpdateInfo class
 * Информация о файле
 */
class CChangelogInfo
{
public:
	CChangelogInfo() {}
	~CChangelogInfo() {}

	//! Название
	QString Name{ "" };

	//! Описание
	QString Description{ "" };
};
//----------------------------------------------------------------------------------
/**
 * @brief The CUpdateInfoListWidgetItem class
 * Элемент QListWidget с информацией о обновлении
 */
class CUpdateInfoListWidgetItem : public QListWidgetItem
{
public:
	CUpdateInfoListWidgetItem(const CUpdateInfo &info)
	: QListWidgetItem(), m_Info(info)
	{
		//! Отображаемый текст - имя файла
		setText(info.Name);

		//! Выбран галочкой в списке
		//setCheckState(Qt::Checked);
	}

	virtual ~CUpdateInfoListWidgetItem() {}

	//! Информация о обновлении
	CUpdateInfo m_Info;
};
//----------------------------------------------------------------------------------
/**
 * @brief The CBackupInfoListWidgetItem class
 * Элемент QListWidget с информацией о резервной версии
 */
class CBackupInfoListWidgetItem : public QListWidgetItem
{
public:
	CBackupInfoListWidgetItem(const CBackupInfo &backup)
	: QListWidgetItem(), m_Backup(backup)
	{
		//! Отображаемый текст - имя файла
		setText(backup.Name);

		//! Выбран галочкой в списке
		//setCheckState(Qt::Checked);
	}

	virtual ~CBackupInfoListWidgetItem() {}

	//! Информация о обновлении
	CBackupInfo m_Backup;
};
//----------------------------------------------------------------------------------
#endif // UPDATEINFO_H
//----------------------------------------------------------------------------------
