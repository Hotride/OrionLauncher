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
#endif // UPDATEINFO_H
//----------------------------------------------------------------------------------
