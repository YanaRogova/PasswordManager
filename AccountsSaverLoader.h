#pragma once

#include <QString>
#include <QJsonObject>

class AccountsSaverLoader
{
public:
	AccountsSaverLoader(QString sPath);

	bool SaveAccounts(const QMap<QString, QMap<QString, QString>>& deviceAppsData, 
		const QMap<QString, QMap<QString, QString>>& otherAppsData);
	void LoadAccounts(QMap<QString, QMap<QString, QString>>& deviceAppsData, QMap<QString, QMap<QString, QString>>& otherAppsData);

private:
	QJsonObject ConvertAccountsToJson(const QMap<QString, QMap<QString, QString>>& mAppsAccounts);
	QJsonObject ObjectToSave(const QMap<QString, QMap<QString, QString>>& deviceAppsData,
		const QMap<QString, QMap<QString, QString>>& otherAppsData);
	QMap<QString, QMap<QString, QString>> ConvertAccountsFromJson(const QJsonObject& jsonObject);
	void LoadGroupsFromObject(const QJsonObject& jsonObject, QMap<QString, QMap<QString, QString>>& deviceAppsData,
		QMap<QString, QMap<QString, QString>>& otherAppsData);

private:
	QString m_sPath;
	QString m_sFileName;
};

