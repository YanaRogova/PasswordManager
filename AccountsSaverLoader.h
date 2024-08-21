#pragma once

#include <QString>
#include <QJsonObject>
#include "AppsManager.h"

class AccountsSaverLoader
{
public:
	AccountsSaverLoader(QString sPath);

	bool SaveAccounts(const AppsManager& deviceAppsData, const AppsManager& otherAppsData);
	void LoadAccounts(AppsManager& deviceAppsData, AppsManager& otherAppsData);

private:
	QJsonObject ConvertAccountsToJson(const AppsManager& mAppsAccounts);
	QJsonObject ObjectToSave(const AppsManager& deviceAppsData,	const AppsManager& otherAppsData);
	AppsManager ConvertAccountsFromJson(const QJsonObject& jsonObject);
	void LoadGroupsFromObject(const QJsonObject& jsonObject, AppsManager& deviceAppsData,
		AppsManager& otherAppsData);

private:
	QString m_sPath;
	QString m_sFileName;
	QString m_sBufFileName;
	QByteArrayView m_hashKey;
};

